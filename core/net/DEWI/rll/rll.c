/*
 * cider_new.c
 *
 *  Created on: Oct 19, 2016
 *      Author: root
 */

#include "rll.h"

#include "/usr/arm-none-eabi/include/sys/_stdint.h"
#include "../../../../examples/DEWI/project-conf.h"
#include "../../../sys/cc.h"
#include "../../../sys/clock.h"
#include "../../../sys/mt.h"
#include "../../../sys/process.h"
#include "../../linkaddr.h"
#include "../../mac/tsch/tsch-asn.h"
#include "../../mac/tsch/tsch-private.h"
#include "../../mac/tsch/tsch-queue.h"
#include "../../mac/tsch/tsch-schedule.h"
#include "../../packetbuf.h"
#include "../../rime/broadcast.h"
#include "../colouring/colouring.h"
#include "../neighTable/neighTable.h"
#include "../scheduler/scheduler.h"

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

int RLLSeqNo = 0;
int lastRxSeqNo = -1;
uint8_t RLL_started = 0;
uint8_t RLL_sendCounter = 0;
uint8_t RLL_schedule_set = 0;
uint8_t RLL_CIDERState = 0;
uint8_t RLLNumChildCH = 0;
uint8_t RLLNumCS = 0;
int8_t RLLTier = -1;
uint8_t RLL_lock = 0;
static struct mt_thread parent_thread;
static struct mt_thread child_thread;
static struct mt_thread cs_thread;
static struct mt_thread app_thread;
static struct mt_thread send_thread;
struct RLL_PACKET RLLPacket;
struct RLL_PACKET RLLParentPacket;
struct RLL_PACKET RLLChildPacket;
struct RLL_PACKET RLLCSPacket;
struct APP_PACKET RLLAppPacket;
struct APP_PACKET RLLSendPacket;
uint8_t lastTimeslotSend = 0;
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
void sendRLLPingMessage();
static void thread_parent(struct RLL_PACKET *data);
static void thread_cs(struct RLL_PACKET *data);
static void thread_child(struct RLL_PACKET *data);
static void thread_app(struct APP_PACKET *data);
static void thread_send(struct APP_PACKET *data);
static void rll_packet_received(struct broadcast_conn *c, const linkaddr_t *from);
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

/***************************************/
/***************************************/
/*	     Function definitions		   */
/***************************************/
/***************************************/

void RLL_init()
{
	RLLSeqNo = random_rand();
	process_start(&dewi_rll_init_process, NULL);
}

void RLL_start()
{

	if (getTier() == 0 && RLL_started == 0)
	{
		process_start(&dewi_rll_process, NULL);
		sendRLLPingMessage();
		setActiveSchedule(2);
		setActiveProtocol(2);
	}
	else if (RLL_started == 0)
	{
		process_start(&dewi_rll_process, NULL);
		RLL_started = 1;
		setActiveSchedule(2);
		setActiveProtocol(2);
	}
}
void RLL_notify()
{
	PRINTF("[RLL]: RLL_notify()\n");
	if (getColour() != -1 || getCIDERState() == 7)
	{
		if (RLL_started == 0)
		{
			PRINTF("[RLL]: RLL_started == 0\n");PRINTF("[RLL]: RLL is active, set schedule\n");
			setActiveProtocol(2);
			process_start(&dewi_rll_process, NULL);
			RLL_started = 1;
			if (RLL_schedule_set == 0)
			{
				setRLLSchedule();
				RLL_schedule_set = 1;
			}

		}
		else if (RLL_started == 1)
		{

			PRINTF("[RLL]: RLL_started == 1\n");
			if (RLL_schedule_set == 1)
			{
				tsch_schedule_print();
				RLL_schedule_set = 2;
			}
		}
	}
	else
	{
		COLOURING_start();
	}

}

static void thread_parent(struct RLL_PACKET *data)
{
	struct tsch_link *temp;
	uint8_t timeslot = 0;
	uint8_t baseTimeslot = 0;
	uint8_t currentTimeslot = 0;
	uint8_t currentCalcTimeslot;
	int8_t tx1 = -1, tx2 = -1;
	struct RLL_PACKET *tempPacket = data;
	while (1)
	{
		if (RLLTier != 0)
		{
//PRINTF("[RLL]: Received from: %d, forward to Parent at: asn-%x.%lx\n",direction,current_asn.ms1b,current_asn.ls4b);
			if (RLLTier % 2 == 0)
			{
				temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
				tx1 = 2;
				tx2 = 6;

				currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;
				if (currentCalcTimeslot == tx1)
				{
					timeslot = temp->timeslot;
					baseTimeslot = timeslot - tx1;
					currentTimeslot = timeslot;
				}
				else if (currentCalcTimeslot == tx2)
				{

					timeslot = temp->timeslot;
					baseTimeslot = timeslot - tx2;
					currentTimeslot = timeslot;
				}
				else
				{
					baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
					currentTimeslot = temp->timeslot - baseTimeslot;
					if (currentTimeslot < tx1)
						timeslot = baseTimeslot + tx1;
					else if (currentTimeslot < tx2)
						timeslot = baseTimeslot + tx2;
					else timeslot = baseTimeslot + 10 + tx1;

					if (timeslot > 50) timeslot = tx1;
				}PRINTF("[RLL]:Send to Parent \%2=0 base timeslot: %d, currentTimeslot: %d, send timeslot: %d at: asn-%x.%lx\n",baseTimeslot,currentTimeslot, timeslot,current_asn.ms1b,current_asn.ls4b);
			}
			else
			{
				tx1 = 4;
				tx2 = 8;
				temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
				currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;
				if (currentCalcTimeslot == tx1)
				{
					timeslot = temp->timeslot;
					baseTimeslot = timeslot - tx1;
					currentTimeslot = timeslot;
				}
				else if (currentCalcTimeslot == tx2)
				{

					timeslot = temp->timeslot;
					baseTimeslot = timeslot - tx2;
					currentTimeslot = timeslot;
				}
				else
				{
					baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
					currentTimeslot = temp->timeslot - baseTimeslot;
					if (currentTimeslot < tx1)
						timeslot = baseTimeslot + tx1;
					else if (currentTimeslot < tx2)
						timeslot = baseTimeslot + tx2;
					else timeslot = baseTimeslot + 10 + tx1;

					if (timeslot > 50) timeslot = tx1;
				}PRINTF("[RLL]:Send to Parent \%2=1 base timeslot: %d, currentTimeslot: %d, send timeslot: %d at: asn-%x.%lx\n",baseTimeslot,currentTimeslot, timeslot,current_asn.ms1b,current_asn.ls4b);
			}

			packetbuf_copyfrom(tempPacket, sizeof(struct RLL_PACKET));
			packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
			packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, timeslot);
			broadcast_send(&rll_broadcast);
		}
		mt_yield();
	}
	mt_exit();
}
static void thread_cs(struct RLL_PACKET *data)
{
	struct tsch_link *temp;
	uint8_t timeslot = 0;
	uint8_t baseTimeslot = 0;
	uint8_t currentTimeslot = 0;
	uint8_t currentCalcTimeslot;
	int8_t tx1 = -1, tx2 = -1;
	struct RLL_PACKET *tempPacket = data;
	while (1)
	{
		PRINTF("[RLL]: forward to CS at: asn-%x.%lx\n",current_asn.ms1b,current_asn.ls4b);PRINTF("[RLL]: currentCalcTimeslot: %d, timeslot: %d\n",currentCalcTimeslot,temp->timeslot);
		temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
		currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;

		if (currentCalcTimeslot != 0)
		{
			baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
			currentTimeslot = temp->timeslot - baseTimeslot;
			timeslot = baseTimeslot + 10;
			if(timeslot == temp->timeslot)
				timeslot = timeslot + 10;

			if (timeslot > 50) timeslot = 10;
		}
		else if (currentCalcTimeslot == 0 && temp->timeslot == 0)
		{
			timeslot = 10;
			baseTimeslot = 0;
			currentTimeslot = 0;
		}
		PRINTF("[RLL]:Send to CS base timeslot: %d, currentTimeslot: %d, send timeslot: %d at: asn-%x.%lx\n",baseTimeslot,currentTimeslot, timeslot,current_asn.ms1b,current_asn.ls4b);
		packetbuf_copyfrom(tempPacket, sizeof(struct RLL_PACKET));
		packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
		packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, timeslot);
		broadcast_send(&rll_broadcast);
		mt_yield();
	}
	mt_exit();
}
static void thread_child(struct RLL_PACKET *data)
{
	struct tsch_link *temp;
	uint8_t timeslot = 0;
	uint8_t baseTimeslot = 0;
	uint8_t currentTimeslot = 0;
	uint8_t currentCalcTimeslot;
	int8_t tx1 = -1, tx2 = -1;
	struct RLL_PACKET *tempPacket = data;
	while (1)
	{
		if (RLLTier % 2 == 0)
		{
			tx1 = 5;
			tx2 = 9;
			temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
			currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;
			if (currentCalcTimeslot == tx1)
			{
				timeslot = temp->timeslot;
				baseTimeslot = timeslot - tx1;
				currentTimeslot = timeslot;
			}
			else if (currentCalcTimeslot == tx2)
			{

				timeslot = temp->timeslot;
				baseTimeslot = timeslot - tx2;
				currentTimeslot = timeslot;
			}
			else
			{
				baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
				currentTimeslot = temp->timeslot - baseTimeslot;
				if (currentTimeslot < tx1)
					timeslot = baseTimeslot + tx1;
				else if (currentTimeslot < tx2)
					timeslot = baseTimeslot + tx2;
				else timeslot = baseTimeslot + 10 + tx1;

				if (timeslot > 50) timeslot = tx1;
			}
		PRINTF("[RLL]:Send to Parent \%2=0 base timeslot: %d, currentTimeslot: %d, send timeslot: %d at: asn-%x.%lx\n",baseTimeslot,currentTimeslot, timeslot,current_asn.ms1b,current_asn.ls4b);
		}
		else
		{

			currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;
			temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
			tx1 = 3;
			tx2 = 7;
			if (currentCalcTimeslot == tx1)
			{
				timeslot = temp->timeslot;
				baseTimeslot = timeslot - tx1;
				currentTimeslot = timeslot;
			}
			else if (currentCalcTimeslot == tx2)
			{

				timeslot = temp->timeslot;
				baseTimeslot = timeslot - tx2;
				currentTimeslot = timeslot;
			}
			else
			{
				baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
				currentTimeslot = temp->timeslot - baseTimeslot;
				if (currentTimeslot < tx1)
					timeslot = baseTimeslot + tx1;
				else if (currentTimeslot < tx2)
					timeslot = baseTimeslot + tx2;
				else timeslot = baseTimeslot + 10 + tx1;

				if (timeslot > 50) timeslot = tx1;
			}
			PRINTF("[RLL]:Send to Parent \%2=1 base timeslot: %d, currentTimeslot: %d, send timeslot: %d at: asn-%x.%lx\n",baseTimeslot,currentTimeslot, timeslot,current_asn.ms1b,current_asn.ls4b);
		}
		packetbuf_copyfrom(tempPacket, sizeof(struct RLL_PACKET));
		packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
		packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, timeslot);
		broadcast_send(&rll_broadcast);
		mt_yield();
	}
	mt_exit();
}

static void thread_app(struct APP_PACKET *data)
{

	while (1)
	{
		struct APP_PACKET *temp = data;
		APPDATACALLBACK(temp);

		mt_yield();
	}
	mt_exit();
}

static void thread_send(struct APP_PACKET *data)
{

	while (1)
	{
		struct APP_PACKET *temp = data;
		RLLPacket.base.dst = linkaddr_null;
		RLLPacket.base.src = linkaddr_node_addr;
		RLLPacket.base.type = RLL;
		RLLPacket.subType = RLL_DATA;
		RLLPacket.appData = *data;
		RLLPacket.seqNo = RLLSeqNo++;
		RLLPacket.initalSRC = linkaddr_node_addr;
		lastRxSeqNo = RLLSeqNo - 1;
		printf("[APP]: try to send msg\n");
		if (RLL_CIDERState == 7)
		{
			struct tsch_link *temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
			uint8_t timeslot = 0;
			uint8_t baseTimeslot = 0;
			uint8_t currentTimeslot = 0;
			uint8_t currentCalcTimeslot = 0;
			uint8_t tx1 = 1;
			temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
			currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;
//						if (currentCalcTimeslot == tx1)
//						{
//							timeslot = temp->timeslot;
//							baseTimeslot = timeslot - tx1;
//							currentTimeslot = timeslot;
//						}
//						else
//						{
			baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
			currentTimeslot = temp->timeslot - baseTimeslot;
			if (currentTimeslot < tx1 - 1)
				timeslot = baseTimeslot + tx1;
			else timeslot = baseTimeslot + 10 + tx1;

			if (timeslot > 50) timeslot = tx1;
						PRINTF("[RLL]:Send to Parent \%2=0 base timeslot: %d, currentTimeslot: %d, send timeslot: %d at: asn-%x.%lx\n",baseTimeslot,currentTimeslot, timeslot,current_asn.ms1b,current_asn.ls4b);


			if (lastTimeslotSend != timeslot)
			{
				lastTimeslotSend = timeslot;
				PRINTF("[RLL]:Send to Parent \%2=1 base timeslot: %d, currentTimeslot: %d, send timeslot: %d at: asn-%x.%lx\n",baseTimeslot,currentTimeslot, timeslot,current_asn.ms1b,current_asn.ls4b);

				packetbuf_copyfrom(&RLLPacket, sizeof(struct RLL_PACKET));
				packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
				packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, timeslot);
				broadcast_send(&rll_broadcast);
			}

		}
		if (RLL_CIDERState == 5)
		{
			struct tsch_link *temp;
			uint8_t timeslot = 0;
			uint8_t baseTimeslot = 0;
			uint8_t currentTimeslot = 0;
			uint8_t currentCalcTimeslot;
			int8_t tx1 = -1, tx2 = -1;
			if (RLLTier != 0)
			{
				if (RLLTier % 2 == 0)
				{
					tx1 = 2;
					tx2 = 6;
					temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
					currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;
					if (currentCalcTimeslot == tx1)
					{
						timeslot = temp->timeslot;
						baseTimeslot = timeslot - tx1;
						currentTimeslot = timeslot;
					}
					else if (currentCalcTimeslot == tx2)
					{

						timeslot = temp->timeslot;
						baseTimeslot = timeslot - tx2;
						currentTimeslot = timeslot;
					}
					else
					{
						baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
						currentTimeslot = temp->timeslot - baseTimeslot;
						if (currentTimeslot < tx1)
							timeslot = baseTimeslot + tx1;
						else if (currentTimeslot < tx2)
							timeslot = baseTimeslot + tx2;
						else timeslot = baseTimeslot + 10 + tx1;

						if (timeslot > 50) timeslot = tx1;
					}
				}
				else
				{
					tx1 = 4;
					tx2 = 8;
					temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
					currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;
					if (currentCalcTimeslot == tx1)
					{
						timeslot = temp->timeslot;
						baseTimeslot = timeslot - tx1;
						currentTimeslot = timeslot;
					}
					else if (currentCalcTimeslot == tx2)
					{

						timeslot = temp->timeslot;
						baseTimeslot = timeslot - tx2;
						currentTimeslot = timeslot;
					}
					else
					{
						baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
						currentTimeslot = temp->timeslot - baseTimeslot;
						if (currentTimeslot < tx1)
							timeslot = baseTimeslot + tx1;
						else if (currentTimeslot < tx2)
							timeslot = baseTimeslot + tx2;
						else timeslot = baseTimeslot + 10 + tx1;

						if (timeslot > 50) timeslot = tx1;
					}
				}

				packetbuf_copyfrom(&RLLPacket, sizeof(struct RLL_PACKET));
				packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
				packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, timeslot);
				broadcast_send(&rll_broadcast);
			}
			if (RLLNumChildCH != 0)
			{
				if (RLLTier % 2 == 0)
				{
					tx1 = 5;
					tx2 = 9;
					temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
					currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;
					if (currentCalcTimeslot == tx1)
					{
						timeslot = temp->timeslot;
						baseTimeslot = timeslot - tx1;
						currentTimeslot = timeslot;
					}
					else if (currentCalcTimeslot == tx2)
					{

						timeslot = temp->timeslot;
						baseTimeslot = timeslot - tx2;
						currentTimeslot = timeslot;
					}
					else
					{
						baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
						currentTimeslot = temp->timeslot - baseTimeslot;
						if (currentTimeslot < tx1)
							timeslot = baseTimeslot + tx1;
						else if (currentTimeslot < tx2)
							timeslot = baseTimeslot + tx2;
						else timeslot = baseTimeslot + 10 + tx1;

						if (timeslot > 50) timeslot = tx1;
					}
				}
				else
				{
					temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
					currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;
					tx1 = 3;
					tx2 = 7;
					if (currentCalcTimeslot == tx1)
					{
						timeslot = temp->timeslot;
						baseTimeslot = timeslot - tx1;
						currentTimeslot = timeslot;
					}
					else if (currentCalcTimeslot == tx2)
					{

						timeslot = temp->timeslot;
						baseTimeslot = timeslot - tx2;
						currentTimeslot = timeslot;
					}
					else
					{
						baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
						currentTimeslot = temp->timeslot - baseTimeslot;
						if (currentTimeslot < tx1)
							timeslot = baseTimeslot + tx1;
						else if (currentTimeslot < tx2)
							timeslot = baseTimeslot + tx2;
						else timeslot = baseTimeslot + 10 + tx1;

						if (timeslot > 50) timeslot = tx1;
					}
				}
				packetbuf_copyfrom(&RLLPacket, sizeof(struct RLL_PACKET));
				packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
				packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, timeslot);
				broadcast_send(&rll_broadcast);
			}
			if (RLLNumCS != 0)
			{
				temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
				currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;

				if (currentCalcTimeslot != 0)
				{
					baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
					currentTimeslot = temp->timeslot - baseTimeslot;
					timeslot = baseTimeslot + 10;
					if (timeslot > 50) timeslot = 10;
				}
				else if (currentCalcTimeslot == 0 && temp->timeslot == 0)
				{
					timeslot = 10;
					baseTimeslot = 0;
					currentTimeslot = 0;
				}
				else
				{
					timeslot = temp->timeslot;
					baseTimeslot = timeslot - 10;
					currentTimeslot = timeslot;
				}
				packetbuf_copyfrom(&RLLPacket, sizeof(struct RLL_PACKET));
				packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
				packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, timeslot);
				broadcast_send(&rll_broadcast);
			}

		}

		mt_yield();
	}
	mt_exit();
}

static void rll_packet_received(struct broadcast_conn *c, const linkaddr_t *from)
{

	struct RLL_PACKET *tempPacket = packetbuf_dataptr();

	int8_t direction = -2;
	switch (tempPacket->subType)
	{
		case RLL_DATA:
			if (tempPacket->seqNo == lastRxSeqNo)
			{
				PRINTF("[RLL]: Packet received, but outdated. Current SeqNo: %d, received SeqNo: %d\n",lastRxSeqNo,tempPacket->seqNo);

			}else if(tempPacket->initalSRC.u16 == linkaddr_node_addr.u16)
			{
				PRINTF("[RLL]: Packet originates to me, do nothing\n",RLLSeqNo,tempPacket->seqNo);
			}
			else{
				//tsch_queue_reset();
				if (RLL_CIDERState == 5)
				{
					struct tsch_link *temp;
					uint8_t timeslot = 0;
					uint8_t baseTimeslot = 0;
					uint8_t currentTimeslot = 0;
					uint8_t currentCalcTimeslot;
					int8_t tx1 = -1, tx2 = -1;

					lastRxSeqNo = tempPacket->seqNo;

					direction = getReceiveDirection(tempPacket->base.src);
					//	PRINTF("[RLL]: received RLL Data MSG at: asn-%x.%lx, forward direction: %d\n",current_asn.ms1b,current_asn.ls4b,direction);
					if (direction == -1 || direction == 0)
					{

						RLLParentPacket.base.src = linkaddr_node_addr;
						RLLParentPacket.base.dst = tempPacket->base.dst;
						RLLParentPacket.base.type = tempPacket->base.type;
						RLLParentPacket.appData = tempPacket->appData;
						RLLParentPacket.seqNo = tempPacket->seqNo;
						RLLParentPacket.subType = tempPacket->subType;

						mt_exec(&parent_thread);

					}
					if ((direction == -1 && RLLNumChildCH > 1)
							|| (direction >= 0 && RLLNumChildCH != 0))
					{
//				PRINTF("[RLL]: Received from: %d, forward to ChildCH at: asn-%x.%lx\n",direction,current_asn.ms1b,current_asn.ls4b);
						RLLChildPacket.base.src = linkaddr_node_addr;
						RLLChildPacket.base.dst = tempPacket->base.dst;
						RLLChildPacket.base.type = tempPacket->base.type;
						RLLChildPacket.appData = tempPacket->appData;
						RLLChildPacket.seqNo = tempPacket->seqNo;
						RLLChildPacket.subType = tempPacket->subType;

						mt_exec(&child_thread);

					}
					if (((direction == -1 || direction == 1) && RLLNumCS != 0)
							|| (direction == 0 && RLLNumCS > 1))
					{
//			RLLChildPacket.base.src = linkaddr_node_addr;
						RLLCSPacket.base.dst = tempPacket->base.dst;
						RLLCSPacket.base.type = tempPacket->base.type;
						RLLCSPacket.appData = tempPacket->appData;
						RLLCSPacket.seqNo = tempPacket->seqNo;
						RLLCSPacket.subType = tempPacket->subType;

						mt_exec(&cs_thread);
					}

				}
				//PRINTF("[RLL]: Data packet type: %d\n",tempPacket->appData.subType);
				RLLAppPacket = tempPacket->appData;
				mt_exec(&app_thread);
				//APPDATACALLBACK(&RLLDataPacket);
			}
			break;
		case RLL_PING:
			setActiveSchedule(2);
			setActiveProtocol(2);
			if (RLL_started == 0) RLL_notify();
			if (getCIDERState() == 5) sendRLLPingMessage();
			break;

	}
}

void sendRLLDataMessage(struct APP_PACKET dataPacket, uint8_t lock)
{
	RLL_lock = lock;
	PRINTF("[RLL]: sendRLLDataMessage, RLL_Started: %d, msg received from APP at: asn-%x.%lx\n",RLL_started,current_asn.ms1b,current_asn.ls4b);
	if (RLL_started == 1)
	{
		RLLSendPacket = dataPacket;

		mt_exec(&send_thread);

	}
}

void RLL_reset()
{
	process_exit(&dewi_rll_process);
	RLL_started = 0;
	RLL_sendCounter = 0;
	RLL_schedule_set = 0;
	etimer_stop(&RLL_timer);
}

void sendRLLPingMessage()
{
	if (RLL_sendCounter < 2)
	{
		struct RLL_PACKET RLLPacket;
		RLLPacket.base.dst = linkaddr_null;
		RLLPacket.base.src = linkaddr_node_addr;
		RLLPacket.base.type = RLL;
		RLLPacket.subType = RLL_PING;
		RLL_sendCounter++;
		packetbuf_copyfrom(&RLLPacket, sizeof(struct RLL_PACKET));

		packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
		packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, 0);

		broadcast_send(&rll_broadcast);
	}
}

PROCESS_THREAD(dewi_rll_init_process,ev,data)
{

	PROCESS_BEGIN()

		PRINTF("[RLL]: RLL open channel\n");
		broadcast_open(&rll_broadcast, BROADCAST_CHANNEL_RLL, &rll_broadcast_rx);
	PROCESS_END();
}

PROCESS_THREAD(dewi_rll_process, ev, data)
{
PROCESS_EXITHANDLER(netflood_close(&rll_broadcast));

PROCESS_BEGIN()
	;
	mt_init();
	mt_start(&parent_thread, thread_parent, &RLLParentPacket);
	mt_start(&child_thread, thread_child, &RLLChildPacket);
	mt_start(&cs_thread, thread_cs, &RLLCSPacket);
	mt_start(&app_thread, thread_app, &RLLAppPacket);
	mt_start(&send_thread, thread_send, &RLLSendPacket);
	etimer_set(&RLL_timer, CLOCK_SECOND * 5);
	while (1)
	{
		PROCESS_YIELD()
		;
		if (ev == PROCESS_EVENT_TIMER)
		{
			PRINTF("[RLL]: RLL active\n");
			if (RLL_schedule_set == 0)
			{
				setRLLSchedule();
				RLL_schedule_set = 1;
			}
			RLL_CIDERState = getCIDERState();
			RLLNumChildCH = getNumChildCH();
			RLLNumCS = getNumCS();
			RLLTier = getTier();
			if (RLL_CIDERState == 5)
				PRINTF("[RLL]: Tier: %d, NumChildCH: %d, NumCS: %d\n",RLLTier,RLLNumChildCH,RLLNumCS);
			etimer_set(&RLL_timer, CLOCK_SECOND * 5);
		}

	}
	mt_stop(&parent_thread);
	mt_stop(&child_thread);
	mt_stop(&cs_thread);
	mt_stop(&app_thread);
	mt_stop(&send_thread);
	mt_remove();
PROCESS_END();
}
