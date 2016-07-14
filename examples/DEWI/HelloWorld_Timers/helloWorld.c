#include "contiki.h"
#include "cpu.h"
#include "dev/leds.h"

#include "reg.h"
#include "dev/gptimer.h"
#include "dev/sys-ctrl.h"
#include "nvic.h"

#include <stdio.h>
#include <stdlib.h>

/*---------------------------------------------------------------------------*/
// Variables for GPTimers
/*---------------------------------------------------------------------------*/

long unsigned int clock;
long unsigned int flag;
long unsigned int timer;
long unsigned int interval;
long unsigned int snap;

/*---------------------------------------------------------------------------*/
// Interruption
/*---------------------------------------------------------------------------*/

void gptimer_1a_isr (void){
	timer = REG(GPT_1_BASE + GPTIMER_TAV);
	snap = REG(GPT_1_BASE + GPTIMER_TAR);
	
	printf("\n%lu s",clock_seconds());
	
	//Just to see the flag
	//printf("flag at the entry of the function: %lu\n", REG(GPT_1_BASE + GPTIMER_RIS) & GPTIMER_RIS_TATORIS);
	
	//Printf and LED PURPLE
	//printf("I am an interruption : Hello world !\n");
	printf("\n___________________________________________\n");
	leds_toggle(LEDS_PURPLE);
	
	//1 tick per us
	printf("Snap shot : %lu \t delay between the interrupt and the entry in the isr function :  %lu ticks\n", snap, timer);
	
	//clear interrupt flag
	REG(GPT_1_BASE + GPTIMER_ICR) |= GPTIMER_ICR_TATOCINT;
	//printf("flag at the end of the function: %lu\n", REG(GPT_1_BASE + GPTIMER_RIS) & GPTIMER_RIS_TATORIS);
	
	printf("time : %lu\n", REG(GPT_1_BASE + GPTIMER_TAV) - timer);
}


/*---------------------------------------------------------------------------*/
// Initialization & Configuration of the timer
/*---------------------------------------------------------------------------*/
void config_timer(void){

	//Initialization
	REG(SYS_CTRL_RCGCGPT) |= SYS_CTRL_RCGCGPT_GPT1;				//enable clock for GPTM1
	
	//Periodic mode
	REG(GPT_1_BASE + GPTIMER_CTL) = 0;						//timer disabled
	REG(GPT_1_BASE + GPTIMER_CFG) = 0x00000000;				//configuration
	REG(GPT_1_BASE + GPTIMER_TAMR) |= GPTIMER_TAMR_TAMR_PERIODIC;	//periodic mode
	REG(GPT_1_BASE + GPTIMER_TAMR) |= GPTIMER_TAMR_TASNAPS;		//capture the value of the free-running timer at time-out
	REG(GPT_1_BASE + GPTIMER_TAMR) |= GPTIMER_TAMR_TACDIR;		//counting up
	REG(GPT_1_BASE + GPTIMER_TAILR) = 16000000;					//interval load	16.000.000
	REG(GPT_1_BASE + GPTIMER_IMR) |= GPTIMER_IMR_TATOIM;			//interrupt time out mode
	
	//Interrupt Vector
	//nvic_interrupt_enable(NVIC_INT_GPTIMER_1A);
	
	REG(GPT_1_BASE + GPTIMER_CTL) |= GPTIMER_CTL_TAEN;			//enable timer
	
}


/*---------------------------------------------------------------------------*/
PROCESS(helloWorld, "Hello World");
AUTOSTART_PROCESSES(&helloWorld);
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(helloWorld, ev, data)
{
	//Values
	clock = sys_ctrl_get_sys_clock();
	flag = REG(GPT_1_BASE + GPTIMER_RIS) & GPTIMER_RIS_TATORIS;
	timer = (REG(GPT_1_BASE + GPTIMER_TAV));
	interval = REG(GPT_1_BASE + GPTIMER_TAILR);
	
	nvic_init();
	
	config_timer();
	
	PROCESS_BEGIN();
	

	while(1){
		//Update values
		flag = REG(GPT_1_BASE + GPTIMER_RIS) & GPTIMER_RIS_TATORIS;
		timer = (REG(GPT_1_BASE + GPTIMER_TAV));

		if (flag || timer == interval){
			gptimer_1a_isr();
		}
	}

	PROCESS_END();
}
