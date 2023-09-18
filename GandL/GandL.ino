#include <avr/io.h>
#include "common.h"
#include "GandL.h"
#include "IntrVFD.h"

uint32_t timerGaugeAndLights=0;
uint32_t weatherInfoUpdate=0;
uint8_t display_cycle=0;
uint32_t bootDelay=0;
typedef struct rxBuff{
  uint8_t data[10];
  uint8_t rxPtr;
}rxBuff;

rxBuff rx232;
boolean stringComplete=false;
void resetDashboard(void);
uint8_t timerCheck(uint32_t tmr, uint32_t setpoint){
  uint32_t now=millis();
  if(now>(tmr+setpoint)){
    return true;
  }
  else
  {
    return false;
  }
}

void handle232(void);
double del = 100000;

void setup() { 
  DDRD |= 0xF0; 
  DDRB |= 0x07;
  Serial.begin(38400); 
  rx232.rxPtr=0;
  Serial.println("Starting...");
  sendInfo(G_Gas,100);
  sendInfo(G_RPM, 700);
  sendInfo(G_MPH, 1200);
  sendInfo(G_Temp, 100);
  sendInfo(G_Lights,2047);
  while(!timerCheck(bootDelay,3000)){
    if(timerCheck(timerGaugeAndLights,50)){
      updateGuages_Lights();
      timerGaugeAndLights=millis();
    }
  }
  resetDashboard();
}

void loop() {
  //sendInfo(0, 20);
  if(stringComplete){
    stringComplete=false;
    handle232();
  }
  if(timerCheck(timerGaugeAndLights,50)){
    updateGuages_Lights();
    timerGaugeAndLights=millis();
  }
  if(timerCheck(weatherInfoUpdate,2000) && prepIt){
    weatherInfoUpdate=millis();
    sendVFDWeather(display_cycle);
    sendInfo(G_Gas,display_cycle*10);
    display_cycle++;
	  if(display_cycle>7){display_cycle=0;}
    Serial.println("Zip");
  }
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    
    if (inChar == '\r') {
      stringComplete = true;
    }
    else if(inChar=='\n'){
      
    }      
    else{
      rx232.data[rx232.rxPtr] = inChar;
      rx232.rxPtr++;
    }
  }
}

void resetDashboard(void){
    Serial.println("Clear Dashboard");
    clearDisp();
    updateVFD();
    sendInfo(G_Gas,0);
    sendInfo(G_RPM,0);
    sendInfo(G_MPH,0);
    sendInfo(G_Temp,0);
    sendInfo(G_Lights,0);
}

void handle232(){
  uint8_t dataLength=rx232.rxPtr;
  uint16_t multiplier=1, temp16=0;
  if(dataLength>0){
    switch(rx232.data[0]){
      case '0':
        dataLength-=1;   //Don't count the first byte identifier
        for(int ii=dataLength; ii>0; ii--){
            temp16+=(rx232.data[ii]-0x30)*multiplier;
            multiplier*=10;
        }
        Serial.println(temp16);  
        sendInfo(G_RPM, temp16);
        break;
     case '1':
        dataLength-=1;   //Don't count the first byte identifier
        for(int ii=dataLength; ii>0; ii--){
            temp16+=(rx232.data[ii]-0x30)*multiplier;
            multiplier*=10;
        }
        Serial.println(temp16);  
        sendInfo(G_MPH, temp16);
        break;
     case '2':
        dataLength-=1;   //Don't count the first byte identifier
        for(int ii=dataLength; ii>0; ii--){
            temp16+=(rx232.data[ii]-0x30)*multiplier;
            multiplier*=10;
        }
        Serial.println(temp16);  
        sendInfo(G_Gas, temp16);
        break;
     
    case '3':
        dataLength-=1;   //Don't count the first byte identifier
        for(int ii=dataLength; ii>0; ii--){
            temp16+=(rx232.data[ii]-0x30)*multiplier;
            multiplier*=10;
        }
        Serial.println(temp16);  
        sendInfo(G_Temp, temp16);
        break;      
     case '4':
        dataLength-=1;   //Don't count the first byte identifier
        for(int ii=dataLength; ii>0; ii--){
            temp16+=(rx232.data[ii]-0x30)*multiplier;
            multiplier*=10;
        }
        Serial.println(temp16);  
        sendInfo(G_Lights, temp16);
        break;
     case '5':
        dataLength-=1;   //Don't count the first byte identifier
        for(int ii=dataLength; ii>0; ii--){
            temp16+=(rx232.data[ii]-0x30)*multiplier;
            multiplier*=10;
        }
        Serial.println("Dead Command");
        break;
     case '6':
        dataLength-=1;   //Don't count the first byte identifier
        if(dataLength>6){
          dataLength=6;
        }
        rx232.data[dataLength+1]=0x00;
        for(int ii=0; ii<6; ii--){
            if(ii<=dataLength){
              vfd[5-ii]=rx232.data[1+ii];
            }else{
              vfd[5-ii]=0x20;
            }
        }
        updateVFD();
        break;

     case '7':
        dataLength-=1;   //Don't count the first byte identifier
        for(int ii=dataLength; ii>0; ii--){
            temp16+=(rx232.data[ii]-0x30)*multiplier;
            multiplier*=10;
        }
        vfd[7]|=0x80;
        vfd[3]=rx232.data[1];
        vfd[2]=rx232.data[2];
        vfd[1]=rx232.data[3];
        vfd[0]=rx232.data[4];
        vfd[5]=' ';  
        vfd[4]=' ';  
        Serial.println(temp16);  
        sendInfo(G_RPM, temp16/10);
        updateVFD();
        break;

     case '8':
        dataLength-=1;   //Don't count the first byte identifier 
        printTextToVFD("L",0,2,J_LEFT,vfd);
        printNumToVFD(1013,2,4,1,J_RIGHT,vfd);
        updateVFD();
        break;

      case '9':
        prepIt=!prepIt;
        break;
      case 'c':
        resetDashboard();
        break;
      case 'd':
        debug=!debug;
        Serial.print("Debug: ");
        Serial.println(debug,HEX);
        break;
      default:
        break;
       
      
    }
  }
  rx232.rxPtr=0;

}

