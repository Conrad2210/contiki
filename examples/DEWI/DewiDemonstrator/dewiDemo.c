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

#define NUM_ADDR_ENTRIES 100		//number of child nodes that a master node can have

static struct etimer button_press_reset;
uint8_t button_press_counter = 0;
uint8_t lastBRIGHTNESS = 0b00000001;
uint16_t seqNo = 0;
linkaddr_t master_addr1; //parent or master address
char waitForTopologyUpdate;  //is 1 if topology information is ongoing, else 0
char isGateway; // is 1 if this node is a gateway, else 0

// instead of having different timers, use counters to trigger actions at multiples of process event timer
char masterAdvertisementCountdown; // for recurrent master advertisements
char sendSensorDataCountdown; // for recurrent transmission of sensor data (temperature and battery level)

struct performanceStat
{ //structure for performance statistics
	uint16_t cumulativeLatency; // just add up the latencies (no need to calculate average values at the node)
	uint16_t packetCounter;     // counting all incoming packets
	uint16_t lastSeq;      // last received sequence number, to be used as a timestamp for the stats
} stat;

struct slavesAddr_entry
{ //structure for each child node that the master or parent node has
	struct slavesAddr_entry *next;
	linkaddr_t addr; //child address
};

struct topologyInfo_entry
{ //structure for each topology info that is collected by gateway node
	struct topologyInfo_entry *next;
	linkaddr_t master, child;
};

LIST(slavesAddr_list); //list of child nodes
MEMB(slavesAddr_mem, struct slavesAddr_entry, NUM_ADDR_ENTRIES);

LIST(topologyInfo_list); //list of topology information
MEMB(topologyInfo_mem, struct topologyInfo_entry, NUM_ADDR_ENTRIES);

PROCESS(dewiDemo, "DEWI Demonstrator, using CIDER and RLL");	//main process
AUTOSTART_PROCESSES(&dewiDemo);

// set the LED color
void setColor(uint16_t R, uint16_t G, uint16_t B)
{
	uint8_t returnvalue;
	i2c_single_send(0x39, LED_RED | R);
	clock_delay_usec(50);
	i2c_single_send(0x39, LED_GREEN | G);
	clock_delay_usec(50);
	returnvalue = i2c_single_send(0x39, LED_BLUE | B);
	//printf("i2c return code is %d\r\n", returnvalue);
	clock_delay_usec(50);

}

// update performance statistics with a new sequence number and a new latency
void updatePerformanceStats(uint16_t seq, uint16_t latency)
{
	stat.cumulativeLatency += latency;
	stat.lastSeq = seq;
	stat.packetCounter++;
}

// convert battery voltage level to battery percentage status, and send over serial port
void sendBatteryStatusByserialP(int b_data, int a, int b)
{
	if (b_data >= 3240)
	{
		printf("node(%d:%d) Battery '100%%' \r\n", a, b);
	}
	else if (b_data >= 3230 && b_data < 3240)
	{
		printf("node(%d:%d) Battery '90%%' \r\n", a, b);
	}
	else if (b_data >= 3200 && b_data < 3230)
	{
		printf("node(%d:%d) Battery '80%%' \r\n", a, b);
	}
	else if (b_data >= 3170 && b_data < 3200)
	{
		printf("node(%d:%d) Battery '70%%' \r\n", a, b);
	}
	else if (b_data >= 3100 && b_data < 3170)
	{
		printf("node(%d:%d) Battery '60%%' \r\n", a, b);
	}
	else if (b_data >= 3050 && b_data < 3100)
	{
		printf("node(%d:%d) Battery '50%%' \r\n", a, b);
	}
	else if (b_data >= 2950 && b_data < 3050)
	{
		printf("node(%d:%d) Battery '40%%' \r\n", a, b);
	}
	else if (b_data >= 2900 && b_data < 2950)
	{
		printf("node(%d:%d) Battery '30%%' \r\n", a, b);
	}
	else if (b_data >= 2800 && b_data < 2900)
	{
		printf("node(%d:%d) Battery '20%%' \r\n", a, b);
	}
	else if (b_data >= 2600 && b_data < 2800)
	{
		printf("node(%d:%d) Battery '10%%' \r\n", a, b);
	}
	else if (b_data >= 2350 && b_data < 2600)
	{
		printf("node(%d:%d) Battery '5%%' \r\n", a, b);
	}
	else if (b_data < 2350)
	{
		printf("node(%d:%d) Battery '0%%' \r\n", a, b);
	}
}

// function to handle incoming sensor events
// input: the data object associated with the event
void handleSensorsEvent(process_data_t data)
{
	PROCESS_CONTEXT_BEGIN(&dewiDemo)
	;
	if (data == &button_sensor)
	{ // receiving user button event

		if (button_sensor.value(BUTTON_SENSOR_VALUE_TYPE_LEVEL) == BUTTON_SENSOR_PRESSED_LEVEL)
		{

			button_press_counter = button_press_counter + 1;
			etimer_set(&button_press_reset, CLOCK_SECOND * 0.5);
			uint16_t temp = random_rand() & 0b00011111;
			printf("I'm a MASTERNODE %d\n",random_rand());
			printf("Red: 0b%b\n",temp);
			i2c_single_send(0x39, (LED_RED | temp));
			temp = random_rand() & 0b00011111;
			printf("Red: 0b%b\n",( temp));
			i2c_single_send(0x39, (LED_BLUE | temp));
			temp = random_rand() & 0b00011111;
			printf("Red: 0b%b\n",( temp));
			i2c_single_send(0x39, (LED_GREEN | temp));

		}
		if (button_press_counter == 10)
		{
			CIDER_start();
		}
	}
	PROCESS_CONTEXT_END(&dewiDemo);

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

	if (strcmp(ch_data, "topologyrefresh") == 0)
	{ // received topology refresh message
		// re-initialize topology information list
		list_init(topologyInfo_list);
		// poll topology data from network
		struct APP_PACKET temp;
		temp.subType = APP_TOPOLOGYREQUEST;
		sendRLLDataMessage(temp);

		// set a flag to wait for topology updates
		waitForTopologyUpdate = 1;

	}
	else if (strcmp(ch_data, "resetstatistics") == 0)
	{ // received statistics reset message
		// broadcast statistics reset to network
		struct APP_PACKET temp;
		temp.subType = APP_STATSRESET;
		sendRLLDataMessage(temp);
		// reset own statistics
		stat.cumulativeLatency = 0;
		stat.packetCounter = 0;
		stat.lastSeq = 0;

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
		sendRLLDataMessage(temp);
		updatePerformanceStats(0, 0);

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
		sendRLLDataMessage(temp);
		updatePerformanceStats(0, 0);

	}
}

// function to handle process events
// input: the data object associated with the event
void handleProcessEvent( data)
{
	if (waitForTopologyUpdate)
	{ // receiving a process event while waiting for topology updates
		// send collected topology information over the serial port
		struct topologyInfo_entry *e = NULL;
		for (e = list_head(topologyInfo_list); e != NULL; e = e->next)
		{ //check if the entry with this child is already present
			printf("MasterSlave_addr data: '%08x' \r\n",
					(e->master.u8[0] << 8) | (e->master.u8[1]));
			printf("MasterSlave_addr data: '%08x' \r\n",
					(e->master.u8[0] << 24) | (e->master.u8[1] << 16) | (e->child.u8[0] << 8) | (e->child.u8[1]));
		}

		// not waiting for topology updates any more
		waitForTopologyUpdate = 0;
	}

	if (sendSensorDataCountdown == 0)
	{ // sensor data counter expired
		// obtain temperature and battery values and send them
		int temperature, battery;
		temperature = cc2538_temp_sensor.value(
		CC2538_SENSORS_VALUE_TYPE_CONVERTED) / 1000;
		battery = vdd3_sensor.value(
		CC2538_SENSORS_VALUE_TYPE_CONVERTED);

		struct APP_PACKET temp;
		temp.subType = APP_SENSORDATA;
		temp.values[0] = linkaddr_node_addr.u8[0];
		temp.values[1] = linkaddr_node_addr.u8[1];
		temp.values[2] = (uint16_t) temperature;
		temp.values[3] = (uint16_t) battery;
		// add performance stats to packet
		temp.values[4] = stat.cumulativeLatency * 1000 / stat.packetCounter; // average latency, multiplied by 1000 to avoid floating point
		temp.values[5] = stat.packetCounter;
		temp.values[6] = stat.lastSeq;
		sendRLLDataMessage(temp);
		if (isGateway)
		{
			// if this is the gateway, send data on serial port as well
			printf("node(%d:%d) Temperature = '%dC' \r\n", linkaddr_node_addr.u8[0],
					linkaddr_node_addr.u8[1], temperature);
			sendBatteryStatusByserialP(battery, linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1]);
			printf("node(%d:%d) Statistics: Packets = '%d', PLR = '%d', Latency = '%d'\r\n",
					linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1], stat.packetCounter, 0,
					(stat.cumulativeLatency * 1000 / stat.packetCounter));
		}
		sendSensorDataCountdown = 10; // do this every 10th loop
	}
	else
	{
		sendSensorDataCountdown--;
	}

#if MASTERNODE
	if (masterAdvertisementCountdown == 0)
	{ // master advertisement counter expired
		// advertise master node status
		// this is for building a dummy topology and isn't needed when topology is created by CIDER
		struct APP_PACKET temp;
		temp.subType = APP_MASTERMSG;
		temp.values[0] = linkaddr_node_addr.u8[0];
		temp.values[1] = linkaddr_node_addr.u8[1];
		sendRLLDataMessage(temp);
		masterAdvertisementCountdown = 5;// advertise in every 5th loop
	}
	else
	{
		masterAdvertisementCountdown--;
	}

#endif

}

// handle an incoming master advertisement if this is not a master
// this is for building a dummy topology and isn't needed when topology is created by CIDER
// input: the incoming packet
void handleMasterMessage(struct APP_PACKET *data)
{
#if !MASTERNODE
	// store master address
	if (!master_addr1.u8[1] && !master_addr1.u8[0])
	{
		master_addr1.u8[1] = (uint8_t) data->values[1];
		master_addr1.u8[0] = (uint8_t) data->values[0];
	}

	// create child response to join master
	struct APP_PACKET temp;
	temp.subType = APP_CHILDMSG;
	temp.values[0] = linkaddr_node_addr.u8[0];
	temp.values[1] = linkaddr_node_addr.u8[1];
	temp.values[2] = master_addr1.u8[0];
	temp.values[3] = master_addr1.u8[1];
	sendRLLDataMessage(temp);
#endif

}

// handle an incoming child message if this is the master
// this is for building a dummy topology and isn't needed when topology is created by CIDER
// input: the incoming packet
void handleChildMessage(struct APP_PACKET *data)
{
#if MASTERNODE
	linkaddr_t childaddress;
	childaddress.u8[0] = data->values[0];
	childaddress.u8[1] = data->values[1];
	printf("got child message from %d:%d\r\n", data->values[0], data->values[1]);
	struct slavesAddr_entry *e = NULL;
	for(e = list_head(slavesAddr_list); e != NULL; e = e->next)
	{ //check if the child node is or not already connect to this master node
		if(linkaddr_cmp(&e->addr, &childaddress))
		{
			return;
		}
	}

	if(e == NULL)
	{ // if not connected yet, add child to the list of child nodes
		e = memb_alloc(&slavesAddr_mem);
		if(e == NULL)
		{
			e = list_chop(slavesAddr_list); // Remove oldest at full history
		}
		linkaddr_copy(&e->addr, &childaddress);
		list_push(slavesAddr_list, e);
	}

#endif

}

// handle an incoming topology information request
// only master nodes need to respond
void handleTopologyRequest()
{
#if MASTERNODE
	int numChildren = list_length(slavesAddr_list); // get current number of children
	printf("got topology request, have %d children\r\n", numChildren);
	int i,j;
	struct slavesAddr_entry * entry = list_head(slavesAddr_list);
	for (i = 0; i <= numChildren/10; i++)
	{ // put information about max. 10 children in packet, send multiple packets if needed
		struct APP_PACKET temp;
		temp.subType = APP_TOPOLOGYREPLY;
		temp.values[0] = (uint16_t) ((i==numChildren/10)?numChildren%10:10);
		temp.values[1] = (uint16_t) linkaddr_node_addr.u8[0];
		temp.values[2] = (uint16_t) linkaddr_node_addr.u8[1];
		for (j = 0; j < ((i==numChildren/10)?numChildren%10:10); j++)
		{
			temp.values[2*j+3] = (uint16_t) entry->addr.u8[0];
			temp.values[2*j+4] = (uint16_t) entry->addr.u8[1];
			entry = entry->next;
		}
		sendRLLDataMessage(temp);

	}
#endif

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
		entry.master.u8[0] = data->values[1];
		entry.master.u8[1] = data->values[2];
		entry.child.u8[0] = data->values[2 * i + 3];
		entry.child.u8[1] = data->values[2 * i + 4];
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

PROCESS_THREAD(dewiDemo, ev, data)  // main demonstrator process
{
	PROCESS_BEGIN()
		;
		initNeighbourTable();
		setCoord(0);
		initScheduler();

		radio_result_t rv = NETSTACK_RADIO.set_value(RADIO_PARAM_TXPOWER, -15);
		//configure buttons
		button_sensor.configure(BUTTON_SENSOR_CONFIG_TYPE_INTERVAL,
		BUTTON_PRESS_EVENT_INTERVAL);

		//initialize I2C
		i2c_init(I2C_SDA_PORT, I2C_SDA_PIN, I2C_SCL_PORT, I2C_SCL_PIN,
		I2C_SCL_FAST_BUS_SPEED);

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

		//initialize lists for child addresses and topology information
		list_init(slavesAddr_list);
		memb_init(&slavesAddr_mem);
		list_init(topologyInfo_list);
		memb_init(&topologyInfo_mem);

		//initialize countdowns for master advertisements and sensor data transmission
		masterAdvertisementCountdown = 0;
		sendSensorDataCountdown = 0;

		//initially set isGateway to 0. Will be set to one when messages are received on serial port
		isGateway = 0;

		//initialize performance stats
		stat.cumulativeLatency = 0;
		stat.packetCounter = 0;
		stat.lastSeq = 0;

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
					sendRLLDataMessage(temp);
					stat.cumulativeLatency = 0;
					stat.packetCounter = 0;
					stat.lastSeq = 0;
				}
				handleSerialInput(data);
			}
			else if (ev == PROCESS_EVENT_TIMER)
			{ // receiving a process event
				if (data == &et)
				{
					handleProcessEvent(data);
					//etimer_set(&et, 3 * CLOCK_SECOND);
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

// callback to handle application data received from the network
// input: the application packet and its sequence number
void applicationDataCallback(struct APP_PACKET *data)
{
if (linkaddr_cmp(&data->dst, &linkaddr_node_addr) == 1 || linkaddr_cmp(&data->dst,
		&tsch_broadcast_address) == 1)
{
	struct asn_t receivedAt = current_asn;
	uint16_t latency = ASN_DIFF(receivedAt,data->timeSend) * 10;
	printf("[APP]: Data received: Type: %d, from: 0x%4x with seqNo: %d, latency: %d\n",
			data->subType, data->src.u16, data->seqNo, latency);

	if (data->subType == APP_COLOR)
	{ // received color packet, set LED color and update performance stats
		uint16_t R, G, B;
		R = data->values[0];
		G = data->values[1];
		B = data->values[2];
		setColor(R, G, B);
		updatePerformanceStats(0, latency);

	}
	else if (data->subType == APP_BRIGHTNESS)
	{ // received brightness packet, set LED brightness and update performance stats
		uint16_t brightness = data->values[0];
		i2c_single_send(0x39, LED_BRIGHTNESS | brightness);
		updatePerformanceStats(0, latency);

	}
	else if ((data->subType == APP_SENSORDATA) && isGateway)
	{ // received sensor data, forward them to serial port if this is the gateway
		printf("node(%d:%d) Temperature = '%dC' \r\n", data->values[0], data->values[1],
				data->values[2]);
		sendBatteryStatusByserialP(data->values[3], data->values[0], data->values[1]);
		printf("node(%d:%d) Statistics: Packets = '%d', PLR = '%d', Latency = '%d'\r\n",
				data->values[0], data->values[1], data->values[5],
				(1000 - (data->values[5] * 1000 / stat.packetCounter)), data->values[4]);

	}
	else if (data->subType == APP_TOPOLOGYREQUEST)
	{ // topology information has been requested, reply with data if master
		handleTopologyRequest();

	}
	else if (data->subType == APP_TOPOLOGYREPLY && waitForTopologyUpdate)
	{ // topology information received, store information
		handleTopologyReply(data);

	}
	else if (data->subType == APP_STATSRESET)
	{ // reset statistics
		stat.cumulativeLatency = 0;
		stat.packetCounter = 0;
		stat.lastSeq = 0;

	}
	else if (data->subType == APP_MASTERMSG)
	{ // received master advertisement, respond to master node
		// this is for building a dummy topology and isn't needed when topology is created by CIDER
		handleMasterMessage(data);

	}
	else if (data->subType == APP_CHILDMSG)
	{ // received child response
		// this is for building a dummy topology and isn't needed when topology is created by CIDER
		handleChildMessage(data);

	}
}
}

void tsch_dewi_callback_joining_network(void)
{
printf("[APP]: joining network\n");
setCoord(0);
initScheduler();
initNeighbourTable();
leds_off(LEDS_ALL);
leds_on(LEDS_GREEN);
}
void tsch_dewi_callback_leaving_network(void)
{
printf("[APP]: Leaving network\n");
scheduler_reset();
neighbourTable_reset();
CIDER_reset();
leds_off(LEDS_ALL);
leds_on(LEDS_RED);
}
void tsch_dewi_callback_ka(void)
{
printf("[APP]: Keep Alive sent\n");
leds_off(LEDS_ALL);
leds_on(LEDS_YELLOW);
}
