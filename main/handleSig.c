/*
 * handleSig.c
 *
 *  Created on: Sep 15, 2023
 *      Author: Ricker
 */

#include "handleSig.h"
#include <stdio.h>
#include <stdint.h>
#include "esp_attr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp8266/gpio_struct.h"
#include "esp8266/spi_struct.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_libc.h"

#include "driver/gpio.h"
#include "driver/spi.h"

#define LATCH_PIN 4
#define GAUGE_CS_PIN 5
#define VFD_LATCH_PIN 2

char z,k;
char temp=0;
unsigned char datachar[9];
uint16_t spiCmd;
unsigned char gaugeString[8];
unsigned char lightString[2];
unsigned char vfdString[8];
//char vfdDimming=0xD0;
char vfdDimming=0x08;
uint8_t latchTrigger=0;
uint8_t gaugeCSTrigger=0;
uint8_t vfdLatchTrigger=0;

static uint8_t spi_dc_level = 0;

esp_err_t spi_delay_us(uint32_t time)
{
    vTaskDelay(time / portTICK_RATE_MS /1000);
    return ESP_OK;
}

uint8_t getDCLevel(void){
	return spi_dc_level=0;
}

static esp_err_t spi_set_dc(uint8_t dc)
{
    spi_dc_level = dc;
    return ESP_OK;
}

// Write an 8-bit cmd
static esp_err_t spi_write_cmd(uint16_t bits, uint8_t cmdBits)
{
    uint32_t buf[2];
    uint16_t cmd[1];
    cmd[0] = spiCmd<<(16-cmdBits);
    buf[1] = (datachar[7] << 24) + (datachar[6]<<16)+(datachar[5]<<8)+datachar[4];
    buf[0] = (datachar[3] << 24) + (datachar[2]<<16)+(datachar[1]<<8)+datachar[0]; // In order to improve the transmission efficiency, it is recommended that the external incoming data is (uint32_t *) type data, do not use other type data.
    spi_trans_t trans = {0};
    trans.cmd=cmd;
    trans.bits.cmd=cmdBits;
    trans.mosi = buf;
    trans.bits.mosi = bits;
    spi_set_dc(0);
    spi_trans(HSPI_HOST, &trans);
    return ESP_OK;
}


//-_-_-_-_-_-_-_-_ Send Dimming Info to VFD -_-_-_-_-_-_-
void sendVFDDimming()
{
	//Load SPI data
	//1,1,1,1,datachar
	spiCmd=0x0F;
	datachar[0]=vfdDimming;

	//handle clock selection/cs

	spi_write_cmd(8,4);
}

void senddispToVFD(void){
	//10
	//Handle the clock enable to the vfd
	spiCmd=0x02;
	datachar[7]=vfdString[0];
	datachar[6]=vfdString[1];
	datachar[5]=vfdString[2];
	datachar[4]=vfdString[3];
	datachar[3]=vfdString[4];
	datachar[2]=vfdString[5];
	datachar[1]=vfdString[6];
	datachar[0]=vfdString[7];

	spi_write_cmd(64,2);
	latchTrigger=8;
}

//Pulse the Latch for Lights
void pulseLLatch(){
	gpio_set_level(LATCH_PIN, 1);
	spi_delay_us(5);
	gpio_set_level(LATCH_PIN, 0);
}

//Pulse for VFD data
void pulseVLatch(){
	gpio_set_level(VFD_LATCH_PIN, 1);
	spi_delay_us(5);
	gpio_set_level(VFD_LATCH_PIN, 0);
}

//Put the Gauge chip select pin where it needs to be
void gaugeCS(uint8_t level){
	gpio_set_level(GAUGE_CS_PIN, level);
}

//After the SPI transfer, do we need to send a latch signal?
void checkForCSLatchTrigger(void){
	if(latchTrigger>0){
		latchTrigger--;
		if(latchTrigger==0){
			pulseLLatch();
		}
	}else if(gaugeCSTrigger>0){
		gaugeCSTrigger--;
		if(gaugeCSTrigger==0){
			gaugeCS(0);
		}
	}else if(vfdLatchTrigger>0){
		vfdLatchTrigger--;
		if(vfdLatchTrigger==0){
			pulseVLatch();
		}
	}
}

//SPI send to lights
void sendToLights()
{

	spiCmd=lightString[0] & 0x0F;
	datachar[0]=lightString[1];

	spi_write_cmd(8,4);
	latchTrigger=1;
}

//SPI send to Gauges
void sendToGauges()
{
	//PORTD |= B10000000;							//Chip select High
	//there will be (4) 10 bit xfers,pack the data
	datachar[4]=(gaugeString[0]<<6)+(gaugeString[1]>>2);
	datachar[3]=(gaugeString[1]<<6)+(gaugeString[2]<<4)+(gaugeString[3]>>4);
	datachar[2]=(gaugeString[3]<<4)+(gaugeString[4]<<2)+(gaugeString[5]>>6);
	datachar[1]=(gaugeString[5]<<2)+(gaugeString[6]&0x03);
	datachar[0]=gaugeString[7];
	gaugeCS(1);	//set the chip select high
	gaugeCSTrigger=5;
	spi_write_cmd(40,0);
}


