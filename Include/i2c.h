#ifndef I2C_H_
#define I2C_H_

#include <avr/io.h>

void I2C_Init(void);
void I2C_Start(void);
void I2C_Write(uint8_t data);
void I2C_Stop(void);

#endif 