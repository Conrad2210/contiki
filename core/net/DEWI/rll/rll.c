/*
 * cider_new.c
 *
 *  Created on: Oct 19, 2016
 *      Author: root
 */

#include "rll.h"

//#include "/usr/arm-none-eabi/include/sys/_stdint.h"
#include "../../../../examples/DEWI/APP/project-conf.h"
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
static struct mt_thread forward_thread;
static struct mt_thread app_thread;
static struct mt_thread send_thread;
struct RLL_PACKET RLLPacket;
struct RLL_PACKET RLLForwardPacket;
struct APP_PACKET RLLAppPacket;
struct APP_PACKET RLLSendPacket;
uint8_t lastTimeslotSend = 0;
int RLL_receiveTimeslot = 0;

struct forwardType
{
	int timeslot;
	int direction;
};
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
static void thread_forward(struct RLL_PACKET *data);
static void thread_app(struct APP_PACKET *data);
static void thread_send(struct APP_PACKET *data);
static void rll_packet_received(struct broadcast_conn *c, const linkaddr_t *from);
void checkQueue();
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

void checkQueue()
{
	int packetsInQueue = tsch_queue_packet_count(&tsch_broadcast_address);
	if (packetsInQueue > 0)
		tsch_queue_reset();
}

void RLL_RxTimeslot(int timeslot)
{
	RLL_receiveTimeslot = timeslot;
}
void RLL_init()
{
	RLLSeqNo = 0;
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
	} else if (RLL_started == 0)
	{
		process_start(&dewi_rll_process, NULL);
		RLL_started = 1;
		setActiveSchedule(2);
		setActiveProtocol(2);
	}
}
void RLL_notify()
{
	ANNOTATE("[RLL]: RLL_notify()\n");
	if (getColour() != -1 || getCIDERState() == 7)
	{
		if (RLL_started == 0)
		{
			ANNOTATE("[RLL]: RLL_started == 0\n");ANNOTATE("[RLL]: RLL is active, set schedule\n");
			setActiveProtocol(2);
			process_start(&dewi_rll_process, NULL);
			RLL_started = 1;
			if (RLL_schedule_set == 0)
			{
				setRLLSchedule();
				RLL_schedule_set = 1;
			}

		} else if (RLL_started == 1)
		{

			ANNOTATE("[RLL]: RLL_started == 1\n");
			if (RLL_schedule_set == 1)
			{
				tsch_schedule_print();
				RLL_schedule_set = 2;
			}
		}
	} else
	{
		COLOURING_start();
	}

}
static void thread_forward(struct RLL_PACKET *data)
{

	struct tsch_link *temp;
	int8_t timeslot = 0;
	uint8_t baseTimeslot = 0;
	uint8_t currentTimeslot = 0;
	uint8_t currentCalcTimeslot;
	int8_t tx1 = -1, tx2 = -1;
	struct forwardType sendSlots[3];
	int8_t direction = -2;
	int8_t i = 0;
	while (1)
	{
		for (i = 0; i < 3; i++)
		{
			sendSlots[i].timeslot = -1;
			sendSlots[i].direction = -2;
		}
		direction = -2;
		timeslot = -1;
		PRINTF("[RLL]: Packet received with direction %d\n",data->direction);
		if (data->direction == 0)
			direction = 0;
		else if (data->direction == 1)
			direction = 1;
		else if (data->direction == -1)
			direction = -1;

		lastRxSeqNo = data->seqNo;

		data->base.src = linkaddr_node_addr;

		switch (direction)
		{
			case 1:
				/*
				 * Case 1: Forward to CS and ChildCH if >0			 *
				 */
				if (getNumChildCH() > 0)
				{
					timeslot = -1;
					if (RLLTier % 2 == 0)
					{
						tx1 = 5;
						tx2 = 9;
						temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
						currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;
						baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
						currentTimeslot = temp->timeslot - baseTimeslot;
						if (currentTimeslot < tx1)
							timeslot = baseTimeslot + tx1;
						else if (currentTimeslot < tx2)
							timeslot = baseTimeslot + tx2;
						else
							timeslot = baseTimeslot + 10 + tx1;

						if (timeslot > 50)
							timeslot = tx1;
					} else
					{

						currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;
						temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
						tx1 = 3;
						tx2 = 7;

						baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
						currentTimeslot = temp->timeslot - baseTimeslot;
						if (currentTimeslot < tx1)
							timeslot = baseTimeslot + tx1;
						else if (currentTimeslot < tx2)
							timeslot = baseTimeslot + tx2;
						else
							timeslot = baseTimeslot + 10 + tx1;

						if (timeslot > 50)
							timeslot = tx1;

					}PRINTF("[RLL]:Send to Child CH \%2=0 base timeslot: %d, currentTimeslot: %d, send timeslot: %d at: asn-%x.%lx\n",baseTimeslot,currentTimeslot, timeslot,current_asn.ms1b,current_asn.ls4b);

					sendSlots[1].timeslot = timeslot;
					sendSlots[1].direction = 1;
				}

				if (getNumCS() > 0)
				{
					timeslot = -1;
					temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
					currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;

					if (currentCalcTimeslot != 0)
					{
						baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
						currentTimeslot = temp->timeslot - baseTimeslot;
						timeslot = baseTimeslot + 10;
						if (timeslot == temp->timeslot)
							timeslot = timeslot + 10;

						if (timeslot > 50)
							timeslot = 10;
					} else if (currentCalcTimeslot == 0 && temp->timeslot == 0)
					{
						timeslot = 10;
						baseTimeslot = 0;
						currentTimeslot = 0;
					}PRINTF("[RLL]:Send to CS base timeslot: %d, currentTimeslot: %d, send timeslot: %d at: asn-%x.%lx\n",baseTimeslot,currentTimeslot, timeslot,current_asn.ms1b,current_asn.ls4b);

					sendSlots[2].timeslot = timeslot;
					sendSlots[2].direction = 1;
				}
				break;
			case 0:
				/*
				 * Case 0: Forward to CS if >1, to ChildCH and Parent	 *
				 */
				if (getTier() != 0)
				{
					/*
					 * Forward to parent
					 */
					timeslot = -1;
					if (RLLTier % 2 == 0)
					{
						temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
						tx1 = 2;
						tx2 = 6;

						currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;
						baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
						currentTimeslot = temp->timeslot - baseTimeslot;
						if (currentTimeslot < tx1)
							timeslot = baseTimeslot + tx1;
						else if (currentTimeslot < tx2)
							timeslot = baseTimeslot + tx2;
						else
							timeslot = baseTimeslot + 10 + tx1;

						if (timeslot > 50)
							timeslot = tx1;

					} else
					{
						tx1 = 4;
						tx2 = 8;
						temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
						currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;
						baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
						currentTimeslot = temp->timeslot - baseTimeslot;
						if (currentTimeslot < tx1)
							timeslot = baseTimeslot + tx1;
						else if (currentTimeslot < tx2)
							timeslot = baseTimeslot + tx2;
						else
							timeslot = baseTimeslot + 10 + tx1;

						if (timeslot > 50)
							timeslot = tx1;
						//				}
					}PRINTF("[RLL]:Send to Parent \%2=1 base timeslot: %d, currentTimeslot: %d, send timeslot: %d at: asn-%x.%lx\n",baseTimeslot,currentTimeslot, timeslot,current_asn.ms1b,current_asn.ls4b);

					sendSlots[0].timeslot = timeslot;
					sendSlots[0].direction = -1;
				}

				if (getNumChildCH() > 0)
				{
					/*
					 * Forward to Child CH
					 */
					timeslot = -1;
					if (RLLTier % 2 == 0)
					{
						tx1 = 5;
						tx2 = 9;
						temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
						currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;
						baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
						currentTimeslot = temp->timeslot - baseTimeslot;
						if (currentTimeslot < tx1)
							timeslot = baseTimeslot + tx1;
						else if (currentTimeslot < tx2)
							timeslot = baseTimeslot + tx2;
						else
							timeslot = baseTimeslot + 10 + tx1;

						if (timeslot > 50)
							timeslot = tx1;
					} else
					{

						currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;
						temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
						tx1 = 3;
						tx2 = 7;

						baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
						currentTimeslot = temp->timeslot - baseTimeslot;
						if (currentTimeslot < tx1)
							timeslot = baseTimeslot + tx1;
						else if (currentTimeslot < tx2)
							timeslot = baseTimeslot + tx2;
						else
							timeslot = baseTimeslot + 10 + tx1;

						if (timeslot > 50)
							timeslot = tx1;

					}PRINTF("[RLL]:Send to Child CH \%2=0 base timeslot: %d, currentTimeslot: %d, send timeslot: %d at: asn-%x.%lx\n",baseTimeslot,currentTimeslot, timeslot,current_asn.ms1b,current_asn.ls4b);

					sendSlots[1].timeslot = timeslot;
					sendSlots[1].direction = 1;
				}

				if (getNumCS() > 1)
				{
					/*
					 * Forward to CS
					 */
					timeslot = -1;
					temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
					currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;

					if (currentCalcTimeslot != 0)
					{
						baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
						currentTimeslot = temp->timeslot - baseTimeslot;
						timeslot = baseTimeslot + 10;
						if (timeslot == temp->timeslot)
							timeslot = timeslot + 10;

						if (timeslot > 50)
							timeslot = 10;
					} else if (currentCalcTimeslot == 0 && temp->timeslot == 0)
					{
						timeslot = 10;
						baseTimeslot = 0;
						currentTimeslot = 0;
					}PRINTF("[RLL]:Send to CS base timeslot: %d, currentTimeslot: %d, send timeslot: %d at: asn-%x.%lx\n",baseTimeslot,currentTimeslot, timeslot,current_asn.ms1b,current_asn.ls4b);

					sendSlots[2].timeslot = timeslot;
					sendSlots[2].direction = 1;
				}
				break;
			case -1:
				/*
				 * Case -1: Forward to CS, to ChildCH and Parent	 *
				 */
				if (getTier() != 0)
				{
					timeslot = -1;
					if (RLLTier % 2 == 0)
					{
						temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
						tx1 = 2;
						tx2 = 6;

						currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;
						baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
						currentTimeslot = temp->timeslot - baseTimeslot;
						if (currentTimeslot < tx1)
							timeslot = baseTimeslot + tx1;
						else if (currentTimeslot < tx2)
							timeslot = baseTimeslot + tx2;
						else
							timeslot = baseTimeslot + 10 + tx1;

						if (timeslot > 50)
							timeslot = tx1;

					} else
					{
						tx1 = 4;
						tx2 = 8;
						temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
						currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;
						baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
						currentTimeslot = temp->timeslot - baseTimeslot;
						if (currentTimeslot < tx1)
							timeslot = baseTimeslot + tx1;
						else if (currentTimeslot < tx2)
							timeslot = baseTimeslot + tx2;
						else
							timeslot = baseTimeslot + 10 + tx1;

						if (timeslot > 50)
							timeslot = tx1;
						//				}
					}PRINTF("[RLL]:Send to Parent \%2=1 base timeslot: %d, currentTimeslot: %d, send timeslot: %d at: asn-%x.%lx\n",baseTimeslot,currentTimeslot, timeslot,current_asn.ms1b,current_asn.ls4b);

					sendSlots[0].timeslot = timeslot;
					sendSlots[0].direction = -1;
				}

				if (getNumChildCH() > 1)
				{
					timeslot = -1;
					if (RLLTier % 2 == 0)
					{
						tx1 = 5;
						tx2 = 9;
						temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
						currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;
						baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
						currentTimeslot = temp->timeslot - baseTimeslot;
						if (currentTimeslot < tx1)
							timeslot = baseTimeslot + tx1;
						else if (currentTimeslot < tx2)
							timeslot = baseTimeslot + tx2;
						else
							timeslot = baseTimeslot + 10 + tx1;

						if (timeslot > 50)
							timeslot = tx1;
					} else
					{

						currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;
						temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
						tx1 = 3;
						tx2 = 7;

						baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
						currentTimeslot = temp->timeslot - baseTimeslot;
						if (currentTimeslot < tx1)
							timeslot = baseTimeslot + tx1;
						else if (currentTimeslot < tx2)
							timeslot = baseTimeslot + tx2;
						else
							timeslot = baseTimeslot + 10 + tx1;

						if (timeslot > 50)
							timeslot = tx1;

					}PRINTF("[RLL]:Send to Child CH \%2=0 base timeslot: %d, currentTimeslot: %d, send timeslot: %d at: asn-%x.%lx\n",baseTimeslot,currentTimeslot, timeslot,current_asn.ms1b,current_asn.ls4b);

					sendSlots[1].timeslot = timeslot;
					sendSlots[1].direction = 1;
				}

				if (getNumCS() > 0)
				{
					timeslot = -1;
					temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
					currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;

					if (currentCalcTimeslot != 0)
					{
						baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
						currentTimeslot = temp->timeslot - baseTimeslot;
						timeslot = baseTimeslot + 10;
						if (timeslot == temp->timeslot)
							timeslot = timeslot + 10;

						if (timeslot > 50)
							timeslot = 10;
					} else if (currentCalcTimeslot == 0 && temp->timeslot == 0)
					{
						timeslot = 10;
						baseTimeslot = 0;
						currentTimeslot = 0;
					}PRINTF("[RLL]:Send to CS base timeslot: %d, currentTimeslot: %d, send timeslot: %d at: asn-%x.%lx\n",baseTimeslot,currentTimeslot, timeslot,current_asn.ms1b,current_asn.ls4b);

					sendSlots[2].timeslot = timeslot;
					sendSlots[2].direction = 1;
				}
				break;
		}

		i = 0;
		int x, y, n = 3;
		for (x = 0; x < n; x++)
		{
			for (y = 0; y < n - 1; y++)
			{
				if (sendSlots[y].timeslot > sendSlots[y + 1].timeslot)
				{
					struct forwardType temp = sendSlots[y + 1];
					sendSlots[y + 1] = sendSlots[y];
					sendSlots[y] = temp;
				}
			}
		}

		if (data->priority == 1)
			checkQueue();

		for (i = 0; i < 3; i++)
		{
			if (sendSlots[i].timeslot != -1)
			{

				data->direction = sendSlots[i].direction;

				PRINTF("[RLL]: Forward packet: timeslot: %d, direction: %d\n",sendSlots[i].timeslot,sendSlots[i].direction);
				packetbuf_copyfrom(data, sizeof(struct RLL_PACKET));
				packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
				packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, sendSlots[i].timeslot);
				broadcast_send(&rll_broadcast);
			}
		}

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
		uint8_t timeslotOffset = 0;
		RLLPacket.base.dst = tsch_broadcast_address;
		RLLPacket.base.src = linkaddr_node_addr;
		RLLPacket.base.type = RLL;
		RLLPacket.subType = RLL_DATA;
		RLLPacket.appData = *data;
		RLLPacket.seqNo = RLLSeqNo++;
		RLLPacket.initalSRC = linkaddr_node_addr;

		RLLPacket.priority = RLL_lock;
		lastRxSeqNo = RLLSeqNo - 1;
		ANNOTATE("[APP]: try to send msg\n");
		if (RLL_CIDERState == 7)
		{
			RLLPacket.direction = 0;
			struct tsch_link *temp = tsch_schedule_get_next_active_link(&current_asn, &timeslotOffset, NULL);
			uint8_t timeslot = 0;
			uint8_t baseTimeslot = 0;
			uint8_t currentTimeslot = 0;
			uint8_t currentCalcTimeslot = 0;
			uint8_t tx1 = 1;
			temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
			currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;

			baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
			currentTimeslot = temp->timeslot - baseTimeslot;
			timeslot = baseTimeslot + 10 + tx1;

			if (timeslot > 50)
				timeslot = tx1;
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
			int i = 0;
			struct forwardType sendSlots[3];
			for (i = 0; i < 3; i++)
			{
				sendSlots[i].timeslot = -1;
				sendSlots[i].direction = -2;
			}
			if (RLLPacket.priority == 1)
				checkQueue();
			if (getTier() != 0)
			{
				/*
				 * Forward to parent
				 */
				timeslot = -1;
				if (RLLTier % 2 == 0)
				{
					temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
					tx1 = 2;
					tx2 = 6;

					currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;
					baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
					currentTimeslot = temp->timeslot - baseTimeslot;
					if (currentTimeslot < tx1)
						timeslot = baseTimeslot + tx1;
					else if (currentTimeslot < tx2)
						timeslot = baseTimeslot + tx2;
					else
						timeslot = baseTimeslot + 10 + tx1;

					if (timeslot > 50)
						timeslot = tx1;

				} else
				{
					tx1 = 4;
					tx2 = 8;
					temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
					currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;
					baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
					currentTimeslot = temp->timeslot - baseTimeslot;
					if (currentTimeslot < tx1)
						timeslot = baseTimeslot + tx1;
					else if (currentTimeslot < tx2)
						timeslot = baseTimeslot + tx2;
					else
						timeslot = baseTimeslot + 10 + tx1;

					if (timeslot > 50)
						timeslot = tx1;
					//				}
				}PRINTF("[RLL]:Send to Parent \%2=1 base timeslot: %d, currentTimeslot: %d, send timeslot: %d at: asn-%x.%lx\n",baseTimeslot,currentTimeslot, timeslot,current_asn.ms1b,current_asn.ls4b);

				sendSlots[0].timeslot = timeslot;
				sendSlots[0].direction = -1;
			}

			if (getNumChildCH() > 0)
			{
				/*
				 * Forward to Child CH
				 */
				timeslot = -1;
				if (RLLTier % 2 == 0)
				{
					tx1 = 5;
					tx2 = 9;
					temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
					currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;
					baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
					currentTimeslot = temp->timeslot - baseTimeslot;
					if (currentTimeslot < tx1)
						timeslot = baseTimeslot + tx1;
					else if (currentTimeslot < tx2)
						timeslot = baseTimeslot + tx2;
					else
						timeslot = baseTimeslot + 10 + tx1;

					if (timeslot > 50)
						timeslot = tx1;
				} else
				{

					currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;
					temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
					tx1 = 3;
					tx2 = 7;

					baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
					currentTimeslot = temp->timeslot - baseTimeslot;
					if (currentTimeslot < tx1)
						timeslot = baseTimeslot + tx1;
					else if (currentTimeslot < tx2)
						timeslot = baseTimeslot + tx2;
					else
						timeslot = baseTimeslot + 10 + tx1;

					if (timeslot > 50)
						timeslot = tx1;

				}PRINTF("[RLL]:Send to Child CH \%2=0 base timeslot: %d, currentTimeslot: %d, send timeslot: %d at: asn-%x.%lx\n",baseTimeslot,currentTimeslot, timeslot,current_asn.ms1b,current_asn.ls4b);

				sendSlots[1].timeslot = timeslot;
				sendSlots[1].direction = 1;
			}

			if (getNumCS() > 1)
			{
				/*
				 * Forward to CS
				 */
				timeslot = -1;
				temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
				currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;

				if (currentCalcTimeslot != 0)
				{
					baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
					currentTimeslot = temp->timeslot - baseTimeslot;
					timeslot = baseTimeslot + 10;
					if (timeslot == temp->timeslot)
						timeslot = timeslot + 10;

					if (timeslot > 50)
						timeslot = 10;
				} else if (currentCalcTimeslot == 0 && temp->timeslot == 0)
				{
					timeslot = 10;
					baseTimeslot = 0;
					currentTimeslot = 0;
				}PRINTF("[RLL]:Send to CS base timeslot: %d, currentTimeslot: %d, send timeslot: %d at: asn-%x.%lx\n",baseTimeslot,currentTimeslot, timeslot,current_asn.ms1b,current_asn.ls4b);

				sendSlots[2].timeslot = timeslot;
				sendSlots[2].direction = 1;
			}


			int x, y, n = 3;
			for (x = 0; x < n; x++)
			{
				for (y = 0; y < n - 1; y++)
				{
					if (sendSlots[y].timeslot > sendSlots[y + 1].timeslot)
					{
						struct forwardType temp = sendSlots[y + 1];
						sendSlots[y + 1] = sendSlots[y];
						sendSlots[y] = temp;
					}
				}
			}
			for (i = 0; i < 3; i++)
			{
				if (sendSlots[i].timeslot != -1)
				{

					switch (sendSlots[i].direction)
					{
						case 0:
							RLLPacket.direction = -1;
							break;
						case 1:
							RLLPacket.direction = 1;
							break;
						case 2:
							RLLPacket.direction = 1;
							break;
					}

					PRINTF("[RLL]: Forward packet: timeslot: %d, direction: %d\n",sendSlots[i].timeslot,sendSlots[i].direction);
					packetbuf_copyfrom(&RLLPacket, sizeof(struct RLL_PACKET));
					packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
					packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, sendSlots[i].timeslot);
					broadcast_send(&rll_broadcast);
				}
			}
		}
		mt_yield();
	}
	mt_exit();
}

static void rll_packet_received(struct broadcast_conn *c, const linkaddr_t *from)
{

	struct RLL_PACKET *tempPacket = packetbuf_dataptr();
	switch (tempPacket->subType)
	{
		case RLL_DATA:
			if (tempPacket->seqNo <= lastRxSeqNo && tempPacket->priority == 1)
			{
				ANNOTATE("[RLL]: Packet received, but outdated. Current SeqNo: %d, received SeqNo: %d\n",lastRxSeqNo,tempPacket->seqNo);

			} else if (tempPacket->initalSRC.u16 == linkaddr_node_addr.u16)
			{
				ANNOTATE("[RLL]: Packet originates to me, do nothing\n",RLLSeqNo,tempPacket->seqNo);
			} else
			{

				if (RLL_CIDERState == 5)
				{

					RLLForwardPacket.appData = tempPacket->appData;
					RLLForwardPacket.base.dst = tsch_broadcast_address;
					RLLForwardPacket.base.type = tempPacket->base.type;
					RLLForwardPacket.initalSRC = tempPacket->initalSRC;
					RLLForwardPacket.priority = tempPacket->priority;
					RLLForwardPacket.seqNo = tempPacket->seqNo;
					RLLForwardPacket.subType = tempPacket->subType;
					RLLForwardPacket.direction = tempPacket->direction;

					mt_exec(&forward_thread);
				}
				//ANNOTATE("[RLL]: Data packet type: %d\n",tempPacket->appData.subType);
				RLLAppPacket = tempPacket->appData;
				mt_exec(&app_thread);
				lastRxSeqNo = RLLForwardPacket.seqNo;
				//APPDATACALLBACK(&RLLDataPacket);
			}
			break;
		case RLL_PING:
			setActiveSchedule(2);
			setActiveProtocol(2);
			if (RLL_started == 0)
				RLL_notify();
			if (getCIDERState() == 5)
				sendRLLPingMessage();
			break;

	}
}

void sendRLLDataMessage(struct APP_PACKET dataPacket, uint8_t lock)
{
	RLL_lock = lock;
	ANNOTATE("[RLL]: sendRLLDataMessage, RLL_Started: %d, msg received from APP at: asn-%x.%lx\n",RLL_started,current_asn.ms1b,current_asn.ls4b);
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

		ANNOTATE("[RLL]: RLL open channel\n");
		broadcast_open(&rll_broadcast, BROADCAST_CHANNEL_RLL, &rll_broadcast_rx);
	PROCESS_END();
}

PROCESS_THREAD(dewi_rll_process, ev, data)
{
PROCESS_EXITHANDLER(netflood_close(&rll_broadcast));

PROCESS_BEGIN()
	;
	mt_init();
	mt_start(&forward_thread, thread_forward, &RLLForwardPacket);
	mt_start(&app_thread, thread_app, &RLLAppPacket);
	mt_start(&send_thread, thread_send, &RLLSendPacket);
	etimer_set(&RLL_timer, CLOCK_SECOND * 5);
	while (1)
	{
		PROCESS_YIELD()
		;
		if (ev == PROCESS_EVENT_TIMER)
		{
			ANNOTATE("[RLL]: RLL active\n");
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
				ANNOTATE("[RLL]: Tier: %d, NumChildCH: %d, NumCS: %d\n",RLLTier,RLLNumChildCH,RLLNumCS);
			etimer_set(&RLL_timer, CLOCK_SECOND * 5);
		}

	}
	mt_stop(&forward_thread);
	mt_stop(&app_thread);
	mt_stop(&send_thread);
	mt_remove();
PROCESS_END();
}
