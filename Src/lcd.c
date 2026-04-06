#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include "lcd.h"
#include "i2c.h"
#include <util/delay.h>

//Send a single byte via I2C protocol
static void LCD_Send_I2C(uint8_t data) {
	I2C_Start();
	I2C_Write(LCD_I2C_ADDR); 
	I2C_Write(data);
	I2C_Stop();
}

// Send 4 bits (1 nibble) along with Register Select (RS) and Enable (EN) pulse 
static void LCD_write_nibble(uint8_t nibble, uint8_t rs) {
	// Data format for PCF8574: [D7 D6 D5 D4 | BL EN RW RS]
	// 0x08 turns on the Backlight (BL)
	uint8_t data = nibble | (rs ? 0x01 : 0x00) | 0x08;
	// Set EN = 1 (0x04)
	LCD_Send_I2C(data | 0x04);
	_delay_us(1);              
	// Set EN = 0
	LCD_Send_I2C(data & ~0x04); 
	_delay_us(50);             
}

// Send an 8-bit command to the LCD 
void LCD_write_command(uint8_t cmd) {
	// Send higher 4 bits (RS=0)
	LCD_write_nibble(cmd & 0xF0, 0);      
	// Send lower 4 bits  (RS=0)
	LCD_write_nibble((cmd << 4) & 0xF0, 0); 
}

//Send a single character to be displayed
void LCD_print_char(char data) {
	// Send higher 4 bits (RS = 1)
	LCD_write_nibble(data & 0xF0, 1); 
	// Send lower 4 bits (RS = 1)       
	LCD_write_nibble((data << 4) & 0xF0, 1); 
}

//Send a full string of characters 
void LCD_print_String(const char *str) {
	while (*str) {
		LCD_print_char(*str++);
	}
}

//Clear the entire LCD screen 
void LCD_Clear(void) {
	LCD_write_command(0x01); // 0x01 is the Clear Display command
	_delay_ms(2);            // Mandatory 2ms delay; clear command takes longer to execute
}

//Move the cursor to a specific row (0 or 1) and column 
void LCD_SetCursor(uint8_t row, uint8_t col) {
	// Row 0 starts at DDRAM address 0x00 -> Command: 0x80 + 0x00 = 0x80
	// Row 1 starts at DDRAM address 0x40 -> Command: 0x80 + 0x40 = 0xC0
	uint8_t pos = (row == 0) ? (0x80 + col) : (0xC0 + col);
	LCD_write_command(pos);
}


void LCD_Init(void) {
	// Wait 50ms for LCD power-up 
	_delay_ms(50); 
	// Software reset sequence 
	LCD_write_nibble(0x30, 0);
	_delay_ms(5);
	LCD_write_nibble(0x30, 0);
	_delay_us(150);
	LCD_write_nibble(0x30, 0);
	
	// Switch to 4-bit mode
	LCD_write_nibble(0x20, 0);
	
	// Config LCD settings
	LCD_write_command(0x28); // 4-bit mode, 2 lines, 5x8 font
	LCD_write_command(0x0C); // Display ON, Cursor OFF, Blink OFF
	LCD_write_command(0x06); 
	LCD_Clear();
}

// Print UID[i] in Dec (0-255)
void LCD_print_number(uint8_t number){
	LCD_print_char((number / 100) + '0');         // Hundreds digit
	LCD_print_char(((number / 10) % 10) + '0');   // Tens digit
	LCD_print_char((number % 10) + '0');          // Units digit
}

// Print UID in UID buffer
void LCD_print_uid(uint8_t *uid){
	for(uint8_t i = 0; i < 4; i++){
		LCD_print_number(uid[i]);
		LCD_print_char(' '); 
	}
}