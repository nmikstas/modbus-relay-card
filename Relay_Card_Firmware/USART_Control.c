#include "USART_Control.h"

//Enable RX, disable TX.
void enable_rx()
{
	prtEN &= ~(1 << EN);
	UCSR0B |= (1 << RXEN0);
	UCSR0B &= ~(1 << TXEN0);
}

//Enable TX, disable RX.
void enable_tx()
{
	prtEN |= (1 << EN);
	UCSR0B &= ~(1 << RXEN0);
	UCSR0B |= (1 << TXEN0);
}

void update_RXTX_LEDs()
{
	//Update RX LED.
	if(rx_timer)
	{
		prtRXLED |= (1 << RXLED);	//Turn on RX LED.
		rx_timer--;
	}
	else
	{
		prtRXLED &= ~(1 << RXLED);	//Turn off RX LED.
	}
	
	//Update TX LED.
	if(tx_timer)
	{
		prtTXLED |= (1 << TXLED);	//Turn on TX LED.
		tx_timer--;
	}
	else
	{
		prtTXLED &= ~(1 << TXLED);	//Turn off TX LED.
	}
}

//This function transmits a single byte to the USART.
void TX_byte(uint8_t data)
{
	enable_tx();					//Enable TX line, disable RX line.
	tx_timer = 20;					//Turn on TX LED for 20 ms.
	UDR0 = data;					//Load data to transmit.
	while(!(UCSR0A & (1 << TXC0)));	//Wait for TX to finish.
	UCSR0A |= (1 << RXC0);			//Flush RX buffer.
	enable_rx();					//Enable RX line, disable TX line.
}

//This function transmits a string of data out the UART.
void TX_string(char *data)
{
	uint8_t tx_string_index = 0;
	
	while(data[tx_string_index])
	{
		TX_byte(data[tx_string_index++]);	//Transmit byte.
	}
}

//Transmit modbus frame back to sender.
void tx_modbus_frame()
{
	uint16_t tx_string_index = 0;
	
	while(tx_string_index < mb_tx_struct.mb_tx_index)
	{
		prtEN |= (1 << EN);
		UCSR0B &= ~(1 << RXEN0);							//Enable TX line, disable RX line.
		UCSR0B |= (1 << TXEN0);
		
		tx_timer = 20;										//Turn on TX LED for 20 ms.
		UDR0 = mb_tx_struct.mb_tx_frame[tx_string_index++];	//Load data to transmit.
		while(!(UCSR0A & (1 << TXC0)));						//Wait for TX to finish.
		UCSR0A |= (1 << RXC0);								//Flush RX buffer.	
		
		prtEN &= ~(1 << EN);
		UCSR0B |= (1 << RXEN0);								//Enable RX line, disable TX line.
		UCSR0B &= ~(1 << TXEN0);	
	}
	
	mb_tx_struct.mb_tx_index = 0;							//Reset tx frame index.
}

//Write "\r" in the terminal window.
void USART_carriage_return()
{
	TX_byte('\r');
}

//Print "OK" in terminal window.
void USART_OK()
{
	TX_string("OK");
}

//Print "ERR" in terminal window.
void USART_ERR()
{
	TX_string("ERR");
}

//Write the configuration menu in the terminal window.
void USART_write_menu()
{
	uint8_t relay_status;
	uint8_t error_code = ERROR;
	char num_string[4];
	
	TX_string("MODBUS CONFIGURATION MENU");
	USART_carriage_return();
	USART_carriage_return();
	
	TX_string("1. MODBUS ADDRESS: ");
	
	error_code = string_from_uint((uint16_t)modbus_address, num_string);
	
	if(error_code == NO_ERROR)
		TX_string(num_string);
	else
		USART_ERR();	
	
	USART_carriage_return();
	TX_string("    1-247");
	USART_carriage_return();
	
	TX_string("2. BAUD RATE: ");
	
	if(modbus_baud == BAUD_9600)
		TX_string("9600");
	else if(modbus_baud == BAUD_19200)
		TX_string("19200");
	else if(modbus_baud == BAUD_38400)
		TX_string("38400");
	else
		USART_ERR();
	
	USART_carriage_return();
	TX_string("    1:9600  2:19200  3:38400");
	USART_carriage_return();
	
	TX_string("3. PARITY: ");
	
	if(modbus_parity == PARITY_EVEN)
		TX_string("EVEN");
	else if(modbus_parity == PARITY_ODD)
		TX_string("ODD");
	else if(modbus_parity == PARITY_NONE)
		TX_string("NONE");
	else
		USART_ERR();
	
	USART_carriage_return();
	TX_string("    1:EVEN  2:ODD  3:NONE");
	USART_carriage_return();
	
	TX_string("4. TRANSMISSION MODE: ");
	
	if(modbus_mode == TRANS_MODE_RTU)
		TX_string("RTU");
	else if(modbus_mode == TRANS_MODE_ASCII)
		TX_string("ASCII");
	else
		USART_ERR();
	
	USART_carriage_return();
	TX_string("    1:RTU  2:ASCII");
	USART_carriage_return();
	USART_carriage_return();
	
	TX_string("CURRENT RELAY STATUS");
	USART_carriage_return();
	
	//Get current status of the relays.
	relay_status = PINC;
	
	TX_string("R1:");
	if(relay_status & (1 << RLY1))
		TX_string("ON ");
	else
		TX_string("OFF ");
		
	TX_string("R2:");
	if(relay_status & (1 << RLY2))
		TX_string("ON ");
	else
		TX_string("OFF ");
		
	TX_string("R3:");
	if(relay_status & (1 << RLY3))
		TX_string("ON ");
	else
		TX_string("OFF ");
	
	TX_string("R4:");
	if(relay_status & (1 << RLY4))
		TX_string("ON ");
	else
		TX_string("OFF ");
		
	USART_carriage_return();
	USART_carriage_return();
	
	//Print firmware version.
	TX_string("FIRMWARE VERSION: ");
	TX_byte(0x30 + FIRMWARE_MAJOR);
	TX_byte('.');
	TX_byte(0x30 + FIRMWARE_MINOR);
	
	//Clear out screen.
	USART_carriage_return();
	USART_carriage_return();
	USART_carriage_return();
	USART_carriage_return();
	USART_carriage_return();
	USART_carriage_return();
	USART_carriage_return();
	USART_carriage_return();
	USART_carriage_return();
}

//This function tokenizes the user input.
void terminal_tok(uint8_t rx_byte)
{
	static uint8_t rx_array[MAX_TERM_LEN];
	static uint8_t array_index;
	uint8_t errno;
		
	//Get byte from USART.
	rx_array[array_index++] = rx_byte;
		
	//Check if array is going to overflow.
	if((array_index >= MAX_TERM_LEN) && (rx_array[MAX_TERM_LEN - 1] != 0x0D))
	{
		array_index = 0;
		USART_carriage_return();
		USART_ERR();
		USART_carriage_return();
		return;
	}
		
	//Special case. empty array prints the menu.
	if(rx_array[0] == 0x0D)
	{
		array_index = 0;
		USART_write_menu();
		return;
	}
		
	//If enter not pressed, keep building array.
	if(rx_array[array_index - 1] != 0x0D)
	{
		return;
	}
		
	//At this point, enter has been pressed and array is ready for processing.
	
	//Convert array hex values to uppercase.
	for(int i = 0; i < array_index; i++)
	{
		if(rx_array[i] >= 0x61 && rx_array[i] < 0x67)
		{
			rx_array[i] -= 0x20;
		}
	}
		
	//Reset array index for next input string.
	array_index = 0;
		
	//Do a quick error check on first character in array.
	if(rx_array[0] < 0x30) //< 0.
	{
		USART_ERR();
		USART_carriage_return();
		return;
	}
		
	if(rx_array[0] > 0x46) //< F.
	{
		USART_ERR();
		USART_carriage_return();
		return;
	}
		
	if(rx_array[0] > 0x39 && rx_array[0] < 0x41) //> 9, < A.
	{
		USART_ERR();
		USART_carriage_return();
		return;
	}
		
	//Get number selection from user input and call appropriate function.
	if(rx_array[0] == '1' && rx_array[1] == ' ')
	{
		errno = selection1(&rx_array[2]);
	}
		
	else if(rx_array[0] == '2' && rx_array[1] == ' ')
	{
		errno = selection2(&rx_array[2]);
	}
		
	else if(rx_array[0] == '3' && rx_array[1] == ' ')
	{
		errno = selection3(&rx_array[2]);
	}
		
	else if(rx_array[0] == '4' && rx_array[1] == ' ')
	{
		errno = selection4(&rx_array[2]);
	}		
		
	else //Invalid selection.
	{
		errno = ERROR;
	}
		
	if(errno)
	{
		USART_ERR();
		USART_carriage_return();
		return;
	}
		
	USART_OK();
	USART_carriage_return();
	return;
}

//Menu selection 1. Modbus address.
uint8_t selection1(uint8_t *rx_array)
{
	uint8_t error = ERROR;
	uint16_t converted_value;
	
	//Do conversion.
	if(rx_array[1] == 0x0D)
	{
		error = convert_1_num((char*)rx_array, &converted_value);
	}
	else if(rx_array[2] == 0x0D)
	{
		error = convert_2_num((char*)rx_array, &converted_value);
	}
	else if(rx_array[3] == 0x0D)
	{
		error = convert_3_num((char*)rx_array, &converted_value);
	}
	
	//Check to see if conversion was successful.
	if(error == ERROR)
		return ERROR;
	
	//Check range and save.
	if(converted_value > 0 && converted_value < 248)
	{
		EEPROM_write_word((uint16_t*)MODBUS_ADDRESS, converted_value);
		modbus_address = converted_value;
		return NO_ERROR;
	}
	
	return ERROR;
}

//Menu selection 2. Modbus baud rate.
uint8_t selection2(uint8_t *rx_array)
{
	if(rx_array[0] == '1' && rx_array[1] == 0x0D)
	{
		EEPROM_write_word((uint16_t*)MODBUS_BAUD_RATE, BAUD_9600);
		modbus_baud = BAUD_9600;
		return NO_ERROR;
	}
	else if(rx_array[0] == '2' && rx_array[1] == 0x0D)
	{
		EEPROM_write_word((uint16_t*)MODBUS_BAUD_RATE, BAUD_19200);
		modbus_baud = BAUD_19200;
		return NO_ERROR;
	}
	else if(rx_array[0] == '3' && rx_array[1] == 0x0D)
	{
		EEPROM_write_word((uint16_t*)MODBUS_BAUD_RATE, BAUD_38400);
		modbus_baud = BAUD_38400;
		return NO_ERROR;
	}
	
	return ERROR;
}

//Menu selection 3.  Modbus parity.
uint8_t selection3(uint8_t *rx_array)
{
	if(rx_array[0] == '1' && rx_array[1] == 0x0D)
	{
		EEPROM_write_word((uint16_t*)MODBUS_PARITY, PARITY_EVEN);
		modbus_parity = PARITY_EVEN;
		return NO_ERROR;
	}
	
	if(rx_array[0] == '2' && rx_array[1] == 0x0D)
	{
		EEPROM_write_word((uint16_t*)MODBUS_PARITY, PARITY_ODD);
		modbus_parity = PARITY_ODD;
		return NO_ERROR;
	}
	
	if(rx_array[0] == '3' && rx_array[1] == 0x0D)
	{
		EEPROM_write_word((uint16_t*)MODBUS_PARITY, PARITY_NONE);
		modbus_parity = PARITY_NONE;
		return NO_ERROR;
	}
	
	return ERROR;
}

//Menu selection 4. Modbus transmission mode.
uint8_t selection4(uint8_t *rx_array)
{
	if(rx_array[0] == '1' && rx_array[1] == 0x0D)
	{
		EEPROM_write_word((uint16_t*)MODBUS_MODE, TRANS_MODE_RTU);
		modbus_mode = TRANS_MODE_RTU;
		return NO_ERROR;
	}
	
	if(rx_array[0] == '2' && rx_array[1] == 0x0D)
	{
		EEPROM_write_word((uint16_t*)MODBUS_MODE, TRANS_MODE_ASCII);
		modbus_mode = TRANS_MODE_ASCII;
		return NO_ERROR;
	}
	
	return ERROR;
}