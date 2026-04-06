
#include "spi.h" 
#define SPI_DDR DDRB
#define MOSI 3
#define SS 2
#define SCK 5

//Init SPI
void SPI_init(void){
	// MOSI, SCK, SS output
	SPI_DDR|= (1<<MOSI) | (1<<SCK)| (1<<SS) ;
	// spi enable, master mode, 1Mhz,MSB transmit first
	SPCR|= (1<<SPE) | (1<<MSTR) | (1<<SPR0)	;
	// not transmit yet
	PORTB|= (1<<SS);
}

// Select chip in PB2
void SPI_start(void){
	PORTB&= ~(1<<SS);
}

// Deselect chip in PB2
void SPI_stop(void){
	PORTB|= (1<<SS);
}

// transmit byte of data using SPI
uint8_t SPI_transmit_byte(uint8_t data){
	
	SPDR=data;
	// Wait for transmitton complete
	while(!(SPSR&(1<<SPIF)));
	// get byte from slave
	return SPDR;
	
}

// Flush SPDR
void SPI_flush(void){
	uint8_t dummy;
	while(SPSR&(1<<SPIF)){
		dummy=SPDR;
	}
}





