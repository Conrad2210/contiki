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

static struct etimer randomColorTimer;
#define TIMER       (CLOCK_SECOND)

#define LED_RED 0b10000000
#define LED_GREEN 0b01100000
#define LED_BLUE 0b01000000
#define LED_BRIGHTNESS 0b00100000
int counter = 0;
uint8_t lastBRIGHTNESS = 0b00000001;

/*---------------------------------------------------------------------------*/

//static struct rtimer rt;
/*---------------------------------------------------------------------------*/
PROCESS(dewi_demo_start, "START_EXAMPLE");
PROCESS(dewi_demo_process, "DEWI DEMO PROCESS");
AUTOSTART_PROCESSES(&dewi_demo_start);
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
//static void
//rt_callback(struct rtimer *t, void *ptr)
//{
//  leds_off(LEDS_PERIODIC);
//}
/*---------------------------------------------------------------------------*/

void printDEMOConfiguration()
{
	printf("\n\n[DEMO]: Print Configuration\n");
	printf("[DEMO]: Tx Power: %d\n", TXRADIOPOWER);
	printf("[DEMO]: LP Device: %x\n", LPDEVICE);
	printf("[DEMO]: TSCH SlotLength: %u\n", TSCH_CONF_DEFAULT_TIMESLOT_LENGTH);
}

PROCESS_THREAD(dewi_demo_start, ev, data)
{
	PROCESS_BEGIN()
	;

#if DEBUG
	printDEMOConfiguration();
#endif

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
	radio_result_t rv = NETSTACK_RADIO.set_value(RADIO_PARAM_TXPOWER, TXRADIOPOWER);
	button_sensor.configure(BUTTON_SENSOR_CONFIG_TYPE_INTERVAL,
	BUTTON_PRESS_EVENT_INTERVAL);

	initNeighbourTable();

	//init RLL
	//init CIDER
	process_start(&dewi_demo_process, NULL);
PROCESS_END();
}

PROCESS_THREAD(dewi_demo_process, ev, data)
{
PROCESS_BEGIN()
	;

	i2c_init(I2C_SDA_PORT, I2C_SDA_PIN, I2C_SCL_PORT, I2C_SCL_PIN, I2C_SCL_FAST_BUS_SPEED);
	i2c_single_send(0x39, (LED_BRIGHTNESS | lastBRIGHTNESS));
	etimer_set(&randomColorTimer, TIMER);
	printf("DEWI Application\n");
#if EXTERNAL_LED
	printf("with external LED\n");
	while (1)
		{

			PROCESS_YIELD()
			;
			if (ev == sensors_event)
			{
				if (data == &button_sensor)
				{

					if (button_sensor.value(BUTTON_SENSOR_VALUE_TYPE_LEVEL) == BUTTON_SENSOR_PRESSED_LEVEL)
					{
						printf("Button pressed\n");
						lastBRIGHTNESS = lastBRIGHTNESS + 3;
						if (lastBRIGHTNESS > 0b00011111)
							lastBRIGHTNESS = 0b00000001;


						printf("[APP]: change brightness to: 0b"BYTETOBINARYPATTERN"\n", BYTETOBINARY((LED_BRIGHTNESS | lastBRIGHTNESS)));
						i2c_single_send(0x39, (LED_BRIGHTNESS | lastBRIGHTNESS));

					}
				}
			}
			else if (ev == PROCESS_EVENT_TIMER)
			{

				uint8_t randNum = rand() % 31;
				uint8_t result;
				//printf("[APP]: change RED to: 0b"BYTETOBINARYPATTERN"\n", BYTETOBINARY((LED_RED | randNum)));
				i2c_single_send(0x39, (LED_RED | randNum));
				clock_delay_usec(50);
				randNum = rand() % 31;
				//printf("[APP]: change GREE to: 0b"BYTETOBINARYPATTERN"\n", BYTETOBINARY((LED_GREEN | randNum)));
				i2c_single_send(0x39, (LED_GREEN | randNum));
				clock_delay_usec(50);
				randNum = rand() % 31;
				//printf("[APP]: change BLUE to: 0b"BYTETOBINARYPATTERN"\n", BYTETOBINARY((LED_BLUE | randNum)));
				i2c_single_send(0x39, (LED_BLUE | randNum));

				etimer_set(&randomColorTimer, TIMER);
			}
		}

#else
	printf("without external LED\n");
#endif

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

