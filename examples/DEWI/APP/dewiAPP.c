/*
 * dewiDemo.c
 *
 *  Created on: Oct 17, 2016
 *      Author: root
 */

#include "contiki.h"
#include "cpu.h"
#include "lib/list.h"
#include "lib/memb.h"
#include "dev/leds.h"
#include "dev/uart.h"
#include "dev/adc-zoul.h"
#include "dev/watchdog.h"
#include "dev/zoul-sensors.h"
#include "dev/button-sensor.h"
#include "dev/serial-line.h"
#include "net/rime/rime.h"
#include "sys/stimer.h"
#include "net/rime/broadcast.h"
#include "net/mac/tsch/tsch.h"
#include "net/mac/tsch/tsch-schedule.h"

#include "net/DEWI/scheduler/scheduler.h"
#include "net/DEWI/rll/rll.h"
#include "net/DEWI/cider/cider.h"
#include "net/DEWI/neighTable/neighTable.h"

#include "i2c.h"
#include "project-conf.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>

#if APP_LOG_LEVEL >= 1
#define DEBUG DEBUG_PRINT
#else /* TSCH_LOG_LEVEL */
#define DEBUG DEBUG_NONE
#endif /* TSCH_LOG_LEVEL */
#include "net/net-debug.h"

/***************************************/
/***************************************/
/*				Macros 				   */
/***************************************/
/***************************************/

#define BYTETOBINARYPATTERN "%d%d%d%d%d%d%d%d"		// to convert byte to binary
#define BYTETOBINARY(byte)  \
  (byte & 0x80 ? 1 : 0), \
  (byte & 0x40 ? 1 : 0), \
  (byte & 0x20 ? 1 : 0), \
  (byte & 0x10 ? 1 : 0), \
  (byte & 0x08 ? 1 : 0), \
  (byte & 0x04 ? 1 : 0), \
  (byte & 0x02 ? 1 : 0), \
  (byte & 0x01 ? 1 : 0)

// LED colors
#define LED_RED 0b10000000
#define LED_GREEN 0b01100000
#define LED_BLUE 0b01000000

#define LED_BRIGHTNESS 0b00100000
#define BUTTON_PRESS_EVENT_INTERVAL (CLOCK_SECOND)

#define NUM_ADDR_ENTRIES 50		//number of child nodes that a master node can have
#define MAX_RESULTS_ENTRIES 100

/***************************************/
/***************************************/
/*				Variables			   */
/***************************************/
/***************************************/

//common Variables
uint8_t button_press_counter = 0;
uint8_t lastBRIGHTNESS = 0b00000001;
uint16_t seqNo = 1;
uint16_t lastSeqNo = 1;
uint8_t lock = 1;
char waitForTopologyUpdate;  //is 1 if topology information is ongoing, else 0
char isGateway; // is 1 if this node is a gateway, else 0
linkaddr_t master_addr1; //parent or master address

///////////////////////////////
////////////////////////////////
//Experiment specific variables
/////////////////////////////
////////////////////////////
uint16_t txPackets = 0;
uint16_t rxPackets = 0;
uint16_t ackAddress;
uint8_t ackType;
uint8_t LQIrx = 0, RSSIrx = 0, TxPowerrx, TXPowerNegativ = 0;
struct resultCounter
{ //structure for each child node that the master or parent node has
	/* The ->next pointer is needed since we are placing these on a
	 Contiki list. */
	struct resultCounter *next;
	uint8_t timeslot;
	uint16_t counter;
};
MEMB(result_memb, struct resultCounter, MAX_RESULTS_ENTRIES);
LIST(result_list);
uint8_t experimentActive = 1;
uint8_t tempresult[50] = { 0 };
uint8_t tempResultCounter = 0;

///////////////////////////////
////////////////////////////////
//Demonstrator specific variables
/////////////////////////////
////////////////////////////
char ledToggle;

// instead of having different timers, use counters to trigger actions at multiples of process event timer
char sendSensorDataCountdown; // for recurrent transmission of sensor data (temperature and battery level)

struct performanceStatEntry
{ //structure for performance statistics
	struct performanceStatEntry *next;
	uint8_t latency; // latency/10
	uint16_t packetCounter;     // counting all received packets for this latency
} stat;

struct topologyInfo_entry
{ //structure for each topology info that is collected by gateway node
	struct topologyInfo_entry *next;
	linkaddr_t master, child;
	uint16_t colour;
};

LIST(topologyInfo_list); //list of topology information
MEMB(topologyInfo_mem, struct topologyInfo_entry, NUM_ADDR_ENTRIES);

LIST(perfStat_list); // list of performance statistics
MEMB(perfStat_mem, struct performanceStatEntry, 255); // as latency is a uint8_t, we can handle max 255 different latencies

/***************************************/
/***************************************/
/*			    Timers		 		   */
/***************************************/
/***************************************/

//common timer
static struct etimer button_press_reset;
const struct etimer processResults_timer, topologyReply_timer, resultReply_timer, ackTimer;

/***************************************/
/***************************************/
/*			Function Headers 		   */
/***************************************/
/***************************************/
void setColor(uint16_t R, uint16_t G, uint16_t B);
void updatePerformanceStats(uint8_t latency);
void sendBatteryStatusByserialP(int b_data, linkaddr_t addr);
void handleSensorsEvent(process_data_t data);
void handleSerialInput(process_data_t data);
void handleProcessEvent();
void handleTopologyRequest();
void handleTopologyReply(struct APP_PACKET *data);

uint8_t addResult(struct resultCounter *res);
void getResult(uint8_t timeslot, struct resultCounter *res);
uint8_t getAllResults(uint8_t timeslot[], uint16_t values[]);
void clearResults();
void sendACK(linkaddr_t addr);

PROCESS(dewiExperiment, "DEWI Experiment, using CIDER and RLL");	//main process
PROCESS(dewiDemo, "DEWI Demo, using CIDER and RLL");	//main process
PROCESS(dewiStart, "DEWI Start process");	//main process
AUTOSTART_PROCESSES(&dewiStart);

void setColor(uint16_t R, uint16_t G, uint16_t B)
{
	i2c_single_send(0x39, LED_RED | R);
	clock_delay_usec(50);
	i2c_single_send(0x39, LED_GREEN | G);
	clock_delay_usec(50);
	i2c_single_send(0x39, LED_BLUE | B);
	clock_delay_usec(50);

}

void updatePerformanceStats(uint8_t latency)
{
	struct performanceStatEntry *e = NULL;
	for (e = list_head(perfStat_list); e != NULL; e = e->next)
	{ //check if the entry with this latency is already present and update it
		if (e->latency == latency)
		{
			e->packetCounter++;
			break;
		}
	}
	if (e == NULL)
	{
		// no entry with this latency yet, create one
		e = memb_alloc(&perfStat_mem);
		e->latency = latency;
		e->packetCounter = 1;
		list_push(perfStat_list, e);
	}
}

// convert battery voltage level to battery percentage status, and send over serial port
void sendBatteryStatusByserialP(int b_data, linkaddr_t addr)
{
	if (b_data >= 3240)
	{
		printf("node(%04x) Battery '100%%' \r\n", addr.u16);
	}
	else if (b_data >= 3230 && b_data < 3240)
	{
		printf("node(%04x) Battery '90%%' \r\n", addr.u16);
	}
	else if (b_data >= 3200 && b_data < 3230)
	{
		printf("node(%04x) Battery '80%%' \r\n", addr.u16);
	}
	else if (b_data >= 3170 && b_data < 3200)
	{
		printf("node(%04x) Battery '70%%' \r\n", addr.u16);
	}
	else if (b_data >= 3100 && b_data < 3170)
	{
		printf("node(%04x) Battery '60%%' \r\n", addr.u16);
	}
	else if (b_data >= 3050 && b_data < 3100)
	{
		printf("node(%04x) Battery '50%%' \r\n", addr.u16);
	}
	else if (b_data >= 2950 && b_data < 3050)
	{
		printf("node(%04x) Battery '40%%' \r\n", addr.u16);
	}
	else if (b_data >= 2900 && b_data < 2950)
	{
		printf("node(%04x) Battery '30%%' \r\n", addr.u16);
	}
	else if (b_data >= 2800 && b_data < 2900)
	{
		printf("node(%04x) Battery '20%%' \r\n", addr.u16);
	}
	else if (b_data >= 2600 && b_data < 2800)
	{
		printf("node(%04x) Battery '10%%' \r\n", addr.u16);
	}
	else if (b_data >= 2350 && b_data < 2600)
	{
		printf("node(%04x) Battery '5%%' \r\n", addr.u16);
	}
	else if (b_data < 2350)
	{
		printf("node(%04x) Battery '0%%' \r\n", addr.u16);
	}
}

// function to handle incoming sensor events
// input: the data object associated with the event
void handleSensorsEvent(process_data_t data)
{
	if (experimentActive == 1)
	{

		PROCESS_CONTEXT_BEGIN(&dewiExperiment)
			;
			if (data == &button_sensor)
			{ // receiving user button event

				if (button_sensor.value(BUTTON_SENSOR_VALUE_TYPE_LEVEL)
						== BUTTON_SENSOR_PRESSED_LEVEL)
				{

					button_press_counter = button_press_counter + 1;
					etimer_set(&button_press_reset, CLOCK_SECOND * 0.5);
					if (button_press_counter == 10)
					{
						CIDER_start();
					}
					if (button_press_counter == 3)
					{
						printNodeStatusForced();
					}
					if (button_press_counter == 2)
					{

						getMinMaxAVGRSSI();

					}

				}

			}
			PROCESS_CONTEXT_END(&dewiExperiment);
	}
	else
	{
		PROCESS_CONTEXT_BEGIN(&dewiDemo)
			;
			if (data == &button_sensor)
			{ // receiving user button event

				if (button_sensor.value(BUTTON_SENSOR_VALUE_TYPE_LEVEL)
						== BUTTON_SENSOR_PRESSED_LEVEL)
				{

					button_press_counter = button_press_counter + 1;
					etimer_set(&button_press_reset, CLOCK_SECOND * 0.5);

					if (getActiveProtocol() == 2)
					{ // button pressed while node is in normal RLL operation, use button to toggle LEDs on/off
						uint8_t brightness = lastBRIGHTNESS;
						if (ledToggle == 1)
						{
							// switch off
							brightness = 0;
							ledToggle = 0;
						}
						else
						{
							// switch on
							ledToggle = 1;
						}
						// set brightness on own LED
						i2c_single_send(0x39, (LED_BRIGHTNESS | brightness));

						// broadcast brightness information to other nodes
						struct APP_PACKET temp;
						temp.subType = APP_BRIGHTNESS;
						temp.values[0] = (uint16_t) brightness;
						temp.timeSend = current_asn;
						temp.dst = tsch_broadcast_address;
						temp.src = linkaddr_node_addr;
						temp.seqNo = seqNo++;
						sendRLLDataMessage(temp, 0);
						updatePerformanceStats(0);

					}

				}
				if (button_press_counter == 10)
				{
					CIDER_start();
				}
			}
			PROCESS_CONTEXT_END(&dewiDemo);
	}
}

// function to handle incoming data on serial port
// input: the data object associated with the event
void handleSerialInput(process_data_t data)
{
	char* ch_data, *ptr;
	ch_data = (char*) data;
	char t1[2];
	long i_data;
	uint32_t R, G, B;
	if (experimentActive == 1)
	{
		if (LQIrx == 1)
		{
			uint16_t temp = (uint16_t) strtol(ch_data, NULL, 16);
			setLQIRadius(temp);
			sendLQI();
			LQIrx = 0;
			return;
		}
		if (RSSIrx == 1)
		{
			uint16_t temp = (int16_t) strtol(ch_data, NULL, 16);
			setRSSIRadius(temp * -1);
			sendRSSI();
			RSSIrx = 0;
			return;
		}
		if (TxPowerrx == 1 && strstr(ch_data, "NEGATIV") == NULL)
		{
			uint16_t temp = (int16_t) strtol(ch_data, NULL, 16);
			radio_value_t result = (radio_value_t) temp;
			if (TXPowerNegativ == 1) result = result * -1;

			TXPowerNegativ = 0;
			radio_result_t rv = NETSTACK_RADIO.set_value(RADIO_PARAM_TXPOWER, result);

			PRINTF("[APP] Set Tx Power to %d, result: %d\n",result,rv);
			sendTXPower();
			TxPowerrx = 0;
			return;
		}
		if (strstr(ch_data, "Experiment") != NULL)
		{

//		if(tsch_queue_packet_count(&tsch_broadcast_address) > 0)
//			tsch_queue_reset();

			PRINTF("[APP]: Experiment received, send message\n");
			struct APP_PACKET packet;
			packet.src = linkaddr_node_addr;
			packet.dst = tsch_broadcast_address;
			packet.timeSend = current_asn;
			packet.seqNo = seqNo++;
			packet.subType = APP_EXPERIMENT;

			lock = 0;
			sendRLLDataMessage(packet, 1);

		}
		else if (strstr(ch_data, "topologyrefresh") != NULL)
		{
			checkQueueStatus();
			PRINTF("[APP]: TopologyRefresh received, send message\n");
			struct APP_PACKET packet;
			packet.src = linkaddr_node_addr;
			packet.dst = tsch_broadcast_address;
			packet.subType = APP_TOPOLOGYREQUEST;

			lock = 1;
			sendRLLDataMessage(packet, 0);
		}
		else if (strstr(ch_data, "LQI") != NULL)
		{

			checkQueueStatus();
			PRINTF("[APP]: LQI request\n");
			LQIrx = 1;
		}
		else if (strstr(ch_data, "RSSI") != NULL)
		{

			checkQueueStatus();
			PRINTF("[APP]: RSSI request\n");
			RSSIrx = 1;
		}
		else if (strstr(ch_data, "TXPOWER") != NULL)
		{

			checkQueueStatus();
			PRINTF("[APP]: TXPOWER request\n");
			TxPowerrx = 1;
		}
		else if (strstr(ch_data, "NEGATIV") != NULL)
		{

			checkQueueStatus();
			PRINTF("[APP]: Negativ TXPOWER incoming\n");
			TXPowerNegativ = 1;
		}
		else if (strstr(ch_data, "STARTCIDER") != NULL)
		{
			checkQueueStatus();
			CIDER_start();
		}
		else if (strstr(ch_data, "0x") != NULL)
		{

			checkQueueStatus();
			uint16_t temp = (uint16_t) strtol(ch_data, NULL, 16);
			if (linkaddr_node_addr.u16 == temp)
			{
				PRINTF("RESULTReplyTxPackets:0x%4x,%d\n", linkaddr_node_addr.u16, txPackets);
				txPackets = 0;

			}
			else if (temp == 0)
			{
				sendACK(tsch_broadcast_address);
			}
			else
			{
				PRINTF("[APP]: APP_RESULTREQUEST received, send message\n");
				struct APP_PACKET packet;
				packet.src = linkaddr_node_addr;
				packet.dst.u16 = temp;
				packet.subType = APP_RESULTREQUEST;

				lock = 1;
				sendRLLDataMessage(packet, 0);
			}
		}
	}
	else
	{
		if (strcmp(ch_data, "topologyrefresh") == 0)
		{ // received topology refresh message
		  // re-initialize topology information list
			list_init(topologyInfo_list);
			// poll topology data from network
			struct APP_PACKET temp;
			temp.subType = APP_TOPOLOGYREQUEST;
			temp.timeSend = current_asn;
			temp.dst = tsch_broadcast_address;
			temp.src = linkaddr_node_addr;
			temp.seqNo = seqNo++;
			sendRLLDataMessage(temp, 0);

			// set a flag to wait for topology updates
			waitForTopologyUpdate = 2;

			// if I am a CH myself, put my own children in topologylist
			if (getCIDERState() == 5)
			{
				uint8_t numChildren = 0; // number of children
				linkaddr_t children[CONF_MAX_NEIGHBOURS];
				numChildren = getChildAddresses(children);
				printf("got topology request, have %d children\r\n", numChildren);

				int i;
				// go through the list of children
				for (i = 0; i < numChildren; i++)
				{
					struct topologyInfo_entry entry;
					entry.master = linkaddr_node_addr;
					entry.child = children[i];
					entry.colour = (uint16_t) getColour();
					struct topologyInfo_entry *e = NULL;
					for (e = list_head(topologyInfo_list); e != NULL; e = e->next)
					{ //check if the entry with this child is already present
						if (linkaddr_cmp(&e->child, &entry.child))
						{
							return;
						}
					}

					if (e == NULL)
					{ // if not present yet, add entry to the list
						e = memb_alloc(&topologyInfo_mem);
						if (e == NULL)
						{
							e = list_chop(topologyInfo_list); // Remove oldest at full history
						}
						linkaddr_copy(&e->child, &entry.child);
						linkaddr_copy(&e->master, &entry.master);
						list_push(topologyInfo_list, e);
					}
				}
			}

		}
		else if (strcmp(ch_data, "resetstatistics") == 0)
		{ // received statistics reset message
		  // broadcast statistics reset to network
			struct APP_PACKET temp;
			temp.subType = APP_STATSRESET;
			temp.timeSend = current_asn;
			temp.dst = tsch_broadcast_address;
			temp.src = linkaddr_node_addr;
			temp.seqNo = seqNo++;
			sendRLLDataMessage(temp, 0);
			// reset own statistics
			list_init(perfStat_list);
		}
		else if (strstr(ch_data, "BRIGHTNESS") != NULL)
		{ // received brightness value
			lastBRIGHTNESS = 0b00000001 + 3 * atoi(strncpy(t1, ch_data + 11, 2));
			if (lastBRIGHTNESS > 0b00011111) lastBRIGHTNESS = 0b00000001;
			// set brightness on own LED
			i2c_single_send(0x39, (LED_BRIGHTNESS | lastBRIGHTNESS));

			// broadcast brightness information to other nodes
			struct APP_PACKET temp;
			temp.subType = APP_BRIGHTNESS;
			temp.values[0] = (uint16_t) lastBRIGHTNESS;
			temp.timeSend = current_asn;
			temp.dst = tsch_broadcast_address;
			temp.src = linkaddr_node_addr;
			temp.seqNo = seqNo++;
			sendRLLDataMessage(temp, 0);
			updatePerformanceStats(0);

		}
		else
		{ // assuming received color value
			i_data = strtol(ch_data, &ptr, 16);	//Convert char data to hex

			R = (int) (i_data & 0x00ff0000) >> 19;  //Select byte 1 and shift to first position
			G = (int) (i_data & 0x0000ff00) >> 11;	//Same for byte 2
			B = (int) (i_data & 0x000000ff) >> 3;	//And byte 3
			// set color on own LED
			setColor(R, G, B);

			// put color in application packet and send it
			struct APP_PACKET temp;
			temp.subType = APP_COLOR;
			temp.values[0] = R;
			temp.values[1] = G;
			temp.values[2] = B;
			temp.timeSend = current_asn;
			temp.dst = tsch_broadcast_address;
			temp.src = linkaddr_node_addr;
			temp.seqNo = seqNo++;
			sendRLLDataMessage(temp, 0);
			updatePerformanceStats(0);

		}
	}

}

void handleProcessEvent()
{
	if (experimentActive == 0)
	{
		if (waitForTopologyUpdate == 1)
		{ // receiving a process event while waiting for topology updates
		  // send collected topology information over the serial port
			struct topologyInfo_entry *e = NULL;
			for (e = list_head(topologyInfo_list); e != NULL; e = e->next)
			{ //check if the entry with this child is already present
				printf("MasterSlave_addr data: '%08x' \r\n", (e->master.u16));
				printf("MasterSlave_addr data: '%08x' \r\n",
						(e->master.u16 << 16) | (e->child.u16));
				printf("Master (%04x) colour: '%d'\r\n", e->master.u16, e->colour);
			}
			if (list_length(topologyInfo_list) == 0)
			{
				printf("No topology info available! Re-starting collection.\r\n");
				// re-initialize topology information list
				list_init(topologyInfo_list);
				// poll topology data from network
				struct APP_PACKET temp;
				temp.subType = APP_TOPOLOGYREQUEST;
				temp.timeSend = current_asn;
				temp.dst = tsch_broadcast_address;
				temp.src = linkaddr_node_addr;
				temp.seqNo = seqNo++;
				sendRLLDataMessage(temp, 0);

				// set a flag to wait for topology updates
				waitForTopologyUpdate = 2;
			}
			else
			{
				// not waiting for topology updates any more
				waitForTopologyUpdate = 0;
			}
		}
		else if (waitForTopologyUpdate > 0)
		{
			waitForTopologyUpdate--;
		}

		if ((sendSensorDataCountdown <= 0) && (getActiveProtocol() == 2))
		{ // sensor data counter expired
		  // obtain temperature and battery values and send them
			int temperature, battery;
			temperature = cc2538_temp_sensor.value(
			CC2538_SENSORS_VALUE_TYPE_CONVERTED) / 1000;
			battery = vdd3_sensor.value(
			CC2538_SENSORS_VALUE_TYPE_CONVERTED);

			struct APP_PACKET temp;
			temp.subType = APP_SENSORDATA;
			temp.temperature = (uint8_t) temperature;
			temp.battery = (uint8_t) battery;
			temp.timeSend = current_asn;
			temp.dst = tsch_broadcast_address;
			temp.src = linkaddr_node_addr;
			temp.seqNo = seqNo++;
			// add performance stats to packet
			int i = 0;
			struct performanceStatEntry *e = NULL;
			for (e = list_head(perfStat_list); e != NULL; e = e->next)
			{ //add performance stats to packet
				temp.timeslot[i] = e->latency;
				temp.values[i] = e->packetCounter;
				if (i == 22)
				{
					// packet full, send it and create a new one
					temp.remainingData = 1;
					sendRLLDataMessage(temp, 0);
					i = 0;
					temp.seqNo = seqNo++;

				}
				else
				{
					temp.remainingData = 0;
					i++;
				}

				//if this is the gateway, send the performance data on the serial port
				if (isGateway)
				{
					printf("node(%04x) Stats: Packets = '%d', Latency = '%d'\r\n",
							linkaddr_node_addr.u16, e->packetCounter, e->latency);
				}
			}
			while (i < 23)
			{
				// fill remaining temp.values with 0 so that we don't accidentially parse undefined values on the other end
				temp.values[i] = 0;
				i++;
			}
			sendRLLDataMessage(temp, 0);

			if (isGateway)
			{
				// if this is the gateway, send data on serial port as well
				printf("node(%04x) Temperature = '%dC' \r\n", linkaddr_node_addr.u16, temperature);
				sendBatteryStatusByserialP(battery, linkaddr_node_addr);
			}
			sendSensorDataCountdown = 5 + linkaddr_node_addr.u16 % 15; // do this every xth loop, x being between 5 and 14
		}
		else
		{
			sendSensorDataCountdown--;
		}
	}

}

void handleTopologyRequest()
{
	if (getCIDERState()==5){
		uint8_t numChildren = 0; // number of children
		linkaddr_t children[CONF_MAX_NEIGHBOURS];
		numChildren = getChildAddresses(children);
		printf("got topology request, have %d children\r\n", numChildren);

		int i,j;
		for (i = 0; i <= numChildren/20; i++)
		{ // put information about max. 20 children in packet, send multiple packets if needed
			struct APP_PACKET temp;
			temp.subType = APP_TOPOLOGYREPLY;
			temp.values[0] = (uint16_t) ((i==numChildren/20)?numChildren%20:20);
			temp.values[1] = linkaddr_node_addr.u16;
			temp.values[2] = (uint16_t) getColour();
			for (j = 0; j < ((i==numChildren/20)?numChildren%20:20); j++)
			{
				temp.values[j+3] = children[i*10+j].u16;
			}
			temp.timeSend = current_asn;
			temp.dst = tsch_broadcast_address;
			temp.src = linkaddr_node_addr;
			temp.seqNo = seqNo++;
			sendRLLDataMessage(temp, 0);
		}
	}

}

// handle incoming topology information
// input: the incoming packet
void handleTopologyReply(struct APP_PACKET *data)
{
	int numEntries = data->values[0]; // extract the amount of child addresses in this packet
	int i;
	// go through the contents of the packet
	for (i = 0; i < numEntries; i++)
	{
		struct topologyInfo_entry entry;
		entry.master.u16 = data->values[1];
		entry.colour = data->values[2];
		entry.child.u16 = data->values[i + 3];
		struct topologyInfo_entry *e = NULL;
		for (e = list_head(topologyInfo_list); e != NULL; e = e->next)
		{ //check if the entry with this child is already present
			if (linkaddr_cmp(&e->child, &entry.child))
			{
				return;
			}
		}

		if (e == NULL)
		{ // if not present yet, add entry to the list
			e = memb_alloc(&topologyInfo_mem);
			if (e == NULL)
			{
				e = list_chop(topologyInfo_list); // Remove oldest at full history
			}
			linkaddr_copy(&e->child, &entry.child);
			linkaddr_copy(&e->master, &entry.master);
			e->colour = entry.colour;
			list_push(topologyInfo_list, e);
		}
	}

}

// callback to handle application data received from the network
// input: the application packet and its sequence number
void applicationDataCallback(struct APP_PACKET *data)
{
	if (experimentActive == 1)
	{
		struct asn_t receivedAt = current_asn;
		uint16_t latency = ASN_DIFF(receivedAt, data->timeSend);
		PRINTF("[APP]: MSG received: Type: %d, from: 0x%4x with seqNo: %d\n", data->subType,
				data->src.u16, data->seqNo);
		if (data->subType == APP_EXPERIMENT)
		{ // received color packet, set LED color and update performance stats
		  //here save data
			if (isGateway == 0)
			{
				if(data->seqNo >= lastSeqNo)
				{
				PRINTF("[APP]: Data received: Type: %d, from: 0x%4x with seqNo: %d, latency: %d\n",
						data->subType, data->src.u16, data->seqNo, latency * 10);
				tempresult[tempResultCounter] = latency;
				tempResultCounter++;
				rxPackets++;
				PROCESS_CONTEXT_BEGIN(&dewiExperiment)
					;
					etimer_stop(&processResults_timer);
					etimer_set(&processResults_timer,
					CLOCK_SECOND);
					PROCESS_CONTEXT_END(&dewiExperiment);
				}
			}

		}
		else if (data->subType == APP_TOPOLOGYREQUEST)
		{
			if (getCIDERState() == 5 && isGateway == 0)
			{
				PROCESS_CONTEXT_BEGIN(&dewiExperiment)
					;
					etimer_stop(&topologyReply_timer);
					etimer_set(&topologyReply_timer,
					CLOCK_SECOND * getColour() + CLOCK_SECOND * 0.2);
					PROCESS_CONTEXT_END(&dewiExperiment);
			}
		}
		else if (data->subType == APP_TOPOLOGYREPLY)
		{
			if (isGateway == 1)
			{

				uint8_t temp;
				PRINTF("[APP]: number received results: %u\n",data->count);
				for (temp = 0; temp < data->count; temp++)
				{
					PRINTF("TPReply:0x%4x,0x%4x,%d,%d\n", data->src.u16, data->values[temp],
							data->timeslot[0], data->timeslot[1]);
				}
			}
		}
		else if (data->subType == APP_RESULTREQUEST)
		{

			PRINTF("[APP]: APP_RESULTREQUEST received: Type: %d, from: 0x%4x for 0x%4x\n",
					data->subType, data->src.u16, data->dst.u16);
			if (data->dst.u16 == linkaddr_node_addr.u16 && isGateway == 0)
			{
				PROCESS_CONTEXT_BEGIN(&dewiExperiment)
					;
					etimer_stop(&resultReply_timer);
					etimer_set(&resultReply_timer,
					CLOCK_SECOND);
					PROCESS_CONTEXT_END(&dewiExperiment);

			}
		}

		else if (data->subType == APP_RESULTREPLY)
		{
			if (isGateway == 1)
			{

				uint8_t temp;
				PRINTF("[APP]: dataSets received: %d, RxPackets: %d\n", data->remainingData,
						data->count);
				for (temp = 0; temp < data->temperature; temp++)
				{
					PRINTF("RESULTReplyLatency:0x%4x,%d,%d\n", data->src.u16, data->timeslot[temp],
							data->values[temp]);
				}
				if (data->remainingData == 0)
					PRINTF("RESULTReplyRxPackets:0x%4x,%d\n", data->src.u16, data->count);


			}
		}
		else if (data->subType == APP_ACK)
		{
			PRINTF("[APP]: Ack received\n");
			rxPackets = 0;
			clearResults();
		}
	}
	else
	{
		if (linkaddr_cmp(&data->dst, &linkaddr_node_addr) == 1
				|| (linkaddr_cmp(&data->dst, &tsch_broadcast_address) == 1
						&& linkaddr_cmp(&data->src, &linkaddr_node_addr) != 1))
		{
			struct asn_t receivedAt = current_asn;
			uint8_t latency = ASN_DIFF(receivedAt, data->timeSend); // latency in number of time slots
			printf("[APP]: Data received: Type: %d, from: 0x%4x with seqNo: %d, latency: %d\n",
					data->subType, data->src.u16, data->seqNo, latency * 10);

			if (data->subType == APP_COLOR)
			{ // received color packet, set LED color and update performance stats
				uint16_t R, G, B;
				R = data->values[0];
				G = data->values[1];
				B = data->values[2];
				setColor(R, G, B);
				updatePerformanceStats(latency);

			}
			else if (data->subType == APP_BRIGHTNESS)
			{ // received brightness packet, set LED brightness and update performance stats
				uint16_t brightness = data->values[0];
				i2c_single_send(0x39, LED_BRIGHTNESS | brightness);
				if (brightness == 0)
					ledToggle = 0;
				else ledToggle = 1;
				updatePerformanceStats(latency);

			}
			else if ((data->subType == APP_SENSORDATA) && isGateway)
			{ // received sensor data, forward them to serial port if this is the gateway
				printf("node(%04x) Temperature = '%dC' \r\n", data->src.u16, data->temperature);
				sendBatteryStatusByserialP(data->battery, data->src);
				uint8_t i = 0;
				while ((i < 23) && (data->values[i] > 0))
				{
					printf("node(%04x) Stats: Packets = '%d', Latency = '%d'\r\n", data->src.u16,
							data->values[i], data->timeslot[i]);
					i++;
				}

			}
			else if (data->subType == APP_TOPOLOGYREQUEST)
			{ // topology information has been requested, reply with data if master
				if (getCIDERState() == 5)
				{
					PROCESS_CONTEXT_BEGIN(&dewiDemo)
						;
						etimer_stop(&topologyReply_timer);
						etimer_set(&topologyReply_timer,
						CLOCK_SECOND * getColour() + CLOCK_SECOND * 0.2);
						PROCESS_CONTEXT_END(&dewiDemo);
				}
				//handleTopologyRequest();

			}
			else if (data->subType == APP_TOPOLOGYREPLY && waitForTopologyUpdate)
			{ // topology information received, store information
				handleTopologyReply(data);

			}
			else if (data->subType == APP_STATSRESET)
			{ // reset statistics
				list_init(perfStat_list);

			}
		}
	}

}

void packetDeletedFromQueue()
{
	if (experimentActive == 1)
	{
		if (lock == 0)
		{
			PRINTF("[APP]: Packet sent\n");
			txPackets++;
		}
	}
}

void tsch_dewi_callback_joining_network(void)
{
	PRINTF("[APP]: joining network\n");
	setCoord(0);
	initScheduler();
	initNeighbourTable();
	if (experimentActive == 1) clearResults();
	leds_off(LEDS_ALL);
	leds_on(LEDS_GREEN);
}
void tsch_dewi_callback_leaving_network(void)
{
	PRINTF("[APP]: Leaving network\n");
	scheduler_reset();
	neighbourTable_reset();
	CIDER_reset();
	clearResults();
	leds_off(LEDS_ALL);
	leds_on(LEDS_RED);
}
void tsch_dewi_callback_ka(void)
{
	PRINTF("[APP]: Keep Alive sent\n");
	leds_off(LEDS_ALL);
	leds_on(LEDS_YELLOW);
}

void sendACK(linkaddr_t addr)
{
	struct APP_PACKET packet;
	packet.src = linkaddr_node_addr;
	packet.dst = addr;
	packet.subType = APP_ACK;
	lock = 1;
	sendRLLDataMessage(packet, 0);
}

void getResult(uint8_t timeslot, struct resultCounter *res)
{
	struct resultCounter *n;
	n = NULL;
	for (n = list_head(result_list); n != NULL; n = list_item_next(n))
	{

		if (n->timeslot == timeslot)
		{
			break;
		}
	}

	if (n != NULL)
	{
		res->timeslot = n->timeslot;
		res->counter = n->counter;
	}
}
uint8_t addResult(struct resultCounter *res)
{
	struct resultCounter *newRes;
	newRes = NULL;
	uint8_t isNewRes = 1;
	for (newRes = list_head(result_list); newRes != NULL; newRes = list_item_next(newRes))
	{

		/* We break out of the loop if the address of the neighbor matches
		 the address of the neighbor from which we received this
		 broadcast message. */
		if (newRes->timeslot == res->timeslot)
		{

			break;
		}
	}

	if (newRes == NULL)
	{
		newRes = memb_alloc(&result_memb);

		/* If we could not allocate a new neighbor entry, we give up. We
		 could have reused an old neighbor entry, but we do not do this
		 for now. */
		if (newRes == NULL)
		{
			return 0;
		}
	}
	newRes->counter = res->counter;
	newRes->timeslot = res->timeslot;
	list_add(result_list, newRes);
	return isNewRes;
}

uint8_t getAllResults(uint8_t timeslot[], uint16_t values[])
{
	struct resultCounter *newRes;
	uint8_t counter = 0;
	for (newRes = list_head(result_list); newRes != NULL; newRes = list_item_next(newRes))
	{
		PRINTF("getAllResults, timeslot: %d, counter: %d\n", newRes->timeslot, newRes->counter);
		timeslot[counter] = newRes->timeslot;
		values[counter] = newRes->counter;
		counter++;
	}
	return counter;
}

void clearResults()
{
	while (list_head(result_list) != NULL)
	{
		memb_free(&result_memb, list_head(result_list));
		list_remove(result_list, list_head(result_list));
	}
}
PROCESS_THREAD(dewiStart, ev, data)  // main demonstrator process
{
	PROCESS_BEGIN()
		;
		//configure buttons
		button_sensor.configure(BUTTON_SENSOR_CONFIG_TYPE_INTERVAL,
		BUTTON_PRESS_EVENT_INTERVAL);
		//initialize process event timer
		static struct etimer waitTimer;
		static struct etimer LED_toggle_timer;
		serial_line_init();
		etimer_set(&LED_toggle_timer, 0.2 * CLOCK_SECOND);
		etimer_set(&waitTimer, 5 * CLOCK_SECOND); //event timer, every 3 seconds , there is a event

		leds_off(LEDS_ALL);
		while (1)
		{
			PROCESS_YIELD()
			;
			if (ev == sensors_event)
			{ // receiving sensor or button event

				if (data == &button_sensor)
				{ // receiving user button event

					if (button_sensor.value(BUTTON_SENSOR_VALUE_TYPE_LEVEL)
							== BUTTON_SENSOR_PRESSED_LEVEL)
					{
						experimentActive = 0;

					}

				}
			}
			else if (ev == PROCESS_EVENT_TIMER)
			{ // receiving a process event
				if (data == &waitTimer)
				{

					leds_off(LEDS_ALL);
					if (experimentActive == 1)
					{
						leds_on(LEDS_RED);
						PRINTF("[APP]: Start Experiment Application\n");
						process_start(&dewiExperiment, NULL);
					}
					else
					{
						leds_on(LEDS_YELLOW);
						PRINTF("[APP]: Start Demonstrator Application\n");
						process_start(&dewiDemo, NULL);

					}
					process_exit(&dewiStart);
				}
				else if(data == &LED_toggle_timer){
					leds_toggle(LEDS_ALL);
					etimer_set(&LED_toggle_timer, 0.2 * CLOCK_SECOND);
				}

			}
		}
	PROCESS_END();
}

PROCESS_THREAD(dewiExperiment, ev, data)  // main demonstrator process
{
PROCESS_BEGIN()
	;
	initNeighbourTable();
	setCoord(0);
	initScheduler();
	PRINTF("[APP]: Started Experiment APP\n");

	//configure buttons
	button_sensor.configure(BUTTON_SENSOR_CONFIG_TYPE_INTERVAL,
	BUTTON_PRESS_EVENT_INTERVAL);

	//initialize serial line
	serial_line_init();

	//initialize process event timer
	static struct etimer et;
	etimer_set(&et, 3 * CLOCK_SECOND); //event timer, every 3 seconds , there is a event

	//don't wait for topology updates initially

	//initialize lists for child addresses and topology information
	//list_init(result_list);
	//memb_init(&result_mem);

	//initially set isGateway to 0. Will be set to one when messages are received on serial port
	isGateway = 0;
	;

	// main loop
	while (1)
	{
		PROCESS_YIELD()
		;
		if (ev == sensors_event)
		{ // receiving sensor or button event
			handleSensorsEvent(data);
		}
		else if (ev == serial_line_event_message)
		{ // receiving data on serial port
			if (isGateway == 0)
			{
				// node realizes that it must be a gateway
				// set isGateway to 1 and reset statistics collection in the network
				isGateway = 1;

			}
			handleSerialInput(data);
		}
		else if (ev == PROCESS_EVENT_TIMER)
		{ // receiving a process event
			if (data == &button_press_reset)
			{
				button_press_counter = 0;
			}
			else if (data == &processResults_timer)
			{
				struct resultCounter Result;
				int i = 0;
				for (i = 0; i < tempResultCounter; i++)
				{
					PRINTF("[APP]: Timeslot: %d\n", tempresult[i]);
					Result.counter = 0;
					Result.timeslot = 0;
					getResult(tempresult[i], &Result);
					Result.timeslot = tempresult[i];
					Result.counter++;
					PRINTF("[APP]: process results, timeslot: %d, counter: %d\n",
							Result.timeslot, Result.counter);
					addResult(&Result);
					tempresult[0];
				}
				tempResultCounter = 0;
				checkQueueStatus();

			}
			else if (data == &topologyReply_timer)
			{

				PRINTF("[APP]: send TopologyReply\n");
				checkQueueStatus();
				struct APP_PACKET packet;
				packet.src = linkaddr_node_addr;
				packet.dst = tsch_broadcast_address;
				packet.subType = APP_TOPOLOGYREPLY;
				packet.timeslot[0] = getTier();
				packet.timeslot[1] = getColour();
				uint8_t numChildren = 0; // number of children
				linkaddr_t children[CONF_MAX_NEIGHBOURS];
				numChildren = getChildAddresses(children);
				PRINTF("got topology request, have %d children\r\n", numChildren);
				uint8_t numPackets = (uint8_t) ceilf((float) numChildren / (float) 23);

				if (numPackets == 1)
								{
									//send one packet

									int temp = 0;
									for (temp = 0; temp < numChildren; temp++)
									{
										packet.values[temp] = children[temp].u16;
									}
									packet.count = numChildren;
									packet.remainingData = 0;
									//packet.
									lock = 1;
									sendRLLDataMessage(packet, 0);
								}
				else
				{
					//send more than one packet
					int temp = 0, sentPacket = 0, lowerBorder;
					PRINTF("numPackets: %d,numChildren: %d\n", numPackets, numChildren);
					for (sentPacket = 0; sentPacket < numPackets; sentPacket++)
					{
						uint8_t maxBorder = (sentPacket + 1) * 23;
						if (maxBorder > numChildren)
						{
							maxBorder = numChildren;
							packet.remainingData = 0;
						}
						else
						{
							packet.remainingData = 1;

						}
						temp = 0;

						for (lowerBorder = 0 + sentPacket * 23; lowerBorder < maxBorder;
								lowerBorder++)
						{
							packet.values[temp] = children[lowerBorder].u16;
							temp++;
						}

						packet.count = temp;
						lock = 1;
						sendRLLDataMessage(packet, 0);
						clock_delay_usec(50000);
					}

				}

			}
			else if (data == &resultReply_timer)
			{
				checkQueueStatus();
				uint16_t values[MAX_RESULTS_ENTRIES] = { 0 };
				uint8_t timeslot[MAX_RESULTS_ENTRIES] = { 0 };
				uint8_t numResults = getAllResults(timeslot, values);
				struct APP_PACKET packet;
				packet.src = linkaddr_node_addr;
				packet.dst = tsch_broadcast_address;
				packet.subType = APP_RESULTREPLY;
				PRINTF("[APP]: send APP_RESULTREPLY with %d results\n", numResults);
				uint8_t numPackets = (uint8_t) ceilf((float) numResults / (float) 23);
				if (numPackets == 1)
								{
					//send one packet

					int temp = 0;
					packet.remainingData = 0;
					packet.temperature = numResults;
					for (temp = 0; temp < numResults; temp++)
					{
						PRINTF("[APP]: Add Result to packet; timeslot: %d, value: %d\n",
								timeslot[temp], values[temp]);
						packet.values[temp] = values[temp];
						packet.timeslot[temp] = timeslot[temp];
					}

					PRINTF("[APP]: #MSG received %d\n", rxPackets);
					packet.count = rxPackets;
					lock = 1;
					sendRLLDataMessage(packet, 0);
				}
				else
				{
					//send more than one packet
					uint8_t temp = 0, sentPacket = 0, lowerBorder;
					PRINTF("numPackets: %d,numChildren: %d\n", numPackets, numResults);
					for (sentPacket = 0; sentPacket < numPackets; sentPacket++)
					{
						uint8_t maxBorder = (sentPacket + 1) * 23;
						if (maxBorder > numResults)
						{
							maxBorder = numResults;
							packet.remainingData = 0;
						}
						else
						{
							packet.remainingData = 1;

						}
						temp = 0;

						for (lowerBorder = 0 + sentPacket * 23; lowerBorder < maxBorder;
								lowerBorder++)
						{
							PRINTF("[APP]: Add Result to packet; timeslot: %d, value: %d\n",
									timeslot[temp], values[temp]);
							packet.values[temp] = values[temp];
							packet.timeslot[temp] = timeslot[temp];
							temp++;
						}

						packet.temperature = temp;
						packet.count = rxPackets;
						lock = 1;
						sendRLLDataMessage(packet, 0);
						clock_delay_usec(50000);
					}

				}

			}
//			else if (data == &ackTimer)
//			{
//
//				if (ackType == APP_RESULTREPLY)
//				{
//					checkQueueStatus();
//					uint16_t values[MAX_RESULTS_ENTRIES] = { 0 };
//					uint8_t timeslot[MAX_RESULTS_ENTRIES] = { 0 };
//					uint8_t numResults = getAllResults(timeslot, values);
//					struct APP_PACKET packet;
//					packet.src = linkaddr_node_addr;
//					packet.dst = tsch_broadcast_address;
//					packet.subType = APP_RESULTREPLY;
//					PRINTF("[APP]: send APP_RESULTREPLY with %d results\n", numResults);
//
//					uint8_t temp = 0;
//					packet.remainingData = numResults;
//					for (temp = 0; temp < numResults; temp++)
//					{
//						PRINTF("[APP]: Add Result to packet; timeslot: %d, value: %d\n",
//								timeslot[temp], values[temp]);
//						packet.values[temp] = values[temp];
//						packet.timeslot[temp] = timeslot[temp];
//					}
//
//					PRINTF("[APP]: #MSG received %d\n", rxPackets);
//					packet.count = rxPackets;
//					lock = 1;
//					sendRLLDataMessage(packet, 0);
//
//					ackType = APP_RESULTREPLY;
//					etimer_set(&ackTimer, CLOCK_SECOND * 5);
//
//				}
//
//			}
		}
		else if (ev == button_press_duration_exceeded)
		{
			if (button_sensor.value(BUTTON_SENSOR_VALUE_TYPE_PRESS_DURATION) == 10)
			{
				PRINTF("[APP]: Start Node as Coordinator\n");
				leds_off(LEDS_ALL);
				leds_on(LEDS_ALL);
				setCoord(1);
				initScheduler();
				tsch_set_coordinator(1);

			}

		}
	}
PROCESS_END();
}

PROCESS_THREAD(dewiDemo, ev, data)  // main demonstrator process
{
PROCESS_BEGIN()
;
initNeighbourTable();
setCoord(0);
initScheduler();
PRINTF("[APP]: Started Demonstartor APP\n");
radio_result_t rv = NETSTACK_RADIO.set_value(RADIO_PARAM_TXPOWER, 0);
//configure buttons
button_sensor.configure(BUTTON_SENSOR_CONFIG_TYPE_INTERVAL,
BUTTON_PRESS_EVENT_INTERVAL);

//initialize I2C
i2c_init(I2C_SDA_PORT, I2C_SDA_PIN, I2C_SCL_PORT, I2C_SCL_PIN,
I2C_SCL_FAST_BUS_SPEED);
i2c_single_send(0x39, 0b00000000);

//set led brightness to inital brightness
i2c_single_send(0x39, (LED_BRIGHTNESS | lastBRIGHTNESS));

//set LED to white
i2c_single_send(0x39,(LED_RED | 0b11111));
i2c_single_send(0x39,(LED_GREEN | 0b11111));
i2c_single_send(0x39,(LED_BLUE | 0b11111));

//initialize serial line
serial_line_init();

//initialize process event timer
static struct etimer et;
etimer_set(&et, 3 * CLOCK_SECOND); //event timer, every 3 seconds , there is a event

//don't wait for topology updates initially
waitForTopologyUpdate = 0;

//initialize master address
master_addr1.u8[0] = 0;
master_addr1.u8[1] = 0;

//initialize list for topology information
list_init(topologyInfo_list);
memb_init(&topologyInfo_mem);

//initialize countdown for sensor data transmission
sendSensorDataCountdown = 0;

//initially set isGateway to 0. Will be set to one when messages are received on serial port
isGateway = 0;

//initially set ledToggle to 1, meaning the LED is on
ledToggle = 1;

//initialize performance stats
list_init(perfStat_list);
memb_init(&perfStat_mem);

// main loop
while (1)
{
	PROCESS_YIELD()
	;
	if (ev == sensors_event)
	{ // receiving sensor or button event
		printf("received sensors event\n");
		handleSensorsEvent(data);
	}
	else if (ev == serial_line_event_message)
	{ // receiving data on serial port
		printf("received serial event\n");
		if (isGateway == 0)
		{
			// node realizes that it must be a gateway
			// set isGateway to 1 and reset statistics collection in the network
			isGateway = 1;
			struct APP_PACKET temp;
			temp.subType = APP_STATSRESET;
			temp.timeSend = current_asn;
			temp.dst = tsch_broadcast_address;
			temp.src = linkaddr_node_addr;
			temp.seqNo = seqNo++;
			sendRLLDataMessage(temp, 0);
			list_init(perfStat_list);
		}
		handleSerialInput(data);
	}
	else if (ev == PROCESS_EVENT_TIMER)
	{ // receiving a process event
		if (data == &et)
		{
			handleProcessEvent(data);
			etimer_set(&et, 3 * CLOCK_SECOND);
		}
		else if (data == &topologyReply_timer)
		{
			handleTopologyRequest();
		}
		else if (data == &button_press_reset)
		{
			button_press_counter = 0;
		}
	}
	else if (ev == button_press_duration_exceeded)
	{
		if (button_sensor.value(BUTTON_SENSOR_VALUE_TYPE_PRESS_DURATION) == 10)
		{
			printf("[APP]: Start Node as Coordinator\n");
			leds_off(LEDS_ALL);
			leds_on(LEDS_ALL);
			setCoord(1);
			initScheduler();
			tsch_set_coordinator(1);

		}
	}
}
PROCESS_END();
}
