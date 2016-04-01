/*
 * test-tsch.c
 *
 *  Created on: Mar 30, 2016
 *      Author: user
 */



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

#include <stdio.h>
#include <stdint.h>
/*---------------------------------------------------------------------------*/
#define LOOP_PERIOD         8
#define LOOP_INTERVAL       (CLOCK_SECOND * LOOP_PERIOD)
#define LEDS_OFF_HYSTERISIS ((RTIMER_SECOND * LOOP_PERIOD) >> 1)
#define LEDS_PERIODIC       LEDS_BLUE
#define LEDS_BUTTON         LEDS_RED
#define LEDS_SERIAL_IN      LEDS_GREEN
#define LEDS_REBOOT         LEDS_ALL
#define LEDS_RF_RX          (LEDS_ALL)
#define BUTTON_PRESS_EVENT_INTERVAL (CLOCK_SECOND)
/*---------------------------------------------------------------------------*/
static struct etimer et;
static struct rtimer rt;
static uint16_t counter;
/*---------------------------------------------------------------------------*/
PROCESS(zoul_demo_process, "Zoul demo process");
AUTOSTART_PROCESSES(&zoul_demo_process);
/*---------------------------------------------------------------------------*/
static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  leds_toggle(LEDS_RF_RX);
  printf("*** Received %u bytes from %u:%u: '0x%04x'\n", packetbuf_datalen(),
         from->u8[0], from->u8[1], *(uint16_t *)packetbuf_dataptr());
}
/*---------------------------------------------------------------------------*/
static const struct broadcast_callbacks bc_rx = { broadcast_recv };
static struct broadcast_conn bc;

static void
tsch_schedule(void)
{
  struct tsch_slotframe *sf_min;
  /* First, empty current schedule */
  tsch_schedule_remove_all_slotframes();
  /* Build 6TiSCH minimal schedule.
   * We pick a slotframe length of TSCH_SCHEDULE_DEFAULT_LENGTH */
  sf_min = tsch_schedule_add_slotframe(0, TSCH_SCHEDULE_DEFAULT_LENGTH);
  /* Add a single Tx|Rx|Shared slot using broadcast address (i.e. usable for unicast and broadcast).
   * We set the link type to advertising, which is not compliant with 6TiSCH minimal schedule
   * but is required according to 802.15.4e if also used for EB transmission.
   * Timeslot: 0, channel offset: 0. */
  tsch_schedule_add_link(sf_min,
	  LINK_OPTION_SHARED | LINK_OPTION_TIME_KEEPING | LINK_OPTION_TX | LINK_OPTION_RX,
      LINK_TYPE_ADVERTISING, &tsch_broadcast_address,
      0, 0);
  tsch_schedule_add_link(sf_min,
      LINK_OPTION_SHARED | LINK_OPTION_TIME_KEEPING | LINK_OPTION_TX | LINK_OPTION_RX,
      LINK_TYPE_NORMAL, &tsch_broadcast_address,
      1, 2);
  tsch_schedule_add_link(sf_min,
      LINK_OPTION_SHARED | LINK_OPTION_TIME_KEEPING | LINK_OPTION_TX | LINK_OPTION_RX,
      LINK_TYPE_NORMAL, &tsch_broadcast_address,
      3, 1);
  tsch_schedule_add_link(sf_min,
      LINK_OPTION_SHARED | LINK_OPTION_TIME_KEEPING | LINK_OPTION_TX | LINK_OPTION_RX,
      LINK_TYPE_NORMAL, &tsch_broadcast_address,
      5, 4);
}
/*---------------------------------------------------------------------------*/
static void
rt_callback(struct rtimer *t, void *ptr)
{
  leds_off(LEDS_PERIODIC);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(zoul_demo_process, ev, data)
{

	//tsch_schedule();
		tsch_set_coordinator(1);
	  PROCESS_EXITHANDLER(broadcast_close(&bc))

	  PROCESS_BEGIN();

	  counter = 0;

	  /* Disable the radio duty cycle and keep the radio on */
	  //NETSTACK_MAC.off(1);

	  broadcast_open(&bc, BROADCAST_CHANNEL, &bc_rx);

	  /* Configure the user button */
	  button_sensor.configure(BUTTON_SENSOR_CONFIG_TYPE_INTERVAL,
	                          BUTTON_PRESS_EVENT_INTERVAL);


	  printf("Zoul test application\n");



	  while(1){

	    PROCESS_YIELD();

	      if(data == &button_sensor) {
	        if(button_sensor.value(BUTTON_SENSOR_VALUE_TYPE_LEVEL) ==
	           BUTTON_SENSOR_PRESSED_LEVEL) {
	          printf("Button pressed\n");
	          packetbuf_copyfrom(&counter, sizeof(counter));
	          broadcast_send(&bc);
	        } else {
	          printf("...and released!\n");
	        }
	      }

	  }


	  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 * @}
 */



void
uip_ds6_link_neighbor_callback(int status, int numtx)
{

}

void uip_debug_lladdr_print(){
}

