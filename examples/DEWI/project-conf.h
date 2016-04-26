/*
 * Copyright (c) 2012, Texas Instruments Incorporated - http://www.ti.com/
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
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**
 * \addtogroup example-dewi
 * @{
 *
 * \file
 * Project specific configuration defines for the basic example-dewi
 */
#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_


#define BROADCAST_CHANNEL_CIDER     129
#define BROADCAST_CHANNEL_SCHEDULE     130

#define CONF_MAX_NEIGHBOURS 100

#undef FRAME802154_CONF_VERSION
#define FRAME802154_CONF_VERSION FRAME802154_IEEE802154E_2012

/* Contiki netstack: MAC */
#undef NETSTACK_CONF_MAC
#define NETSTACK_CONF_MAC     tschmac_driver

/* Contiki netstack: RDC */
#undef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC     nordc_driver


#define TSCH_LOG_LEVEL 2
//#define TSCH_PACKET_EB_WITH_SLOTFRAME_AND_LINK 1
//#define TSCH_INIT_SCHEDULE_FROM_EB 1

#define TSCH_CONF_EB_PERIOD (4 * CLOCK_SECOND)
#define TSCH_CONF_KEEPALIVE_TIMEOUT (24 * CLOCK_SECOND)


#undef SYS_CTRL_CONF_OSC32K_USE_XTAL
#define SYS_CTRL_CONF_OSC32K_USE_XTAL 1
#define TSCH_CONF_WITH_LINK_SELECTOR 1
#define TSCH_CONF_DEFAULT_TIMESLOT_LENGTH 10000
#define TSCH_CONF_ADAPTIVE_TIMESYNC 1
#define TSCH_CONF_DEFAULT_HOPPING_SEQUENCE (uint8_t[]){ 13, 24, 20, 18, 24,20,18,13,13,20,24,18,20,13,18,24 }
#define TSCH_CONF_WITH_LINK_SELECTOR 1
#define TSCH_SCHEDULE_CONF_MAX_LINKS 100

#endif /* PROJECT_CONF_H_ */

/** @} */
