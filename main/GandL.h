#ifndef GandL_h
#define GandL_h
#include <stdint.h>
	enum{G_RPM=0,G_MPH=1,G_Gas=2,G_Temp=3,G_Lights=4};
	void sendInfo(uint8_t gauge, uint16_t value);
	void sendVFD(uint8_t *c, uint8_t n);
	void testVFD(uint16_t value);
	extern void vfdPrep(void);
	extern void updateGuages_Lights(void);
#endif
