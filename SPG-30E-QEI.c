//=============================================================================
// Filename: SPG-30E-QEI.c
//-----------------------------------------------------------------------------
// Compiled using MPLAB-C18 v3.37 student edition
//=============================================================================
// Company	: Cytron Technologies Sdn Bhd, Malaysia
// Revision	: 1.00
// Date		: 25 Feb 2011
// Hardware	: SK40C (with PIC18F4431, external crystal 20MHz and LCD 16x2), 
//			  SPG-30E-30K DC geared motor with encoder
//			  and L293D motor driver IC
// Tutorial URL: http://tutorial.cytron.com.my/2012/01/17/quadrature-encoder/
//=============================================================================

#include <p18f4431.h>
#include "xlcd.h"
#include "delays.h"

//=============================================================================
//	Configuration Bits
//=============================================================================
#pragma	config OSC = HS				// HS oscillator
#pragma	config FCMEN = OFF			// Fail-Safe Clock Monitor disabled
#pragma	config IESO = OFF			// Oscillator Switchover mode disabled
#pragma	config PWRTEN = OFF			// PWRT disabled
#pragma	config BOREN = OFF			// Brown-out Reset disabled in hardware and software 
#pragma	config WDTEN = OFF			// WDT disabled (control is placed on the SWDTEN bit)
#pragma	config MCLRE = ON			// MCLR pin enabled; RE3 input pin disabled 
#pragma	config LVP = OFF			// Single-Supply ICSP disabled 

//=============================================================================
//	Define Pins
//=============================================================================
#define	led1		LATBbits.LATB6		//active High
#define	led2		LATBbits.LATB7

#define sw1			PORTBbits.RB0
#define sw2			PORTBbits.RB1

#define cw	{LATBbits.LATB2=1; LATBbits.LATB3=0;}				// Motor clockwise turn
#define ccw {LATBbits.LATB2=0; LATBbits.LATB3=1;}				// Motor counter-clockwise turn
#define brake {LATBbits.LATB2=0; LATBbits.LATB3=0; CCPR2L=255;}	// Motor brake

//=============================================================================
//	Function Prototypes
//=============================================================================
void Delay_1msX (unsigned int miliseconds);
void Delay_100msX (unsigned int msec);
void DelayAndPositionDisplay(unsigned char count);
void ISRHigh(void);
void ISRLow(void);

//=============================================================================
//	Global Variables
//=============================================================================
unsigned char PIDEnable=0;
unsigned int t, CurrentPosition, DesirePosition;

//=============================================================================
//	Main Program
//=============================================================================
void main (void)
{		
	// Set I/O input output
	TRISA = 0b11111111;
	TRISB = 0b00000011;	
	TRISC = 0b11111101;					
	TRISD = 0b00000000;			// Configure PORTD I/O direction
	ANSEL0 = 0b11100111;		// RA3 and RA4 set as digital input
	PORTB = 0;					// Clear Port B
	PORTD = 0;					// Clear Port D
	
	// Configure for Quadrature Encoder Interface
	QEICON = 0b00011000;		// QEI enabled in 4x update mode
	POSCNTH=0;					// Clear position count register (high byte)
	POSCNTL=0;					// Clear position count register (low byte)
	
	// Configuration for PWM output (controlling motor speed)
	T2CON  	= 0b00000101;		// Timer 2 on
	CCP2CON = 0b00001100;		// PWM mode
	PR2	  	= 0b11111111;		// PR2 set to 255

	// Configuration for timer interrupt (100Hz, PID control update use)
	T1CON	= 0b00000001;		
	RCONbits.IPEN	= 1;
	INTCONbits.GIE	= 1;
	INTCONbits.PEIE	= 1;
	PIE1bits.TMR1IE = 1;
	IPR1bits.TMR1IP = 1;
	PIR1bits.TMR1IF = 0;
	
	Delay_1msX(1);				// Delay for 1ms
	
	// Configuration for external LCD  	
	OpenXLCD( EIGHT_BIT & LINES_5X7 );
	ClearXLCD();		             	// Clear display
	
	// Program start here
	putrsXLCD("SPG-30E Quad Enc");		// Send string to LCD
	SetCurXLCD(20);						// Cursor go to lower line (refer xlcd.c for detail)
	putrsXLCD("Position:");				// Send string to LCD	

	brake;								// Motor brake

	while(1)
	{
		if(!sw1)						// Test for SW1 pressing
		{
			PIDEnable = 1;				// Enable interrupt for PID control
			while(1)					// Motor running for mode 1
			{
				DesirePosition=120;
				DelayAndPositionDisplay(70);
				DesirePosition=210;
				DelayAndPositionDisplay(70);
				DesirePosition=300;
				DelayAndPositionDisplay(70);
				DesirePosition=390;
				DelayAndPositionDisplay(70);
				DesirePosition=480;
				DelayAndPositionDisplay(70);
				DesirePosition=390;
				DelayAndPositionDisplay(70);
				DesirePosition=300;
				DelayAndPositionDisplay(70);
				DesirePosition=210;
				DelayAndPositionDisplay(70);		
			}
		}
		else if(!sw2)					// Test for SW2 pressing
		{
			PIDEnable = 1;				// Enable interrupt for PID control
			while(1)					// Motor running for mode 2			
			{ 	
				DesirePosition=120;
				DelayAndPositionDisplay(250);	
				DesirePosition=1200;	
				DelayAndPositionDisplay(250);		
			}	
		}		
		DelayAndPositionDisplay(1);	
	}
}//End of main

//=============================================================================
//	Subroutines
//=============================================================================
/********************************************************************
*       Function Name:  DelayAndPositionDisplay                    	*
*       Return Value:   void                                        *
*       Parameters:     count: amount of delay	(max= 255)			*
*       Description:    This routine generates various delays 		*
*						while displaying current position 			*
*						on LCD.										*
********************************************************************/
void DelayAndPositionDisplay(unsigned char count)
{
	unsigned char x, y;
	for(x=0; x<count; x++)
	{
		SetCurXLCD(30);					// Set cursor to location 30 (refer xlcd.c for detail)
		putnumXLCD(CurrentPosition,5);	// Displaying current position on LCD	
		for(y=0; y<50; y++);			// Time delay
	}
}//End of DelayAndPositionDisplay

/********************************************************************
*       Function Name:  Delay_1msX                               	*
*       Return Value:   void                                        *
*       Parameters:     miliseconds: amount of delay				*
*       Description:    This routine generates various delays 		*
*						needed by xlcd functions.					*
*						For delay of 1ms (18F4431 Fosc at 20MHz)	*
*						Cycles = (TimeDelay * Fosc) / 4				*
*						Cycles = (1ms * 20MHz) / 4					*
*						Cycles = 5,000								*
*						Since call of function also takes some 		*
*						instruction cycles, the exact value to get	*
*						1ms delay is less than 5,000.				*
********************************************************************/
void Delay_1msX (unsigned int miliseconds)
{
	t=0;
	while(t<miliseconds)
	{
		Delay1KTCYx(4);
		Delay10TCYx(96);
		Nop();
		Nop();
		Nop();
		Nop();
		Nop();
		t++;
	}
}//End of Delay_1msX


/********************************************************************
*       Function Name:  Delay_100msX                               	*
*       Return Value:   void                                        *
*       Parameters:     miliseconds: amount of delay				*
*       Description:    This routine generates various delays 		*
*						needed by xlcd functions.					*
*						For delay of 100ms							*
*						(18F4431 Fosc at 20MHz)						*
*						Cycles = (TimeDelay * Fosc) / 4				*
*						Cycles = (100ms * 20MHz) / 4				*
*						Cycles = 500,000							*
*						Since call of function also takes some 		*
*						instruction cycles, the exact value to get	*
*						1ms delay is less than 500,000.				*
********************************************************************/
void Delay_100msX (unsigned int msec)
{
	t=0;
	while(t<msec)
	{
		Delay10KTCYx(49);
		Delay1KTCYx(9);
		Delay10TCYx(96);
		t++;
	}
}//End of Delay_100msX

//=====================================================================================
//	Interrupt vector
//=====================================================================================
#pragma	code InterruptVectorHigh = 0x08
void InterruptVectorHigh(void)
{
 	_asm
		goto ISRHigh		// jump to interrupt service routine
	_endasm
}
#pragma code
#pragma	code InterruptVectorLow = 0x18
void InterruptVectorLow(void)
{
 	_asm
		goto ISRLow			// jump to interrupt service routine
	_endasm
}
#pragma code

//=====================================================================================
//	Interupt Service Routine
//	this a function reserved for interrupt service routine
//	User may need it in advance development of the program
//=====================================================================================
#pragma interrupt ISRHigh
void ISRHigh(void)
{
	signed int Output, Error0, ErrorDifferent;
	static signed int Error1, Error2, Error3, Sum_E;	
	
	if(PIR1bits.TMR1IF)
	{
		TMR1H = 0x3C;				// Set timer 1 value for 100Hz (10ms) interrupt service routine
		TMR1L = 0xAF;
		PIR1bits.TMR1IF = 0;		// Clear interrupt flag
		
		CurrentPosition = POSCNTH;						// Reading  current position (high byte)
		CurrentPosition = (CurrentPosition<<8)|POSCNTL;	// Reading  current position (low byte), 16 bits
		
		Error0 = DesirePosition - CurrentPosition;		// Counting current error
		
		if(PIDEnable)				// Test for PID Enable
		{
			if(Error0>150)			// Motor run full speed if current position is too far from desire position
			{
				ccw;				// Counter-clockwise turn for positive error	
				CCPR2L=255;			// Full speed
				Sum_E=0;			// Clear summing error
			}
			else if(Error0<-150)	// Motor run full speed if current position is too far from desire position
			{
				cw;					// Clockwise turn for negative error
				CCPR2L=255;			// Full speed
				Sum_E=0;			// Clear summing error
			}
			else 					// Motor PID control when nearly to desire position
			{			
				// Integral term
				Sum_E+=Error0;							// Summing error (Integral term)
				if(Sum_E>240) Sum_E=240;				// Limit summing error
				else if(Sum_E<-240) Sum_E=-240;			// Limit summing error
				if((Error0>-2)&&(Error0<2))Sum_E=0;		// Clear summing error to reduce the oscillation
				
				// Derivative term
				ErrorDifferent = Error0 - Error3;					// Error different between current error and last third error 
				
				// PID output
				Output = (Error0*4) + (Sum_E) + (ErrorDifferent*22);// Output = Proportional term + Integral term + Derivative term
				
				// Motor PID control
				if(Output>1)
				{
					ccw;								// Counter-clockwise turn for positive error
					if(Output>255) Output=255;			// Limit maximum output speed
					else if(Output<140) Output=140;		// Mininum output for motor dead zone
					CCPR2L=Output;						// Motor speed proportional to PID output
				}
				else if(Output<-1)
				{
					cw;									// Clockwise turn for negative error
					Output=(-Output);					// Modulus for negative output
					if(Output>255) Output=255;			// Limit maximum output speed
					else if(Output<140) Output=140;		// Mininum output for motor dead zone
					CCPR2L=Output;						// Motor speed proportional to PID output
				}
				else brake;								// Brake the motor if desire position reached
				
				// Previous errors saving for next Derivative term counting use 
				Error3 = Error2;
				Error2 = Error1;
				Error1 = Error0;	
			}	
		}				
	}
}//End of ISRHigh

#pragma interrupt ISRLow
void ISRLow(void)
{
	
}//End of ISRLow
