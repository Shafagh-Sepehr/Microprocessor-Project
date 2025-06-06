#include <avr/io.h>
#define F_CPU 1000000
#include <util/delay.h>

#ifndef LCD_H_
#define LCD_H_

#define LCD_DPRT PORTA //LCD DATA PORT
#define LCD_DDDR DDRA //LCD DATA DDR
#define LCD_DPIN PINA //LCD DATA PIN
#define LCD_CPRT PORTC //LCD COMMANDS PORT
#define LCD_CDDR DDRC //LCD COMMANDS DDR
#define LCD_CPIN PINC //LCD COMMANDS PIN
#define LCD_EN 0 //LCD EN
#define LCD_RW 1 //LCD RW
#define LCD_RS 2 //LCD RS

void lcd_command( unsigned char cmnd )
{
	LCD_DPRT = cmnd; //send cmnd to data port
	LCD_CPRT &= ~ (1<<LCD_RS); //RS = 0 for command
	LCD_CPRT &= ~ (1<<LCD_RW); //RW = 0 for write
	LCD_CPRT |= (1<<LCD_EN); //EN = 1 for H-to-L pulse
	_delay_us(2000); //wait to make enable wide
	LCD_CPRT &= ~ (1<<LCD_EN); //EN = 0 for H-to-L pulse
	_delay_us(2000); //wait to make enable wide
}

void lcd_data( unsigned char data )
{
	LCD_DPRT = data; //send data to data port
	LCD_CPRT |= (1<<LCD_RS); //RS = 1 for data
	LCD_CPRT &= ~ (1<<LCD_RW); //RW = 0 for write
	LCD_CPRT |= (1<<LCD_EN); //EN = 1 for H-to-L pulse
	_delay_us(2000); //wait to make enable wide
	LCD_CPRT &= ~ (1<<LCD_EN); //EN = 0 for H-to-L pulse
	_delay_us(2000); //wait to make enable wide
}

void lcd_init()
{
	LCD_DDDR = 0xFF;
	LCD_CDDR = 0xFF;
	LCD_CPRT &=~(1<<LCD_EN); //LCD_EN = 0
	_delay_us(2000); //wait for init
	lcd_command(0x38); //init. LCD 2 line, 5 × 7 matrix
	lcd_command(0x0C); //display on, cursor off
	lcd_command(0x01); //clear LCD
	_delay_us(2000); //wait
	lcd_command(0x06); //shift cursor right
}
void lcd_print( char * str )
{
	unsigned char i = 0;
	while(str[i]!=0)
	{
		lcd_data(str[i]);
		i++ ;
	}
}



#endif