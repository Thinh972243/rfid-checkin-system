#include "uart.h"

// UART init
void UART_init(uint16_t ubrr){
	// transmit speed 9600bps
	UBRR0H= (uint8_t) (ubrr>>8);
	UBRR0L= (uint8_t) ubrr;
	// Asynchronus mode, 8 bit frame, no parity, only transmit
	UCSR0C|= (1<<UCSZ01)| (1<< UCSZ00);
	// enable transmit
	UCSR0B|= (1<<TXEN0)	;
}

// Transmit char of data
void UART_transmit(unsigned char frame_trans){
	while(!(UCSR0A&(1<<UDRE0))){
		;
	}
	UDR0= frame_trans;
}

// Transmit string of data
void UART_print_string(const char *str){
	while(*str){
		UART_transmit(*str);
		str++;	
	}
}


// Function for debugging. Print Status code for PICC_RequestA, if success continue for PICC_Anticollision_Select
void UART_print_StatusCode(StatusCode status){
	switch(status){
		case STATUS_OK:
		UART_print_string("STATUS_OK");
		break;
		case STATUS_ERROR:
		UART_print_string("STATUS_ERROR");
		break;
		case STATUS_COLLISION:
		UART_print_string("STATUS_COLLISION");
		break;
		case STATUS_TIMEOUT:
		UART_print_string("STATUS_TIMEOUT");
		break;
		case STATUS_NO_ROOM:
		UART_print_string("STATUS_NO_ROOM");
		break;
		case STATUS_INVALID:
		UART_print_string("STATUS_INVALID");
		break;
		case STATUS_CRC_WRONG:
		UART_print_string("STATUS_CRC_WRONG");
		break;
		case STATUS_MIFARE_NACK:
		UART_print_string("STATUS_MIFARE_NACK");	
		break;
		default: 
		UART_print_string("UKNOWN");
	}
}

// Print UID in hex through hercules
void UART_print_Hex(uint8_t number){
	//look up table
	char hex_table[]="0123456789ABCDEF";
	UART_transmit(hex_table[(number>>4)&0x0F]);
	UART_transmit(hex_table[number&0x0F]);
}

// Print uid when PICC_Anticollision_Select return Status_OK
void UART_print_UID(uint8_t *uid){
	for (uint8_t i=0; i<4; i++){
		UART_print_Hex(uid[i]);
		UART_transmit(' ');
	}
}
