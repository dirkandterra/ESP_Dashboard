//Guage and Lights Only
//#include "common.h"
#include "GandL.h"
#include "handleSig.h"



int gaugeData[25]={0,34,65,96,125,155,181,204,232,256,279,303,329,352,382,410,438,466,497,528,559,590,621,649,677};
int gaugeData2[25]={0,23,44,62,83,102,120,138,156,176,196};
char CBuffLen=0;
char t0_int;// = 0x30;
int t0_cyc=0;
char t0_disp;
char segrotate=1;
char digsel=2;
int rpmData=261;
int mphData=261;
int gasData=38;
int tempData=38;
int lightData=1;
char mode=0;
int oldMPH=261;
int oldRPM=261;
uint8_t dectrip=0;
uint8_t vfd_count=0;
//261,5.25,22,41.1,61.8,80,96.5,113.5,936
int gypsyMath(int in);
int gypsyMath2(int in);
void vfdPrep(void);


void sendInfo(uint8_t gauge, uint16_t value){
	
	switch(gauge){
		
		case 0:
			rpmData=(int)((double)value*12/7);
			rpmData=gypsyMath(rpmData);
			break;
	
		case 1:
			mphData=(int)value;
			mphData=gypsyMath(mphData);
			break;
		
		case 2:
			gasData=(int)value;
			gasData=gypsyMath2(gasData);
			break;
			
		case 3:
			tempData=(int)value;
			tempData=gypsyMath2(tempData);
			break;
			
		default:
		case 4:
			lightData=(int)value;
			break;
	}

}

//****************************************************
void updateGuages_Lights(){
  int diff;

  diff=rpmData-oldRPM;
  if (diff<100 && diff>-100) {oldRPM=rpmData;}
  else {oldRPM=oldRPM+diff/4;}

  diff=mphData-oldMPH;
  if (diff<100 && diff>-100) {oldMPH=mphData;}
  else {oldMPH=oldMPH+diff/4;}


  gaugeString[0]=oldMPH/256;
  gaugeString[1]=oldMPH%256;
  gaugeString[2]=oldRPM/256;
  gaugeString[3]=oldRPM%256;
  gaugeString[4]=tempData/256;
  gaugeString[5]=tempData%256;
  gaugeString[6]=gasData/256;
  gaugeString[7]=gasData%256;
  sendToGauges();

  lightString[0]=lightData/256;
  lightString[1]=lightData%256;
  sendToLights();
}

int gypsyMath(int in)
{
if (in>=1200)
	{
		return 937;	//676+261
	}
	else
	{
		int i=in/50;
		i=((in%50)*(gaugeData[i+1]-gaugeData[i]))/50 +gaugeData[i];
		return (i+261);
	}
}

int gypsyMath2(int in)
{
if (in>=100)
  {
    return 232;  //38+192
  }
  else
  {
    int i=in/10;
    i=((in%10)*(gaugeData2[i+1]-gaugeData2[i]))/10 +gaugeData2[i];
    return (i+38);
  }
}
