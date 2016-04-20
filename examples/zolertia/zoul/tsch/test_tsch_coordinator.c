/*
 * test-tsch.c
 *
 *  Created on: Mar 30, 2016
 *      Author: user
 */

#include "test_tsch_coordinator.h"

#include "contiki.h"
#include "cpu.h"
#include "sys/etimer.h"
#include "sys/rtimer.h"
#include "dev/leds.h"
#include "dev/uart.h"
#include "dev/button-sensor.h"
#include "dev/adc-zoul.h"
#include "dev/zoul-sensors.h"
#include "dev/watchdog.h"
#include "dev/serial-line.h"
#include "dev/sys-ctrl.h"
#include "net/netstack.h"
#include "net/rime/broadcast.h"
#include "net/mac/tsch/tsch.h"
#include "net/mac/tsch/tsch-schedule.h"
#include "packet_types.h"
#include <stdio.h>
#include <stdint.h>
/*---------------------------------------------------------------------------*/
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
/*---------------------------------------------------------------------------*/
static struct etimer scheduleUpdate;
//static struct rtimer rt;
static uint16_t counter;
/*---------------------------------------------------------------------------*/
PROCESS(start_app, "START_APP");
PROCESS(zoul_demo_process, "Zoul demo process");
AUTOSTART_PROCESSES(&start_app, &zoul_demo_process);
/*---------------------------------------------------------------------------*/
static void broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
	leds_toggle(LEDS_RF_RX);
	printf("*** Received %u bytes from %u:%u: '0x%04x'\n", packetbuf_datalen(), from->u8[0], from->u8[1], *(uint16_t *) packetbuf_dataptr());
}

static void schedule_update_received(struct broadcast_conn *c, const linkaddr_t *from)
{
	printf("*** Received Schedule Update %u bytes from %u:%u: '0x%04x'\n", packetbuf_datalen(), from->u8[0], from->u8[1], *(uint16_t *) packetbuf_dataptr());
}

/*---------------------------------------------------------------------------*/
static const struct broadcast_callbacks bc_rx =
{ broadcast_recv };
static const struct broadcast_callbacks schedule_bc_rx =
{ schedule_update_received };
static struct broadcast_conn bc, schedule_bc;
CIDER_PACKET temp;
static void tsch_schedule(void)
{
	struct tsch_slotframe *sf_min;
	/* First, empty current schedule */
	tsch_schedule_remove_all_slotframes();
	/* Build 6TiSCH minimal schedule.
	 * We pick a slotframe length of TSCH_SCHEDULE_DEFAULT_LENGTH */
	sf_min = tsch_schedule_add_slotframe(0, 25);
	/* Add a single Tx|Rx|Shared slot using broadcast address (i.e. usable for unicast and broadcast).
	 * We set the link type to advertising, which is not compliant with 6TiSCH minimal schedule
	 * but is required according to 802.15.4e if also used for EB transmission.
	 * Timeslot: 0, channel offset: 0. */
	tsch_schedule_add_link(sf_min,LINK_OPTION_RX | LINK_OPTION_TX | LINK_OPTION_SHARED | LINK_OPTION_TIME_KEEPING, LINK_TYPE_ADVERTISING, &tsch_broadcast_address, 0, 0);
	tsch_schedule_add_link(sf_min,
	LINK_OPTION_RX | LINK_OPTION_TX, LINK_TYPE_NORMAL, &tsch_broadcast_address, 1, 1);
	tsch_schedule_add_link(sf_min,
	LINK_OPTION_RX | LINK_OPTION_TX, LINK_TYPE_NORMAL, &tsch_broadcast_address, 2, 2);
	tsch_schedule_add_link(sf_min,
	LINK_OPTION_RX | LINK_OPTION_TX, LINK_TYPE_NORMAL, &tsch_broadcast_address, 8, 5);
	tsch_schedule_add_link(sf_min,
	LINK_OPTION_RX | LINK_OPTION_TX, LINK_TYPE_NORMAL, &tsch_broadcast_address, 12, 3);
	tsch_schedule_add_link(sf_min,
	LINK_OPTION_RX | LINK_OPTION_TX, LINK_TYPE_NORMAL, &tsch_broadcast_address, 18, 5);
}
/*---------------------------------------------------------------------------*/
//static void
//rt_callback(struct rtimer *t, void *ptr)
//{
//  leds_off(LEDS_PERIODIC);
//}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(start_app, ev, data)
{
	PROCESS_BEGIN()
		;

		test_tsch_coordinator_init();

	PROCESS_END();
}

PROCESS_THREAD(zoul_demo_process, ev, data)
{

PROCESS_EXITHANDLER(broadcast_close(&bc))

temp.stage = 0x5;
temp.shortAddr = 0xFFFF;
temp.base.dst = 0xffff;
temp.base.src = linkaddr_node_addr.u16;
temp.base.type = 0x00;
PROCESS_BEGIN()
;

counter = 0;

/* Disable the radio duty cycle and keep the radio on */
//NETSTACK_MAC.off(1);
broadcast_open(&bc, BROADCAST_CHANNEL, &bc_rx);
broadcast_open(&schedule_bc, BROADCAST_CHANNEL_SCHEDULE, &schedule_bc_rx);

/* Configure the user button */
button_sensor.configure(BUTTON_SENSOR_CONFIG_TYPE_INTERVAL,
BUTTON_PRESS_EVENT_INTERVAL);

etimer_set(&scheduleUpdate, SCHEDULE_INTERVAL);
printf("Zoul test application\n");

while (1)
{

	PROCESS_YIELD()
	;
	if (ev == PROCESS_EVENT_TIMER)
	{
		printf("create schedule update packet\n");
		packetbuf_copyfrom(&temp, sizeof(CIDER_PACKET));
#if TSCH_WITH_LINK_SELECTOR
		packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
		packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, 0);
#endif
		broadcast_send(&schedule_bc);
		etimer_set(&scheduleUpdate, SCHEDULE_INTERVAL);
	}
	if (data == &button_sensor)
	{
		if (button_sensor.value(BUTTON_SENSOR_VALUE_TYPE_LEVEL) ==
		BUTTON_SENSOR_PRESSED_LEVEL)
		{
			printf("Button pressed\n");
			packetbuf_copyfrom(&counter, sizeof(counter));
#if TSCH_WITH_LINK_SELECTOR
			packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
			packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, 1);
#endif
			broadcast_send(&bc);
		}
		else
		{
			printf("...and released!\n");
		}
	}

}

PROCESS_END();
}

void test_tsch_coordinator_init(void)
{
printf("Coordinator: initialization start\n");
tsch_schedule();
tsch_set_coordinator(1);
printf("Coordinator: initialization done\n");
}
/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 * @}
 */

void uip_ds6_link_neighbor_callback(int status, int numtx)
{

}

void uip_debug_lladdr_print()
{
}

