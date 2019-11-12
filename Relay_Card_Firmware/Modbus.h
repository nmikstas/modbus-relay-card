#ifndef MODBUS_H_
#define MODBUS_H_

#include <avr/io.h>
#include "Board_IO.h"
#include "USART_Control.h"

#define MAX_RX_FRAME 513				//Max rx frame size in bytes.
#define MAX_TX_FRAME 256				//Max tx frame size in bytes.

#define MB_NO_ERROR 0					//General no error code.
#define MB_ERROR 1						//General error code.

//RTU indexes.
#define MB_RTU_SLAVE_ADDR 0				//Index to slave address.
#define MB_RTU_FUNC_CODE  1				//Index to function code.
#define MB_RTU_ADDR_HI    2				//Index to starting address high byte.
#define MB_RTU_ADDR_LO    3				//Index to starting address low byte.
#define MB_RTU_QTY_HI     4				//Index to quantity of inputs/outputs high byte.
#define MB_RTU_QTY_LO     5				//Index to quantity of inputs/outputs low byte.

//ASCII indexes.


//Function codes.
#define READ_COILS               0x01
#define READ_DISCRETE_INPUTS     0x02
#define READ_HOLDING_REGISTERS   0x03
#define READ_INPUT_REGISTERS     0x04
#define WRITE_SINGLE_COIL        0x05
#define WRITE_SINGLE_REGISTER    0x06
#define READ_EXCEPTION_STATUS    0x07
#define DIAGNOSTIC               0x08
#define GET_COM_EVENT_COUNTER    0x0B
#define GET_COM_EVENT_LOG        0x0C
#define WRITE_MULTIPLE_COILS     0x0F
#define WRITE_MULTIPLE_REGISTERS 0x10
#define REPORT_SERVER_ID         0x11
#define READ_FILE_RECORD         0x14
#define WRITE_FILE_RECORD        0x15
#define MASK_WRITE_REGISTER      0x16
#define READ_WRITE_MULTI_REGS    0x17
#define READ_FIFO_QUEUE          0x18
#define READ_DEVICE_ID           0x2B

//Extra functions.
#define ENCAP_INTERFACE_TRANS    0x2B
#define CANOPEN_GEN_REFERENCE    0x2B

//Exception codes.
#define ILLEGAL_FUNCTION		 0x01
#define ILLEGAL_DATA_ADDRESS     0x02
#define ILLEGAL_DATA_VALUE       0x03
#define SERVER_DEVICE_FAILURE    0x04
#define ACKNOWLEDGE              0x05
#define MEMORY_PARITY_ERROR      0x06
#define GATEWAY_PATH_UNAVAIL     0x0A
#define GATWAY_TARGET_NO_RESP    0x0B

//Diagnostic counters and event log.
uint16_t mb_bus_msg_count;
uint16_t mb_bus_com_err_count;
uint16_t mb_slave_exc_err_count;
uint16_t mb_slave_message_count;
uint16_t mb_no_response_count;
uint16_t mb_slave_nak_count;
uint16_t mb_slave_busy_count;
uint16_t mb_char_overrun_count;

//Log array and pointers.
uint8_t mb_event_log[64];
uint8_t mb_log_head;
uint8_t mb_log_tail;

//Tx struct.
typedef struct
{
	uint8_t mb_tx_frame[MAX_TX_FRAME];	//Array containing raw tx frame.
	uint16_t mb_tx_index;				//Index into rx frame.
} Mb_tx_struct;

//Rx struct.
typedef struct 
{
	uint8_t mb_rx_frame[MAX_RX_FRAME];	//Array containing raw rx frame.
	uint16_t mb_rx_index;				//Index into rx frame.
	
	uint8_t mb_rx_crc_lo;				//Low byte of received CRC value.
	uint8_t mb_rx_crc_hi;				//High byte of received CRC value.
	uint8_t mb_calc_crc_lo;				//Low byte of calculated CRC value.
	uint8_t mb_calc_crc_hi;				//High byte of calculated CRC value.
	
	uint8_t mb_slave_addr;				//Slave address of rx frame.
	
	uint8_t mb_function_code;			//Function code.
	uint16_t mb_sub_function;			//Sub function code.
	
	uint16_t mb_address;				//Start address of range of requested values.
	uint16_t mb_values;					//Range of values requested.
	
	uint16_t mb_data_index;				//Index into data field(if present).
	uint16_t mb_data_length;			//Length of data field.	
} Mb_rx_struct;

Mb_tx_struct mb_tx_struct;				//Create tx struct.
Mb_rx_struct mb_rx_struct;				//Create rx struct.

void mb_rx_byte(uint8_t rx_byte);							//Fill modbus rx frame buffer.
uint16_t CRC_16(uint8_t* data_frame, uint16_t data_len);	//The function returns the CRC by means of calculation.

//RTU functions.
uint16_t mb_rtu_get_address();								//Return 16-bit starting address from rx frame.
uint16_t mb_rtu_get_quantity();								//Return 16-bit quantity of inputs/outputs from rx frame.
void mb_rtu_exception(uint8_t mb_func, uint8_t error_code);	//Create tx frame for exception.
uint8_t  mb_rtu_rx_parse();									//Parse the modbus rx array into its sub-components(RTU only).

//ASCII functions.


#endif /* MODBUS_H_ */