/*
 * Copyright (c) 2015, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/**
 * \file
 *         CIDER: Clustering protocol for dense environments, working on top of TSCH
 *         see 'CIDER - Clustering In Dense EnviRonments' - XXX - 2016
 *
 * \author Conrad Dandelski <conrad.dandelski@mycit.ie>
 */

#include "cider.h"
#include "neighTable.h"
#define CIDER_PING_INTERVAL       (CLOCK_SECOND * 15)
#define DEBUG DEBUG_PRINT


uint8_t CIDER_isActive = 0;
enum CIDERsubpackettype CIDER_currentStep = PING;
uint8_t CIDER_ping_sent = 0;
uint8_t CIDER_ping_recvd = 0;
uint8_t CIDER_ND = 0, CIDER_LPD = 0, CIDER_CD = 0;
float CIDER_AvgRSSI = 0;
float CIDER_M1 = 0.5;
float CIDER_M2 = 0.3;
float CIDER_M3 = 0.15;
float CIDER_M4 = 0.05;
float CIDER_WEIGHT = 0;
struct CIDER_PACKET createCCIDERPacket();
static struct etimer CIDER_ping_timer;
static struct ctimer CIDER_send_ping_timer;
PROCESS(dewi_cider_process, "DEWI cider PROCESS");

uint8_t getTxPower8bit(int tx)
{

	uint8_t tx8 = 0x00;
	switch (tx)
	{
		case 7:
			tx8 = 0xFF;
			break;
		case 5:
			tx8 = 0xed;
			break;
		case 3:
			tx8 = 0xd5;
			break;
		case 1:
			tx8 = 0xc5;
			break;
		case 0:
			tx8 = 0xb6;
			break;
		case -1:
			tx8 = 0xb0;
			break;
		case -3:
			tx8 = 0xa1;
			break;
		case -5:
			tx8 = 0x91;
			break;
		case -7:
			tx8 = 0x88;
			break;
		case -9:
			tx8 = 0x72;
			break;
		case -11:
			tx8 = 0x62;
			break;
		case -13:
			tx8 = 0x58;
			break;
		case -15:
			tx8 = 0x42;
			break;
		case -24:
			tx8 = 0x00;
			break;
	}
	return tx8;
}

int getTxPowerInt(uint8_t tx)
{
	int txInt = -24;
	switch (tx)
	{
		case 0xFF:
			txInt = 7;
			break;
		case 0xed:
			txInt = 5;
			break;
		case 3:
			txInt = 0xd5;
			break;
		case 0xc5:
			txInt = 1;
			break;
		case 0xb6:
			txInt = 0;
			break;
		case 0xb0:
			txInt = -1;
			break;
		case 0xa1:
			txInt = -3;
			break;
		case 0x91:
			txInt = -5;
			break;
		case 0x88:
			txInt = -7;
			break;
		case 0x72:
			txInt = -9;
			break;
		case 0x62:
			txInt = -11;
			break;
		case 0x58:
			txInt = -13;
			break;
		case 0x42:
			txInt = -15;
			break;
		case 0x0:
			txInt = -24;
			break;
	}
	return txInt;
}

static void cider_packet_received(struct broadcast_conn *c,
		const linkaddr_t *from)
{

	struct CIDER_PACKET *temp = packetbuf_dataptr();
	radio_result_t rv = (int8_t) NETSTACK_RADIO.get_value(RADIO_PARAM_LAST_RSSI,
			&temp->args[3]);

	printf("[CIDER]:Received CIDER packet %u bytes from %u:%u: \n",
			packetbuf_datalen(), from->u8[0], from->u8[1]);

	switch (temp->subType)
	{
		printf("[CIDER]: Received CIDER Message, type: %u\n", temp->subType);

		struct neighbour *n = getNeighbour(from);
		if (n == NULL)
			n = initNeighbour();

	case PING:
		n->addr = temp->base.src;
		n->last_rssi = temp->args[3];
		n->last_asn = tsch_get_current_asn();
		n->txPW = getTxPowerInt(temp->args[0]);
		n->isLPD = temp->args[1];
		n->distance = calcDistance(dBmTomW(n->txPW), dBmTomW(n->last_rssi));
		addNeighbour(n);
		printTable();
		break;
	case NEIGHBOUR_UPDATE:
		n->last_rssi = temp->args[3];
		n->nodeDegree = temp->args[0];
		n->lpDegree = temp->args[1];
		n->clusterDegree = temp->args[2];
		addNeighbour(n);
		printTable();
		break;
	case WEIGHT_UPDATE:
		n->last_rssi = temp->args[3];
		n->weight = temp->args[0];
		addNeighbour(n);
		printTable();
		break;
	case CH_COMPETITION:
		break;
	case CH_ADVERT:
		break;
	case LP_PING:
		break;
	case COVERAGE_UPDATE:
		break;
	default:
		printf("[CIDER]: Received CIDER Message, type: unknown\n");

	}
	if (CIDER_ping_sent == 1)
	{
		printf("[CIDER]:Stop Ping Timer\n");
		CIDER_ping_recvd = 1;
	}
}

static const struct broadcast_callbacks cider_bc_rx = { cider_packet_received };

static struct broadcast_conn cider_bc;

int CIDER_notify()
{
	//todo: Implement check if RLL;
	if (CIDER_isActive == 0)
	{
		CIDER_isActive = 1;
		process_start(&dewi_cider_process, NULL);
	} else
	{
		CIDER_isActive = 0;
		process_exit(&dewi_cider_process);
	}

	CIDER_createSchedule();

	return 1;
}

int CIDER_getIsActive()
{
	return CIDER_isActive;
}

void CIDER_createSchedule()
{
	if (CIDER_isActive == 1)
	{
		ScheduleInfo_t temp;
		temp.handle = 0x00;
		temp.slotframeLength = 51;
		int i;
		printf("[CIDER]: Create CIDER schedule\n");
		for (i = 0; i < temp.slotframeLength; i++)
		{
			temp.links[i].addr = &tsch_broadcast_address;
			temp.links[i].channel_offset = 0;
			temp.links[i].isActive = 1;

			if (i == 0)
			{
				temp.links[i].link_type = LINK_TYPE_ADVERTISING;
				temp.links[i].link_options = LINK_OPTION_RX | LINK_OPTION_TX
						| LINK_OPTION_SHARED | LINK_OPTION_TIME_KEEPING;
			} else
			{
				temp.links[i].link_type = LINK_TYPE_NORMAL;
				temp.links[i].link_options = LINK_OPTION_RX | LINK_OPTION_TX;
			}

			temp.links[i].timeslot = i;
		}

		setSchedule(temp);
	} else
	{
		clearSchedule();
	}
}

void CIDER_calcWeight()
{
	CIDER_ND = getNumNeighbours();
	CIDER_LPD = getNumLPDevices();
	CIDER_CD = getNumCluster();
	CIDER_AvgRSSI = getAvgRSSI();
	CIDER_WEIGHT = CIDER_M1 * CIDER_ND + CIDER_M2 + CIDER_CD
			+ CIDER_M3 * CIDER_LPD + CIDER_M4 * CIDER_AvgRSSI;
}
struct CIDER_PACKET createCIDERPacket()
{
	struct CIDER_PACKET CIDERPacket;

	int *tempTX;
	tempTX = 0;
	switch (CIDER_currentStep)
	{
		case PING: //PING message
#if DEBUG
		printf("[CIDER]:SEND CIDER PING MESSAGE\n");
#endif
			CIDERPacket.base.dst = tsch_broadcast_address;
			CIDERPacket.base.src = linkaddr_node_addr;
			CIDERPacket.base.type = CIDER;
			CIDERPacket.subType = PING;
			radio_value_t chan;
			radio_result_t rv;
			rv = NETSTACK_RADIO.get_value(RADIO_PARAM_TXPOWER, &chan);
			CIDERPacket.args[0] = getTxPower8bit(chan);
#if LPDEVICE
			CIDERPacket.args[1] = 1;
#else
			CIDERPacket.args[1] = 0;
#endif
			break;
		case NEIGHBOUR_UPDATE:
#if DEBUG
			printf("[CIDER]:SEND CIDER NEIGHBOUR MESSAGE\n");
#endif
			CIDERPacket.base.dst = tsch_broadcast_address;
			CIDERPacket.base.src = linkaddr_node_addr;
			CIDERPacket.base.type = CIDER;
			CIDERPacket.subType = NEIGHBOUR_UPDATE;
			CIDERPacket.args[0] = getNumNeighbours();
			CIDERPacket.args[1] = getNumLPDevices();
			CIDERPacket.args[2] = getNumCluster();
			break;
		case WEIGHT_UPDATE:
			break;
		case CH_COMPETITION:
			break;
		case CH_ADVERT:
			break;
		case LP_PING:
			break;
		case COVERAGE_UPDATE:
			break;

		default:
			//do nothing
			break;
	}
	return CIDERPacket;

}

void sendCIDERPacket()
{

	printf("[CIDER]: Send ping msg now, at %u\n", tsch_get_current_asn());
	struct CIDER_PACKET CIDERPacket;
	CIDERPacket = createCIDERPacket();
	packetbuf_copyfrom(&CIDERPacket, sizeof(struct CIDER_PACKET));
	broadcast_send(&cider_bc);
	CIDER_ping_sent = 1;
	if (!CIDER_ping_recvd)
		etimer_set(&CIDER_ping_timer, CIDER_PING_INTERVAL);
}

PROCESS_THREAD(dewi_cider_process, ev, data)
{
	uint16_t msgDelay;
	clock_time_t delay;
	PROCESS_EXITHANDLER(broadcast_close(&cider_bc))
	broadcast_open(&cider_bc, BROADCAST_CHANNEL_CIDER, &cider_bc_rx);
	PROCESS_BEGIN()
		;

		etimer_set(&CIDER_ping_timer, CLOCK_SECOND);
		while (1)
		{
			PROCESS_YIELD()
			;
			if (ev == PROCESS_EVENT_TIMER)
			{
				msgDelay = (linkaddr_node_addr.u16 & 0b0000111111111111);
				delay = (msgDelay / 1000.0) * CLOCK_SECOND;

				printf(
						"[CIDER]: linkaddr: 0x%x, Delay msg by: %d ms and CLOCKSECONDS: %d, at %u \n",
						linkaddr_node_addr.u16, msgDelay, delay,
						tsch_get_current_asn());
				ctimer_set(&CIDER_send_ping_timer, delay, sendCIDERPacket,
				NULL);

			}
		}

	PROCESS_END();
}

