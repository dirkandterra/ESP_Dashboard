/*
 * handleSig.h
 *
 *  Created on: Sep 15, 2023
 *      Author: Ricker
 */

#ifndef MAIN_HANDLESIG_H_
#define MAIN_HANDLESIG_H_

#include <stdint.h>
#include "esp_attr.h"
#include "esp_err.h"
#define CONFIG_LOG_DEFAULT_LEVEL 3
#define LATCH_PIN 12
#define GAUGE_CS_PIN 16
#define VFD_LATCH_CLKEN_PIN 2
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<LATCH_PIN) | (1ULL<<GAUGE_CS_PIN)| (1ULL<<VFD_LATCH_CLKEN_PIN))

esp_err_t spi_delay_us(uint32_t time);

void senddispToVFD(void);
char storedispdata(char);
char iconupdate(char);
void pulseLCDClk(void);
void pulseLLatch(void);
void pulseGLClk(void);
void dashDelay(char);
uint8_t getDCLevel(void);
void init_GPIO(void);


void sendToGauges(void);
void sendToLights(void);
void sendVFDDimming(void);
void pulseLLatch(void);
extern unsigned char gaugeString[8];
extern unsigned char lightString[2];
extern unsigned char vfdString[8];

void checkForCSLatchTrigger(void);


#endif /* MAIN_HANDLESIG_H_ */
