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
char waitForTopologyUpdate;  //is >0 if topology information collection is ongoing, else 0
char isGateway; // is 1 if this node is a gateway, else 0
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

PROCESS(dewiDemo, "DEWI Demonstrator, using CIDER and RLL");	//main process
AUTOSTART_PROCESSES(&dewiDemo);

// set the LED color
void setColor(uint16_t R, uint16_t G, uint16_t B)
{
	i2c_single_send(0x39, LED_RED | R);
	clock_delay_usec(50);
	i2c_single_send(0x39, LED_GREEN | G);
	clock_delay_usec(50);
	i2c_single_send(0x39, LED_BLUE | B);
	clock_delay_usec(50);

}

// update performance statistics with new latency data from the latest packet
void updatePerformanceStats(uint8_t latency)
{
	struct performanceStatEntry *e = NULL;
	for (e = list_head(perfStat_list); e != NULL; e = e->next)
	{ //check if the entry with this latency is already present and update it
		if (e->latency == latency){
			e->packetCounter++;
			break;
		}
	}
	if (e == NULL){
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
	PROCESS_CONTEXT_BEGIN(&dewiDemo)
	;
	if (data == &button_sensor)
	{ // receiving user button event

		if (button_sensor.value(BUTTON_SENSOR_VALUE_TYPE_LEVEL) == BUTTON_SENSOR_PRESSED_LEVEL)
		{

			button_press_counter = button_press_counter + 1;
			etimer_set(&button_press_reset, CLOCK_SECOND * 0.5);

			if (getActiveProtocol()==2)
			{	// button pressed while node is in normal RLL operation, use button to toggle LEDs on/off
				uint8_t brightness = lastBRIGHTNESS;
				if (ledToggle==1){
					// switch off
					brightness = 0;
					ledToggle = 0;
				}else{
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
				sendRLLDataMessage(temp);
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
		temp.timeSend = current_asn;
		temp.dst = tsch_broadcast_address;
		temp.src = linkaddr_node_addr;
		temp.seqNo = seqNo++;
		sendRLLDataMessage(temp);

		// set a flag to wait for topology updates
		waitForTopologyUpdate = 2;

		// if I am a CH myself, put my own children in topologylist
		if (getCIDERState()==5){
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
		sendRLLDataMessage(temp);
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
		sendRLLDataMessage(temp);
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
		sendRLLDataMessage(temp);
		updatePerformanceStats(0);

	}
}

// function to handle process events
// input: the data object associated with the event
void handleProcessEvent( data)
{
	if (waitForTopologyUpdate == 1)
	{ // receiving a process event while waiting for topology updates
		// send collected topology information over the serial port
		struct topologyInfo_entry *e = NULL;
		for (e = list_head(topologyInfo_list); e != NULL; e = e->next)
		{ //check if the entry with this child is already present
			printf("MasterSlave_addr data: '%08x' \r\n",
					(e->master.u16));
			printf("MasterSlave_addr data: '%08x' \r\n",
					(e->master.u16 << 16) | (e->child.u16));
			printf("Master (%04x) colour: '%d'\r\n", e->master.u16, e->colour);
		}
		if (list_length(topologyInfo_list)==0){
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
			sendRLLDataMessage(temp);

			// set a flag to wait for topology updates
			waitForTopologyUpdate = 2;
		} else {
			// not waiting for topology updates any more
			waitForTopologyUpdate = 0;
		}
	} else if (waitForTopologyUpdate > 0){
		waitForTopologyUpdate--;
	}

	if ((sendSensorDataCountdown <= 0) &&(getActiveProtocol() == 2))
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
			if (i==22){
				// packet full, send it and create a new one
				temp.remainingData = 1;
				sendRLLDataMessage(temp);
				i = 0;
				temp.seqNo = seqNo++;

			} else {
				temp.remainingData = 0;
				i++;
			}

			//if this is the gateway, send the performance data on the serial port
			if (isGateway){
				printf("node(%04x) Stats: Packets = '%d', Latency = '%d'\r\n",
						linkaddr_node_addr.u16, e->packetCounter, e->latency);
			}
		}
		while (i < 23){
			// fill remaining temp.values with 0 so that we don't accidentially parse undefined values on the other end
			temp.values[i] = 0;
			i++;
		}
		sendRLLDataMessage(temp);

		if (isGateway)
		{
			// if this is the gateway, send data on serial port as well
			printf("node(%04x) Temperature = '%dC' \r\n", linkaddr_node_addr.u16, temperature);
			sendBatteryStatusByserialP(battery, linkaddr_node_addr);
		}
		sendSensorDataCountdown = 5 + linkaddr_node_addr.u16%10; // do this every xth loop, x being between 5 and 14
	}
	else
	{
		sendSensorDataCountdown--;
	}


}

// handle an incoming topology information request
// only cluster heads need to respond
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
			sendRLLDataMessage(temp);
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

PROCESS_THREAD(dewiDemo, ev, data)  // main demonstrator process
{
	PROCESS_BEGIN()
		;
		initNeighbourTable();
		setCoord(0);
		initScheduler();

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
					sendRLLDataMessage(temp);
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
if (linkaddr_cmp(&data->dst, &linkaddr_node_addr) == 1 || (linkaddr_cmp(&data->dst,
		&tsch_broadcast_address) == 1 && linkaddr_cmp(&data->src, &linkaddr_node_addr) != 1))
{
	struct asn_t receivedAt = current_asn;
	uint8_t latency = ASN_DIFF(receivedAt,data->timeSend); // latency in number of time slots
	printf("[APP]: Data received: Type: %d, from: 0x%4x with seqNo: %d, latency: %d\n",
			data->subType, data->src.u16, data->seqNo, latency*10);

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
		if (brightness==0)
			ledToggle=0;
		else
			ledToggle=1;
		updatePerformanceStats(latency);

	}
	else if ((data->subType == APP_SENSORDATA) && isGateway)
	{ // received sensor data, forward them to serial port if this is the gateway
		printf("node(%04x) Temperature = '%dC' \r\n", data->src.u16, data->temperature);
		sendBatteryStatusByserialP(data->battery, data->src);
		uint8_t i = 0;
		while ((i < 23) && (data->values[i] > 0)){
			printf("node(%04x) Stats: Packets = '%d', Latency = '%d'\r\n",
					data->src.u16, data->values[i], data->timeslot[i]);
			i++;
		}

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
		list_init(perfStat_list);

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
