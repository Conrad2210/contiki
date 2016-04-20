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

#include "net/DEWI/scheduler/scheduler.h"
#include "net/DEWI/rll/rll.h"
#include "net/DEWI/cider/cider.h"
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
AUTOSTART_PROCESSES(&dewi_demo_start,&dewi_demo_process);
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

		ScheduleInfo_t temp;
		temp.handle = 0x00;
		temp.slotframeLength = 51;
		int i;
		printf("Start setting Schedule\n");
		for (i = 0; i < temp.slotframeLength; i++)
		{
			temp.links[i].addr = &tsch_broadcast_address;
			temp.links[i].channel_offset = 0;
			temp.links[i].isActive = 1;


			if (i == 0){
				temp.links[i].link_type = LINK_TYPE_ADVERTISING;
				temp.links[i].link_options = LINK_OPTION_RX | LINK_OPTION_TX | LINK_OPTION_SHARED | LINK_OPTION_TIME_KEEPING;
			}
			else{
				temp.links[i].link_type = LINK_TYPE_NORMAL;
				temp.links[i].link_options = LINK_OPTION_RX | LINK_OPTION_TX;
			}

			temp.links[i].timeslot = i;
		}

#if ISCOORD
		printf("Coordinator: initialization start\n");
		tsch_set_coordinator(1);
		setCoord(1);
		initScheduler();
		setSchedule(temp);


#else
		printf("Node: initialization start\n");
		setCoord(0);
		initScheduler();
#endif

		//init SCHEDULER
		//init RLL
		//init CIDER
	PROCESS_END();
}

PROCESS_THREAD(dewi_demo_process, ev, data)
{
PROCESS_BEGIN();
button_sensor.configure(BUTTON_SENSOR_CONFIG_TYPE_INTERVAL,BUTTON_PRESS_EVENT_INTERVAL);
	/* Configure the user button */
	printf("DEWI Application\n");
#if EXTERNAL_LED
	printf("with external LED\n");

#else
	printf("without external LED\n");
#endif
	while (1)
	{

		PROCESS_YIELD();
		if (data == &button_sensor)
		{

			if (button_sensor.value(BUTTON_SENSOR_VALUE_TYPE_LEVEL) == BUTTON_SENSOR_PRESSED_LEVEL)
			{
				printf("Button pressed\n");
			}

		}
	}
PROCESS_END();
}

void init_coordinator(void)
{

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

