#ifndef HELPER_H_
#define HELPER_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "Board_IO.h"

void init_micro();									//Initialize the microcontroller.
void update_Mode_LEDs(uint16_t ms_timer);			//Update the hand and config LEDs.
void update_hand_mode_relays();						//Update relay states if in hand mode.
void update_modes();								//Get status of hand switch and config header and update device.
void check_and_set_mode();							//Check mode header and setup USART accordingly.
void terminal_setup();								//Setup serial port for terminal communications.
void modbus_setup();								//Setup serial port for modbus communications.
uint8_t string_from_uint(uint16_t i, char *s);		//Converts an unsigned integer into a null terminated string.
uint8_t convert_1_num(char *p, uint16_t *value);	//Takes one decimal value from an array and converts to a numeric value.
uint8_t convert_2_num(char *p, uint16_t *value);	//Takes two decimal values from an array and converts to a numeric value.
uint8_t convert_3_num(char *p, uint16_t *value);	//Takes three decimal values from an array and converts to a numeric value.
uint8_t check_int_range(const uint8_t value);		//Check if character is a valid decimal value.

#endif /* HELPER_H_ */