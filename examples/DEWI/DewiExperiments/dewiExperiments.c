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
uint16_t seqNo = 1;
uint16_t txPackets = 0;
uint16_t rxPackets = 0;
linkaddr_t master_addr1; //parent or master address
char waitForTopologyUpdate;  //is 1 if topology information is ongoing, else 0
char isGateway; // is 1 if this node is a gateway, else 0
struct resultCounter { //structure for each child node that the master or parent node has
	uint8_t timeslot;
	uint16_t counter;
};

uint8_t tempresult[50] = { 0 };
uint8_t tempResultCounter = 0;
const struct etimer processResults_timer;
uint8_t addResult(struct resultCounter *res);
void getResult(uint8_t timeslot, struct resultCounter *res);
LIST(result_list); //list of child nodes
MEMB(result_mem, struct resultCounter, 10);

PROCESS(dewiDemo, "DEWI Demonstrator, using CIDER and RLL");	//main process
AUTOSTART_PROCESSES(&dewiDemo);

// function to handle incoming sensor events
// input: the data object associated with the event
void handleSensorsEvent(process_data_t data) {
	PROCESS_CONTEXT_BEGIN(&dewiDemo)
		;
		if (data == &button_sensor) { // receiving user button event

			if (button_sensor.value(BUTTON_SENSOR_VALUE_TYPE_LEVEL)
					== BUTTON_SENSOR_PRESSED_LEVEL) {

				button_press_counter = button_press_counter + 1;
				etimer_set(&button_press_reset, CLOCK_SECOND * 0.2);

			}
			if (button_press_counter == 10) {
				CIDER_start();
			}
		}
		PROCESS_CONTEXT_END(&dewiDemo);

}

// function to handle incoming data on serial port
// input: the data object associated with the event
void handleSerialInput(process_data_t data) {
	char* ch_data, *ptr;
	ch_data = (char*) data;
	char t1[2];
	long i_data;
	uint32_t R, G, B;
	if (strstr(ch_data, "Experiment") != NULL) {
		tsch_queue_reset();
		printf("[APP]: Experiment received, send message\n");
		struct APP_PACKET packet;
		packet.src = linkaddr_node_addr;
		packet.dst = tsch_broadcast_address;
		packet.timeSend = current_asn;
		packet.seqNo = seqNo++;
		packet.subType = APP_EXPERIMENT;

		txPackets++;
		sendRLLDataMessage(packet);

	}

}

// function to handle process events
// input: the data object associated with the event
void handleProcessEvent( data) {

}

// handle an incoming topology information request
// only master nodes need to respond
void handleTopologyRequest() {

}

// handle incoming topology information
// input: the incoming packet
void handleTopologyReply(struct APP_PACKET *data) {

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

		//initialize serial line
		serial_line_init();

		//initialize process event timer
		static struct etimer et;
		etimer_set(&et, 3 * CLOCK_SECOND); //event timer, every 3 seconds , there is a event

		//don't wait for topology updates initially

		//initialize lists for child addresses and topology information
		list_init(result_list);
		memb_init(&result_mem);

		//initially set isGateway to 0. Will be set to one when messages are received on serial port
		isGateway = 0;
		;

		// main loop
		while (1) {
			PROCESS_YIELD()
			;
			if (ev == sensors_event) { // receiving sensor or button event
				printf("received sensors event\n");
				handleSensorsEvent(data);
			} else if (ev == serial_line_event_message) { // receiving data on serial port
				printf("received serial event\n");
				if (isGateway == 0) {
					// node realizes that it must be a gateway
					// set isGateway to 1 and reset statistics collection in the network
					isGateway = 1;

				}
				handleSerialInput(data);
			} else if (ev == PROCESS_EVENT_TIMER) { // receiving a process event
				if (data == &button_press_reset) {
					button_press_counter = 0;
				} else if (data == &processResults_timer) {
					printf("[APP]: process results\n");
					struct resultCounter Result;
					int i = 0;
					for (i = 0; i < tempResultCounter; i++) {
						printf("[APP]: Timeslot: %d\n", tempresult[i]);
						Result.counter = 0;
						Result.timeslot = 0;
						getResult(tempresult[i], &Result);
						if (Result.timeslot == 0)
							Result.timeslot = tempresult[i];
						Result.counter++;

						addResult(&Result);
						tempresult[0];
					}
					tempResultCounter = 0;

				}
			} else if (ev == button_press_duration_exceeded) {
				if (button_sensor.value(BUTTON_SENSOR_VALUE_TYPE_PRESS_DURATION)
						== 10) {
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
void applicationDataCallback(struct APP_PACKET *data) {
if (linkaddr_cmp(&data->dst, &linkaddr_node_addr) == 1
		|| linkaddr_cmp(&data->dst, &tsch_broadcast_address) == 1) {
	struct asn_t receivedAt = current_asn;
	uint16_t latency = ASN_DIFF(receivedAt, data->timeSend);
	printf(
			"[APP]: Data received: Type: %d, from: 0x%4x with seqNo: %d, latency: %d\n",
			data->subType, data->src.u16, data->seqNo, latency * 10);

	if (data->subType == APP_EXPERIMENT) { // received color packet, set LED color and update performance stats
										   //here save data
		if (isGateway == 0) {
			tempresult[tempResultCounter] = latency;
			tempResultCounter++;
			txPackets++;
			PROCESS_CONTEXT_BEGIN(&dewiDemo)
			;
			etimer_stop(&processResults_timer);
			etimer_set(&processResults_timer, CLOCK_SECOND * 1);
			PROCESS_CONTEXT_END(&dewiDemo);
		}

	} else if (data->subType == APP_QUERYRESULT) {
		printf("[APP]: return results\n");
	}
}
}

void packetDeletedFromQueue() {
printf("[APP]: Packet deleted from Queue\n");
txPackets--;
}

void tsch_dewi_callback_joining_network(void) {
printf("[APP]: joining network\n");
setCoord(0);
initScheduler();
initNeighbourTable();
leds_off(LEDS_ALL);
leds_on(LEDS_GREEN);
}
void tsch_dewi_callback_leaving_network(void) {
printf("[APP]: Leaving network\n");
scheduler_reset();
neighbourTable_reset();
CIDER_reset();
leds_off(LEDS_ALL);
leds_on(LEDS_RED);
}
void tsch_dewi_callback_ka(void) {
printf("[APP]: Keep Alive sent\n");
leds_off(LEDS_ALL);
leds_on(LEDS_YELLOW);
}

void getResult(uint8_t timeslot, struct resultCounter *res) {
struct resultCounter *n;
n = NULL;
for (n = list_head(result_list); n != NULL; n = list_item_next(n)) {
	/* We break out of the loop if the address of the neighbor matches
	 the address of the neighbor from which we received this
	 broadcast message. */
	if (n->timeslot == timeslot) {
		break;
	}
}

if (n == NULL)
	res = NULL;
else
	res = n;

}
uint8_t addResult(struct resultCounter *res) {
struct resultCounter *newRes;
uint8_t isNewRes = 1;
for (newRes = list_head(result_list); newRes != NULL;
		newRes = list_item_next(newRes)) {

	/* We break out of the loop if the address of the neighbor matches
	 the address of the neighbor from which we received this
	 broadcast message. */
	if (newRes->timeslot == res->timeslot) {
		break;
	}
}

if (newRes == NULL) {
	newRes = memb_alloc(&result_mem);

	/* If we could not allocate a new neighbor entry, we give up. We
	 could have reused an old neighbor entry, but we do not do this
	 for now. */
	if (newRes == NULL) {
		return 0;
	}
}
newRes->counter = res->counter;
newRes->timeslot = res->timeslot;
list_add(result_list, newRes);
return isNewRes;
}
