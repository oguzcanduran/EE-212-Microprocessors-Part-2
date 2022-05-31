#include <MKL25Z4.H>
#include "math.h"
#include "lcd.h"
#include <stdio.h>
#include <string.h>
#define ECHO_PIN_RISING (0) //PTB0 - TPM1_CH0 -ALT3
//#define ECHO_PIN_FALLING (1) //PTB1 - TPM1_CH1 - ALT3
#define TRIGGER_PIN (0) //PTD0 - TPM0_CH0 ALT4

int delay = 100;
int direction = 0;
int count=0;
volatile int data;
volatile int data_array[60];
volatile int finishflag=0;
volatile int pompa=0;
volatile int p;
volatile int temparea;
volatile int pompadelay;
volatile int f;
volatile int distance;
volatile int previous=0;
volatile int current=0;
int j;
char buffer[10];
volatile unsigned int interval=0; //interval for input capture-time between rising edge





void delayMs(int n) {
    int i;
    int j;

    for(i = 0 ; i < n; i++)
        for (j = 0; j < 100; j++) {}
}
	volatile	int m;





const char steps[ ] = {0x9, 0x3, 0x6, 0xC}; //1001,0101,0110,1100
int i = 0;    

int main (void) {
    
		init_lcd_pins();
		init_pins();
		init_timer();

		
		lcd_init();
		lcd_clear();
		
		lcd_goto(0,0);
		lcd_write_string("UltrasonicSensor");
		lcd_goto(0,1);
		lcd_write_string("---Is Scanning---");
    
    /* PTD 3, 2, 1, 0 for motor control */
    SIM->SCGC5 |= 0x2800;       /* enable clock to Port E */
    PORTC->PCR[0] = 0x100;      /* make PTD0 pin as GPIO */
    PORTC->PCR[1] = 0x100;      /* make PTD1 pin as GPIO */
    PORTC->PCR[2] = 0x100;      /* make PTD2 pin as GPIO */
    PORTC->PCR[3] = 0x100;      /* make PTD3 pin as GPIO */
    PTC->PDDR |= 0x0F;          /* make PTD3-0 as output pin */
		
    PORTE->PCR[0] = 0x100;      /* make PTD0 pin as GPIO */
    PORTE->PCR[1] = 0x100;      /* make PTD1 pin as GPIO */
    PORTE->PCR[2] = 0x100;      /* make PTD2 pin as GPIO */
    PORTE->PCR[3] = 0x100;      /* make PTD3 pin as GPIO */
    PTE->PDDR |= 0x0F;          /* make PTD3-0 as output pin */

    while(1) {

			for (j=0;j<3000;j++){  // step motorlar baslangiç
				 count++;
				if (j%50==0){
					
					if (distance==31 || distance==30)   // ölçülecek biçilecek
					{ finishflag=1;
					PTE->PDOR = 0;
					PTC->PDOR=0;
					break;}
					else if (distance >15){distance=previous-14;}
					
					else{
						current=14-distance;
						temparea+=previous*current/2;
						previous=current;
					
					//data_array[j%50]=distance;
				/*
					if (j%200==0){
						sprintf(buffer, "Distance: %d cm", distance);
						lcd_goto(0,1);
						lcd_write_string(buffer);
						memset(buffer, 0, sizeof buffer);
					}  */ 
					
					}   }
				
					
				PTC->PDOR = (steps[i++& 3]);  // yukari motor

				if (j%10==0){PTE->PDOR = (steps[m++ & 3]);}  // yatay motor
				
				delayMs(delay);}   // step motorlar bitis
			
				
				
				
			
				
				
			if(finishflag==1){break;}
	/*		
			for (p=1;p<60;p++){
				temparea+= data_array[p-1]*data_array[p]*0.104529/2 ;}
			temparea+=data_array[0]*data_array[59]*0.104529/2;      // temp area olustu her slice için
				
				*/
			pompa+=temparea;		
		}
		
		
		lcd_goto(0,0);
		lcd_write_string("Process Finished");
		sprintf(buffer,"Volume: %d mml", pompa);
		lcd_goto(0,1);
		lcd_write_string(buffer);
		memset(buffer, 0, sizeof buffer);
		
		SIM->SCGC5 |= 0x02000;
		PORTE->PCR[4] = 0x0100;
		PTE->PDDR |= 0x010;  
		PTE->PDOR = 0x010;
		delayMs(pompa*500); // 50000 for 1 sec
		PTE->PDOR = 0;  
		
		for (f=count-1;f>0;f--){
		
		PTC->PDOR = (steps[f& 3]);
		delayMs(100);
		}
		
}

/* Delay n milliseconds */
/* The CPU core clock is set to MCGFLLCLK at 41.94 MHz in SystemInit(). */

void TPM1_IRQHandler(){
		static int ctr=0;
		static unsigned int previous=0;
		unsigned int current=0;
		
		if (TPM1->STATUS & TPM_STATUS_CH0F_MASK){// check if input(rising or falling) occurred
				current=TPM1->CONTROLS[0].CnV;
				
				current |= (ctr <<16); // add the no. of overflows. current = CnV + ctr*MOD [(ctr*2^16)]
				//Each ctr tick is 2^16,
				//without above, current value could also be more than prev.
				interval = current-previous;
				previous=current;
				distance = interval * 0.10468;
				//sprintf(buffer, "Distance: %d", interval);
				//lcd_goto(0,1);
				//lcd_write_string(buffer);
				TPM1->CONTROLS[0].CnSC |=TPM_CnSC_CHF_MASK; //clear input capture flag
		}

		if (TPM1->SC & TPM_SC_TOF_MASK){
				ctr++; //a timer overflow occurred.
				TPM1->SC |= TPM_SC_TOF_MASK ; //clear the interrupt on timer overflow
		}
}



int calculate_time(int interval){
	return interval / 163840;  //interval * prescale/clock frequency
}


int calculate_distance(int time){
	return 17150 * time;   //(343/2) * time * 100
	
}
