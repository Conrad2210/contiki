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
 *         type_defs used for DEWI-Stack
 *
 *
 * \author Conrad Dandelski <conrad.dandelski@mycit.ie>
 */
#include "tsch-schedule.h"

#ifndef DEWI_NIMBUS_CONTIKI_CORE_NET_DEWI_COMMON_TYPE_DEFS_H_
#define DEWI_NIMBUS_CONTIKI_CORE_NET_DEWI_COMMON_TYPE_DEFS_H_

#define MAX_NUM_LINKS 2

typedef struct{
		  /* MAC address of neighbor */
		  linkaddr_t *addr;
		  //

		  uint8_t isActive;
		  /* Identifier of Slotframe to which this link belongs
		   * Unused. */
		  /* uint8_t handle; */
		  /* Timeslot for this link */
		  uint16_t timeslot;
		  /* Channel offset for this link */
		  uint16_t channel_offset;
		  /* A bit string that defines
		   * b0 = Transmit, b1 = Receive, b2 = Shared, b3 = Timekeeping, b4 = reserved */
		  uint8_t link_options;
		  /* Type of link. NORMAL = 0. ADVERTISING = 1, and indicates
		     the link may be used to send an Enhanced beacon. */
		  enum link_type link_type;

}linkInfo_t;

typedef struct{
		//slotframe handle CIDER:0; RLL:1
		uint16_t handle;
		uint16_t slotframeLength;
		linkInfo_t links[MAX_NUM_LINKS];
}ScheduleInfo_t;



#endif /* DEWI_NIMBUS_CONTIKI_CORE_NET_DEWI_COMMON_TYPE_DEFS_H_ */
