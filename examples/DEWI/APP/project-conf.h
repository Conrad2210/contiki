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
//
//Tx Power values
// static const output_config_t output_power[] = {
//  {  7, 0xFF },
//  {  5, 0xED },
//  {  3, 0xD5 },
//  {  1, 0xC5 },
//  {  0, 0xB6 },
//  { -1, 0xB0 },
//  { -3, 0xA1 },
//  { -5, 0x91 },
//  { -7, 0x88 },
//  { -9, 0x72 },
//  {-11, 0x62 },
//  {-13, 0x58 },
//  {-15, 0x42 },
//  {-24, 0x00 },
//};


//CIDER CONFIG
#define CONF_M1 0.5;
#define CONF_M2 0.3;
#define CONF_M3 0.15;
#define CONF_M4 0.05;

//CIDER FeatureScale config

#define CONF_ND_min 1.0;
#define CONF_ND_max 50.0;
#define CONF_CD_min 1.0;
#define CONF_CD_max 50.0;
#define CONF_LP_min 1.0;
#define CONF_LP_max 50.0;
#define CONF_RSSI_min -96.0;
#define CONF_RSSI_max -30.0;
#define CLUSTER_RADIUS -72

#define CIDER_LOG_LEVEL 1
#define NEIGHBOURTABLE_LOG_LEVEL 0
#define TSCH_LOG_LEVEL 2
#define COLOURING_LOG_LEVEL 1
#define SCHEDULER_LOG_LEVEL 0
#define RLL_LOG_LEVEL 1
#define APP_LOG_LEVEL 1


#define APPDATACALLBACK applicationDataCallback
#define TSCHDELETEPACKET packetDeletedFromQueue
#define RLLRXTIMESLOT RLL_RxTimeslot

#ifndef LPDEVICE
#define LPDEVICE 0


#if LPDEVICE
#define TXRADIOPOWER -24 //-24dBm
#else
#define TXRADIOPOWER 0 //0dBm
#endif

//LPM settings
#undef LPM_CONF_ENABLE
#define LPM_CONF_ENABLE       0 /**< Set to 0 to disable LPM entirely */


#undef LPM_CONF_MAX_PM
#define LPM_CONF_MAX_PM       0


#undef LPM_CONF_MODE
#define LPM_CONF_MODE 0



#undef NETFLOOD_CHANNEL_APP
#define NETFLOOD_CHANNEL_APP     129

#undef BROADCAST_CHANNEL_CIDER
#define BROADCAST_CHANNEL_CIDER     130

#undef BROADCAST_CHANNEL_SCHEDULE
#define BROADCAST_CHANNEL_SCHEDULE     131

#undef BROADCAST_CHANNEL_RLL
#define BROADCAST_CHANNEL_RLL    132

#undef BROADCAST_CHANNEL_COLOURING
#define BROADCAST_CHANNEL_COLOURING    133

#undef CONF_MAX_NEIGHBOURS
#define CONF_MAX_NEIGHBOURS 100/* Netstack layers */

#undef NETSTACK_CONF_MAC
#define NETSTACK_CONF_MAC     tschmac_driver
#undef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC     nordc_driver
#undef NETSTACK_CONF_FRAMER
#define NETSTACK_CONF_FRAMER  framer_802154

#define NETSTACK_CONF_RADIO           cc2538_rf_driver

/* IEEE802.15.4 frame version */
#undef FRAME802154_CONF_VERSION
#define FRAME802154_CONF_VERSION FRAME802154_IEEE802154E_2012


#undef TSCH_PACKET_CONF_EB_WITH_SLOTFRAME_AND_LINK
#define TSCH_PACKET_CONF_EB_WITH_SLOTFRAME_AND_LINK 1

#undef TSCH_INIT_SCHEDULE_FROM_EB
#define TSCH_INIT_SCHEDULE_FROM_EB 1

#undef TSCH_CONF_EB_PERIOD
#define TSCH_CONF_EB_PERIOD (3*CLOCK_SECOND)


#undef TSCH_CALLBACK_JOINING_NETWORK
#define TSCH_CALLBACK_JOINING_NETWORK tsch_dewi_callback_joining_network

#undef TSCH_CALLBACK_LEAVING_NETWORK
#define TSCH_CALLBACK_LEAVING_NETWORK tsch_dewi_callback_leaving_network

#undef SYS_CTRL_CONF_OSC32K_USE_XTAL
#define SYS_CTRL_CONF_OSC32K_USE_XTAL 1

#undef TSCH_CONF_AUTOSELECT_TIME_SOURCE
#define TSCH_CONF_AUTOSELECT_TIME_SOURCE 0


#undef TSCH_PACKET_CONF_EB_WITH_SLOTFRAME_AND_LINK
#define TSCH_PACKET_CONF_EB_WITH_SLOTFRAME_AND_LINK 1

#undef TSCH_CONF_WITH_LINK_SELECTOR
#define TSCH_CONF_WITH_LINK_SELECTOR 1

#undef TSCH_SCHEDULE_CONF_WITH_6TISCH_MINIMAL
#define TSCH_SCHEDULE_CONF_WITH_6TISCH_MINIMAL 0

#undef TSCH_CONF_DEFAULT_TIMESLOT_LENGTH
#define TSCH_CONF_DEFAULT_TIMESLOT_LENGTH 10000

#undef TSCH_CONF_ADAPTIVE_TIMESYNC
#define TSCH_CONF_ADAPTIVE_TIMESYNC 0

#undef TSCH_CONF_DEFAULT_HOPPING_SEQUENCE
#define TSCH_CONF_DEFAULT_HOPPING_SEQUENCE (uint8_t[]){ 11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26}
#undef CIDER_CONF_OFFSETS
#define CIDER_CONF_OFFSETS (uint8_t[]){0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}

#undef TSCH_CONF_JOIN_HOPPING_SEQUENCE
#define TSCH_CONF_JOIN_HOPPING_SEQUENCE TSCH_CONF_DEFAULT_HOPPING_SEQUENCE

#undef TSCH_CONF_WITH_LINK_SELECTOR
#define TSCH_CONF_WITH_LINK_SELECTOR 1

#undef TSCH_SCHEDULE_CONF_MAX_LINKS
#define TSCH_SCHEDULE_CONF_MAX_LINKS 51

#undef QUEUEBUF_CONF_NUM
#define QUEUEBUF_CONF_NUM                    32


//#undef TSCH_CONF_DESYNC_THRESHOLD
//#define TSCH_CONF_DESYNC_THRESHOLD (2 * TSCH_CONF_KEEPALIVE_TIMEOUT)

#undef WITH_DEWI
#define WITH_DEWI 1

#undef AUTOSTART_CIDER
#define AUTOSTART_CIDER 0

#undef TSCH_QUEUE_CONF_MAX_NEIGHBOR_QUEUES
#define TSCH_QUEUE_CONF_MAX_NEIGHBOR_QUEUES 8

#endif
#endif
/** @} */
