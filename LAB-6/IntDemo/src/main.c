
#include <MKL25Z4.H>
#include <stdio.h>
#include "switches.h"
#include "math.h"


extern volatile unsigned switch_pressed;
extern void init_debug_signals(void);


void DAC0_init(void);
void delayrifat(int n);
void sineWave (void);
void TriangeWave(void);
void squareWave(void);
void ADC0_init(void);
void potwave (void);
static int result=3000;
int main (void) {
	 
	init_switch();
	__enable_irq();
	DAC0_init(); 
	ADC0_init();
	ADC0->SC1[0] = 0;
	while (1) {
		DEBUG_PORT->PTOR = MASK(DBG_MAIN_POS);
		if((ADC0->SC1[0] & 0x80)) {result = ADC0->R[0]; 
		ADC0->SC1[0] = 0;} 
		if (count ==0){TriangeWave();}
		else if (count == 1){squareWave();}
		else if (count == 2) {sineWave();}
		else {potwave();}
	}
}

void TriangeWave(void)
	{
	int i;
	int result2;
	 double TriangeWave[] = {0.498, 0.664, 0.830, 1, 0.830, 0.664, 0.498,0.332, 0.166, 0, 0.166, 0.332};


		for (i = 0; i < 12; i++) {

			int result2=result*TriangeWave[i];
			DAC0->DAT[0].DATL = result2 & 0xff; 
			DAC0->DAT[0].DATH = (result2 >> 8) & 0x0f;
			delayrifat(271); 
		
	}
}

void squareWave(void){ 
unsigned int i=0;
 double squareWave[] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

for (i = 0; i < 12; i++) {
	int result2=result*squareWave[i];
			DAC0->DAT[0].DATL = result2 & 0xff; 
			DAC0->DAT[0].DATH = (result2 >> 8) & 0x0f;
			delayrifat(298);
		
	}
}

void sineWave (void)
	{
	int i;
	 double sineWave[] = {0.5, 0.75, 0.933, 1.0, 0.933, 0.75, 0.5, 0.25, 0.067, 0, 0.067, 0.25};
		for (i = 0; i < 12; i++) {
	int result2=result*sineWave[i];
			DAC0->DAT[0].DATL = result2 & 0xff; 
			DAC0->DAT[0].DATH = (result2 >> 8) & 0x0f;
			delayrifat(274); 
		
	}
}

void DAC0_init(void) 
{
	SIM->SCGC6 |= 0x80000000; 
	DAC0->C1 = 0; 
	DAC0->C0 = 0x80 | 0x20; 
}

void delayrifat(int n) 
{
	int i;
	int j;
	for(i = 0 ; i < n; i++)
	for (j = 0; j < 1; j++) {}
}

void ADC0_init(void)
{
	SIM->SCGC5 |= 0x2000; 
	PORTE->PCR[20] = 0; 
	SIM->SCGC6 |= 0x8000000; 
	ADC0->SC2 &= ~0x40;
	ADC0->CFG1 = 0x40 | 0x10 | 0x04 | 0x00;
}


void potwave (void)
	{
			DAC0->DAT[0].DATL = result&0xff; 
			DAC0->DAT[0].DATH = (result >> 8) & 0x0f;
			delayrifat(12); 
		
}