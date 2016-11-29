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
 *         CIDER header file
 *         see 'CIDER - Clustering In Dense EnviRonments' - XXX - 2016
 *
 * \author Conrad Dandelski <conrad.dandelski@mycit.ie>
 */

#ifndef DEWI_NIMBUS_CONTIKI_CORE_NET_DEWI_CIDER_CIDER_H_
#define DEWI_NIMBUS_CONTIKI_CORE_NET_DEWI_CIDER_CIDER_H_

#include "contiki.h"
//#include "cpu.h"
#include "sys/etimer.h"
#include "sys/rtimer.h"
#include "dev/leds.h"
#include "dev/watchdog.h"
#include "dev/serial-line.h"
//#include "dev/sys-ctrl.h"
#include "net/netstack.h"
#include "net/rime/broadcast.h"
#include "net/mac/tsch/tsch-schedule.h"
#include "net/DEWI/colouring/colouring.h"
#include "net/DEWI/scheduler/scheduler.h"




void CIDER_setLPD(uint8_t is);
uint8_t CIDER_getLPD();
int CIDER_getIsActive();
int CIDER_notify();
void CIDER_reset();

void CIDER_start();

#endif /* DEWI_NIMBUS_CONTIKI_CORE_NET_DEWI_CIDER_CIDER_H_ */
