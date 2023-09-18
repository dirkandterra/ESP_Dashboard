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

#define OLED_DC_GPIO     12
#define OLED_RST_GPIO    15
#define OLED_PIN_SEL  (1ULL<<OLED_DC_GPIO) | (1ULL<<OLED_RST_GPIO)

esp_err_t spi_delay_us(uint32_t time);

void senddispToVFD(void);
char storedispdata(char);
char iconupdate(char);
void pulseLCDClk(void);
void pulseLLatch(void);
void pulseGLClk(void);
void dashDelay(char);
uint8_t getDCLevel(void);


void sendToGauges(void);
void sendToLights(void);
void sendVFDDimming(void);
void pulseLLatch(void);
extern unsigned char gaugeString[8];
extern unsigned char lightString[2];
extern unsigned char vfdString[8];

void checkForCSLatchTrigger(void);


#endif /* MAIN_HANDLESIG_H_ */
