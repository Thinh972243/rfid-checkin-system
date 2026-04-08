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
	// Init PCD and Protocol SPI
	SPI_init();
	SPI_flush();
	PCD_init();
	
	uint8_t ATQA_length;
	uint8_t ATQA_cmd[2];
	StatusCode status;
	uint8_t uid[4];
	uint8_t sak;

	while (1) {
		status = PICC_RequestA(&ATQA_length, &ATQA_cmd[0]); 
		
		if (status == STATUS_OK) {
			// Anticollision loop and select
			status = PICC_Anticollision_Select(&uid[0], &sak);
		    UART_print_StatusCode(status);
			UART_transmit('\n');
			if(status == STATUS_OK){
				UART_print_UID(uid);
				UART_transmit('\n');
			}
			// Delay for testcase
			_delay_ms(500); 
		}
}
}