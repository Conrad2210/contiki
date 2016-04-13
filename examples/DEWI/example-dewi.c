/*
 * test-tsch.c
 *
 *  Created on: Mar 30, 2016
 *      Author: user
 */

#include "example-dewi.h"

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
#define SCHEDULE_INTERVAL       (CLOCK_SECOND * 5)
#define LEDS_OFF_HYSTERISIS ((RTIMER_SECOND * LOOP_PERIOD) >> 1)
#define LEDS_PERIODIC       LEDS_BLUE
#define LEDS_BUTTON         LEDS_RED
#define LEDS_SERIAL_IN      LEDS_GREEN
#define LEDS_REBOOT         LEDS_ALL
#define LEDS_RF_RX          (LEDS_ALL)
#define BUTTON_PRESS_EVENT_INTERVAL (CLOCK_SECOND)
/*---------------------------------------------------------------------------*/

//static struct rtimer rt;

/*---------------------------------------------------------------------------*/
PROCESS(dewi_demo_start, "START_EXAMPLE");
PROCESS(dewi_demo_process, "DEWI DEMO PROCESS");
AUTOSTART_PROCESSES(&dewi_demo_start, &dewi_demo_process);
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
//static void
//rt_callback(struct rtimer *t, void *ptr)
//{
//  leds_off(LEDS_PERIODIC);
//}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(dewi_demo_start, ev, data)
{
	PROCESS_BEGIN();
#if ISCOORD
	test_tsch_coordinator_init();

#else
	printf("INIT as TSCH node\n");
#endif

	//init SCHEDULER
	//init RLL
	//init CIDER
	PROCESS_END();
}

PROCESS_THREAD(dewi_demo_process, ev, data)
{
	PROCESS_BEGIN();

	/* Configure the user button */
	printf("DEWI Application\n");
#if EXTERNAL_LED
	printf("with external LED\n");
#else
	printf("without external LED\n");
#endif

	PROCESS_END();
}

void test_tsch_coordinator_init(void)
{
	printf("Coordinator: initialization start\n");
	tsch_set_coordinator(1);
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

