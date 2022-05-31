#include <MKL25Z4.H>
#include <stdio.h>
#include <math.h>


#define RS 0x04     /* PTA2 mask */ 
#define RW 0x10     /* PTA4 mask */ 
#define EN 0x20     /* PTA5 mask */

void Delay(volatile unsigned int time_del);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void LCD_init(void);
void LCD_ready(void);
void keypad_init(void);
uint32_t keypad_getkey(void);



int main(void){ 
uint32_t freq1=0;
uint32_t freq2=0;
uint32_t duty1=0;
uint32_t duty2=0;

int f1 ;
int f2 ;
int d1;
int d2;

unsigned int A = 0;
unsigned int B = 0;
unsigned int C = 0;
unsigned int D = 0;

int temp = 0;
	
	uint32_t key;
	uint32_t lookupnum[] = {1, 2, 3, 10, 4, 5, 6, 11, 7, 8, 9, 12, 14, 0, 15, 13}; 
	uint32_t digit;


	
			SIM->SCGC5 |= 0x2000; /* enable clock to Port E */
			PORTE->PCR[31] = 0x0300; /* PTE31 used by TPM0 */
			SIM->SCGC6 |= 0x01000000; /* enable clock to TPM0 */
			SIM->SOPT2 |= 0x01000000; /* use MCGFLLCLK as timer counter clock */
			

			SIM->SCGC5 |= 0x2000; /* enable clock to Port E */
			PORTE->PCR[20] = 0x0300; /* PTE20 used by TPM1 */
			SIM->SCGC6 |= 0x03000000; /* enable clock to TPM1  */
			SIM->SOPT2 |= 0x01000000; /* use MCGFLLCLK as timer counter clock */
			




	LCD_init();
	keypad_init();
	
	LCD_command(0x80);
	LCD_data('F');
	LCD_data('1');
	LCD_data(':');
	LCD_command(0x86);
	LCD_data('H');
	LCD_data('z');
	LCD_command(0x89);
	LCD_data('D');
	LCD_data('1');
	LCD_data(':');
	LCD_data('%');
	
	LCD_command(0xC0);
	LCD_data('F');
	LCD_data('2');
	LCD_data(':');
	LCD_command(0xC6);
	LCD_data('H');
	LCD_data('z');
	LCD_command(0xC9);
	LCD_data('D');
	LCD_data('2');
	LCD_data(':');
	LCD_data('%');




while(1){
	key = keypad_getkey();
	Delay(500000);
	if (key != 0){
		digit = lookupnum[key - 1];
		if (digit > 9){
			
			switch (digit){
				case 10:
					if (A){f1 = temp;}
					else{
						LCD_command(0x83);
						temp = 0;}
					A ^= 1;	
					break;
						
				case 11:
					if (B){d1 = temp;}
					else{
						LCD_command(0x8D);
						temp = 0;}
					B ^= 1;	
					break;
						
				case 12:
					if (C){f2 = temp;}
					else{
						LCD_command(0xC3);
						temp = 0;}
					C ^= 1;	
					break;
						
				case 13:
					if (D){d2 = temp;}
					else{
						LCD_command(0xCD);
						temp = 0;}
					D ^= 1;	
					break;
					
				case 14:
					f1 *= 2;
					f2 *= 2;
					break;
				case 15:	
					d1 += 10;
					d2 += 10;
					break;}
				}
		
			else if ((digit < 10) && (A | B | C | D)){
				LCD_data('0' + digit);
				temp = (temp*10) + digit;				
				}
			}
			freq1= round((float)(327656/f1)); 
			freq2 = round((float)(327656/f2));
			
			duty1 = round((float)((327656*d1)/(f1*100)));
			duty2 = round((float)((327656*d2)/(f2*100)));
			//TPM0->SC = 0; /* disable timer */
			TPM0->CONTROLS[4].CnSC = 0x20 | 0x08; /* edge-aligned, pulse high */
			TPM0->MOD = freq1; /* Set up modulo register for 60 kHz */
			TPM0->CONTROLS[4].CnV = duty1; /* Set up channel value for 33% dutycycle */
			TPM0->SC = 0x0D| 0x20; /* enable TPM0 with prescaler /16 */
			
			//TPM0->SC = 0; /* disable timer */
			TPM1->CONTROLS[0].CnSC = 0x20 | 0x08; /* edge-aligned, pulse high */
			TPM1->MOD = round(freq2); /* Set up modulo register for 60 kHz */
			TPM1->CONTROLS[0].CnV = round(duty2); /* Set up channel value for 33% dutycycle */
			TPM1->SC = 0x0D| 0x20; /* enable TPM0 with prescaler /16 */
		} 
}	




void LCD_init(void)
{
    SIM->SCGC5 |= 0x1000;       /* enable clock to Port D */ 
    PORTD->PCR[0] = 0x100;      /* make PTD0 pin as GPIO */
    PORTD->PCR[1] = 0x100;      /* make PTD1 pin as GPIO */
    PORTD->PCR[2] = 0x100;      /* make PTD2 pin as GPIO */
    PORTD->PCR[3] = 0x100;      /* make PTD3 pin as GPIO */
    PORTD->PCR[4] = 0x100;      /* make PTD4 pin as GPIO */
    PORTD->PCR[5] = 0x100;      /* make PTD5 pin as GPIO */
    PORTD->PCR[6] = 0x100;      /* make PTD6 pin as GPIO */
    PORTD->PCR[7] = 0x100;      /* make PTD7 pin as GPIO */
    PTD->PDDR = 0xFF;           /* make PTD7-0 as output pins */
    
    SIM->SCGC5 |= 0x0200;       /* enable clock to Port A */ 
    PORTA->PCR[2] = 0x100;      /* make PTA2 pin as GPIO */
    PORTA->PCR[4] = 0x100;      /* make PTA4 pin as GPIO */
    PORTA->PCR[5] = 0x100;      /* make PTA5 pin as GPIO */
    PTA->PDDR |= 0x34;          /* make PTA5, 4, 2 as output pins */
    
    LCD_command(0x38);      /* set 8-bit data, 2-line, 5x7 font */
    LCD_command(0x01);      /* clear screen, move cursor to home */
    LCD_command(0x0F);      /* turn on display, cursor blinking */
}

/* This function waits until LCD controller is ready to
 * accept a new command/data before returns.
 */
void LCD_ready(void)
{
    uint32_t status;
    
    PTD->PDDR = 0x00;          /* PortD input */
    PTA->PCOR = RS;         /* RS = 0 for status */
    PTA->PSOR = RW;         /* R/W = 1, LCD output */
    
    do {    /* stay in the loop until it is not busy */
			  PTA->PCOR = EN;
			  Delay(500);
        PTA->PSOR = EN;     /* raise E */
        Delay(500);
        status = PTD->PDIR; /* read status register */
        PTA->PCOR = EN;
        Delay(500);			/* clear E */
    } while (status & 0x80UL);    /* check busy bit */
    
    PTA->PCOR = RW;         /* R/W = 0, LCD input */
    PTD->PDDR = 0xFF;       /* PortD output */
}

void LCD_command(unsigned char command)
{
    LCD_ready();			/* wait until LCD is ready */
    PTA->PCOR = RS | RW;    /* RS = 0, R/W = 0 */
    PTD->PDOR = command;
    PTA->PSOR = EN;         /* pulse E */
    Delay(500);
    PTA->PCOR = EN;
}

void LCD_data(unsigned char data)
{
    LCD_ready();			/* wait until LCD is ready */
    PTA->PSOR = RS;         /* RS = 1, R/W = 0 */
    PTA->PCOR = RW;
    PTD->PDOR = data;
    PTA->PSOR = EN;         /* pulse E */
    Delay(500);
    PTA->PCOR = EN;
}

/* Delay n milliseconds
 * The CPU core clock is set to MCGFLLCLK at 41.94 MHz in SystemInit().
 */

/* delay n microseconds
 * The CPU core clock is set to MCGFLLCLK at 41.94 MHz in SystemInit().
 */


void Delay(volatile unsigned int time_del) {
  while (time_del--) 
		{
  }
}


void keypad_init(void)
{
    SIM->SCGC5 |= 0x0800;       /* enable clock to Port C */ 
    PORTC->PCR[0] = 0x103;      /* make PTC0 pin as GPIO and enable pullup*/
    PORTC->PCR[1] = 0x103;      /* make PTC1 pin as GPIO and enable pullup*/
    PORTC->PCR[2] = 0x103;      /* make PTC2 pin as GPIO and enable pullup*/
    PORTC->PCR[3] = 0x103;      /* make PTC3 pin as GPIO and enable pullup*/
    PORTC->PCR[4] = 0x103;      /* make PTC4 pin as GPIO and enable pullup*/
    PORTC->PCR[5] = 0x103;      /* make PTC5 pin as GPIO and enable pullup*/
    PORTC->PCR[6] = 0x103;      /* make PTC6 pin as GPIO and enable pullup*/
    PORTC->PCR[7] = 0x103;      /* make PTC7 pin as GPIO and enable pullup*/
    PTC->PDDR = 0x00;         /* make PTC7-0 as input pins */
}




uint32_t keypad_getkey(void){
    uint32_t row, col;
    const char row_select[] = {0x01, 0x02, 0x04, 0x08}; /* one row is active */

    /* check to see any key pressed */
    PTC->PDDR |= 0x0F;          /* rows output */
    PTC->PCOR = 0x0F;               /* ground rows */
    Delay(500);                 /* wait for signal return */
    col =  PTC->PDIR & 0xF0UL;     /* read all columns */
    PTC->PDDR = 0;              /*  rows input */
    if (col == 0xF0UL)
        return 0;               /* no key pressed */

    /* If a key is pressed, it gets here to find out which key.
     * It activates one row at a time and read the input to see
     * which column is active. */
    for (row = 0; row < 4; row++)
    {
        PTC->PDDR = 0;                  /* disable all rows */
        PTC->PDDR |= row_select[row];   /* enable one row */
        PTC->PCOR = row_select[row];    /* drive the active row low */
        Delay(500);                     /* wait for signal to settle */
        col = PTC->PDIR & 0xF0UL;         /* read all columns */
        if (col != 0xF0UL) break;         /* if one of the input is low, some key is pressed. */
    }
    PTC->PDDR = 0;                      /* disable all rows */
    if (row == 4) 
        return 0;                       /* if we get here, no key is pressed */
 
    /* gets here when one of the rows has key pressed, check which column it is */
    if (col == 0xE0UL) return row * 4 + 1;    /* key in column 0 */
    if (col == 0xD0UL) return row * 4 + 2;    /* key in column 1 */
    if (col == 0xB0UL) return row * 4 + 3;    /* key in column 2 */
    if (col == 0x70UL) return row * 4 + 4;    /* key in column 3 */

    return 0;   /* just to be safe */
}