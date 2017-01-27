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
struct RLL_PACKET RLLPacket;
struct RLL_PACKET RLLParentPacket;
struct RLL_PACKET RLLChildPacket;
struct RLL_PACKET RLLCSPacket;
struct APP_PACKET RLLAppPacket;
struct APP_PACKET RLLSendPacket;
uint8_t lastTimeslotSend = 0;
int RLL_receiveTimeslot = 0;
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
static void sendToParent(struct RLL_PACKET *data);
static void sendToChildCH(struct RLL_PACKET *data);
static void sendToCS(struct RLL_PACKET *data);
static void thread_send(struct APP_PACKET *data);

static void rll_packet_received(struct broadcast_conn *c, const linkaddr_t *from);

void checkQueue();
/***************************************/
/***************************************/
/*			Channel Variables		   */
/***************************************/
/***************************************/

static struct broadcast_conn rll_broadcast;
static const struct broadcast_callbacks rll_broadcast_rx = {rll_packet_received};

/***************************************/
/***************************************/
/*			   Processes			   */
/***************************************/
/***************************************/
PROCESS(dewi_rll_init_process, "DEWI RLL PROCESS");

PROCESS(dewi_rll_process, "DEWI RLL PROCESS");

/***************************************/
/***************************************/
/*	     Function definitions		   */
/***************************************/
/***************************************/

void checkQueue()
{
    int packetsInQueue = tsch_queue_packet_count(&tsch_broadcast_address);

    if (packetsInQueue > 0)
        tsch_queue_flush_nbr_queue(tsch_queue_get_nbr(&tsch_broadcast_address));
}

void RLL_RxTimeslot(int timeslot)
{
    RLL_receiveTimeslot = timeslot;
}

void RLL_init()
{
    RLLSeqNo = random_rand();
    process_start(&dewi_rll_init_process, NULL);
}

void RLL_start()
{

    if (getTier() == 0 && RLL_started == 0) {
        process_start(&dewi_rll_process, NULL);
        sendRLLPingMessage();
        setActiveSchedule(2);
        setActiveProtocol(2);
    } else if (RLL_started == 0) {
        process_start(&dewi_rll_process, NULL);
        RLL_started = 1;
        setActiveSchedule(2);
        setActiveProtocol(2);
    }
}

void RLL_notify()
{
    ANNOTATE("[RLL]: RLL_notify()\n");
    if (getColour() != -1 || getCIDERState() == 7) {
        if (RLL_started == 0) {
            ANNOTATE("[RLL]: RLL_started == 0\n");
            ANNOTATE("[RLL]: RLL is active, set schedule\n");
            setActiveProtocol(2);
            process_start(&dewi_rll_process, NULL);
            RLL_started = 1;
            if (RLL_schedule_set == 0) {
                setRLLSchedule();
                RLL_schedule_set = 1;
            }

        } else if (RLL_started == 1) {

            ANNOTATE("[RLL]: RLL_started == 1\n");
            if (RLL_schedule_set == 1) {
                tsch_schedule_print();
                RLL_schedule_set = 2;
            }
        }
    } else {
        COLOURING_start();
    }

}


static void thread_send(struct APP_PACKET *data)
{

    while (1) {
        struct APP_PACKET *temp = data;
        uint8_t timeslotOffset = 0;
        RLLPacket.base.dst = tsch_broadcast_address;
        RLLPacket.base.src = linkaddr_node_addr;
        RLLPacket.base.type = RLL;
        RLLPacket.subType = RLL_DATA;
        RLLPacket.appData = *data;
        RLLPacket.seqNo = RLLSeqNo++;
        RLLPacket.initalSRC = linkaddr_node_addr;
        lastRxSeqNo = RLLSeqNo - 1;
        ANNOTATE("[APP]: try to send msg\n");
        if (RLL_CIDERState == 7) {
            struct tsch_link *temp = tsch_schedule_get_next_active_link(&current_asn, &timeslotOffset, NULL);
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
//			if (timeslotOffset >= 1)
//				timeslot = baseTimeslot + tx1;
//			else
            timeslot = baseTimeslot + 10 + tx1;

            if (timeslot > 50) timeslot = tx1;
            PRINTF("[RLL]:Send to Parent \%2=0 base timeslot: %d, currentTimeslot: %d, send timeslot: %d at: asn-%x.%lx\n",
                   baseTimeslot, currentTimeslot, timeslot, current_asn.ms1b, current_asn.ls4b);

            if (lastTimeslotSend != timeslot) {
                lastTimeslotSend = timeslot;
                PRINTF("[RLL]:Send to Parent \%2=1 base timeslot: %d, currentTimeslot: %d, send timeslot: %d at: asn-%x.%lx\n",
                       baseTimeslot, currentTimeslot, timeslot, current_asn.ms1b, current_asn.ls4b);

                packetbuf_copyfrom(&RLLPacket, sizeof(struct RLL_PACKET));
                packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
                packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, timeslot);
                broadcast_send(&rll_broadcast);
            }

        }
        if (RLL_CIDERState == 5) {
            struct tsch_link *temp;
            uint8_t timeslot = 0;
            uint8_t baseTimeslot = 0;
            uint8_t currentTimeslot = 0;
            uint8_t currentCalcTimeslot;
            int8_t tx1 = -1, tx2 = -1;
            if (RLLTier != 0) {
                if (RLLTier % 2 == 0) {
                    tx1 = 2;
                    tx2 = 6;
                    temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
                    currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;
                    if (currentCalcTimeslot == tx1) {
                        timeslot = temp->timeslot;
                        baseTimeslot = timeslot - tx1;
                        currentTimeslot = timeslot;
                    } else if (currentCalcTimeslot == tx2) {

                        timeslot = temp->timeslot;
                        baseTimeslot = timeslot - tx2;
                        currentTimeslot = timeslot;
                    } else {
                        baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
                        currentTimeslot = temp->timeslot - baseTimeslot;
                        if (currentTimeslot < tx1)
                            timeslot = baseTimeslot + tx1;
                        else if (currentTimeslot < tx2)
                            timeslot = baseTimeslot + tx2;
                        else timeslot = baseTimeslot + 10 + tx1;

                        if (timeslot > 50) timeslot = tx1;
                    }
                } else {
                    tx1 = 4;
                    tx2 = 8;
                    temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
                    currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;
                    if (currentCalcTimeslot == tx1) {
                        timeslot = temp->timeslot;
                        baseTimeslot = timeslot - tx1;
                        currentTimeslot = timeslot;
                    } else if (currentCalcTimeslot == tx2) {

                        timeslot = temp->timeslot;
                        baseTimeslot = timeslot - tx2;
                        currentTimeslot = timeslot;
                    } else {
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
            if (RLLNumChildCH != 0) {
                if (RLLTier % 2 == 0) {
                    tx1 = 5;
                    tx2 = 9;
                    temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
                    currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;
                    if (currentCalcTimeslot == tx1) {
                        timeslot = temp->timeslot;
                        baseTimeslot = timeslot - tx1;
                        currentTimeslot = timeslot;
                    } else if (currentCalcTimeslot == tx2) {

                        timeslot = temp->timeslot;
                        baseTimeslot = timeslot - tx2;
                        currentTimeslot = timeslot;
                    } else {
                        baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
                        currentTimeslot = temp->timeslot - baseTimeslot;
                        if (currentTimeslot < tx1)
                            timeslot = baseTimeslot + tx1;
                        else if (currentTimeslot < tx2)
                            timeslot = baseTimeslot + tx2;
                        else timeslot = baseTimeslot + 10 + tx1;

                        if (timeslot > 50) timeslot = tx1;
                    }
                } else {
                    temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
                    currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;
                    tx1 = 3;
                    tx2 = 7;
                    if (currentCalcTimeslot == tx1) {
                        timeslot = temp->timeslot;
                        baseTimeslot = timeslot - tx1;
                        currentTimeslot = timeslot;
                    } else if (currentCalcTimeslot == tx2) {

                        timeslot = temp->timeslot;
                        baseTimeslot = timeslot - tx2;
                        currentTimeslot = timeslot;
                    } else {
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
            if (RLLNumCS != 0) {
                temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
                currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;

                if (currentCalcTimeslot != 0) {
                    baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
                    currentTimeslot = temp->timeslot - baseTimeslot;
                    timeslot = baseTimeslot + 10;
                    if (timeslot > 50) timeslot = 10;
                } else if (currentCalcTimeslot == 0 && temp->timeslot == 0) {
                    timeslot = 10;
                    baseTimeslot = 0;
                    currentTimeslot = 0;
                } else {
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


static void sendToParent(struct RLL_PACKET *data){
    struct tsch_link *temp;
    uint8_t timeslot = 0;
    uint8_t baseTimeslot = 0;
    uint8_t currentTimeslot = 0;
    uint8_t currentCalcTimeslot;
    int8_t tx1 = -1, tx2 = -1;
    RLLParentPacket.base.src = linkaddr_node_addr;
    RLLParentPacket.base.dst = data->base.dst;
    RLLParentPacket.base.type = data->base.type;
    RLLParentPacket.appData = data->appData;
    RLLParentPacket.seqNo = data->seqNo;
    RLLParentPacket.subType = data->subType;
    RLLParentPacket.priority = data->priority;
    if (RLLTier != 0) {
        if (RLLTier % 2 == 0) {
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
            else timeslot = baseTimeslot + 10 + tx1;

            if (timeslot > 50) timeslot = tx1;
            PRINTF("[RLL]:Send to Parent \%2=0 base timeslot: %d, currentTimeslot: %d, send timeslot: %d at: asn-%x.%lx\n",
                   baseTimeslot, currentTimeslot, timeslot, current_asn.ms1b, current_asn.ls4b);
        } else {
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
            else timeslot = baseTimeslot + 10 + tx1;

            if (timeslot > 50) timeslot = tx1;

            PRINTF("[RLL]:Send to Parent \%2=1 base timeslot: %d, currentTimeslot: %d, send timeslot: %d at: asn-%x.%lx\n",
                   baseTimeslot, currentTimeslot, timeslot, current_asn.ms1b, current_asn.ls4b);
        }

        packetbuf_copyfrom(&RLLParentPacket, sizeof(struct RLL_PACKET));
        packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
        packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, timeslot);
        broadcast_send(&rll_broadcast);

    }
}
static void sendToChildCH(struct RLL_PACKET *data){
    struct tsch_link *temp;
    uint8_t timeslot = 0;
    uint8_t baseTimeslot = 0;
    uint8_t currentTimeslot = 0;
    uint8_t currentCalcTimeslot;
    int8_t tx1 = -1, tx2 = -1;

    RLLChildPacket.base.src = linkaddr_node_addr;
    RLLChildPacket.base.dst = data->base.dst;
    RLLChildPacket.base.type = data->base.type;
    RLLChildPacket.appData = data->appData;
    RLLChildPacket.seqNo = data->seqNo;
    RLLChildPacket.subType = data->subType;
    RLLChildPacket.priority = data->priority;
    if (RLLTier % 2 == 0) {
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
        else timeslot = baseTimeslot + 10 + tx1;

        if (timeslot > 50) timeslot = tx1;

        PRINTF("[RLL]:Send to Parent \%2=0 base timeslot: %d, currentTimeslot: %d, send timeslot: %d at: asn-%x.%lx\n",
               baseTimeslot, currentTimeslot, timeslot, current_asn.ms1b, current_asn.ls4b);
    } else {

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
        else timeslot = baseTimeslot + 10 + tx1;

        if (timeslot > 50) timeslot = tx1;
        PRINTF("[RLL]:Send to Parent \%2=1 base timeslot: %d, currentTimeslot: %d, send timeslot: %d at: asn-%x.%lx\n",
               baseTimeslot, currentTimeslot, timeslot, current_asn.ms1b, current_asn.ls4b);
    }
    packetbuf_copyfrom(&RLLChildPacket, sizeof(struct RLL_PACKET));
    packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
    packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, timeslot);
    broadcast_send(&rll_broadcast);
}
static void sendToCS(struct RLL_PACKET *data){
    struct tsch_link *temp;
    uint8_t timeslot = 0;
    uint8_t baseTimeslot = 0;
    uint8_t currentTimeslot = 0;
    uint8_t currentCalcTimeslot;
    int8_t tx1 = -1, tx2 = -1;
    RLLCSPacket.base.src = linkaddr_node_addr;
    RLLCSPacket.base.dst = data->base.dst;
    RLLCSPacket.base.type = data->base.type;
    RLLCSPacket.appData = data->appData;
    RLLCSPacket.seqNo = data->seqNo;
    RLLCSPacket.subType = data->subType;
    RLLCSPacket.priority = data->priority;


    PRINTF("[RLL]: forward to CS at: asn-%x.%lx\n", current_asn.ms1b, current_asn.ls4b);
    ANNOTATE("[RLL]: currentCalcTimeslot: %d, timeslot: %d\n", currentCalcTimeslot, temp->timeslot);
    temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
    currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;

    if (currentCalcTimeslot != 0) {
        baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
        currentTimeslot = temp->timeslot - baseTimeslot;
        timeslot = baseTimeslot + 10;
        if (timeslot == temp->timeslot) timeslot = timeslot + 10;

        if (timeslot > 50) timeslot = 10;
    } else if (currentCalcTimeslot == 0 && temp->timeslot == 0) {
        timeslot = 10;
        baseTimeslot = 0;
        currentTimeslot = 0;
    }
    PRINTF("[RLL]:Send to CS base timeslot: %d, currentTimeslot: %d, send timeslot: %d at: asn-%x.%lx\n",
           baseTimeslot, currentTimeslot, timeslot, current_asn.ms1b, current_asn.ls4b);
    packetbuf_copyfrom(&RLLCSPacket, sizeof(struct RLL_PACKET));
    packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
    packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, timeslot);
    broadcast_send(&rll_broadcast);
}

static void rll_packet_received(struct broadcast_conn *c, const linkaddr_t *from)
{
    if (RLL_receiveTimeslot < 0) {
        RLL_receiveTimeslot = TSCH_SCHEDULE_CONF_MAX_LINKS + RLL_receiveTimeslot;
    }
    uint8_t tempTimeslot = RLL_receiveTimeslot - ((uint16_t) RLL_receiveTimeslot / 10) * 10;
    ANNOTATE("[RLL]: Message received in Timeslot: %d, official timeslot: %d\n", tempTimeslot, RLL_receiveTimeslot);

    struct RLL_PACKET *tempPacket = packetbuf_dataptr();

    int8_t direction = -2;

    if (tempTimeslot == 1)
        direction = 0;
    else if (tempTimeslot == 3 || tempTimeslot == 7 || tempTimeslot == 5 || tempTimeslot == 9)
        direction = 1;
    else if (tempTimeslot == 4 || tempTimeslot == 8 || tempTimeslot == 2 || tempTimeslot == 6)
        direction = -1;

    switch (tempPacket->subType) {
        case RLL_DATA:
            if (tempPacket->seqNo == lastRxSeqNo) {
                ANNOTATE("[RLL]: Packet received, but outdated. Current SeqNo: %d, received SeqNo: %d\n", lastRxSeqNo,
                         tempPacket->seqNo);

            } else if (tempPacket->initalSRC.u16 == linkaddr_node_addr.u16) {
                ANNOTATE("[RLL]: Packet originates to me, do nothing\n", RLLSeqNo, tempPacket->seqNo);
            } else {
                if(tempPacket->priority == 1)
                    checkQueue();

                if (RLL_CIDERState == 5) {

                    lastRxSeqNo = tempPacket->seqNo;


                    ANNOTATE("[RLL]: received RLL Data MSG at: asn-%x.%lx, forward direction: %d\n", current_asn.ms1b,
                             current_asn.ls4b, direction);
                    if ((direction == -1 || direction == 0) && getTier() != 0) {
                        sendToParent(tempPacket);
                    }

                    if ((direction == -1 && RLLNumChildCH > 1)
                        || (direction >= 0 && RLLNumChildCH != 0)) {

                        sendToChildCH(tempPacket);

                    }
                    if (((direction == -1 || direction == 1) && RLLNumCS != 0)
                        || (direction == 0 && RLLNumCS > 1)) {

                        sendToCS(tempPacket);
                    }

                }
                //ANNOTATE("[RLL]: Data packet type: %d\n",tempPacket->appData.subType);
                RLLAppPacket = tempPacket->appData;
                //mt_exec(&app_thread);
                APPDATACALLBACK(&RLLAppPacket);
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
    ANNOTATE("[RLL]: sendRLLDataMessage, RLL_Started: %d, msg received from APP at: asn-%x.%lx\n", RLL_started,
             current_asn.ms1b, current_asn.ls4b);
    if (RLL_started == 1) {



        uint8_t timeslotOffset = 0;
        RLLPacket.base.dst = tsch_broadcast_address;
        RLLPacket.base.src = linkaddr_node_addr;
        RLLPacket.base.type = RLL;
        RLLPacket.subType = RLL_DATA;
        RLLPacket.appData = dataPacket;
        RLLPacket.seqNo = RLLSeqNo++;
        RLLPacket.initalSRC = linkaddr_node_addr;
        RLLPacket.priority = lock;
        lastRxSeqNo = RLLSeqNo - 1;
        ANNOTATE("[APP]: try to send msg\n");
        if (RLL_CIDERState == 7) {
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

            if (timeslot > 50) timeslot = tx1;
            PRINTF("[RLL]:Send to Parent \%2=0 base timeslot: %d, currentTimeslot: %d, send timeslot: %d at: asn-%x.%lx\n",
                   baseTimeslot, currentTimeslot, timeslot, current_asn.ms1b, current_asn.ls4b);

            if (lastTimeslotSend != timeslot) {
                lastTimeslotSend = timeslot;
                PRINTF("[RLL]:Send to Parent \%2=1 base timeslot: %d, currentTimeslot: %d, send timeslot: %d at: asn-%x.%lx\n",
                       baseTimeslot, currentTimeslot, timeslot, current_asn.ms1b, current_asn.ls4b);

                packetbuf_copyfrom(&RLLPacket, sizeof(struct RLL_PACKET));
                packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
                packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, timeslot);
                broadcast_send(&rll_broadcast);
            }

        }
        if (RLL_CIDERState == 5) {
            struct tsch_link *temp;
            uint8_t timeslot = 0;
            uint8_t baseTimeslot = 0;
            uint8_t currentTimeslot = 0;
            uint8_t currentCalcTimeslot;
            int8_t tx1 = -1, tx2 = -1;
            if (RLLTier != 0) {
                if (RLLTier % 2 == 0) {
                    tx1 = 2;
                    tx2 = 6;
                    temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
                    currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;

                        baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
                        currentTimeslot = temp->timeslot - baseTimeslot;
                        if (currentTimeslot < tx1)
                            timeslot = baseTimeslot + tx1;
                        else if (currentTimeslot < tx2)
                            timeslot = baseTimeslot + tx2;
                        else timeslot = baseTimeslot + 10 + tx1;

                        if (timeslot > 50) timeslot = tx1;

                } else {
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
                        else timeslot = baseTimeslot + 10 + tx1;

                        if (timeslot > 50) timeslot = tx1;

                }

                packetbuf_copyfrom(&RLLPacket, sizeof(struct RLL_PACKET));
                packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
                packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, timeslot);
                broadcast_send(&rll_broadcast);
            }
            if (RLLNumChildCH != 0) {
                if (RLLTier % 2 == 0) {
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
                        else timeslot = baseTimeslot + 10 + tx1;

                        if (timeslot > 50) timeslot = tx1;

                } else {
                    temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
                    currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;
                    tx1 = 3;
                    tx2 = 7;

                        baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
                        currentTimeslot = temp->timeslot - baseTimeslot;
                        if (currentTimeslot < tx1)
                            timeslot = baseTimeslot + tx1;
                        else if (currentTimeslot < tx2)
                            timeslot = baseTimeslot + tx2;
                        else timeslot = baseTimeslot + 10 + tx1;

                        if (timeslot > 50) timeslot = tx1;

                }
                packetbuf_copyfrom(&RLLPacket, sizeof(struct RLL_PACKET));
                packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
                packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, timeslot);
                broadcast_send(&rll_broadcast);
            }
            if (RLLNumCS != 0) {
                temp = tsch_schedule_get_next_active_link(&current_asn, 0, NULL);
                currentCalcTimeslot = temp->timeslot - ((uint16_t) temp->timeslot / 10) * 10;

                if (currentCalcTimeslot != 0) {
                    baseTimeslot = ((uint16_t) temp->timeslot / 10) * 10;
                    currentTimeslot = temp->timeslot - baseTimeslot;
                    timeslot = baseTimeslot + 10;
                    if (timeslot > 50) timeslot = 10;
                }
                packetbuf_copyfrom(&RLLPacket, sizeof(struct RLL_PACKET));
                packetbuf_set_attr(PACKETBUF_ATTR_TSCH_SLOTFRAME, 0);
                packetbuf_set_attr(PACKETBUF_ATTR_TSCH_TIMESLOT, timeslot);
                broadcast_send(&rll_broadcast);
            }

        }

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
    if (RLL_sendCounter < 2) {
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

PROCESS_THREAD(dewi_rll_init_process, ev, data)
{

    PROCESS_BEGIN()

    ANNOTATE("[RLL]: RLL open channel\n");
    broadcast_open(&rll_broadcast, BROADCAST_CHANNEL_RLL, &rll_broadcast_rx);
    PROCESS_END();
}

PROCESS_THREAD(dewi_rll_process, ev, data)
{
    PROCESS_EXITHANDLER(netflood_close(&rll_broadcast));

    PROCESS_BEGIN();
    etimer_set(&RLL_timer, CLOCK_SECOND * 5);
    while (1) {
        PROCESS_YIELD();
        if (ev == PROCESS_EVENT_TIMER) {
            ANNOTATE("[RLL]: RLL active\n");
            if (RLL_schedule_set == 0) {
                setRLLSchedule();
                RLL_schedule_set = 1;
            }
            RLL_CIDERState = getCIDERState();
            RLLNumChildCH = getNumChildCH();
            RLLNumCS = getNumCS();
            RLLTier = getTier();
            if (RLL_CIDERState == 5)
                ANNOTATE("[RLL]: Tier: %d, NumChildCH: %d, NumCS: %d\n", RLLTier, RLLNumChildCH, RLLNumCS);
            etimer_set(&RLL_timer, CLOCK_SECOND * 5);
        }

    }

    PROCESS_END();
}
