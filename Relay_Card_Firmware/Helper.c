#include "Helper.h"

//Initialize the micro-controller.
void init_micro()
{	
	//Turn off unused hardware.
	PRR =  (1 << PRTIM0) | (1 << PRSPI);
	ACSR = (1  << ACD); //Turn off analog comparator.
	
	//Tweek oscillator(if necessary). Original value 0xB4.
	OSCCAL = 0xBC;
	
	//Set pins as outputs.
	DDRB |= (1 << HANDRED);
	DDRB |= (1 << HANDGRN);
	DDRB |= (1 << MODERED);
	DDRB |= (1 << MODEGRN);
	DDRC |= (1 << RXLED);
	DDRC |= (1 << TXLED);
	DDRC |= (1 << RLY1);
	DDRC |= (1 << RLY2);
	DDRC |= (1 << RLY3);
	DDRC |= (1 << RLY4);
	DDRD |= (1 << EN);
	
	//Set pins as inputs.
	DDRD &= ~(1 << HAND1);
	DDRD &= ~(1 << HAND2);
	DDRD &= ~(1 << HAND3);
	DDRD &= ~(1 << HAND4);
	DDRB &= ~(1 << HAND);
	DDRD &= ~(1 << MODE);
	
	//Turn on pull-up resistors.
	prtHAND1 |= (1 << HAND1);
	prtHAND2 |= (1 << HAND2);
	prtHAND3 |= (1 << HAND3);
	prtHAND4 |= (1 << HAND4);
	prtHAND  |= (1 << HAND);
	prtMODE  |= (1 << MODE);
	
	//Initial USART setup.
	UCSR0A = (1 << U2X0);			//2x USART clock.
	UCSR0B = (1 << RXCIE0);			//Enable RX interrupt.
	
	//Load TIMER1 configuration.
	TCCR1B = 0x09;					//No prescaling, CTC.
	OCR1A  = 8150;					//Match counter every 1 ms.
	TIMSK1 = 0x02;					//Enable TIMER1_COMPA interrupt.
	
	//Load TIMER2 setup.
	TCCR2A = 0x02;					//CTC operation.
	TIMSK2 = 0x02;					//Enable TIMER2_COMPA interrupt.
	
	sei(); //Enable global interrupts.
}

//Converts an unsigned integer into a null terminated string.
//return values:
//0 = no errors.
uint8_t string_from_uint(uint16_t i, char *s)
{
	uint8_t number_started = FALSE; //Indicate conversion has started.
	
	if(i / 10000) //Get upper digit if present.
	{
		number_started = TRUE;
		*s = i / 10000 + 0x30;
		i %= 10000;
		s++;
	}
	
	if(i / 1000 || number_started) //Get next digit if present.
	{
		number_started = TRUE;
		*s = i / 1000 + 0x30;
		i %= 1000;
		s++;
	}
	
	if(i / 100 || number_started) //Get next digit if present.
	{
		number_started = TRUE;
		*s = i / 100 + 0x30;
		i %= 100;
		s++;
	}
	
	if(i / 10 || number_started) //Get next digit if present.
	{
		number_started = TRUE;
		*s = i / 10 + 0x30;
		i %= 10;
		s++;
	}
	
	//Always do last steps.
	*s = i + 0x30; //Get last digit.
	s++;
	
	*s = 0;	//Null terminate string.
	
	return 0;
}

//Takes one decimal value from an array and converts to a numeric value.
uint8_t convert_1_num(char *p, uint16_t *value)
{
	if(check_int_range(p[0])) //Check to make sure character is a valid integer character.
	{
		return ERROR;
	}
	
	(*value) = (p[0] - 0x30);
	
	return NO_ERROR;
}

//Takes two decimal values from an array and converts to a numeric value.
uint8_t convert_2_num(char *p, uint16_t *value)
{
	uint16_t working_value;
	
	if(convert_1_num(p, &working_value))
	{
		return ERROR;
	}
	
	working_value *= 10; //Upper digit. Multiply by 10.
	
	if(convert_1_num(&p[1], value))
	{
		return ERROR;
	}
	
	*value = (*value) + working_value;
	
	return NO_ERROR;
}

//Takes three decimal values from an array and converts to a numeric value.
uint8_t convert_3_num(char *p, uint16_t *value)
{
	uint16_t working_value1, working_value2;
	
	if(convert_1_num(p, &working_value1))
	{
		return ERROR;
	}
	
	working_value1 *= 100; //Upper digit. Multiply by 100.
	
	if(convert_1_num(&p[1], &working_value2))
	{
		return ERROR;
	}
	
	working_value2 *= 10; //Middle digit. Multiply by 10.
	
	if(convert_1_num(&p[2], value))
	{
		return ERROR;
	}
	
	*value = (*value) + working_value1 + working_value2;
	
	return NO_ERROR;
}

//Check if character is a valid decimal value.
uint8_t check_int_range(const uint8_t value)
{
	if(value < '0')
	{
		return ERROR;
	}
	
	if(value > '9')
	{
		return ERROR;
	}
	
	return NO_ERROR;
}

//Update the hand and config LEDs.
void update_Mode_LEDs(uint16_t ms_timer)
{
	//Update the blinking pattern of the LEDs.
	if(this_hand_status) //Blink hand status red.
	{
		if(ms_timer <= 500)
		{
			prtHANDRED |= (1 << HANDRED);
			prtHANDGRN &= ~(1 << HANDGRN);
		}
		else
		{
			prtHANDRED &= ~(1 << HANDRED);
			prtHANDGRN &= ~(1 << HANDGRN);
		}
	}
	else //Blink hand status green.
	{
		if(ms_timer <= 500)
		{
			prtHANDRED &= ~(1 << HANDRED);
			prtHANDGRN |= (1 << HANDGRN);
		}
		else
		{
			prtHANDRED &= ~(1 << HANDRED);
			prtHANDGRN &= ~(1 << HANDGRN);
		}
	}
	
	if(this_mode_status) //Blink mode status red.
	{
		if(ms_timer > 500)
		{
			prtMODERED |= (1 << MODERED);
			prtMODEGRN &= ~(1 << MODEGRN);
		}
		else
		{
			prtMODERED &= ~(1 << MODERED);
			prtMODEGRN &= ~(1 << MODEGRN);
		}
	}
	else //Blink mode status green.
	{
		if(ms_timer > 500)
		{
			prtMODERED &= ~(1 << MODERED);
			prtMODEGRN |= (1 << MODEGRN);
		}
		else
		{
			prtMODERED &= ~(1 << MODERED);
			prtMODEGRN &= ~(1 << MODEGRN);
		}
	}
}

//Update relay states if in hand mode.
void update_hand_mode_relays()
{
	uint8_t hand_bits;
	
	if(this_hand_status)
	{
		hand_bits = prtHANDCHANNELS;
		
		if(hand_bits & (1 << HAND1))
			prtRLY1 &= ~(1 << RLY1);
		else
			prtRLY1 |= (1 << RLY1);
		
		if(hand_bits & (1 << HAND2))
			prtRLY2 &= ~(1 << RLY2);
		else
			prtRLY2 |= (1 << RLY2);
			
		if(hand_bits & (1 << HAND3))
			prtRLY3 &= ~(1 << RLY3);
		else
			prtRLY3 |= (1 << RLY3);
		
		if(hand_bits & (1 << HAND4))
			prtRLY4 &= ~(1 << RLY4);
		else
			prtRLY4 |= (1 << RLY4);
	}
}

//Get status of hand switch and config header and
//update the device accordingly.
void update_modes()
{
	//Enumerations for change flags.
	enum hand_change hc = HNO_CHANGE;
	enum mode_change mc = MNO_CHANGE;
	
	//Get current hand switch and config header status.
	this_hand_status = prtHAND;
	this_mode_status = prtMODE;
	
	this_hand_status &= (1 << HAND);
	this_mode_status &= (1 << MODE);
	
	//If hand switch status has changed, set change flag.
	if(this_hand_status != last_hand_status)
	{
		if(this_hand_status)
		hc = HAND_MODE;
		else
		hc = AUTO_MODE;
	}
	
	//If config header status has changed, set change flag.
	if(this_mode_status != last_mode_status)
	{
		if(this_mode_status)
		mc = CONFIG_MODE;
		else
		mc = PROD_MODE;
	}
	
	//Switch to new mode.
	if(hc == HAND_MODE)
	{
		//Any special config at mode switch goes here.
		//No config needed at this time.
	}
	else if(hc == AUTO_MODE)
	{
		//Any special config at mode switch goes here.
		//No config needed at this time.
	}
	
	if(mc == CONFIG_MODE)
	{
		terminal_setup();	//Setup terminal mode.
	}
	else if(mc == PROD_MODE)
	{
		modbus_setup();		//Setup modbus mode.
	}
	
	//Update statuses.
	last_hand_status = this_hand_status;
	last_mode_status = this_mode_status;
}

//Check mode header and setup USART accordingly.
void check_and_set_mode()
{
	uint8_t mode;
	
	mode = prtMODE;
	
	if(mode & (1 << MODE))
		terminal_setup();
	else
		modbus_setup();
}

//Setup serial port for terminal communications.
void terminal_setup()
{
	uint16_t ubrr = B_9600;			//Get baud rate for terminal.
	
	UBRR0H = (uint8_t)(ubrr >> 8);	//Load upper ubrr byte.
	UBRR0L = (uint8_t)ubrr;			//Load lower ubrr byte.
	
	UCSR0C &= ~(1 << UMSEL01);		//Asynchronous mode.
	UCSR0C &= ~(1 << UMSEL00);
				
	UCSR0C |= (1 << UCSZ01);		//Use 8 bit bytes.
	UCSR0C |= (1 << UCSZ00);
	
	UCSR0C &= ~(1 << UPM01);		//No parity.
	UCSR0C &= ~(1 << UPM00);
	
	UCSR0C &= ~(1 << USBS0);		//1 stop bit.
}

//Setup serial port for modbus communications.
void modbus_setup()
{
	uint16_t ubrr;
	
	//setup modbus baud rate.
	if(modbus_baud == BAUD_38400)
	{
		ubrr = B_38_4;
		t1_5char = B_38_4_1_5_CHAR;	//Setup character timing values.
		t3_5char = B_38_4_3_5_CHAR;
	}
	else if(modbus_baud == BAUD_19200)
	{
		ubrr = B_19_2;
		t1_5char = B_19_2_1_5_CHAR;	//Setup character timing values.
		t3_5char = B_19_2_3_5_CHAR;
	}
	else
	{
		ubrr = B_9600;
		t1_5char = B_9600_1_5_CHAR;	//Setup character timing values.
		t3_5char = B_9600_3_5_CHAR;
	}
	
	UBRR0H = (uint8_t)(ubrr >> 8);	//Load upper ubrr byte.
	UBRR0L = (uint8_t)ubrr;			//Load lower ubrr byte.
	
	UCSR0C &= ~(1 << UMSEL01);		//Asynchronous mode.
	UCSR0C &= ~(1 << UMSEL00);
		
	UCSR0C |= (1 << UCSZ01);		//Use 8 bit bytes.
	UCSR0C |= (1 << UCSZ00);	
	
	//Setup parity and stop bits.
	if(modbus_parity == PARITY_EVEN)
	{
		UCSR0C |= (1 << UPM01);		//Even parity.
		UCSR0C &= ~(1 << UPM00);
		
		UCSR0C &= ~(1 << USBS0);	//1 stop bit.
	}
	
	else if(modbus_parity == PARITY_ODD)
	{
		UCSR0C |= (1 << UPM01);		//Odd parity.
		UCSR0C |= (1 << UPM00);
		
		UCSR0C &= ~(1 << USBS0);	//1 stop bit.
	} 
	
	else
	{
		UCSR0C &= ~(1 << UPM01);	//No parity.
		UCSR0C &= ~(1 << UPM00);
		
		UCSR0C |= (1 << USBS0);		//2 stop bits.
	}
}