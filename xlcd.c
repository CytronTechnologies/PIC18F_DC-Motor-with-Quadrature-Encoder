
#include <p18f4431.h>
#include "xlcd.h"


/********************************************************************
*       Function Name:  OpenXLCD                                    *
*       Return Value:   void                                        *
*       Parameters:     lcdtype: sets the type of LCD (lines)       *
*       Description:    This routine configures the LCD. Based on   *
*                       the Hitachi HD44780 LCD controller. The     *
*                       routine will configure the I/O pins of the  *
*                       microcontroller, setup the LCD for 8-bit   	*
*                       mode and clear the display. The user must  	*
*                       provide the delay routines mentioned at the *
*                       beginning of "xlcd.h". The code is provided *
*						at the end of this file.					*
********************************************************************/
void OpenXLCD(unsigned char lcdtype)
{
        DATA_PORT = 0;
        TRIS_DATA_PORT = 0xff;
        TRIS_RS = 0;
        TRIS_E = 0;
        RS_PIN = 0;                     // Register select pin made low
        E_PIN = 0;                      // Clock pin made low

        // Delay for 15ms to allow for LCD Power on reset
        Delay_1msX(15);
        
        // Setup interface to LCD
        TRIS_DATA_PORT = 0;             // Data port output
        DATA_PORT = 0b00110000;         // Function set cmd(8-bit interface)
        E_PIN = 1;                      // Clock the cmd in
        Delay_1msX(1);
        E_PIN = 0;
        
        // Delay for at least 4.1ms
        Delay_1msX(5);

        // Setup interface to LCD
        DATA_PORT = 0b00110000;         // Function set cmd(8-bit interface)
	    E_PIN = 1;                      // Clock the cmd in
        Delay_1msX(1);
        E_PIN = 0;

        // Delay for at least 100us
        Delay_1msX(1);

        // Setup interface to LCD
        DATA_PORT = 0b00110000;         // Function set cmd(8-bit interface)
        E_PIN = 1;                      // Clock cmd in
        Delay_1msX(1);
        E_PIN = 0;

        TRIS_DATA_PORT = 0xff;          // Make data port input


		// Set data interface width, # lines, font
        WriteCmdXLCD(lcdtype);          // Function set cmd

        // Turn the display off then on
        WriteCmdXLCD(DOFF&CURSOR_OFF&BLINK_OFF);	// Display OFF/Blink OFF
        WriteCmdXLCD(DON&CURSOR_OFF&BLINK_OFF);		// Display ON/Blink ON

        // Clear display
        WriteCmdXLCD(0x01);             // Clear display

        // Set entry mode inc, no shift
        WriteCmdXLCD(SHIFT_CUR_LEFT);   // Entry Mode

        // Set DD Ram address to 0
        SetDDRamAddr(0);                // Set Display data ram address to 0

        return;
}


/********************************************************************
*       Function Name:  putsXLCD
*       Return Value:   void
*       Parameters:     buffer: pointer to string
*       Description:    This routine writes a string of bytes to the
*                       Hitachi HD44780 LCD controller. The user
*                       must check to see if the LCD controller is
*                       busy before calling this routine. The data
*                       is written to the character generator RAM or
*                       the display data RAM depending on what the
*                       previous SetxxRamAddr routine was called.
********************************************************************/
void putsXLCD(char *buffer)
{
        while(*buffer)                  // Write data to LCD up to null
        {
                WriteDataXLCD(*buffer); // Write character to LCD
                buffer++;               // Increment buffer
        }
        return;
}


/********************************************************************
*       Function Name:  putrsXLCD
*       Return Value:   void
*       Parameters:     buffer: pointer to string
*       Description:    This routine writes a string of bytes to the
*                       Hitachi HD44780 LCD controller. The user
*                       must check to see if the LCD controller is
*                       busy before calling this routine. The data
*                       is written to the character generator RAM or
*                       the display data RAM depending on what the
*                       previous SetxxRamAddr routine was called.
********************************************************************/
void putrsXLCD(const rom char *buffer)
{
        while(*buffer)                  // Write data to LCD up to null
        {
                WriteDataXLCD(*buffer); // Write character to LCD
                buffer++;               // Increment buffer
        }
        return;
}


/********************************************************************
*       Function Name:  SetCGRamAddr                                *
*       Return Value:   void                                        *
*       Parameters:     CGaddr: character generator ram address     *
*       Description:    This routine sets the character generator   *
*                       address of the Hitachi HD44780 LCD          *
*                       controller. The user must check to see if   *
*                       the LCD controller is busy before calling   *
*                       this routine.                               *
********************************************************************/
void SetCGRamAddr(unsigned char CGaddr)
{
        TRIS_DATA_PORT = 0;                     // Make data port ouput
        DATA_PORT = CGaddr | 0b01000000;        // Write cmd and address to port
        RS_PIN = 0;
        Delay_1msX(1);
        E_PIN = 1;                              // Clock cmd and address in
        Delay_1msX(1);
        E_PIN = 0;
        Delay_1msX(1);
        TRIS_DATA_PORT = 0xff;                  // Make data port inputs

        return;
}


/********************************************************************
*       Function Name:  SetDDRamAddr                                *
*       Return Value:   void                                        *
*       Parameters:     CGaddr: display data address                *
*       Description:    This routine sets the display data address  *
*                       of the Hitachi HD44780 LCD controller. The  *
*                       user must check to see if the LCD controller*
*                       is busy before calling this routine.        *
********************************************************************/
void SetDDRamAddr(unsigned char DDaddr)
{
        TRIS_DATA_PORT = 0;                     // Make port output
        DATA_PORT = DDaddr | 0b10000000;        // Write cmd and address to port
        RS_PIN = 0;
        Delay_1msX(1);
        E_PIN = 1;                              // Clock the cmd and address in
        Delay_1msX(1);
        E_PIN = 0;
        Delay_1msX(1);
        TRIS_DATA_PORT = 0xff;                  // Make port input

        return;
}


/********************************************************************
*       Function Name:  WriteCmdXLCD                                *
*       Return Value:   void                                        *
*       Parameters:     cmd: command to send to LCD                 *
*       Description:    This routine writes a command to the Hitachi*
*                       HD44780 LCD controller. The user must check *
*                       to see if the LCD controller is busy before *
*                       calling this routine.                       *
********************************************************************/
void WriteCmdXLCD(unsigned char cmd)
{
        TRIS_DATA_PORT = 0;             // Data port output
        DATA_PORT = cmd;                // Write command to data port
        RS_PIN = 0;                     // for sending a command
        Delay_1msX(1);
        E_PIN = 1;                      // Clock the command in
        Delay_1msX(1);
        E_PIN = 0;
        Delay_1msX(1);
        TRIS_DATA_PORT = 0xff;          // Data port input

        return;
}


/********************************************************************
*       Function Name:  WriteDataXLCD                               *
*       Return Value:   void                                        *
*       Parameters:     data: data byte to be written to LCD        *
*       Description:    This routine writes a data byte to the      *
*                       Hitachi HD44780 LCD controller. The user    *
*                       must check to see if the LCD controller is  *
*                       busy before calling this routine. The data  *
*                       is written to the character generator RAM or*
*                       the display data RAM depending on what the  *
*                       previous SetxxRamAddr routine was called.   *
********************************************************************/
void WriteDataXLCD(char data)
{
        TRIS_DATA_PORT = 0;             // Make port output
        DATA_PORT = data;               // Write data to port
        RS_PIN = 1;                     // Set control bits
        Delay_1msX(1);
        E_PIN = 1;                      // Clock data into LCD
        Delay_1msX(1);
        E_PIN = 0;
        RS_PIN = 0;                     // Reset control bits
        TRIS_DATA_PORT = 0xff;          // Make port input

        return;
}


/********************************************************************
*       Function Name:  ClearXLCD                               	*
*       Return Value:   void                                        *
*       Parameters:     void       	 								*
*       Description:    This routine clears the LCD display.		*
********************************************************************/
void ClearXLCD(void)
{
	WriteCmdXLCD(0x01);
}


/********************************************************************
*       Function Name:  SetCurXLCD                               	*
*       Return Value:   void                                        *
*       Parameters:     data: data byte to be written to LCD        *
*       Description:    This routine set the location of the 		*
*						LCD(2X16) cursor.							*
*						If the given value is (0-15) the cursor 	*
*						will be at the upper line.					*
*						If the given value is (20-35) the cursor 	*
*						will be at the lower line.					*
*		-----------------------------------------------------		*
*		| |00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15| |		*
*		| |20|21|22|23|24|25|26|27|28|29|30|31|32|33|34|35| |		*
*		-----------------------------------------------------		*
********************************************************************/
void SetCurXLCD(unsigned char data)				
{
 	if(data<16)
	{
	 	SetDDRamAddr(0x80+data);
	}
	else
	{
	 	data=data-20;
		SetDDRamAddr(0xc0+data);
	}
}

/********************************************************************
*       Function Name:  putnumXLCD                              	*
*       Return Value:   void                                        *
*       Parameters:     data: numbers for displaying on LCD			*
*						num_of_digit: number digit of the data		*
*						to be written on LCD (max: 7 digits)		*
*       Description:    This routine write the variable or number 	*
*						on LCD.										*
********************************************************************/
void putnumXLCD(unsigned long data,unsigned char num_of_digit)
{
	if(num_of_digit>=7)
	{
		data=data%10000000;
		WriteDataXLCD(data/1000000+0x30);
	}	
	if(num_of_digit>=6)
	{
		data=data%100000;
		WriteDataXLCD(data/10000+0x30);
	}		
	if(num_of_digit>=5)
	{
		data=data%100000;
		WriteDataXLCD(data/10000+0x30);
	}	
	if(num_of_digit>=4)
	{
		data=data%10000;
		WriteDataXLCD(data/1000+0x30);
	}
	if(num_of_digit>=3)
	{
		data=data%1000;
		WriteDataXLCD(data/100+0x30);
	}
	if(num_of_digit>=2)
	{
		data=data%100;
		WriteDataXLCD(data/10+0x30);
	}
	if(num_of_digit>=1)
	{
		data=data%10;
		WriteDataXLCD(data+0x30);
	}
}

