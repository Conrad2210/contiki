/*
 * demonstrator.h
 *
 *  Created on: Aug 10, 2016
 *      Author: user
 */

#ifndef DEWI_NIMBUS_CONTIKI_EXAMPLES_DEWI_DEMONSTRATOR_H_
#define DEWI_NIMBUS_CONTIKI_EXAMPLES_DEWI_DEMONSTRATOR_H_

#include "contiki.h"
#include "cpu.h"
#include "sys/etimer.h"
#include "sys/rtimer.h"
#include "dev/leds.h"
#include "dev/uart.h"
#include "dev/button-sensor.h"
#include "dev/adc-zoul.h"
#include "dev/zoul-sensors.h"
#include "dev/watchdog.h"
#include "dev/serial-line.h"
#include "dev/sys-ctrl.h"
#include "i2c.h"
#include "net/netstack.h"
#include "net/rime/broadcast.h"
#include "net/rime/netflood.h"
#include "net/mac/tsch/tsch.h"
#include "net/mac/tsch/tsch-schedule.h"

#include "net/DEWI/scheduler/scheduler.h"
#include "net/DEWI/rll/rll.h"
#include "net/DEWI/cider/cider.h"
#include "net/DEWI/neighTable/neighTable.h"
#include <stdio.h>
#include <stdint.h>


void init_coordinator(void);
#endif /* DEWI_NIMBUS_CONTIKI_EXAMPLES_DEWI_DEMONSTRATOR_H_ */
