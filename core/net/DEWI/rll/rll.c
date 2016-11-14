/*
 * cider_new.c
 *
 *  Created on: Oct 19, 2016
 *      Author: root
 */
#include "rll.h"

#include "contiki.h"
#include "neighTable.h"
#include "dev/leds.h"
#include "project-conf.h"

#if RLL_LOG_LEVEL >= 1
#define DEBUG DEBUG_PRINT
#else /* TSCH_LOG_LEVEL */
#define DEBUG DEBUG_NONE
#endif /* TSCH_LOG_LEVEL */
#include "net/net-debug.h"

/***************************************/
/***************************************/
/*				Macros 				   */
/***************************************/
/***************************************/

#define RLL_INTERVAL       (CLOCK_SECOND * 10)
/***************************************/
/***************************************/
/*				Variables			   */
/***************************************/
/***************************************/

uint8_t RLLSeqNo = 0;
/***************************************/
/***************************************/
/*			    Timers		 		   */
/***************************************/
/***************************************/
static struct etimer RLL_timer;
/***************************************/
/***************************************/
/*			Function Headers 		   */
/***************************************/
/***************************************/
void sendRLLMessage(struct APP_PACKET dataPacket);
static void rll_packet_received(struct broadcast_conn *c,
		const linkaddr_t *from);
/***************************************/
/***************************************/
/*			Channel Variables		   */
/***************************************/
/***************************************/

static struct broadcast_conn rll_broadcast;
static const struct broadcast_callbacks rll_broadcast_rx = { rll_packet_received };

/***************************************/
/***************************************/
/*			   Processes			   */
/***************************************/
/***************************************/
PROCESS(dewi_rll_init_process, "DEWI Coluring PROCESS");
PROCESS(dewi_rll_process, "DEWI Coluring PROCESS");


PROCESS_THREAD(dewi_rll_init_process,ev,data){

	PROCESS_BEGIN()

			PRINTF("[RLL]: RLL open channel\n");
	broadcast_open(&rll_broadcast,BROADCAST_CHANNEL_RLL,&rll_broadcast_rx);
	PROCESS_END();
}

PROCESS_THREAD(dewi_rll_process, ev, data) {
	PROCESS_EXITHANDLER(netflood_close(&rll_broadcast));

	PROCESS_BEGIN()
		;
etimer_set(&RLL_timer,CLOCK_SECOND*5);
		while (1) {
			PROCESS_YIELD();
			if (ev == PROCESS_EVENT_TIMER) {
				printf("[RLL]: RLL active\n");
				etimer_set(&RLL_timer,CLOCK_SECOND*5);
			}

		}
		PROCESS_END();
}


/***************************************/
/***************************************/
/*	     Function definitions		   */
/***************************************/
/***************************************/

void RLL_init(){

	process_start(&dewi_rll_init_process,NULL);
}

void RLL_start(){

	process_start(&dewi_rll_process,NULL);
}
void RLL_notify(){
	PRINTF("[RLL]: RLL is active, set schedule\n");
}

static void rll_packet_received(struct broadcast_conn *c,
		const linkaddr_t *from) {

	struct RLL_PACKET *temp = packetbuf_dataptr();

	//applicationDataCallback(temp->appData,0);
}




void sendRLLMessage(struct APP_PACKET dataPacket){
	struct RLL_PACKET RLLPacket;
	RLLPacket.base.dst = linkaddr_null;
	RLLPacket.base.src = linkaddr_node_addr;
	RLLPacket.base.type = RLL;
	RLLPacket.subType = RLL_DATA;
	RLLPacket.appData = dataPacket;
}

