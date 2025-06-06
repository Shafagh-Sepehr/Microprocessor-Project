#include <stdlib.h>
#ifndef EEPROM_H_
#define EEPROM_H_

unsigned char eeprom_read(unsigned int address){
	while(EECR & (1<<EEWE)); //wait for last write to finish
	EEAR = address & 0x03FF; //write address to address register
	EECR |= (1<<EERE); //start EEPROM read by writing EERE
	return EEDR;
}

void eeprom_write(unsigned int address, unsigned char data){
	while(EECR & (1<<EEWE)); //wait for last write to finish
	EEAR = address & 0x03FF; //write 0x5F to address register
	EEDR = data; //write data to data register
	EECR |= (1<<EEMWE); //write one to EEMWE
	EECR |= (1<<EEWE); //start EEPROM write
}

unsigned char cal_checksum(unsigned int n){
	unsigned char checksum = 0;
	for (unsigned int i = 0; i < n; i++)
		checksum += eeprom_read(i);
	return (~checksum) + 1;
}

void eeprom_first_time_boot(){
	eeprom_write(0,'G');
	eeprom_write(1,0x01);
	eeprom_write(2,'0');
	eeprom_write(3,'0');
	eeprom_write(4,'0');
	eeprom_write(5,'0');
	eeprom_write(6, cal_checksum(6));
	//lcd_data(cal_checksum(6));

}

unsigned char eeprom_init(unsigned char** passwords){
	unsigned char first = eeprom_read(0);

	if(first != 'G'){
		
		eeprom_first_time_boot();
		return eeprom_init(passwords);
	}

	unsigned char count = eeprom_read(1);
	
	(*passwords) = (unsigned char*)malloc(sizeof(unsigned char)*count*4);


	unsigned char checksum = eeprom_read((count*4)+2);
	for (unsigned char i = 0; i<count; i++) {
		(*passwords)[i*4]   = eeprom_read((i*4)+2);
		(*passwords)[i*4+1] = eeprom_read((i*4)+3);
		(*passwords)[i*4+2] = eeprom_read((i*4)+4);
		(*passwords)[i*4+3] = eeprom_read((i*4)+5);

			
		checksum += (*passwords)[i*4]  +
					(*passwords)[i*4+1]+
					(*passwords)[i*4+2]+
					(*passwords)[i*4+3];


	}



	checksum+='G';
	checksum+=count;

	if(checksum!=0){
		lcd_data(';');
		_delay_ms(1000);
		eeprom_first_time_boot();
		return eeprom_init(passwords);
	}

	return count;
}

void eeprom_change_password(unsigned char index, unsigned char* new_password){
	unsigned char count = eeprom_read(1);

	for (char i=0; i<4;i++)
		eeprom_write(index*4+2+i,new_password[i]);
	
	clear_and_go_home();
	
	eeprom_write(count*4+2,cal_checksum(count*4+2));
}

void eeprom_add_password(unsigned char* new_password){
	unsigned char count = eeprom_read(1);

	
		
	for (char i=0; i<4;i++)
		eeprom_write(count*4+2+i, new_password[i]);

	
	eeprom_write(1,count+1);
	eeprom_write((count+1)*4+2,cal_checksum((count+1)*4+2));
}



#endif /* EEPROM_H_ */