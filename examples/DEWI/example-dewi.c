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
#include "i2c.h"
#include "net/netstack.h"
#include "net/rime/broadcast.h"
#include "net/mac/tsch/tsch.h"
#include "net/mac/tsch/tsch-schedule.h"

#include "net/DEWI/scheduler/scheduler.h"
#include "net/DEWI/rll/rll.h"
#include "net/DEWI/cider/cider.h"
#include "net/DEWI/neighTable/neighTable.h"
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

static struct etimer timer;
#define TIMER       (CLOCK_SECOND * 1)
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
	PROCESS_BEGIN()
	;

#if ISCOORD
	printf("Coordinator: initialization start\n");
	tsch_set_coordinator(1);
	setCoord(1);
	initScheduler();

#else
	printf("Node: initialization start\n");
	setCoord(0);
	initScheduler();
#endif

	i2c_init(I2C_SDA_PORT, I2C_SDA_PIN, I2C_SCL_PORT, I2C_SCL_PIN, I2C_SCL_FAST_BUS_SPEED);
	i2c_master_enable();
	uint8_t err = 0x00;
	printf("Error Test: 0x%x\n", err);
	err = i2c_single_send(0x39, 0b00000000);
	printf("Error All OFF: 0x%x\n", err);
	i2c_master_enable();
	err = i2c_single_send(0x39, 0x3F);
	printf("Error Current: 0x%x\n", err);

	initNeighbourTable();
	//init SCHEDULER
	//init RLL
	//init CIDER
PROCESS_END();
}

PROCESS_THREAD(dewi_demo_process, ev, data)
{
PROCESS_BEGIN()
;

etimer_set(&timer, TIMER);

button_sensor.configure(BUTTON_SENSOR_CONFIG_TYPE_INTERVAL, BUTTON_PRESS_EVENT_INTERVAL);
/* Configure the user button */
printf("DEWI Application\n");
#if EXTERNAL_LED
printf("with external LED\n");

#else
printf("without external LED\n");
#endif
while (1)
{

	PROCESS_YIELD()
	;

	if (data == &button_sensor)
	{

		if (button_sensor.value(BUTTON_SENSOR_VALUE_TYPE_LEVEL) == BUTTON_SENSOR_PRESSED_LEVEL)
		{
			printf("Button pressed\n");
		}

	}

	if (ev == PROCESS_EVENT_TIMER)
	{
		uint8_t test;
			i2c_master_enable();
			test = i2c_single_send(0x39, 0x1F);
			printf("Error LED1: 0x%x\n", test);

			i2c_master_enable();
			test = i2c_single_send(0x39, 0x1F);
			printf("Error LED2: 0x%x\n", test);

			i2c_master_enable();
			test = i2c_single_send(0x39, 0x1F);
			printf("Error LED3: 0x%x\n", test);

		etimer_set(&timer, TIMER);
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

