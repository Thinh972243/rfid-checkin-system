#include "i2c.h"

// Init I2C at speed of 100Khz
void I2C_Init(void) {
	TWSR = 0x00; // Prescaler = 1
	TWBR = 0x48; // Fucntion: (F_CPU/F_I2C - 16)/2
	TWCR = (1 << TWEN); //
}

// Start protocol
void I2C_Start(void) {
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT))); // Ch? c? TWINT ???c set
}

// Send 1 byte of data through I2C
void I2C_Write(uint8_t data) {
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT))); // Ch? g?i xong
}

// Stop protocol
void I2C_Stop(void) {
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}