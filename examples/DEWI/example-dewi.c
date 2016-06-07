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

static struct etimer timer;
#define TIMER       (CLOCK_SECOND * 0.1)

#define LED_RED 0b10000000
#define LED_GREEN 0b01100000
#define LED_BLUE 0b01000000

int counter = 0;
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
	button_sensor.configure(BUTTON_SENSOR_CONFIG_TYPE_INTERVAL,
	                        BUTTON_PRESS_EVENT_INTERVAL);
	i2c_init(I2C_SDA_PORT, I2C_SDA_PIN, I2C_SCL_PORT, I2C_SCL_PIN, I2C_SCL_FAST_BUS_SPEED);
	uint8_t err = 0x00;
	printf("Error Test: 0x%x\n", err);
	err = i2c_single_send(0x39, 0b00000000);
	printf("Error All OFF: 0x%x\n", err);

	clock_delay_usec(50);
	err = i2c_single_send(0x39, 0b001100001);
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
	if(ev == sensors_event){
		if (data == &button_sensor)
			{

				if (button_sensor.value(BUTTON_SENSOR_VALUE_TYPE_LEVEL) == BUTTON_SENSOR_PRESSED_LEVEL)
				{
					printf("Button pressed\n");
				}else {
			          printf("...and released!\n");
			        }

			}
	}else	if (ev == PROCESS_EVENT_TIMER)
	{

			uint8_t randNum = rand() % 31;
			uint8_t result;
			if(counter > 20)
				result =printf("Set LED RED to: 0b"BYTETOBINARYPATTERN"\n", BYTETOBINARY((LED_RED | randNum)));
			i2c_single_send(0x39, (LED_RED | randNum));
			//printf("Error LED1: 0x%x\n", test);
			clock_delay_usec(50);
			randNum = rand() % 31;
			if(counter > 20)
				printf("Set LED GREEN to: 0b"BYTETOBINARYPATTERN"\n", BYTETOBINARY((LED_GREEN | randNum)));
			i2c_single_send(0x39, (LED_GREEN | randNum));
			clock_delay_usec(50);
			randNum = rand() % 31;
			if(counter > 20)
				printf("Set LED BLUE to: 0b"BYTETOBINARYPATTERN"\n", BYTETOBINARY((LED_BLUE | randNum)));
			i2c_single_send(0x39, (LED_BLUE | randNum));

			//printf("Error LED3: 0x%x\n", test);
			if(counter > 20)
				counter = 0;
			counter++;

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

