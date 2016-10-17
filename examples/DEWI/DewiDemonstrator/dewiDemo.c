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

#define LED_RED 0b10000000
#define LED_GREEN 0b01100000
#define LED_BLUE 0b01000000

#define LED_BRIGHTNESS 0b00100000
#define BUTTON_PRESS_EVENT_INTERVAL (CLOCK_SECOND)

PROCESS(dewiDemo, "DEWI Demonstrator, using CIDER and RLL");	//main process
AUTOSTART_PROCESSES(&dewiDemo);

PROCESS_THREAD(dewiDemo, ev, data)  //process to initialize
{
	PROCESS_BEGIN()
		;
	rllInit();
		//configure buttons
		button_sensor.configure(BUTTON_SENSOR_CONFIG_TYPE_INTERVAL,
		BUTTON_PRESS_EVENT_INTERVAL);

		//configure serial line
		serial_line_init();
		while (1) {
			PROCESS_YIELD()
			;
			if (ev == sensors_event) {
				if (data == &button_sensor) {
					if (button_sensor.value(BUTTON_SENSOR_VALUE_TYPE_LEVEL)
							== BUTTON_SENSOR_PRESSED_LEVEL) {
						printf("[APP]: Create App packet\n");
						struct APP_PACKET temp;
						temp.subType = COLOR;
						sendRLLMessage(temp);
					}
				}
			}
		}
	PROCESS_END();
}

void applicationDataCallback(struct APP_PACKET data, uint8_t seq) {
printf("[APP]: Data received: Type: %d, SeqNo: %d\n", data.subType, seq);
}
