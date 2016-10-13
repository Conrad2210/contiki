/*
 * demonstrator-coordinator.c
 *
 *  Created on: Aug 10, 2016
 *      Author: user
 */

#include "demonstrator.h"

/*---------------------------------------------------------------------------*/

#define BYTETOBINARYPATTERN "%d%d%d%d%d%d%d%d"
#define BYTETOBINARY(byte)  \
  (byte & 0x80 ? 1 : 0), \
  (byte & 0x40 ? 1 : 0), \
  (byte & 0x20 ? 1 : 0), \
  (byte & 0x10 ? 1 : 0), \
  (byte & 0x08 ? 1 : 0), \
  (byte & 0x04 ? 1 : 0), \
  (byte & 0x02 ? 1 : 0), \
  (byte & 0x01 ? 1 : 0)

#define LOOP_PERIOD         8
#define LOOP_INTERVAL       (CLOCK_SECOND * LOOP_PERIOD)
#define SCHEDULE_INTERVAL       (CLOCK_SECOND * 5)
#define LEDS_OFF_HYSTERISIS ((RTIMER_SECOND * LOOP_PERIOD) >> 1)
#define LEDS_PERIODIC       LEDS_BLUE
#define LEDS_BUTTON         LEDS_RED
#define LEDS_SERIAL_IN      LEDS_GREEN
#define LEDS_REBOOT         LEDS_ALL
#define LEDS_RF_RX          (LEDS_ALL)
#define BUTTON_PRESS_EVENT_INTERVAL (CLOCK_SECOND)

static struct etimer button_press_reset;
uint8_t button_press_counter = 0;
#define TIMER       (CLOCK_SECOND)

uint8_t lastBRIGHTNESS = 0b00000001;
uint8_t seq = 0;
void tsch_dewi_callback_joining_network(void);
void tsch_dewi_callback_leaving_network(void);

PROCESS(dewi_demo_process, "DEWI DEMO PROCESS");
AUTOSTART_PROCESSES(&dewi_demo_process);

static void app_netflood_packet_received(struct broadcast_conn *c,
		const linkaddr_t *from) {
	struct APP_PACKET *temp = packetbuf_dataptr();
	printf(
			"[SCHEDULER]: Received Schedule Update %u bytes from %u:%u: '0x%04x'\n",
			packetbuf_datalen(), from->u8[0], from->u8[1],
			*(uint16_t *) packetbuf_dataptr());

}

static const struct netflood_callbacks app_netflood_rx = {
		app_netflood_packet_received };
static struct netflood_conn app_netflood;

PROCESS_THREAD(dewi_demo_process, ev, data) {
	PROCESS_BEGIN()
		;
		printf("Coordinator: initialization start\n");

		setCoord(1);
		initScheduler();

		tsch_set_coordinator(1);
		clock_wait(CLOCK_SECOND * 2);
		initNeighbourTable();

		radio_result_t rv = NETSTACK_RADIO.set_value(RADIO_PARAM_TXPOWER,
				-24);
		button_sensor.configure(BUTTON_SENSOR_CONFIG_TYPE_INTERVAL,
		BUTTON_PRESS_EVENT_INTERVAL);

		PROCESS_EXITHANDLER(netflood_close(&app_netflood))
		netflood_open(&app_netflood, CLOCK_SECOND, NETFLOOD_CHANNEL_APP,
				&app_netflood_rx);
		printf("DEWI Application\n");

		printf("with external LED\n");
		leds_off(LEDS_ALL);
		leds_on(LEDS_GREEN);
		while (1) {

			PROCESS_YIELD()
			;

			leds_on(LEDS_GREEN);
			if (ev == sensors_event) {
				if (data == &button_sensor) {

					if (button_sensor.value(
							BUTTON_SENSOR_VALUE_TYPE_LEVEL) == BUTTON_SENSOR_PRESSED_LEVEL) {
						button_press_counter = button_press_counter + 1;
						etimer_set(&button_press_reset, CLOCK_SECOND);

					}

					if (button_press_counter == 5) {
						CIDER_start();
					}
				}
			} else if (ev == PROCESS_EVENT_TIMER) {
				button_press_counter = 0;

			} else if (ev == button_press_duration_exceeded) {
				if (button_sensor.value(BUTTON_SENSOR_VALUE_TYPE_PRESS_DURATION)
						== 10) {
					printf("[APP]: Restart APP\n");
					struct APP_PACKET temp;
					temp.base.dst = tsch_broadcast_address;
					;
					temp.base.src = linkaddr_node_addr;
					temp.subType = RESET;

					packetbuf_copyfrom(&temp, sizeof(struct APP_PACKET));
					packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
					packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, 0);
					netflood_send(&app_netflood, seq++);
					tsch_dewi_callback_leaving_network();
					tsch_dewi_callback_joining_network();

				}
			}
		}

	PROCESS_END();
}

void init_coordinator(void) {

}
/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 * @}
 */

void uip_ds6_link_neighbor_callback(int status, int numtx) {

}

void uip_debug_lladdr_print() {
}

void tsch_dewi_callback_joining_network(void) {
printf("[APP]: joining network\n");
setCoord(1);
initScheduler();
initNeighbourTable();
leds_on(LEDS_GREEN);
}
void tsch_dewi_callback_leaving_network(void) {
printf("[APP]: Leaving network\n");
scheduler_reset();
neighbourTable_reset();
CIDER_reset();
}
void tsch_dewi_callback_ka(void){
	printf("[APP]: Keep Alive sent\n");
	leds_off(LEDS_ALL);
	leds_on(LEDS_YELLOW);
}
