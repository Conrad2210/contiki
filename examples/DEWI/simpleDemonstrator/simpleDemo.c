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
#include "i2c.h"
#include "project-conf.h"

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

#define MAX_RETRANSMISSIONS 10		//when unicast, try 10 times 
#define NUM_ADDR_ENTRIES 100		//number of slave node that master node can have 	

#define LED_RED 0b10000000
#define LED_GREEN 0b01100000
#define LED_BLUE 0b01000000

#define LED_BRIGHTNESS 0b00100000

static struct stimer timer15s;			//a 15 seconds timer 
static struct stimer timer30s;			//a 30 seconds timer

uint8_t lastBRIGHTNESS = 0b00000001;

char m_available[20] = "M available\0";	// a signal which mean if the parent node is or not allowed to have new son node
linkaddr_t master_addr1;						//parent or master address

struct slavesAddr_entry {//a structure for each slave or son node that the master or parent node has
	struct slavesAddr_entry *next;
	linkaddr_t addr;								//son address
	int b_bool;	//if son node doesn't sent address signal to the parent node, b_bool = 0, else b_bool = 1
	uint8_t checktimes;	//when check b_bool = 0, then check++  when check ==2  then the son node are disconnected
};

//list of slaves(sons) nodes
LIST(slavesAddr_list);
MEMB(slavesAddr_mem, struct slavesAddr_entry, NUM_ADDR_ENTRIES);

/*---------------------------------------------------------------------------*/
PROCESS(simpleDemo_start, "Simple Demonstrator START");	// process to initialize
PROCESS(simpleDemo, "Simple Demonstrator");						//main process 
AUTOSTART_PROCESSES(&simpleDemo_start, &simpleDemo);// start auto with these two process when turn on node
/*---------------------------------------------------------------------------*/
static struct broadcast_conn colour_con, brightness_con;
static struct runicast_conn data_runicast;
/*---------------------------------------------------------------------------*/

//when node receive a string of node address by serial port, then convert it to linkaddr type
int stringAddrtoInt_master_availability(linkaddr_t *tmp, char * data) {
	char p1[3];
	char p2[2];
	linkaddr_t tmp2;
	tmp2.u8[0] = atoi(strncpy(p1, data, 3));
	tmp2.u8[1] = atoi(strncpy(p2, (data + 4), 3));
	printf("tmp2 : %d:%d  node : %d:%d\n", tmp2.u8[0], tmp2.u8[1],
			linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1]);
	linkaddr_copy(tmp, &tmp2);
	if (linkaddr_node_addr.u8[0] == tmp2.u8[0]
			&& linkaddr_node_addr.u8[1] == tmp2.u8[1]) {
		return 1;
	}
	return 0;

}

// convert battery volatge level to battery percentage status, and send by serial port to be save in the datatable
void sendBatteryStatusByserialP(int b_data, int a, int b) {
	if (b_data >= 3240) {
		printf("node(%d:%d) Battery '100%%' \r\n", a, b);
	} else if (b_data >= 3230 && b_data < 3240) {
		printf("node(%d:%d) Battery '90%%' \r\n", a, b);
	} else if (b_data >= 3200 && b_data < 3230) {
		printf("node(%d:%d) Battery '80%%' \r\n", a, b);
	} else if (b_data >= 3170 && b_data < 3200) {
		printf("node(%d:%d) Battery '70%%' \r\n", a, b);
	} else if (b_data >= 3100 && b_data < 3170) {
		printf("node(%d:%d) Battery '60%%' \r\n", a, b);
	} else if (b_data >= 3050 && b_data < 3100) {
		printf("node(%d:%d) Battery '50%%' \r\n", a, b);
	} else if (b_data >= 2950 && b_data < 3050) {
		printf("node(%d:%d) Battery '40%%' \r\n", a, b);
	} else if (b_data >= 2900 && b_data < 2950) {
		printf("node(%d:%d) Battery '30%%' \r\n", a, b);
	} else if (b_data >= 2800 && b_data < 2900) {
		printf("node(%d:%d) Battery '20%%' \r\n", a, b);
	} else if (b_data >= 2600 && b_data < 2800) {
		printf("node(%d:%d) Battery '10%%' \r\n", a, b);
	} else if (b_data >= 2350 && b_data < 2600) {
		printf("node(%d:%d) Battery '5%%' \r\n", a, b);
	} else if (b_data < 2350) {
		printf("node(%d:%d) Battery '0%%' \r\n", a, b);
	}
}

static void colorRecvd(struct broadcast_conn *c, const linkaddr_t *from) {
//	  leds_toggle(LEDS_ALL);
	uint32_t R, G, B;
	int i;
	//received data firstly convert into string to verify what data is it
	char* mssg = (char *) packetbuf_dataptr();
	//printf("mssg : %s\n",mssg);
	int temperature_data;
	int battery_data;
	linkaddr_t *tmp;
	char temporal[6];
	//received data convert into unint to use in the case that it is light color data
	uint32_t temp = *(uint32_t *) packetbuf_dataptr();

	//if the data is a string signal to call all nodes which has not parent node to connect to the node that a signal data from
	if (strcmp(mssg, "M available") == 0 && !master_addr1.u8[1]
			&& !master_addr1.u8[0]) {
		printf("message %s received from %d:%d ..........  : %d : %d :\n", mssg,
				from->u8[0], from->u8[1], master_addr1.u8[0],
				master_addr1.u8[1]);

		if (!runicast_is_transmitting(&data_runicast)) { //save master address then send to the master node, self-address by unicast
			linkaddr_copy(&master_addr1, from);
			packetbuf_copyfrom("Slave", sizeof("Slave\0"));
			printf("%u.%u: sending runicast to address %u.%u\n",
					linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1],
					master_addr1.u8[0], master_addr1.u8[1]);
			runicast_send(&data_runicast, &master_addr1, MAX_RETRANSMISSIONS);
		}
		data_runicast.is_tx = 0;

	} else if (strstr(mssg, "MasterSlave_addr") != NULL) {
		printf("%s \r\n", mssg);
		if (linkaddr_node_addr.u8[0]
				== atoi(strncpy(temporal, strstr(mssg, "(") + 1, 3)) //2  for erase the master node address if this slave node are disconnected
				&& linkaddr_node_addr.u8[0]
						== atoi(strncpy(temporal, strstr(mssg, ".") + 1, 3))) { //2  if it doesn't work, erase all code with number 2 as comment.
			master_addr1.u8[0] = 0;											//2
			master_addr1.u8[1] = 0;											//2
		}
	} else if (strstr(mssg, "from") != NULL && strstr(mssg, "tp") != NULL
			&& strstr(mssg, "bat") != NULL) {
		tmp->u8[0] = atoi(strncpy(temporal, strstr(mssg, "(") + 1, 3));
		tmp->u8[1] = atoi(strncpy(temporal, strstr(mssg, ":") + 1, 3));
		//printf("%d tmp : %d:%d \n",strlen(mssg), tmp->u8[0],tmp->u8[1] );
		temperature_data = atoi(strncpy(temporal, strstr(mssg, "tp") + 3, 3));
		battery_data = atoi(strncpy(temporal, strstr(mssg, "bat") + 4, 5));
		printf("node(%d:%d) Temperature = '%dC' \r\n", tmp->u8[0], tmp->u8[1],
				temperature_data);
		sendBatteryStatusByserialP(battery_data, tmp->u8[0], tmp->u8[1]);
	} else if (strcmp(mssg, "M available") != 0
			&& strstr(mssg, "MasterSlave_addr") == NULL
			&& strstr(mssg, "from") == NULL && strstr(mssg, "tp") == NULL
			&& strstr(mssg, "bat") == NULL
			&& linkaddr_cmp(&master_addr1, from)) {

		//if this is a light color data, then handle the led color
		printf("*** Received %u bytes from %u:%u: '0x%08x'\n",
				packetbuf_datalen(), from->u8[0], from->u8[1], temp);
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

		for (i = 0; i < 5; ++i) {
			packetbuf_copyfrom(&temp, sizeof(long));
			broadcast_send(&colour_con);
		}

	}
}

//handle brightness 
static void brightnessRecvd(struct broadcast_conn *c, const linkaddr_t *from) {
//	  leds_toggle(LEDS_ALL);
	lastBRIGHTNESS = *(uint8_t *) packetbuf_dataptr();
	printf("*** Received %u bytes from %u:%u: '0x%08x'\n", packetbuf_datalen(),
			from->u8[0], from->u8[1], lastBRIGHTNESS);
	i2c_single_send(0x39, (LED_BRIGHTNESS | lastBRIGHTNESS));
}

//only master node can handle unicast received data
static void recv_runicast(struct runicast_conn *c, const linkaddr_t *from,
		uint8_t seqno) {
#if MASTERNODE
	char mssg[15];
	char MSaddress[33];
	// firstly convert data in three type, string, battery, temprerature
	short temperature_data = *(short*)packetbuf_dataptr();
	int battery_data = *(int*)packetbuf_dataptr();
	strcpy(mssg, packetbuf_dataptr());
	//printf("message %s received from %d:%d .....\n",mssg, from->u8[0], from->u8[1]);
	if(strcmp(mssg, "Slave")==0||strcmp(mssg, "slave")==0) {
		//if the data is a string, a signal to add new slave node
		printf("MasterSlave_addr data: '%08x' \r\n",(linkaddr_node_addr.u8[0]<<24)|(linkaddr_node_addr.u8[1]<<16)|(from->u8[0]<<8)|(from->u8[1]));

		sprintf(MSaddress,"MasterSlave_addr data: '%08x'",(linkaddr_node_addr.u8[0]<<24)|(linkaddr_node_addr.u8[1]<<16)|(from->u8[0]<<8)|(from->u8[1]));
		//printf("%s %d re \n",MSaddress, strlen(MSaddress));
		packetbuf_copyfrom(MSaddress, strlen(MSaddress));
		broadcast_send(&colour_con);

		printf("message %s received from %d:%d .....%s\n",mssg, from->u8[0], from->u8[1], m_available);
		struct slavesAddr_entry *e = NULL;
		for(e = list_head(slavesAddr_list); e != NULL; e = e->next) { //check if the slave node is or not already connect to this master node
			if(linkaddr_cmp(&e->addr, from)) {
				e->b_bool = 1; //if there is the slave node, then b_bool = 1 and quit this function
				e->checktimes = 0;
				return;
			}
		}

		if(e == NULL) { // if not already connect, then add to the list of slaves nodes and b_bool = 1 then quit this function
			printf("MasterSlave_addr data: '%08x' \r\n",(linkaddr_node_addr.u8[0]<<24)|(linkaddr_node_addr.u8[1]<<16)|(from->u8[0]<<8)|(from->u8[1]));
			e = memb_alloc(&slavesAddr_mem);
			if(e == NULL) {
				e = list_chop(slavesAddr_list); // Remove oldest at full history
			}
			linkaddr_copy(&e->addr, from);
			e->b_bool = 1;
			e->checktimes = 0;
			list_push(slavesAddr_list, e);
		}
		printf("message received from %d:%d, checktimes %d bool : %d\n",from->u8[0], from->u8[1], e->checktimes, e->b_bool);
	} else if (strcmp(mssg, "Be available")==0) //if the data is a string, a signal to enable to have new slave
	{ //then enable to have new slaveofr 15 seconds
		strcpy(m_available, "M available\0");
		stimer_restart(&timer15s);
	}
#endif
}

static void sent_runicast(struct runicast_conn *c, const linkaddr_t *to,
		uint8_t retransmissions) {
	printf("runicast message sent to %d:%d, retransmissions %d\n", to->u8[0],
			to->u8[1], retransmissions);
}
static void timedout_runicast(struct runicast_conn *c, const linkaddr_t *to,
		uint8_t retransmissions) {
	printf(
			"runicast message timed out when sending to %d:%d, retransmissions %d\n",
			to->u8[0], to->u8[1], retransmissions);
}

/*---------------------------------------------------------------------------*/
static const struct broadcast_callbacks colour_rx = { colorRecvd },
		brightness_rx = { brightnessRecvd };

static const struct runicast_callbacks runicast_callbacks = { recv_runicast,
		sent_runicast, timedout_runicast };
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(simpleDemo_start, ev, data)  //process to initialize
{
	PROCESS_BEGIN()
		;

		//configure buttons
		button_sensor.configure(BUTTON_SENSOR_CONFIG_TYPE_INTERVAL,
		BUTTON_PRESS_EVENT_INTERVAL);

		//configure I2C
		i2c_init(I2C_SDA_PORT, I2C_SDA_PIN, I2C_SCL_PORT, I2C_SCL_PIN,
		I2C_SCL_FAST_BUS_SPEED);

		//configure serial line
		serial_line_init();

		//configure braodcast channel
		broadcast_open(&colour_con, COLOUR_CHANNEL, &colour_rx);
		broadcast_open(&brightness_con, BRIGHTNESS_CHANNEL, &brightness_rx);
		runicast_open(&data_runicast, ADDRMasterSlave_CHANNEL,
				&runicast_callbacks);

		//turn all leds off
		i2c_single_send(0x39, 0b00000000);
		master_addr1.u8[0] = 0;
		master_addr1.u8[1] = 0;
		//lock delay
		clock_delay_usec(50);

		//set led brightness to inital brightness
		i2c_single_send(0x39, (LED_BRIGHTNESS | lastBRIGHTNESS));
#if MASTERNODE
		printf("I'm a MASTERNODE\n");
		i2c_single_send(0x39, (LED_RED | 0b00011111));
		i2c_single_send(0x39, (LED_BLUE | 0b00011111));
		i2c_single_send(0x39, (LED_GREEN | 0b00011111));
#endif
	PROCESS_END();
}

PROCESS_THREAD(simpleDemo, ev, data)    //main process
{

PROCESS_EXITHANDLER(broadcast_close(&colour_con)
;
)

PROCESS_EXITHANDLER(broadcast_close(&brightness_con)
;
)

PROCESS_BEGIN()
	;

	char *ch_data, *ptr;
	char t1[2];
	char deleteNode[33];	//2
//char deleteNode[33];  //1
	long i_data;
	short temperature;
	int battery;
	char temp_battery[33];
	uint32_t R, G, B;
	static struct etimer et;
	struct slavesAddr_entry *e = NULL;
	stimer_set(&timer15s, 15);
	stimer_set(&timer30s, 30);
	/* list of slaves nodes */
	list_init(slavesAddr_list);
	memb_init(&slavesAddr_mem);

	etimer_set(&et, 3 * CLOCK_SECOND); //event timer, every 3 seconds , there is a event

	while (1) {
		PROCESS_YIELD()
		;
		if (ev == sensors_event) {
#if MASTERNODE   //only matser can change the brighthness of all its slaves nodes and itself by pushing on its button
			if (data == &button_sensor)
			{

				if (button_sensor.value(BUTTON_SENSOR_VALUE_TYPE_LEVEL) == BUTTON_SENSOR_PRESSED_LEVEL)
				{
					lastBRIGHTNESS = lastBRIGHTNESS + 3;
					if (lastBRIGHTNESS > 0b00011111)
					lastBRIGHTNESS = 0b00000001;
					printf("[Button]: change brightness to: 0b"BYTETOBINARYPATTERN"\n", BYTETOBINARY((LED_BRIGHTNESS | lastBRIGHTNESS)));
					i2c_single_send(0x39, (LED_BRIGHTNESS | lastBRIGHTNESS));

					packetbuf_copyfrom(&lastBRIGHTNESS, sizeof(uint8_t));
					broadcast_send(&brightness_con);
				}

			}
#endif
		} else if (ev == serial_line_event_message) // if there are data sent in the serial port
				{
			ch_data = (char*) data;
			linkaddr_t tmp1;
			if (strcmp(ch_data, "serial") == 0) // if serial port data is a 'serial' signal , it mean that this node will be enable to have new slaves nodes during 15 seconds
					{
				linkaddr_copy(&master_addr1, &linkaddr_node_addr);
				strcpy(m_available, "M available\0");
				printf("%s \r\n", m_available);
				stimer_restart(&timer15s);

			} else if (strstr(ch_data, ":") != NULL) { // if the serial port data has the character  ':'
													   //then it mean it is a node address, the node with this address will be enable
													   //to have new slaves nodes during 15 seconds

				if (stringAddrtoInt_master_availability(&tmp1, ch_data)) // if it is itself
						{
					printf("tmp1 : %d:%d\n", tmp1.u8[0], tmp1.u8[1]);
					strcpy(m_available, "M available\0");
					printf("%s \r\n", m_available);
					stimer_restart(&timer15s);

				} else {
					if (!runicast_is_transmitting(&data_runicast)) { // if it is another node
						packetbuf_copyfrom("Be available\0",
								sizeof("Be available\0"));
						runicast_send(&data_runicast, &tmp1,
								MAX_RETRANSMISSIONS);
					}
					data_runicast.is_tx = 0;
				}
			} else if (strstr(ch_data, "BRIGHTNESS") != NULL) { // is the serial port data is a data for change the brigthness then change it for all nodes
				lastBRIGHTNESS = 0b00000001
						+ 3 * atoi(strncpy(t1, ch_data + 11, 2));
				if (lastBRIGHTNESS > 0b00011111)
					lastBRIGHTNESS = 0b00000001;
				printf("[WEB]: change brightness to: 0b"BYTETOBINARYPATTERN"\n",
						BYTETOBINARY((LED_BRIGHTNESS | lastBRIGHTNESS)));
				i2c_single_send(0x39, (LED_BRIGHTNESS | lastBRIGHTNESS));

				packetbuf_copyfrom(&lastBRIGHTNESS, sizeof(uint8_t));
				broadcast_send(&brightness_con);
			} else { // if it is not all previous serial port data, then it must be a light color data, then handle the light color
				printf("ch_data %s\n", ch_data);
				i_data = strtol(ch_data, &ptr, 16);	//Convert char data to hex

				R = (int) (i_data & 0x00ff0000) >> 19;//Select byte 1 and shift to first position
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

				printf("%d.%d: broadcast_send\n", linkaddr_node_addr.u8[0],
						linkaddr_node_addr.u8[1]);

				packetbuf_copyfrom(&i_data, sizeof(i_data));
				broadcast_send(&colour_con);
			}

		} else if (ev == PROCESS_EVENT_TIMER) {  // 3 seconds timer event

#if MASTERNODE  // every 3 secondes send to database the master address
			printf("MasterSlave_addr data: '%08x' \r\n",(linkaddr_node_addr.u8[0]<<8)|(linkaddr_node_addr.u8[1]));
			//then send to 'available' signal to all nodes which haven't parent, if this node is enable to have new slaves nodes
			if(strcmp(m_available, "M available")==0) {
				packetbuf_copyfrom(m_available, strlen(m_available));
				broadcast_send(&colour_con);
			}
			if (stimer_expired(&timer15s)!=0) //after 15 seconds, disable this node to be able to have new slaves nodes
			{
				strcpy(m_available, "M unavailable\0");
			}
#endif

			if (stimer_expired(&timer30s) != 0)  //every 30 seconds, do ...
					{
				temperature = cc2538_temp_sensor.value(
				CC2538_SENSORS_VALUE_TYPE_CONVERTED) / 1000;
				battery = vdd3_sensor.value(
						CC2538_SENSORS_VALUE_TYPE_CONVERTED);
#if MASTERNODE //only master node,  send by serial port its temperature and battery data
				printf("node(%d:%d) Temperature = '%dC' \r\n",linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1], temperature);
				sendBatteryStatusByserialP(battery, linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1]);

				//then check connectivity of all its slaves nodes
				for(e = list_head(slavesAddr_list); e != NULL; e = e->next) {
					if(e->b_bool == 0) {printf("checktimes %d\n", e->checktimes);
						e->checktimes++;
					} else {printf("checktimes %d  b %d\n", e->checktimes, e->b_bool);
						e->b_bool = 0;
					}
					if (e->checktimes >= 2)
					{	printf("checktimes %d  b %d\n", e->checktimes, e->b_bool);
						printf("MasterSlave_addr data: '%08x' \r\n",(0x00FF0000)|(e->addr.u8[0]<<8)|(e->addr.u8[1]));
						sprintf(deleteNode,"MasterSlave_addr data: '%08x' (%d.%d)",(0x00FF0000)|(e->addr.u8[0]<<8)|(e->addr.u8[1]), (e->addr.u8[0]<<8), (e->addr.u8[1])); //2
						//sprintf(deleteNode,"MasterSlave_addr data: '%08x'",(0x00FF0000)|(e->addr.u8[0]<<8)|(e->addr.u8[1]));  //1
						printf("%s  re \n",deleteNode);
						packetbuf_copyfrom(deleteNode, strlen(deleteNode));
						broadcast_send(&colour_con);
						list_remove(slavesAddr_list, e);
					}
				}

#endif
				//all nodes broadcast their temparature and battery imformation
				sprintf(temp_battery, "from (%d:%d) tp:%dC bat:%d mV ",
						linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1],
						temperature, battery);
				//printf("%d %s \n", strlen(temp_battery),temp_battery);
				packetbuf_copyfrom(temp_battery, strlen(temp_battery));
				broadcast_send(&colour_con);

				stimer_restart(&timer30s);
			}

			//slave node send every 3 seconds a signal to parent node, to prouve, it still is connected to the parent
			if (master_addr1.u8[0] > 0 && master_addr1.u8[1] > 0
					&& !linkaddr_cmp(&master_addr1, &linkaddr_node_addr)) {
				if (!runicast_is_transmitting(&data_runicast)) {
					packetbuf_copyfrom("slave\0", sizeof("slave\0"));
					printf("%u.%u: sending runicast to address %u:%u\n",
							linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1],
							master_addr1.u8[0], master_addr1.u8[1]);
					runicast_send(&data_runicast, &master_addr1,
					MAX_RETRANSMISSIONS);
				}
				data_runicast.is_tx = 0;
			}
			uint16_t temp = random_rand() & 0b00011111;
			printf("I'm a MASTERNODE %d\n",random_rand());
			printf("Red: 0b%b\n",temp);
			i2c_single_send(0x39, (LED_RED | temp));
			temp = random_rand() & 0b00011111;
			printf("Red: 0b%b\n",( temp));
			i2c_single_send(0x39, (LED_BLUE | temp));
			temp = random_rand() & 0b00011111;
			printf("Red: 0b%b\n",( temp));
			i2c_single_send(0x39, (LED_GREEN | temp));
			etimer_set(&et, 3 * CLOCK_SECOND);
		}
	}
PROCESS_END();
}

