#ifndef EEPROM_H_
#define EEPROM_H_

#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include "Board_IO.h"

//EEPROM Addresses for various settings.
#define MODBUS_ADDRESS 0
#define MODBUS_BAUD_RATE 2
#define MODBUS_PARITY 4
#define MODBUS_MODE 6

#define EEPROM_PARK 999	//Park address for EEPROM read and writes.

void EEPROM_write_word(uint16_t *address, uint16_t data);		//This function writes a 16-bit value to the EEPROM.
uint16_t EEPROM_read_word(uint16_t *address);					//This function reads a 16-bit value from the EEPROM.
void EEPROM_read_all();											//This function reads all set points from the EEPROM.

#endif /* EEPROM_H_ */