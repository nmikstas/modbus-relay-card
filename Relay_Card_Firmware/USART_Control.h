#ifndef USART_CONTROL_H_
#define USART_CONTROL_H_

#include <avr/io.h>
#include "Board_IO.h"
#include "Helper.h"
#include "EEPROM.h"
#include "Modbus.h"

//Timers used to strobe RX and TX LEDS.
uint8_t rx_timer, tx_timer;

//RX array data.
#define MAX_TERM_LEN 7					//Max length of terminal RX array.
uint8_t rx_term_string[MAX_TERM_LEN];	//Array for storing terminal RX string.
uint8_t rx_term_string_index;			//Index into terminal RX array above.

void enable_rx();						//Enable the RS485 RX line, disable TX.
void enable_tx();						//Enable the RS485 TX line, disable RX.
void TX_byte(uint8_t data);				//Transmit a single byte of data on the RS485 line.
void TX_string(char *data);				//This function transmits a string of data out the UART.
void tx_modbus_frame();					//Transmit modbus frame back to sender.
void update_RXTX_LEDs();				//Update RX/TX LEDs.
void terminal_tok(uint8_t rx_byte);		//This function tokenizes the user input.
void USART_carriage_return();			//Write "\r" in the terminal window.
void USART_ERR();						//Print "ERR" in terminal window.
void USART_OK();						//Print "OK" in terminal window.
void USART_write_menu();				//Write the configuration menu in the terminal window.
uint8_t selection1(uint8_t *rx_array);	//Menu selection 1.
uint8_t selection2(uint8_t *rx_array);	//Menu selection 2.
uint8_t selection3(uint8_t *rx_array);	//Menu selection 3.
uint8_t selection4(uint8_t *rx_array);	//Menu selection 4.

#endif /* USART_CONTROL_H_ */