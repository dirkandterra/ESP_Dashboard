/* spi_oled example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include "sdkconfig.h"
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
#include "handleSig.h"
#include "GandL.h"
#include "IntrVFD.h"
#include "rs232Handler.h"

static const char *TAG = "ESP_Dash";
uint8_t bootDelay=60;

#define SPI_MOSIONLY_INTERFACE   0x040    /* CS_EN:0, MISO_EN:0, MOSI_EN:1, BYTE_TX_ORDER:0, BYTE_TX_ORDER:0, BIT_RX_ORDER:0, BIT_TX_ORDER:0, CPHA:0, CPOL:0 */

static void IRAM_ATTR spi_event_callback(int event, void *arg)
{
    switch (event) {
        case SPI_INIT_EVENT: {

        }
        break;
        case SPI_TRANS_START_EVENT: {
        	if(SPIDEBUG){ESP_LOGI(TAG, "Callback %d",0);}
        }
        break;
        case SPI_TRANS_DONE_EVENT: {
			checkForCSLatchTrigger();			//Will trigger latch or cs high/low if needed
        }
        break;
        case SPI_DEINIT_EVENT: {
        }
        break;
    }
}

void maxDashboard(void){
	  sendInfo(G_Gas,100);
	  sendInfo(G_RPM, 700);
	  sendInfo(G_MPH, 1200);
	  sendInfo(G_Temp, 100);
	  sendInfo(G_Lights,2047);
}

void resetDashboard(void){
	ESP_LOGI(TAG, "Clear Dashboard");
    //clearDisp();
    //updateVFD();
    sendInfo(G_Gas,0);
    sendInfo(G_RPM,0);
    sendInfo(G_MPH,0);
    sendInfo(G_Temp,0);
    sendInfo(G_Lights,0);
}

static void IRAM_ATTR dash_periodic_task(void* arg)
{
	int x=0;
	while (1) {
		//Clear display after boot-test
		if(bootDelay>0){
			bootDelay--;
			if(bootDelay==0){
				resetDashboard();
			}
		}
		if(CommandLen>0){
			switch (CommandBuff[0]){
			default:
			case 0:
				break;
			case '5':
				sendInfo(G_Gas,50);
				sendInfo(G_RPM, 350);
				sendInfo(G_MPH, 600);
				sendInfo(G_Temp, 50);
				break;
			case '0':
				sendInfo(G_Gas,0);
				sendInfo(G_RPM,0);
				sendInfo(G_MPH,0);
				sendInfo(G_Temp,0);
				break;
			}
			CommandLen=0;
		}
		if(bootDelay>0 && (bootDelay%5==0)){ESP_LOGI(TAG, "sent %d", bootDelay);}
		//sendVFDDimming();
		vTaskDelay(50 / portTICK_RATE_MS);
		updateGuages_Lights();
		x++;
	}
}

void app_main(void)
{

    ESP_LOGI(TAG, "init gpio");
	init_GPIO();
    ESP_LOGI(TAG, "init hspi");
    spi_config_t spi_config;
    // Load default interface parameters
    // CS_EN:1, MISO_EN:1, MOSI_EN:1, BYTE_TX_ORDER:1, BYTE_TX_ORDER:1, BIT_RX_ORDER:0, BIT_TX_ORDER:0, CPHA:0, CPOL:0
    spi_config.interface.val = SPI_MOSIONLY_INTERFACE;
    // Load default interrupt enable
    // TRANS_DONE: true, WRITE_STATUS: false, READ_STATUS: false, WRITE_BUFFER: false, READ_BUFFER: false
    spi_config.intr_enable.val = SPI_MASTER_DEFAULT_INTR_ENABLE;
    // CPOL: 1, CPHA: 1
    spi_config.interface.cpol = 1;
    spi_config.interface.cpha = 1;
    // Set SPI to master mode
    // 8266 Only support half-duplex
    spi_config.mode = SPI_MASTER_MODE;
    // Set the SPI clock frequency division factor
    spi_config.clk_div = SPI_2MHz_DIV;
    // Register SPI event callback function
    spi_config.event_cb = spi_event_callback;
    spi_init(HSPI_HOST, &spi_config);

    //ESP_LOGI(TAG, "init clear");
    //resetDashboard();
    maxDashboard();
    rs232Init();

    // Create a task to handler UART event from ISR
     xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, 12, NULL);
     xTaskCreate(dash_periodic_task, "dashboard_task", 2048, NULL, 4, NULL);

}
