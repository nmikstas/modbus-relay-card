#ifndef BOARD_IO_H_
#define BOARD_IO_H_

//Firmware revision numbers.
#define FIRMWARE_MAJOR 0
#define FIRMWARE_MINOR 1 

//LED defines.
#define prtHANDRED PORTB
#define prtHANDGRN PORTB
#define prtMODERED PORTB
#define prtMODEGRN PORTB
#define prtRXLED   PORTC
#define prtTXLED   PORTC

#define HANDRED 0
#define HANDGRN 1
#define MODERED 6
#define MODEGRN 7
#define RXLED   5
#define TXLED   4

//Relay defines.
#define prtRLY1 PORTC
#define prtRLY2 PORTC
#define prtRLY3 PORTC
#define prtRLY4 PORTC

#define RLY1 0
#define RLY2 1
#define RLY3 2
#define RLY4 3

//Hand control defines.
#define prtHAND1 PIND
#define prtHAND2 PIND
#define prtHAND3 PIND
#define prtHAND4 PIND

#define prtHANDCHANNELS PIND

#define HAND1 2
#define HAND2 3
#define HAND3 4
#define HAND4 5

//TX/RX defines.
#define prtEN PORTD

#define EN 7

//Mode header defines.
#define prtMODE PIND

#define MODE 6

//Hand switch defines.
#define prtHAND PINB

#define HAND 2

//USART baud rate defines.
#define B_2400  416
#define B_4800  207
#define B_9600  103
#define B_14_4  68
#define B_19_2  51
#define B_28_8  34
#define B_38_4  25
#define B_57_6  16
#define B_76_8  12
#define B_115_2 8
#define B_230_4 3
#define B_250K  3
#define B_500K  1
#define B_1M    0

//Define character timing values.
#define B_9600_1_5_CHAR 54
#define B_9600_3_5_CHAR 126
#define B_19_2_1_5_CHAR 27
#define B_19_2_3_5_CHAR 63
#define B_38_4_1_5_CHAR 14
#define B_38_4_3_5_CHAR 32

//Configuration defines.
#define BAUD_9600  1
#define BAUD_19200 2
#define BAUD_38400 3

#define PARITY_EVEN  1
#define PARITY_ODD 2
#define PARITY_NONE 3

#define TRANS_MODE_RTU  1
#define TRANS_MODE_ASCII 2

//Used to determine return value error codes.
#define NO_ERROR 0
#define ERROR 1

#define FALSE 0
#define TRUE 1

//Modbus RX timer states.
#define MB_RX_IDLE         0
#define MB_RX_READY_TO_TX  1

//variables for hand switch and config header status bits.
uint8_t last_hand_status, this_hand_status;
uint8_t last_mode_status, this_mode_status;

//Enumerations to define changes in the hand switch and config header.
enum hand_change {HNO_CHANGE, HAND_MODE, AUTO_MODE};
enum mode_change {MNO_CHANGE, CONFIG_MODE, PROD_MODE};

//Modbus configuration values.
uint16_t modbus_address;
uint16_t modbus_baud;
uint16_t modbus_parity;
uint16_t modbus_mode;

//Character timing values.
uint8_t t1_5char;
uint8_t t3_5char;

#endif /* BOARD_IO_H_ */