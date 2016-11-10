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
#include "net/DEWI/rll/rll.h"

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


uint8_t lastBRIGHTNESS = 0b00000001;
linkaddr_t master_addr1; //parent or master address
char waitForTopologyUpdate;  //is 1 if topology information is ongoing, else 0
char isGateway; // is 1 if this node is a gateway, else 0

// instead of having different timers, use counters to trigger actions at multiples of process event timer
char masterAdvertisementCountdown; // for recurrent master advertisements
char sendSensorDataCountdown; // for recurrent transmission of sensor data (temperature and battery level)

struct slavesAddr_entry { //structure for each child node that the master or parent node has
	struct slavesAddr_entry *next;
	linkaddr_t addr; //child address
};

struct topologyInfo_entry { //structure for each topology info that is collected by gateway node
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
void setColor(uint16_t R, uint16_t G, uint16_t B){
	i2c_single_send(0x39, LED_RED | R);
	clock_delay_usec(50);
	i2c_single_send(0x39, LED_GREEN | G);
	clock_delay_usec(50);
	i2c_single_send(0x39, LED_BLUE | B);
	clock_delay_usec(50);

}

// convert battery voltage level to battery percentage status, and send over serial port
void sendBatteryStatusByserialP(int b_data, int a, int b) {
	if (b_data >= 3240) {
		printf("node(%d:%d) Battery '100%%' \r\n", a, b);
	} else if (b_data >= 3230 && b_data < 3240) {
		printf("node(%d:%d) Battery '90%%' \r\n", a, b);
	} else if (b_data >= 3200 && b_data < 3230) {
		printf("node(%d:%d) Battery '80%%' \r\n", a, b);
	} else if (b_data >= 3170 && b_data < 3200) {
		printf("node(%d:%d) Battery '70%%' \r\n", a, b);
	} else if (b_data >= 3100 && b_data < 3170) {
		printf("node(%d:%d) Battery '60%%' \r\n", a, b);
	} else if (b_data >= 3050 && b_data < 3100) {
		printf("node(%d:%d) Battery '50%%' \r\n", a, b);
	} else if (b_data >= 2950 && b_data < 3050) {
		printf("node(%d:%d) Battery '40%%' \r\n", a, b);
	} else if (b_data >= 2900 && b_data < 2950) {
		printf("node(%d:%d) Battery '30%%' \r\n", a, b);
	} else if (b_data >= 2800 && b_data < 2900) {
		printf("node(%d:%d) Battery '20%%' \r\n", a, b);
	} else if (b_data >= 2600 && b_data < 2800) {
		printf("node(%d:%d) Battery '10%%' \r\n", a, b);
	} else if (b_data >= 2350 && b_data < 2600) {
		printf("node(%d:%d) Battery '5%%' \r\n", a, b);
	} else if (b_data < 2350) {
		printf("node(%d:%d) Battery '0%%' \r\n", a, b);
	}
}

// function to handle incoming sensor events
// input: the data object associated with the event
void handleSensorsEvent(process_data_t data){
	if (data == &button_sensor) { // receiving user button event

		if (button_sensor.value(BUTTON_SENSOR_VALUE_TYPE_LEVEL)
				== BUTTON_SENSOR_PRESSED_LEVEL) {
			// creating an application packet when the button is pressed
			// currently, this is a color packet resetting the color
			// this packet is broadcasted via RLL
			printf("[APP]: Create App packet\n");
			struct APP_PACKET temp;
			temp.subType = COLOR;
			temp.values[0] = (uint16_t) 0;
			temp.values[1] = (uint16_t) 0;
			temp.values[2] = (uint16_t) 0;
			sendRLLMessage(temp);
			// resetting the color on our own LED
			setColor(0, 0, 0);
		}
	}

}

// function to handle incoming data on serial port
// input: the data object associated with the event
void handleSerialInput(process_data_t data){
	char* ch_data, *ptr;
	ch_data = (char*) data;
	char t1[2];
	long i_data;
	uint32_t R, G, B;

	if (strcmp(ch_data, "topologyrefresh") == 0) { // received topology refresh message
		// re-initialize topology information list
		list_init(topologyInfo_list);
		// poll topology data from network
		struct APP_PACKET temp;
		temp.subType = TOPOLOGYREQUEST;
		sendRLLMessage(temp);

		// set a flag to wait for topology updates
		waitForTopologyUpdate = 1;

	} else if (strstr(ch_data, "BRIGHTNESS") != NULL) { // received brightness value
		lastBRIGHTNESS = 0b00000001
				+ 3 * atoi(strncpy(t1, ch_data + 11, 2));
		if (lastBRIGHTNESS > 0b00011111)
			lastBRIGHTNESS = 0b00000001;
		// set brightness on own LED
		i2c_single_send(0x39, (LED_BRIGHTNESS | lastBRIGHTNESS));

		// broadcast brightness information to other nodes
		struct APP_PACKET temp;
		temp.subType = BRIGHTNESS;
		temp.values[0] = (uint16_t) lastBRIGHTNESS;
		sendRLLMessage(temp);

	} else { // assuming received color value
		i_data = strtol(ch_data, &ptr, 16);	//Convert char data to hex

		R = (int) (i_data & 0x00ff0000) >> 19;  //Select byte 1 and shift to first position
		G = (int) (i_data & 0x0000ff00) >> 11;	//Same for byte 2
		B = (int) (i_data & 0x000000ff) >> 3;	//And byte 3
		// set color on own LED
		setColor(R, G, B);

		// put color in application packet and send it
		struct APP_PACKET temp;
		temp.subType = COLOR;
		temp.values[0] = R;
		temp.values[1] = G;
		temp.values[2] = B;
		sendRLLMessage(temp);

	}
}

// function to handle process events
// input: the data object associated with the event
void handleProcessEvent(data){
	if (waitForTopologyUpdate){ // receiving a process event while waiting for topology updates
		// send collected topology information over the serial port
		struct topologyInfo_entry *e = NULL;
		for(e = list_head(topologyInfo_list); e != NULL; e = e->next) { //check if the entry with this child is already present
			printf("MasterSlave_addr data: '%08x' \r\n",(e->master.u8[0]<<8)|(e->master.u8[1]));
			printf("MasterSlave_addr data: '%08x' \r\n",(e->master.u8[0]<<24)|(e->master.u8[1]<<16)|(e->child.u8[0]<<8)|(e->child.u8[1]));
		}

		// not waiting for topology updates any more
		waitForTopologyUpdate =0;
	}

	if (sendSensorDataCountdown == 0){ // sensor data counter expired
		// obtain temperature and battery values and send them
		int temperature, battery;
		temperature = cc2538_temp_sensor.value(
		CC2538_SENSORS_VALUE_TYPE_CONVERTED) / 1000;
		battery = vdd3_sensor.value(
				CC2538_SENSORS_VALUE_TYPE_CONVERTED);

		struct APP_PACKET temp;
		temp.subType = SENSORDATA;
		temp.values[0] = linkaddr_node_addr.u8[0];
		temp.values[1] = linkaddr_node_addr.u8[1];
		temp.values[2] = (uint16_t) temperature;
		temp.values[3] = (uint16_t) battery;
		sendRLLMessage(temp);
		if (isGateway){
			// if this is the gateway, send data on serial port as well
			printf("node(%d:%d) Temperature = '%dC' \r\n",linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1], temperature);
			sendBatteryStatusByserialP(battery, linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1]);
		}
		sendSensorDataCountdown = 10; // do this every 10th loop
	} else {
		sendSensorDataCountdown--;
	}

#if MASTERNODE
	if (masterAdvertisementCountdown == 0){ // master advertisement counter expired
		// advertise master node status
		// this is for building a dummy topology and isn't needed when topology is created by CIDER
		struct APP_PACKET temp;
		temp.subType = MASTERMSG;
		temp.values[0] = linkaddr_node_addr.u8[0];
		temp.values[1] = linkaddr_node_addr.u8[1];
		sendRLLMessage(temp);
		masterAdvertisementCountdown = 5; // advertise in every 5th loop
	} else {
		masterAdvertisementCountdown--;
	}

#endif

}

// handle an incoming master advertisement if this is not a master
// this is for building a dummy topology and isn't needed when topology is created by CIDER
// input: the incoming packet
void handleMasterMessage(struct APP_PACKET data){
#if !MASTERNODE
	    // store master address
		if (!master_addr1.u8[1] && !master_addr1.u8[0]){
			master_addr1.u8[1] = (uint8_t) data.values[1];
			master_addr1.u8[0] = (uint8_t) data.values[0];
		}

		// create child response to join master
		struct APP_PACKET temp;
		temp.subType = CHILDMSG;
		temp.values[0] = linkaddr_node_addr.u8[0];
		temp.values[1] = linkaddr_node_addr.u8[1];
		temp.values[2] = master_addr1.u8[0];
		temp.values[3] = master_addr1.u8[1];
		sendRLLMessage(temp);
#endif

}

// handle an incoming child message if this is the master
// this is for building a dummy topology and isn't needed when topology is created by CIDER
// input: the incoming packet
void handleChildMessage(struct APP_PACKET data){
#if MASTERNODE
	linkaddr_t childaddress;
	childaddress.u8[0] = data.values[0];
	childaddress.u8[1] = data.values[1];
	struct slavesAddr_entry *e = NULL;
	for(e = list_head(slavesAddr_list); e != NULL; e = e->next) { //check if the child node is or not already connect to this master node
		if(linkaddr_cmp(&e->addr, &childaddress)) {
			return;
		}
	}

	if(e == NULL) { // if not connected yet, add child to the list of child nodes
		e = memb_alloc(&slavesAddr_mem);
		if(e == NULL) {
			e = list_chop(slavesAddr_list); // Remove oldest at full history
		}
		linkaddr_copy(&e->addr, &childaddress);
		list_push(slavesAddr_list, e);
	}

#endif

}

// handle an incoming topology information request
// only master nodes need to respond
void handleTopologyRequest(){
#if MASTERNODE
		int numChildren = list_length(slavesAddr_list); // get current number of children
		int i,j;
		struct slavesAddr_entry * entry = list_head(slavesAddr_list);
		for (i = 0; i <= numChildren/10; i++){ // put information about max. 10 children in packet, send multiple packets if needed
			struct APP_PACKET temp;
			temp.subType = TOPOLOGYREPLY;
			temp.values[0] = (uint16_t) ((i==numChildren/10)?numChildren%10:10);
			temp.values[1] = (uint16_t) linkaddr_node_addr.u8[0];
			temp.values[2] = (uint16_t) linkaddr_node_addr.u8[1];
			for (j = 0; j < ((i==numChildren/10)?numChildren%10:10); j++){
				temp.values[2*j+3] = (uint16_t) entry->addr.u8[0];
				temp.values[2*j+4] = (uint16_t) entry->addr.u8[1];
				entry = entry->next;
			}
			sendRLLMessage(temp);

		}
#endif

}

// handle incoming topology information
// input: the incoming packet
void handleTopologyReply(struct APP_PACKET data){
	int numEntries = data.values[0]; // extract the amount of child addresses in this packet
	int i;
	// go through the contents of the packet
	for (i = 0; i < numEntries; i++){
		struct topologyInfo_entry entry;
		entry.master.u8[0] = data.values[1];
		entry.master.u8[1] = data.values[2];
		entry.child.u8[0] = data.values[2*i+3];
		entry.child.u8[1] = data.values[2*i+4];
		struct topologyInfo_entry *e = NULL;
		for(e = list_head(topologyInfo_list); e != NULL; e = e->next) { //check if the entry with this child is already present
			if(linkaddr_cmp(&e->child, &entry.child)) {
				return;
			}
		}

		if(e == NULL) { // if not present yet, add entry to the list
			e = memb_alloc(&topologyInfo_mem);
			if(e == NULL) {
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
	    // initialize RLL
	    rllInit();

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

		// main loop
		while (1) {
			PROCESS_YIELD()
			;
			if (ev == sensors_event) { // receiving sensor or button event
				handleSensorsEvent(data);
			} else if (ev == serial_line_event_message){ // receiving data on serial port
				isGateway = 1;
				handleSerialInput(data);
			} else if (ev == PROCESS_EVENT_TIMER){ // receiving a process event
				handleProcessEvent(data);
				etimer_set(&et, 3 * CLOCK_SECOND);
			}
		}
	PROCESS_END();
}

// callback to handle application data received from the network
// input: the application packet and its sequence number
void applicationDataCallback(struct APP_PACKET data, uint8_t seq) {
	printf("[APP]: Data received: Type: %d, SeqNo: %d\n", data.subType, seq);

	if (data.subType == COLOR){ // received color packet, set LED color
		uint16_t R, G, B;
		R = data.values[0];
		G = data.values[1];
		B = data.values[2];
		setColor(R, G, B);

	} else if (data.subType == BRIGHTNESS){ // received brightness packet, set LED brightness
		uint16_t brightness = data.values[0];
		i2c_single_send(0x39, LED_BRIGHTNESS | brightness);

	} else if ((data.subType == SENSORDATA) && isGateway){ // received sensor data, forward them to serial port if this is the gateway
		printf("node(%d:%d) Temperature = '%dC' \r\n",data.values[0], data.values[1], data.values[2]);
		sendBatteryStatusByserialP(data.values[3], data.values[0], data.values[1]);

	} else if (data.subType == TOPOLOGYREQUEST){ // topology information has been requested, reply with data if master
		handleTopologyRequest();

	} else if (data.subType == TOPOLOGYREPLY && waitForTopologyUpdate) { // topology information received, store information
		handleTopologyReply(data);

	} else if (data.subType == MASTERMSG){ // received master advertisement, respond to master node
		// this is for building a dummy topology and isn't needed when topology is created by CIDER
		handleMasterMessage(data);

	} else if (data.subType == CHILDMSG){ // received child response
		// this is for building a dummy topology and isn't needed when topology is created by CIDER
		handleChildMessage(data);

	}
}
