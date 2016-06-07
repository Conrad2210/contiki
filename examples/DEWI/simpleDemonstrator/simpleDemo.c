#include "contiki.h"
#include "cpu.h"
#include "dev/leds.h"
#include "dev/uart.h"
#include "dev/adc-zoul.h"
#include "dev/watchdog.h"
#include "dev/button-sensor.h"
#include "dev/serial-line.h"
#include "net/rime/broadcast.h"
#include "../cpu/cc2538/dev/i2c.h"
#include "../platform/zoul/remote/board.h"
#include "../examples/zolertia/zoul/project-conf.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
/*---------------------------------------------------------------------------*/
#define BUTTON_PRESS_EVENT_INTERVAL (CLOCK_SECOND)
#define LEDS_PERIODIC       LEDS_BLUE
#define LEDS_BUTTON         LEDS_RED
#define LEDS_SERIAL_IN      LEDS_GREEN
#define LEDS_REBOOT         LEDS_ALL

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

#define LED_RED 0b10000000
#define LED_GREEN 0b01100000
#define LED_BLUE 0b01000000

#define LED_BRIGHTNESS 0b00100000
int counter = 0;
uint8_t lastBRIGHTNESS = 0b00000001;

/*---------------------------------------------------------------------------*/
PROCESS(simpleDemo_start, "Simple Demonstrator START");
PROCESS(simpleDemo, "Simple Demonstrator");
AUTOSTART_PROCESSES(&simpleDemo_start, &simpleDemo);
/*---------------------------------------------------------------------------*/
static void colorRecvd(struct broadcast_conn *c, const linkaddr_t *from)
{
//	  leds_toggle(LEDS_ALL);
	uint32_t R, G, B;

	uint32_t temp = *(uint32_t *) packetbuf_dataptr();
	printf("*** Received %u bytes from %u:%u: '0x%08x'\n", packetbuf_datalen(), from->u8[0], from->u8[1], temp);
	R = (int) (temp & 0x00ff0000) >> 19;
	G = (int) (temp & 0x0000ff00) >> 11;
	B = (int) (temp & 0x000000ff) >> 3;
	printf("R %x\n", R);
	printf("G %x\n", G);
	printf("B %x\n", B);
	i2c_single_send(0x39, LED_RED | R);
	clock_delay_usec(50);
	i2c_single_send(0x39, LED_GREEN | G);
	clock_delay_usec(50);
	i2c_single_send(0x39, LED_BLUE | B);
	clock_delay_usec(50);
}

static void brightnessRecvd(struct broadcast_conn *c, const linkaddr_t *from)
{
//	  leds_toggle(LEDS_ALL);
	lastBRIGHTNESS = *(uint8_t *)packetbuf_dataptr();
	printf("*** Received %u bytes from %u:%u: '0x%08x'\n", packetbuf_datalen(), from->u8[0], from->u8[1],lastBRIGHTNESS);
	i2c_single_send(0x39, (LED_BRIGHTNESS | lastBRIGHTNESS));
//	uint32_t R, G, B;
//	uint32_t temp = *(uint32_t *) packetbuf_dataptr();
//	printf("*** Received %u bytes from %u:%u: '0x%08x'\n", packetbuf_datalen(), from->u8[0], from->u8[1], temp);
//	R = (int) (temp & 0x00ff0000) >> 19;
//	G = (int) (temp & 0x0000ff00) >> 11;
//	B = (int) (temp & 0x000000ff) >> 3;
//	printf("R %x\n", R);
//	printf("G %x\n", G);
//	printf("B %x\n", B);
//	i2c_single_send(0x39, LED_RED | R);
//	clock_delay_usec(50);
//	i2c_single_send(0x39, LED_GREEN | G);
//	clock_delay_usec(50);
//	i2c_single_send(0x39, LED_BLUE | B);
//	clock_delay_usec(50);
}
/*---------------------------------------------------------------------------*/
static const struct broadcast_callbacks colour_rx =
{ colorRecvd }, brightness_rx = {brightnessRecvd};
static struct broadcast_conn colour_con,brightness_con;

PROCESS_THREAD(simpleDemo_start, ev, data)
{
	PROCESS_BEGIN()
		;

		//configure buttons
		button_sensor.configure(BUTTON_SENSOR_CONFIG_TYPE_INTERVAL, BUTTON_PRESS_EVENT_INTERVAL);

		//configure I2C
		i2c_init(I2C_SDA_PORT, I2C_SDA_PIN, I2C_SCL_PORT, I2C_SCL_PIN, I2C_SCL_FAST_BUS_SPEED);

		//configure serial line
		serial_line_init();

		//configure braodcast channel
		broadcast_open(&colour_con, COLOUR_CHANNEL, &colour_rx);
		broadcast_open(&brightness_con, BRIGHTNESS_CHANNEL, &brightness_rx);

		//turn all leds off
		i2c_single_send(0x39, 0b00000000);

		//lock delay
		clock_delay_usec(50);

		//set led brightness to inital brightness
		i2c_single_send(0x39, (LED_BRIGHTNESS | lastBRIGHTNESS));
	PROCESS_END();
}

PROCESS_THREAD(simpleDemo, ev, data)
{

PROCESS_BEGIN()
	;

	char *ch_data, *ptr;
	long i_data;
	uint32_t R, G, B;

	while (1)
	{
		PROCESS_YIELD()
		;
		if (ev == sensors_event)
		{
#if MASTERNODE
			if (data == &button_sensor)
			{

				if (button_sensor.value(BUTTON_SENSOR_VALUE_TYPE_LEVEL) == BUTTON_SENSOR_PRESSED_LEVEL)
				{
					lastBRIGHTNESS = lastBRIGHTNESS + 3;
					if (lastBRIGHTNESS > 0b00011111)
						lastBRIGHTNESS = 0b00000001;


					printf("[APP]: change brightness to: 0b"BYTETOBINARYPATTERN"\n", BYTETOBINARY((LED_BRIGHTNESS | lastBRIGHTNESS)));
					i2c_single_send(0x39, (LED_BRIGHTNESS | lastBRIGHTNESS));

					packetbuf_copyfrom(&lastBRIGHTNESS, sizeof(uint8_t));
					broadcast_send(&brightness_con);
				}

			}
#endif
		}
		else if(ev == serial_line_event_message)
		{
			ch_data = (char*) data;
			printf("ch_data %s\n", ch_data);
			i_data = strtol(ch_data, &ptr, 16);	//Convert char data to hex

			R = (int) (i_data & 0x00ff0000) >> 19;	//Select byte 1 and shift to first position
			G = (int) (i_data & 0x0000ff00) >> 11;	//Same for byte 2
			B = (int) (i_data & 0x000000ff) >> 3;	//And byte 3
			printf("R %x\n", R);
			printf("G %x\n", G);
			printf("B %x\n", B);
			i2c_single_send(0x39, LED_RED | R);
			clock_delay_usec(50);
			i2c_single_send(0x39, LED_GREEN | G);
			clock_delay_usec(50);
			i2c_single_send(0x39, LED_BLUE | B);
			clock_delay_usec(50);

			packetbuf_copyfrom(&i_data, sizeof(i_data));
			broadcast_send(&colour_con);
		}
	}
PROCESS_END();
}
