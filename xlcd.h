#ifndef __XLCD_H
#define __XLCD_H

/* PIC18 XLCD peripheral routines.
 *
 *   Notes:
 *		- This file is modified from Microchip C18 "xlcd.h"
 *		  to adapt to Cytron SK40C development board.      
 *		- These libraries routines are written to support the
 *        Hitachi HD44780 LCD controller.
 *      - The user must define the following items:
 *          - The LCD interface type (8-bits)
 *          - The data port
 *              - The tris register for data port
 *              - The control signal ports and pins
 *              - The control signal port tris and pins
 *          - The user must provide this delay routine in program:
 *				- Delay_1msX(unsigned int t) provides 
 *				  t miliseconds delay.
 *				- User may copy the routine from "xlcd.c".
 */


/* DATA_PORT defines the port to which the LCD data lines are connected */
#define DATA_PORT      	PORTD
#define TRIS_DATA_PORT 	TRISD

/* CTRL_PORT defines the port where the control lines are connected.
 * These are just samples, change to match your application.
 */
#define RW_PIN   		   					/* PORT for RW; Connected to ground in SK40C */ 
#define TRIS_RW  		    				/* TRIS for RW; Connected to ground in SK40C */ 
#define RS_PIN   		LATBbits.LATB4   	/* PORT for RS */ 
#define TRIS_RS  		TRISBbits.TRISB4   	/* TRIS for RS */ 
#define E_PIN    		LATBbits.LATB5   	/* PORT for D  */ 
#define TRIS_E   		TRISBbits.TRISB5   	/* TRIS for E  */

/* Display ON/OFF Control defines */
#define DON        			0b00001111	/* Display on      */
#define DOFF       			0b00001011	/* Display off     */
#define CURSOR_ON  			0b00001111	/* Cursor on       */
#define CURSOR_OFF 			0b00001101	/* Cursor off      */
#define BLINK_ON   			0b00001111	/* Cursor Blink    */
#define BLINK_OFF  			0b00001110	/* Cursor No Blink */

/* Cursor or Display Shift defines */
#define SHIFT_CUR_LEFT    	0b00010011 	/* Cursor shifts to the left   */
#define SHIFT_CUR_RIGHT   	0b00010111 	/* Cursor shifts to the right  */
#define SHIFT_DISP_LEFT   	0b00011011 	/* Display shifts to the left  */
#define SHIFT_DISP_RIGHT  	0b00011111 	/* Display shifts to the right */
#define INC_CUR				0b00000110	/* Increment cursor after each byte written */

/* Function Set defines */
#define EIGHT_BIT  			0b00111111	/* 8-bit Interface               */
#define LINE_5X7   			0b00110011	/* 5x7 characters, single line   */
#define LINE_5X10  			0b00110111	/* 5x10 characters               */
#define LINES_5X7  			0b00111011	/* 5x7 characters, multiple line */

#define PARAM_SCLASS 		auto
#define MEM_MODEL 			far  		/* Change this to near for small memory model */


/* OpenXLCD
 * Configures I/O pins for external LCD
 */
void OpenXLCD(PARAM_SCLASS unsigned char);

/* SetCGRamAddr
 * Sets the character generator address
 */
void SetCGRamAddr(PARAM_SCLASS unsigned char);

/* SetDDRamAddr
 * Sets the display data address
 */
void SetDDRamAddr(PARAM_SCLASS unsigned char);

/* WriteCmdXLCD
 * Writes a command to the LCD
 */
void WriteCmdXLCD(PARAM_SCLASS unsigned char);

/* WriteDataXLCD
 * Writes a data byte to the LCD
 */
void WriteDataXLCD(PARAM_SCLASS char);

/* putcXLCD
 * A putc is a write
 */
#define putcXLCD WriteDataXLCD

/* putsXLCD
 * Writes a string of characters to the LCD
 */
void putsXLCD(PARAM_SCLASS char *);

/* putrsXLCD
 * Writes a string of characters in ROM to the LCD
 */
void putrsXLCD(const rom char *);

/* Delay_1msX
 * User defines these routines according to the oscillator frequency
 */
extern void Delay_1msX(unsigned int);

// Clear the display
void ClearXLCD(void);

// Set the location of the lcd cursor
void SetCurXLCD(unsigned char);

// Write numbers 
void putnumXLCD(unsigned long data,unsigned char num_of_digit);

#endif
