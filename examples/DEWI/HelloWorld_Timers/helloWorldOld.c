#include "contiki.h"
#include "cpu.h"
#include "dev/button-sensor.h"
//#include "rtcc.h"
#include "dev/i2c.h"
#include "dev/leds.h"

#include "systick.h"
#include "reg.h"
#include "dev/gptimer.h"
#include "dev/sys-ctrl.h"

#include <stdio.h>
#include <stdlib.h>

#include "sys/clock.h"

//#define BUTTON_PRESS_EVENT_INTERVAL (CLOCK_SECOND)

//static uint8_t rtc_buffer[sizeof(simple_td_map)];
//static simple_td_map *simple_td = (simple_td_map *)rtc_buffer;

/*---------------------------------------------------------------------------*/
//#ifndef DATE
//#define DATE "Unknown"
//#endif
/*---------------------------------------------------------------------------*/

#define PRESCALER_VALUE         (SYS_CTRL_SYS_CLOCK / SYS_CTRL_32MHZ - 1)

/*---------------------------------------------------------------------------*/
// Interruption
/*---------------------------------------------------------------------------*/

void timer1A_isr (void){
	printf("flag before: %lu\n", REG(GPT_1_BASE + GPTIMER_RIS) & GPTIMER_RIS_TATORIS);
	printf("I am an interruption : Hello world !\n");
	leds_toggle(LEDS_PURPLE);
	long unsigned int timerR = REG(GPT_1_BASE + GPTIMER_TAR);
	long unsigned int timerV = REG(GPT_1_BASE + GPTIMER_TAV);
	printf("timer : %lu \tTAV :  %lu\n", timerR, timerV);
	//rtcc_print(RTCC_PRINT_DATE_DEC);
	
	//clear interrupt flag
	REG(GPT_1_BASE + GPTIMER_ICR) |= GPTIMER_ICR_TATOCINT;
	printf("flag after: %lu\n", REG(GPT_1_BASE + GPTIMER_RIS) & GPTIMER_RIS_TATORIS);
}


/*---------------------------------------------------------------------------*/
PROCESS(helloWorld, "Hello World");
AUTOSTART_PROCESSES(&helloWorld);
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(helloWorld, ev, data)
{

	int i = 0 ;

	PROCESS_BEGIN();
	
	//Initialization
	REG(SYS_CTRL_RCGCGPT) |= SYS_CTRL_RCGCGPT_GPT1;				//enable clock for GPTM1
	

	//pin assign
	//ioc_set_sel(GPIO_D_NUM, 5, IOC_PXX_SEL_GPT1_ICP1);			//LED RED
	//ioc_set_over(GPIO_A_NUM, 6, IOC_OVERRIDE_PUE);
	//GPIO_PERIPHERAL_CONTROL(GPIO_PORT_TO_BASE(GPIO_A_NUM), GPIO_PIN_MASK(6));

/*	
	//Input edge timing mode
	REG(GPT_1_BASE + GPTIMER_CTL) = 0;						//timer disabled
	REG(GPT_1_BASE + GPTIMER_CFG) = 0x00000004;				//configuration
	REG(GPT_1_BASE + GPTIMER_TAMR) |= GPTIMER_TAMR_TAMR;
	REG(GPT_1_BASE + GPTIMER_TAMR) |= GPTIMER_TAMR_TACMR;
	REG(GPT_1_BASE + GPTIMER_CTL) = 0;
	REG(GPT_1_BASE + GPTIMER_TAILR) |= 16000*1000;				//interval load
	REG(GPT_1_BASE + GPTIMER_IMR) |= GPTIMER_IMR_CAMIM;
	REG(GPT_1_BASE + GPTIMER_CTL) |= GPTIMER_CTL_TAEN;			//enable interrupt
*/
	


	//Periodic mode test
	REG(GPT_1_BASE + GPTIMER_CTL) = 0;						//timer disabled
	REG(GPT_1_BASE + GPTIMER_CFG) = 0x00000000;				//configuration
	REG(GPT_1_BASE + GPTIMER_TAMR) |= GPTIMER_TAMR_TAMR_PERIODIC;	//periodic mode
	REG(GPT_1_BASE + GPTIMER_TAMR) |= GPTIMER_TAMR_TASNAPS;		//capture the value of the free-running timer at time-out
	REG(GPT_1_BASE + GPTIMER_TAMR) |= GPTIMER_TAMR_TACDIR;		//counting up
	REG(GPT_1_BASE + GPTIMER_TAILR) = 0xF42400;				//interval load
	REG(GPT_1_BASE + GPTIMER_TAPR) = 0xF42400;					//prescale value
	REG(GPT_1_BASE + GPTIMER_IMR) |= GPTIMER_IMR_TATOIM;			//interrupt time out mode
	REG(GPT_1_BASE + GPTIMER_CTL) |= GPTIMER_CTL_TAEN;			//enable interrupt
	

	
	long unsigned int status = REG(GPT_1_BASE + GPTIMER_RIS) & GPTIMER_RIS_TATORIS;
	//long unsigned int timerR = (REG(GPT_1_BASE + GPTIMER_TAR));
	long unsigned int timerV = (REG(GPT_1_BASE + GPTIMER_TAV));
	
	
	long unsigned int interval = REG(GPT_1_BASE + GPTIMER_TAILR);
	//long unsigned int prescale = REG(GPT_1_BASE + GPTIMER_TAPR);
	
	
	
/*
	//rtcc test
	static char *next;
	
	
	//Get the date
	printf("RE-Mote Hello World test, system date: %s\n", DATE);
	
	if(strcmp("Unknown", DATE) == 0) {
    		printf("Fail: could not retrieve date from system\n");
    		PROCESS_EXIT();
  	}
	
	//Initialization & Configuration
	rtcc_init();
	RTCC_REGISTER_INT1(timer0B_irq);
	
	button_sensor.configure(BUTTON_SENSOR_CONFIG_TYPE_INTERVAL, BUTTON_PRESS_EVENT_INTERVAL);
	
	// Configure the RTC with the current values
	simple_td->weekdays    = (uint8_t)strtol(DATE, &next, 10);
  	simple_td->day         = (uint8_t)strtol(next, &next, 10);
  	simple_td->months      = (uint8_t)strtol(next, &next, 10);
  	simple_td->years       = (uint8_t)strtol(next, &next, 10);
  	simple_td->hours       = (uint8_t)strtol(next, &next, 10);
  	simple_td->minutes     = (uint8_t)strtol(next, &next, 10);
  	simple_td->seconds     = (uint8_t)strtol(next, &next, 10);
  	simple_td->miliseconds = (uint8_t)strtol(next, NULL, 10);
  	
  	simple_td->mode = RTCC_24H_MODE;
  	
  	simple_td->century = RTCC_CENTURY_20XX;
  	
  	// Set the time and date
  	if(rtcc_set_time_date(simple_td) == AB08_ERROR) {
    		printf("Fail: Time and date not configured\n");
    		PROCESS_EXIT();
  	}
	
	rtcc_set_alarm_time_date(simple_td, RTCC_ALARM_ON, RTCC_REPEAT_10THS);
*/
		
/*
	//gpio test	
	//Configure the interrupts pins 
  	GPIO_SOFTWARE_CONTROL(GPTM_INT1_PORT_BASE, GPTM_INT1_PIN_MASK);
  	GPIO_SET_INPUT(GPTM_INT1_PORT_BASE, GPTM_INT1_PIN_MASK);
  	
  	// Pull-up resistor, detect falling edge 
  	GPIO_DETECT_EDGE(GPTM_INT1_PORT_BASE, GPTM_INT1_PIN_MASK);
  	GPIO_TRIGGER_SINGLE_EDGE(GPTM_INT1_PORT_BASE, GPTM_INT1_PIN_MASK);
  	GPIO_DETECT_FALLING(GPTM_INT1_PORT_BASE, GPTM_INT1_PIN_MASK);
  	gpio_register_callback(timer0B_irq, GPIO_A_NUM, 0);
  
	//init GPTM
	REG(SYS_CTRL_RCGCGPT) |= SYS_CTRL_RCGCGPT_GPT0;
	
	//configuration
	
	nvic_interrupt_enable(NVIC_INT_GPTIMER_0B);
	REG(GPT_0_BASE + GPTIMER_CFG) |= 0x4;				//16 bit timer comfiguration
	REG(GPT_0_BASE + GPTIMER_CTL) |= GPTIMER_CTL_TBEN;		//Timer B is enabled
	REG(GPT_0_BASE + GPTIMER_SYNC) |= GPTIMER_SYNC_SYNC0;		//Synchronization
	REG(GPT_0_BASE + GPTIMER_IMR) |= GPTIMER_IMR_CBEIM;		//Capture event enable
*/	
	
	
	
	
	while(1){
		//PROCESS_YIELD();
		//printf("i : %d\t status : %lu\n",i, REG(GPT_1_BASE + GPTIMER_RIS) & GPTIMER_RIS_TATORIS);
		status = REG(GPT_1_BASE + GPTIMER_RIS) & GPTIMER_RIS_TATORIS;
		//timerR = (REG(GPT_1_BASE + GPTIMER_TAR));
		timerV = (REG(GPT_1_BASE + GPTIMER_TAV));
		
		//printf("%d\t\t status : %lu\t timerR : %lu\t timerV : %lu\n", i, status, timerR, timerV);
		
		if (status || timerV == interval){
			//printf("%lu\n", status);
			timer1A_isr();
		}
		
		//clock_wait(100);
		
		/*
		if (data == &button_sensor){
			//printf("%lu\n",REG(GPT_1_BASE + GPTIMER_RIS) & GPTIMER_RIS_CAMRIS);
			printf("Button !\n");
		}*/
		//i++;
	}

	PROCESS_END();
}

void config_timer(){


}
