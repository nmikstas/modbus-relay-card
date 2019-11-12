//Modbus relay card firmware Revision 0.1
//This firmware is designed for an AtMega328 running at 8MHz.
//The fuse bits should be set to 0xFF 0xD5 0xE2.

#ifndef MODBUS_RELAY_BOARD_V_1_H_
#define MODBUS_RELAY_BOARD_V_1_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "Board_IO.h"
#include "Helper.h"
#include "USART_Control.h"
#include "EEPROM.h"
#include "Modbus.h"

//Get status of hand switch and config header and update the device accordingly.
void update_modes();

#endif //MODBUS_RELAY_BOARD_V_1_H_