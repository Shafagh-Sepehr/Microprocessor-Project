#include <avr/io.h>
#define F_CPU 1000000
#include <util/delay.h>
#include "lcd.h"


void usart_init_recv()
{
	UCSRB = (1<<RXEN);
	UCSRC = (1<< UCSZ1)|(1<<UCSZ0)|(1<<URSEL);
	UBRRL = 0x33;
}

char get_usart_data(){
	while (! (UCSRA & (1<<RXC)));
	return UDR;
}

int main(void)
{
	usart_init_recv();
	lcd_init();
	
	DDRC |= (1<<3);
	while(1){
		PORTC &= ~(1<<3);
		
		unsigned char password[5];
		for (char i=0;i<4;i++)
		password[i] = get_usart_data();
		password[4] = '\0';
		
		lcd_print("entered password");
		lcd_command(0xC0);
		lcd_print("is ");
		lcd_print((char *)password);
		lcd_print(".");
		
		
		PORTC |= (1<<3);
		
		_delay_ms(3000);
		lcd_command(0x01);
		lcd_command(0x02);
		
		get_usart_data();
	}
	
	
	while(1);
	return 0;
}

