#include <avr/io.h>
#define F_CPU 1000000
#include <util/delay.h>
#include "lcd.h"

#ifndef KEYPAD_H_
#define KEYPAD_H_

#define KEY_PORT PORTA //keyboard PORT
#define KEY_DDR DDRA //keyboard DDR
#define KEY_PIN PINA //keyboard PIN


unsigned char keypad[4][3] = {
	{'1','2','3'},
	{'4','5','6'},
	{'7','8','9'},
	{'*','0','#'}
};

void keypad_init() {
	KEY_DDR = 0xF0; //makes usart port input again but it's ok
	KEY_PORT |= 0xFE;
	KEY_PORT &= 0x0E; //ground all rows at once
}

char keypad_scan() {
	unsigned char colloc, rowloc;
	
	
	do {
		do {
			
			_delay_ms(20); //call delay
			colloc = (KEY_PIN & 0x0E); //see if any key is pressed
		} while (colloc == 0x0E); //keep checking for key press
		_delay_ms(20); //call delay for debounce
		colloc = (KEY_PIN & 0x0E); //read columns
	} while (colloc == 0x0E); //wait for key press
	
	while (1) {
		KEY_PORT = 0xEE; //ground row 0
		colloc = (KEY_PIN & 0x0E); //read the columns

		if (colloc != 0x0E) //column detected
		{
			rowloc = 0; //save row location
			break; //exit while loop
		}
		KEY_PORT = 0xDE; //ground row 1
		colloc = (KEY_PIN & 0x0E); //read the columns
		if (colloc != 0x0E) //column detected
		{
			rowloc = 1; //save row location
			break; //exit while loop
		}
		KEY_PORT = 0xBE; //ground row 2
		colloc = (KEY_PIN & 0x0E); //read the columns
		if (colloc != 0x0E) //column detected
		{
			rowloc = 2; //save row location
			break; //exit while loop
		}
		KEY_PORT = 0x7E; //ground row 3
		colloc = (KEY_PIN & 0x0E); //read the columns
		rowloc = 3; //save row location
		break; //exit while loop
	}
	//check column and send result to Port D
	//1100 1010 0110
	char temp;
	if (colloc == 0x0C)
	temp = keypad[rowloc][0];
	else if (colloc == 0x0A)
	temp = keypad[rowloc][1];
	else
	temp = keypad[rowloc][2];
	
	KEY_PORT |= 0x0E;
	return temp;
}

#endif /* KEYPAD_H_ */