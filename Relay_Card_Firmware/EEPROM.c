#include "EEPROM.h"

//This function writes a 16-bit value to the EEPROM.
void EEPROM_write_word(uint16_t *address, uint16_t data)
{
	cli(); //Disable interrupts.
	
	eeprom_write_word(address, data);
	
	sei(); //Enable interrupts.
	
	eeprom_write_word((uint16_t *)EEPROM_PARK, 0);	//Park the pointer.
}

//This function reads a 16-bit value from the EEPROM.
uint16_t EEPROM_read_word(uint16_t *address)
{
	uint16_t read_data;
	
	cli(); //Disable interrupts.
	
	read_data = eeprom_read_word(address);
	
	sei(); //Enable interrupts.
	
	eeprom_read_word((uint16_t *)EEPROM_PARK);	//Park the pointer.
	
	return read_data;
}

//This function reads all set points from the EEPROM.
void EEPROM_read_all()
{
	modbus_address = EEPROM_read_word((uint16_t *)MODBUS_ADDRESS);
	modbus_baud = EEPROM_read_word((uint16_t *)MODBUS_BAUD_RATE);
	modbus_parity = EEPROM_read_word((uint16_t *)MODBUS_PARITY);
	modbus_mode = EEPROM_read_word((uint16_t *)MODBUS_MODE);
}
