/*
 * rs232Handler.h
 *
 *  Created on: Sep 19, 2023
 *      Author: Ricker
 */

#ifndef MAIN_RS232HANDLER_H_
#define MAIN_RS232HANDLER_H_
#include <stdint.h>

char CommandBuff[20];
extern uint8_t CommandLen;

extern uint8_t SPIDEBUG;

extern void uart_event_task(void *pvParameters);
void rs232Init(void);


#endif /* MAIN_RS232HANDLER_H_ */
