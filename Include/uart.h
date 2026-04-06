/*
 * UART.h
 *
 * Created: 08/02/2026 12:16:29
 *  Author: thinh
 */ 


#ifndef UART_H_
#define UART_H_


#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include "mfrc522.h"
#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#define BAUDRATE 9600
#define UBRR_CALC (uint32_t) (F_CPU/(16*(uint32_t)BAUDRATE)-1)

#ifdef __cplusplus
extern "C"{
#endif
	void UART_init(uint16_t ubrr);
	void UART_transmit(unsigned char frame_trans);
	void UART_print_string(const char *str);
	void UART_print_StatusCode(StatusCode status);
	void UART_print_Hex(uint8_t number);
	void UART_print_UID(uint8_t *uid);
	#ifdef __cplusplus
	}
#endif




#endif 