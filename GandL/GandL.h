#ifndef GandL_h
#define GandL_h
  enum{G_RPM=0,G_MPH=1,G_Gas=2,G_Temp=3,G_Lights=4};
	int sendInfo(uint8_t gauge, uint16_t value);
  int sendVFD(uint8_t *c, uint8_t n);
	int testVFD(uint16_t value);
  extern void vfdPrep(void);
  void updateGuages_Lights(void);

#endif
