#include "modbus.h"

//Fill modbus rx frame buffer.
void mb_rx_byte(uint8_t rx_byte)
{
	if(mb_rx_struct.mb_rx_index < MAX_RX_FRAME)				//Save byte only if there is room.
		mb_rx_struct.mb_rx_frame[mb_rx_struct.mb_rx_index++] = rx_byte;
}

//The function returns the CRC by means of calculation.
uint16_t CRC_16(uint8_t* data_frame, uint16_t data_len)
{
	const uint16_t POLY = 0xA001;	//Calculation polynomial.
	uint16_t CRC = 0xFFFF;			//Set initial CRC register contents.
	uint8_t n;						//Bit shifting counter.

	while(data_len--)				//Do all bytes.
	{
		CRC ^= *data_frame++;
		n = 0;

		while(n++ < 8)				//Shift all bits.
		{
			if(CRC & 0x0001)		//If LSB is set, XOR with CRC polynomial.
			CRC = (CRC >> 1) ^ POLY;
			else
			CRC >>= 1;
		}
	}

	return CRC;
}

//Return 16-bit starting address from rx frame.
uint16_t mb_rtu_get_address()
{
	uint16_t address;
	
	address = mb_rx_struct.mb_rx_frame[MB_RTU_ADDR_HI] << 8;
	address |= mb_rx_struct.mb_rx_frame[MB_RTU_ADDR_LO];
	
	return address;
}

//Return 16-bit quantity of inputs/outputs from rx frame.
uint16_t mb_rtu_get_quantity()
{
	uint16_t quantity;
	
	quantity = mb_rx_struct.mb_rx_frame[MB_RTU_QTY_HI] << 8;
	quantity |= mb_rx_struct.mb_rx_frame[MB_RTU_QTY_LO];
	
	return quantity;
}

//Parse the modbus rx array into its sub-components(RTU only).
uint8_t mb_rtu_rx_parse()
{
	uint16_t crc;
	
	//Get received CRC value.
	mb_rx_struct.mb_rx_crc_hi = mb_rx_struct.mb_rx_frame[mb_rx_struct.mb_rx_index - 1];
	mb_rx_struct.mb_rx_crc_lo = mb_rx_struct.mb_rx_frame[mb_rx_struct.mb_rx_index - 2];
	
	//Calculate CRC value and save in rx struct.
	crc = CRC_16(mb_rx_struct.mb_rx_frame, mb_rx_struct.mb_rx_index - 2);
	mb_rx_struct.mb_calc_crc_hi = crc >> 8;
	mb_rx_struct.mb_calc_crc_lo = crc;
	
	//If frame is less than 3 characters, exit with error.
	if(mb_rx_struct.mb_rx_index < 3)
	{
		return MB_ERROR;
	}
	
	//If CRCs do not match, exit with error.
	if(mb_rx_struct.mb_calc_crc_hi != mb_rx_struct.mb_rx_crc_hi ||
	   mb_rx_struct.mb_calc_crc_lo != mb_rx_struct.mb_rx_crc_lo)
	   {
		return MB_ERROR;
	   }
	///Get slave address.
	mb_rx_struct.mb_slave_addr = mb_rx_struct.mb_rx_frame[MB_RTU_SLAVE_ADDR];
	
	//Get function code.
	mb_rx_struct.mb_function_code = mb_rx_struct.mb_rx_frame[MB_RTU_FUNC_CODE];
	
	//Determine how to proceed next.
	switch(mb_rx_struct.mb_function_code)
	{
		case READ_COILS:
		case READ_DISCRETE_INPUTS:
		case READ_HOLDING_REGISTERS:
		case READ_INPUT_REGISTERS:
		case WRITE_SINGLE_COIL:
		case WRITE_SINGLE_REGISTER:
			//Get starting address.
			mb_rx_struct.mb_address = mb_rtu_get_address();
			//Get quantity of inputs.
			mb_rx_struct.mb_values = mb_rtu_get_quantity();	
		break;
		
		case DIAGNOSTIC:
			//Get sub function code.
			//Get data index.
			//Get data length.
		break;
		
		case WRITE_MULTIPLE_COILS:
		case WRITE_MULTIPLE_REGISTERS:
			//Get address.
			//Get quantity.
			//Get byte count.
			//Get data index.
			//Get data length.
		break;
		
		case READ_FILE_RECORD:
			//Do file record stuff.
		break;
		
		case WRITE_FILE_RECORD:
			//Do file record stuff.
		break;
		
		case MASK_WRITE_REGISTER:
			//Get address.
			//Get AND mask.
			//Get OR mask.
		break;
		
		case READ_WRITE_MULTI_REGS:
			//Do multi register stuff.
		break;
		
		case READ_FIFO_QUEUE:
			//Get FIFO pointer address.
		break;
		
		case READ_DEVICE_ID:
			//Do device ID stuff.
		break;
		
		case READ_EXCEPTION_STATUS:
		case GET_COM_EVENT_COUNTER:
		case GET_COM_EVENT_LOG:
		case REPORT_SERVER_ID:
		default:
		//Nothing left to do. function code only.
		break;
	}
	
	return MB_NO_ERROR;
}

//Create tx frame for exception.
void mb_rtu_exception(uint8_t mb_func, uint8_t error_code)
{
	uint16_t crc;
	
	//Assemble first part of frame.
	mb_tx_struct.mb_tx_frame[mb_tx_struct.mb_tx_index++] = modbus_address;
	mb_tx_struct.mb_tx_frame[mb_tx_struct.mb_tx_index++] = mb_func + 0x80;
	mb_tx_struct.mb_tx_frame[mb_tx_struct.mb_tx_index++] = error_code;
	
	//Calculate CRC.
	crc = CRC_16(mb_tx_struct.mb_tx_frame, mb_tx_struct.mb_tx_index);
	
	//Add CRC to end of frame.
	mb_tx_struct.mb_tx_frame[mb_tx_struct.mb_tx_index++] = crc;
	mb_tx_struct.mb_tx_frame[mb_tx_struct.mb_tx_index++] = crc >> 8;
}