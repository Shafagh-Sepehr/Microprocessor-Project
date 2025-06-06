#include <avr/io.h>
#include <stdbool.h>
#define F_CPU 1000000
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include "keypad.h"
#include "eeprom.h"

#define LED_DDR		DDRD
#define LED_PORT	PORTD
#define LED_PIN_NUM	5

#define ADC_DDR DDRA
#define ADC_PORT PORTA
#define ADC_PIN PINA
#define ADC_PIN_NUM 0

#define DAC_DDR DDRB
#define DAC_PORT PORTB
#define DAC_PIN PINB

#define  DAC_lone_DDR DDRD
#define  DAC_lone_PORT PORTD
#define  DAC_lone_PIN_NUM 2


unsigned char* passwords = 0;
bool names_showing = false;
unsigned char tries = 0;
unsigned char password_count = 0;


void password_text(){
	lcd_print("enter password:");
	lcd_command(0x0F);
	lcd_command(0xC0);
	lcd_data('>');
	
}

void interrupt_init(){
	DDRB &= ~(1<<2);
	PORTB |= (1<<2);
	MCUCSR |= (1 << ISC2);
	GICR = (1 << INT2);
	sei();
}

void usart_init_trans (void)
{
	UCSRB = (1<<TXEN);
	UCSRC = (1<< UCSZ1)|(1<<UCSZ0)|(1<<URSEL);
	UBRRL = 0x33;
}

void usart_send (unsigned char ch)
{
	while (! (UCSRA & (1<<UDRE)));
	UDR = ch;
}

bool password_match(unsigned char * passwords,unsigned char* password){
	bool match = false;
	

	for (char i = 0; i < password_count; i++){

		bool equal = true;
		for (char j = 0; j < 4 ; j++)
		if (passwords[i*4+j] != password[j]){
			equal = false;
			break;
		}

		if (equal){
			match = true;
			break;

		}
	}
	return match;
}

void wrong_password(){
	lcd_command(0xC0);
	lcd_print("WRONG PASSWORD ");
	lcd_data(tries+'1');
	if(++tries == 1){
		tries = 0;
		
		
		LED_DDR |= (1<<LED_PIN_NUM);
		
		
		DAC_DDR |= 0b11111011;
		ADCSRA= 0x83;
		ADMUX= 0x60;
		
		DAC_lone_DDR |= (1<<DAC_lone_PIN_NUM);
		
		//data will be left-justified
		for (int i = 0; i<10 ; i++){
			LED_PORT |= (1<<LED_PIN_NUM);
			_delay_ms(100);
			
			ADCSRA|=(1<<ADSC); //start conversion
			while((ADCSRA&(1<<ADIF))==0);//wait for conversion to finish
			char garbage = ADCL;
			char temp = ADCH; // send data to dac
			
			DAC_PORT = temp;
			
			
			
			if((temp & (1<<DAC_lone_PIN_NUM)) != 0)
			DAC_lone_PORT |= (1<<DAC_lone_PIN_NUM);
			else
			DAC_lone_PORT &= ~(1<<DAC_lone_PIN_NUM);
			
			LED_PORT &= ~(1<<LED_PIN_NUM);
			_delay_ms(100);
		}
		DAC_PORT = 0;

	}
	else
	_delay_ms(1000);
	lcd_command(0xC0);
	lcd_print(">               ");
	lcd_command(0xC1);

}
void print_menu(){
	clear_and_go_home();
	lcd_print("1-change 2-show");
	lcd_command(0xC0);
	lcd_print("3-add 4-exit");
}

void main_app(unsigned char * password){
	lcd_command(0x01);
	lcd_command(0x02);
	lcd_command(0x0C);
	lcd_print("Welcome");
	
	for (char i=0;i<4;i++)
	usart_send(password[i]);
	
	//_delay_ms(1200);
	lcd_command(0x01);
	lcd_command(0x02);

	print_menu();
}

bool get_password(char out){
	static unsigned char password[4];
	static unsigned char count = 0;
	
	

	if(count<4 && out != '*' && out !='#'){
		password[count] = out ;
		count++;
		lcd_data(out);
	}
	else if (count>0 && out == '#'){
		lcd_command(0xC0+count);
		lcd_data(' ');
		count--;
		lcd_command(0xC1+count);
	}
	else if(count == 4 && out == '*'){
		bool match = password_match(passwords,password);
		if(match){
			main_app(password);
			count = 0;
			return true;
		}
		
		else{
			wrong_password();
			count = 0;
		}
	}
	
	return false;
}

bool change(char out){
	static unsigned char index = '-';
	static unsigned char password[4];
	static unsigned char count = 0;

	if(index == '-'){
		
		index = out;

		clear_and_go_home();

		lcd_print("enter new one:");
		lcd_command(0xC0);
		lcd_command(0x0F);
		lcd_data('>');
		return true;
	}

	

	if(count<4 && out != '*' && out !='#'){
		password[count++] = out ;
		lcd_data(out);
	}
	else if (count>0 && out == '#'){
		lcd_command(0xC0+count);
		lcd_data(' ');
		count--;
		lcd_command(0xC1+count);
	}
	else if(count == 4 && out == '*'){
		eeprom_change_password(index-'0', password);
		
		index = '-';
		free(passwords);
		password_count = eeprom_init(&passwords);
		print_menu();
		lcd_command(0x0C);//turn off cursor
		count = 0;
		return false;
	}
	return true;
}

void show_passwords(){
	
	
	for (char i = 0; i < password_count; i++)
	{
		lcd_data(passwords[i*4]);
		lcd_data(passwords[i*4+1]);
		lcd_data(passwords[i*4+2]);
		lcd_data(passwords[i*4+3]);
		lcd_data(' ');
		if(i == 2)
		lcd_command(0xC0);
	}
	_delay_ms(2000);
	print_menu();
}

bool add_password(char out){
	static unsigned char password[4];
	static unsigned char count = 0;

	if(count<4 && out != '*' && out !='#'){
		password[count++] = out ;
		lcd_data(out);
	}
	else if (count>0 && out == '#'){
		lcd_command(0xC0+count);
		lcd_data(' ');
		count--;
		lcd_command(0xC1+count);
	}
	else if(count == 4 && out == '*'){
		eeprom_add_password(password);
		
		free(passwords);
		password_count = eeprom_init(&passwords);//refresh password array
		print_menu();
		lcd_command(0x0C);//turn off cursor
		count = 0;
		return false;
	}
	return true;
}

bool get_options(char out){
	

	static bool changing = false;
	static bool adding = false;
	
	
	if(changing){
		changing = change(out);
	}
	else if(adding){
		
		adding = add_password(out);
	}
	else{
		clear_and_go_home();
		switch (out)
		{
			case '1':{
				lcd_print("which one?");
				changing = true;
			}
			break;
			case '2':{
				show_passwords();
			}
			break;
			case '3':{
				if(password_count>=6){
					clear_and_go_home();
					lcd_print("can't add more");
					lcd_command(0xC0);
					lcd_print("passwords");
					_delay_ms(1500);
					print_menu();
					break;
				}
				adding = true;
				clear_and_go_home();
				lcd_print("enter new pass:");
				lcd_command(0xC0);
				lcd_data('>');
				lcd_command(0x0F);
				
			}
			break;
			case '4':{
				return false;
			}
			break;
			case '6':{
				clear_and_go_home();
				lcd_print("reseting eeprom");
				eeprom_first_time_boot();
				password_count = eeprom_init(&passwords);
				_delay_ms(1000);
				print_menu();
			}
			break;
			default:
			print_menu();
			break;
		}
	}
	return true;
}


ISR(INT2_vect){
	
	if(names_showing){
		names_showing=false;
		return;
	}

	char out = keypad_scan();

	static bool entered = false;

	if(!entered){
		
		entered = get_password(out);
	}
	else{
		entered = get_options(out);
		tries = 0;
		if(!entered){
			password_text();
			usart_send('-');
		}
	}
	
	keypad_init();
}

void show_names(){
	names_showing = true;
	
	lcd_command(0x90);
	lcd_print("Shafagh Sepehr   40125203");
	lcd_command(0xD0);
	lcd_print("Kourosh Jamshidi 40120523");
	char c = 40;
	while(c-- && names_showing){
		_delay_ms(150);
		lcd_command(0x18);
	}
	clear_and_go_home();
}



int main(void) {
	
	
	usart_init_trans();
	lcd_init();
	
	
	keypad_init();
	password_count = eeprom_init(&passwords);
	
	
	interrupt_init();
	show_names();
	password_text();
	
	
	while(1){
		
	}


}
