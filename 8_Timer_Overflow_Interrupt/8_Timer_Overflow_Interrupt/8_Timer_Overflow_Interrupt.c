/********************************************************************************
Written by: Vinod Desai,Sachitanand Malewar NEX Robotics Pvt. Ltd.
Edited by: e-Yantra team
AVR Studio Version 6

Date: 19th October 2012


 This experiment demonstrates use of timer overflow interrupt

 Concepts covered:  Use of timer overflow interrupt to do tasks in a periodic way

 In this example timer 0's overflow interrupt is used to turn on and off buzzer with the time period of 1 second
 
 Connections: Buzzer is connected to the PORTC 3 pin.

 Note: 
 
 1. Make sure that in the configuration options following settings are 
 	done for proper operation of the code

 	Microcontroller: atmega2560
 	Frequency: 14745600
 	Optimization: -O0 (For more information read section: Selecting proper optimization 
 					options below figure 2.22 in the Software Manual)


 2. TIMSK4 = 0x01; //timer4 overflow interrupt enable must be done in init devices. It should not be done in timer4_init()

*********************************************************************************/

/********************************************************************************

   Copyright (c) 2010, NEX Robotics Pvt. Ltd.                       -*- c -*-
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.

   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

   * Source code can be used for academic purpose. 
	 For commercial use permission form the author needs to be taken.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. 

  Software released under Creative Commence cc by-nc-sa licence.
  For legal information refer to: 
  http://creativecommons.org/licenses/by-nc-sa/3.0/legalcode

********************************************************************************/
#define F_CPU 14745600
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd.h"
int tot_overflow = 0; //used as a variable for buzzer state toggel action

//Function to initialize Buzzer 
void buzzer_pin_config (void)
{
 DDRC = DDRC | 0x08;		//Setting PORTC 3 as outpt
 PORTC = PORTC & 0xF7;		//Setting PORTC 3 logic low to turnoff buzzer
}

void lcd_port_config (void)
{
	DDRC = DDRC | 0xF7; //all the LCD pin's direction set as output
	PORTC = PORTC & 0x80; // all the LCD pins are set to logic 0 except PORTC 7
}

void port_init (void)
{
 buzzer_pin_config();
 lcd_port_config();
}
//TIMER4 initialize - prescale:1024
// WGM: 0) Normal, TOP=0xFFFF
// desired value: 1Hz
// actual value:  1.000Hz (0.0%)
void timer4_init(void)
{
 TCCR4B = 0x00; //stop
 TCNT4H = 0xFF; //Counter higher 8 bit value
 TCNT4L = 0xFF; //Counter lower 8 bit value
 OCR4AH = 0x00; //Output Compair Register (OCR)- Not used
 OCR4AL = 0x00; //Output Compair Register (OCR)- Not used
 OCR4BH = 0x00; //Output Compair Register (OCR)- Not used
 OCR4BL = 0x00; //Output Compair Register (OCR)- Not used
 OCR4CH = 0x00; //Output Compair Register (OCR)- Not used
 OCR4CL = 0x00; //Output Compair Register (OCR)- Not used
 ICR4H  = 0x00; //Input Capture Register (ICR)- Not used
 ICR4L  = 0x00; //Input Capture Register (ICR)- Not used
 TCCR4A = 0x00; 
 
 TCCR4C = 0x00;
 TCCR4B = 0x04; //start Timer
}


//to stop timer
void stop_timer()
{
	
	TCCR4A = 0x00;
}
//This ISR can be used to schedule events like refreshing ADC data, LCD data
ISR(TIMER4_OVF_vect)
{
	// keep a track of number of overflows
	tot_overflow++;
	//TIMER4 has overflowed
	TCNT4H = 0xFF; //reload counter high value
	TCNT4L = 0xFF;//reload counter low value
}

/*int milllis()
{
	while(TCNT4 >= 57)
	{
		count++;
		TCNT4 = 0;
	}
return(count);			
} */

int millis(void)
{
	int time=0;
	//time=1000*(tot_overflow + (TCNT4-7936.0)/57600);
	lcd_print(2,3,tot_overflow,2);
	time=1000*((tot_overflow*65536)/57598 + (TCNT4-7936.0)/57598);
	TCCR4B=0x00;
	return time;
	
}
void init_devices(void)
{
 cli(); //Clears the global interrupts
 port_init(); //Initializes all ports 
 timer4_init();
// TIMSK4 = 0x01; //timer4 overflow interrupt enable
 sei();   //Enables the global interrupts
}

//Main Function
int main(void)
{   int t;
	init_devices();
	lcd_set_4bit();                // set the LCD in 4 bit mode
	lcd_init();
	timer4_init();
	_delay_ms(60);
	t=millis();
	lcd_print(1,3,t,4);
	//stop_timer();
}


