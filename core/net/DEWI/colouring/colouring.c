/*
 * cider_new.c
 *
 *  Created on: Oct 19, 2016
 *      Author: root
 */
#include "colouring.h"

#include "contiki.h"
#include "neighTable.h"
#include "dev/leds.h"
#include "project-conf.h"



#if COLOURING_LOG_LEVEL >= 1
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


/***************************************/
/***************************************/
/*				Variables			   */
/***************************************/
/***************************************/


/***************************************/
/***************************************/
/*			    Timers		 		   */
/***************************************/
/***************************************/
struct etimer colouring_timer;

/***************************************/
/***************************************/
/*			Function Headers 		   */
/***************************************/
/***************************************/
static void colouring_packet_received(struct broadcast_conn *c, const linkaddr_t *from);

/***************************************/
/***************************************/
/*			Channel Variables		   */
/***************************************/
/***************************************/
static const struct broadcast_callbacks colouring_bc_rx = { colouring_packet_received };

static struct broadcast_conn colouring_bc;
/***************************************/
/***************************************/
/*			   Processes			   */
/***************************************/
/***************************************/
PROCESS(dewi_coluring_process, "DEWI Coluring PROCESS");
PROCESS_THREAD(dewi_coluring_process, ev, data)
{
	PROCESS_EXITHANDLER(broadcast_close(&colouring_bc))
	broadcast_open(&colouring_bc, BROADCAST_CHANNEL_COLOURING, &colouring_bc_rx);
	PROCESS_BEGIN()
		;

		etimer_set(&colouring_timer, CLOCK_SECOND);
		while (1)
		{
			PROCESS_YIELD()
			;
			if (ev == PROCESS_EVENT_TIMER)
			{
				if (&colouring_timer == data)
				{
				}
			}

		}

	PROCESS_END();
}

/***************************************/
/***************************************/
/*	     Function definitions		   */
/***************************************/
/***************************************/

