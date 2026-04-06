


#ifndef SPI_H
#define SPI_H

#include <stdio.h>
#include <avr/io.h>
#include <stdbool.h>




typedef enum spi_mode{
	SPI_MODE_0,
	SPI_MODE_1,
	SPI_MODE_2,
	SPI_MODE_3,
	}spi_mode;


void SPI_init(void);
void SPI_start(void);
void SPI_stop(void);
uint8_t SPI_transmit_byte(uint8_t data);
void SPI_flush(void);




#endif  // SPI_H

