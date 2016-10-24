/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
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
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         A very simple Contiki application showing how Contiki programs look
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "contiki.h"
#include "etimer.h"

#include <stdio.h> /* For printf() */
/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
PROCESS(hello_world_process_1, "Hello world process");
struct etimer timera, timerb, timerc;
AUTOSTART_PROCESSES(&hello_world_process,&hello_world_process_1);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_process, ev, data)
{
  PROCESS_BEGIN();
  etimer_set(&timera, CLOCK_SECOND*5);
  etimer_set(&timerc, CLOCK_SECOND*6);
  printf("Hello, world\n");
  while(1){
	  PROCESS_YIELD();
	  if(ev == PROCESS_EVENT_TIMER)
	  {

		  if(&timera == data)
		  {
			  printf("[APP]: TimerA: %u expired\n",data);
			  etimer_set(&timera, CLOCK_SECOND*5);

		  }
		  else if(&timerc == data){

			  printf("[APP]: TimerC: %u expired\n",data);
			  etimer_set(&timerc, CLOCK_SECOND*5);
		  }
	  }
  }
  
  PROCESS_END();
}

PROCESS_THREAD(hello_world_process_1, ev, data)
{
  PROCESS_BEGIN();
  etimer_set(&timerb, CLOCK_SECOND*7);
  printf("Hello, world\n");
  while(1){
	  PROCESS_YIELD();
	  if(ev == PROCESS_EVENT_TIMER)
	  {
		  printf("[APP]: TimerB: %u expired\n",data);
		  if(&timerb == data)
			  printf("[APP] yeah data is the same\n");

		  etimer_set(&timerb, CLOCK_SECOND*5);
	  }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
