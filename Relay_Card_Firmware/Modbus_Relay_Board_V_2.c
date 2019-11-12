//Modbus relay card firmware Revision 0.1
//This firmware is designed for an AtMega328 running at 8MHz.
//The fuse bits should be set to 0xFF 0xD5 0xE2.

#include "Modbus_Relay_Board_V_2.h"

int main(void)
{	
	/*
	EEPROM_write_word((uint16_t *)MODBUS_ADDRESS, 1);
	EEPROM_write_word((uint16_t *)MODBUS_BAUD_RATE, 1);
	EEPROM_write_word((uint16_t *)MODBUS_PARITY, 1);
	EEPROM_write_word((uint16_t *)MODBUS_MODE, 1);
	*/
	
	init_micro();
	enable_rx();
	EEPROM_read_all();
	check_and_set_mode();
	
	while(1){}
}

//Interrupt based USART RX function.
ISR(USART_RX_vect)
{
	uint8_t rx_byte;
	
	rx_timer = 20;	//Turn on RX LED for 20 ms.
	
	rx_byte = UDR0; //Get RX byte.
	
	if(!last_mode_status)		//Check if in Modbus mode.
	{
		cli();
		TCNT2 = 0x00;			//Reset timer value.
		OCR2A  = t1_5char;		//Set timer value to look for next character.
		TCCR2B = 0x06;			//Start character timer (clock / 256).
		mb_rx_byte(rx_byte);	//Send byte to modbus rx function.
		sei();
	}
	
	else if(last_mode_status)	//Check if in configuration mode.
	{
		TX_byte(rx_byte);		//Echo byte.
		terminal_tok(rx_byte);	//Send byte to terminal tokenizer.
	}
}

//16-bit timer used for 1 ms system timing.
ISR(TIMER1_COMPA_vect)
{
	static uint16_t ms_timer;
	
	//Get current status of hand switch and config header.
	update_modes();
	
	//Update RX/TX LEDs.
	update_RXTX_LEDs();
	
	//Update the hand and config LEDs.
	update_Mode_LEDs(ms_timer);
	
	//Update relay states if hand mode active.
	update_hand_mode_relays();
	
	//Increment timer.
	ms_timer++;
	
	//Reset timer value every second.
	if(ms_timer >= 1000)
		ms_timer = 0;
}

//8-bit timer used for character timing.
ISR(TIMER2_COMPA_vect)
{
	static uint8_t rx_timer_state = MB_RX_IDLE;
	uint8_t error;
	uint16_t crc;
	
	//First state indicates the frame has ended.
	if(rx_timer_state == MB_RX_IDLE)
	{
		rx_timer_state = MB_RX_READY_TO_TX;
		OCR2A  = t3_5char;	
	}
	
	//Second state indicates the device is ready to transmit a response.
	else if(rx_timer_state == MB_RX_READY_TO_TX)
	{		
		TCCR2B = 0x00;				//Stop timer.
		
		error = mb_rtu_rx_parse();	//Parse frame and get error code.
		
		if(error)
		{
			//Do error stuff.
		}
		
		//Do functions associated with this slave device.
		else if(mb_rx_struct.mb_slave_addr == modbus_address)
		{
			//Individual functions need to be handled outside the modbus library
			//because each device is unique.
			switch(mb_rx_struct.mb_function_code)
			{
				//Read status of relays.
				case READ_COILS:
					mb_tx_struct.mb_tx_frame[mb_tx_struct.mb_tx_index++] = 0x01;
					mb_tx_struct.mb_tx_frame[mb_tx_struct.mb_tx_index++] = 0x01;
					mb_tx_struct.mb_tx_frame[mb_tx_struct.mb_tx_index++] = 0x01;
					mb_tx_struct.mb_tx_frame[mb_tx_struct.mb_tx_index++] = 0x00;
					mb_tx_struct.mb_tx_frame[mb_tx_struct.mb_tx_index++] = 0x51;
					mb_tx_struct.mb_tx_frame[mb_tx_struct.mb_tx_index++] = 0x88;
					//Transmit frame.
					tx_modbus_frame();
				break;
				
				case WRITE_SINGLE_COIL:
					//Check if value is valid.
					if(mb_rx_struct.mb_values != 0x0000 && mb_rx_struct.mb_values != 0xFF00)
					{
						mb_rtu_exception(mb_rx_struct.mb_function_code, ILLEGAL_DATA_VALUE);
						tx_modbus_frame();
					}
					//Check if address is valid.
					else if(mb_rx_struct.mb_address > 3)
					{
						mb_rtu_exception(mb_rx_struct.mb_function_code, ILLEGAL_DATA_ADDRESS);
						tx_modbus_frame();
					}
					//Check to see if in hand mode.
					else if(this_hand_status)
					{
						mb_rtu_exception(mb_rx_struct.mb_function_code, SERVER_DEVICE_FAILURE);
						tx_modbus_frame();
					}
					//Update relays and send response.
					else
					{
						if(mb_rx_struct.mb_address == 0x0000)
						{
							if(mb_rx_struct.mb_values == 0xFF00)
								prtRLY1 |= (1 << RLY1);
							else
								prtRLY1 &= ~(1 << RLY1);
						}
						else if(mb_rx_struct.mb_address == 0x0001)
						{
							if(mb_rx_struct.mb_values == 0xFF00)
							prtRLY2 |= (1 << RLY2);
							else
							prtRLY2 &= ~(1 << RLY2);
						}
						else if(mb_rx_struct.mb_address == 0x0002)
						{
							if(mb_rx_struct.mb_values == 0xFF00)
							prtRLY3 |= (1 << RLY3);
							else
							prtRLY3 &= ~(1 << RLY3);
						}
						else if(mb_rx_struct.mb_address == 0x0003)
						{
							if(mb_rx_struct.mb_values == 0xFF00)
							prtRLY4 |= (1 << RLY4);
							else
							prtRLY4 &= ~(1 << RLY4);
						}
						
						//Generate frame.
						mb_tx_struct.mb_tx_frame[mb_tx_struct.mb_tx_index++] = modbus_address;
						mb_tx_struct.mb_tx_frame[mb_tx_struct.mb_tx_index++] = WRITE_SINGLE_COIL;
						mb_tx_struct.mb_tx_frame[mb_tx_struct.mb_tx_index++] = mb_rx_struct.mb_address >> 8;
						mb_tx_struct.mb_tx_frame[mb_tx_struct.mb_tx_index++] = mb_rx_struct.mb_address;
						mb_tx_struct.mb_tx_frame[mb_tx_struct.mb_tx_index++] = mb_rx_struct.mb_values >> 8;
						mb_tx_struct.mb_tx_frame[mb_tx_struct.mb_tx_index++] = mb_rx_struct.mb_values;
						
						//Calculate CRC.
						crc = CRC_16(mb_tx_struct.mb_tx_frame, mb_tx_struct.mb_tx_index);
						
						//Add CRC to frame.
						mb_tx_struct.mb_tx_frame[mb_tx_struct.mb_tx_index++] = crc;
						mb_tx_struct.mb_tx_frame[mb_tx_struct.mb_tx_index++] = crc >> 8;
						
						//Transmit frame.
						tx_modbus_frame();
					}
					
				break;
				
				//Unsupported function.
				default:
					mb_rtu_exception(mb_rx_struct.mb_function_code, ILLEGAL_FUNCTION);
					tx_modbus_frame();
				break;
			}
		}
		
		//Do functions associated with a broadcast.
		else if(mb_rx_struct.mb_slave_addr == 0)
		{
			
		}
		
		//Update counters.  Frame not associated with this slave device.
		else
		{
			
		}
		
		rx_timer_state = MB_RX_IDLE;	//Reset control variables for next rx frame.
		mb_rx_struct.mb_rx_index = 0;
		mb_tx_struct.mb_tx_index = 0;			
	}
	
}