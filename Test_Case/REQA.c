#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <stdbool.h>
#include "uart.h"
#include "spi.h"
#include "mfrc522.h"

int main(void) {
	
	// Init UART
	UART_init(UBRR_CALC);
	// Init PCD and Protocol
	SPI_init();
	SPI_flush();
	PCD_init();
	
	uint8_t ATQA_length;
	uint8_t ATQA_cmd[2];
	StatusCode status;
	

	while (1)
	{
		//REQA
		status = PICC_RequestA(&ATQA_length, &ATQA_cmd[0]);
		
		// Print StatusCode
		UART_print_StatusCode(status);
		UART_transmit('\n'); 
		
		// Delay 0.5s for testcase
		_delay_ms(500);
	}
}