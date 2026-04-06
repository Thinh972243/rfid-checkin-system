#ifndef LCD_H_
#define LCD_H_

#include <avr/io.h>


#define LCD_I2C_ADDR 0x4E

void LCD_Init(void);
void LCD_write_command(uint8_t cmd);
void LCD_print_char(char data);
void LCD_print_String(const char *str);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_print_number(uint8_t number);
void LCD_print_uid(uint8_t *uid);
#endif /* LCD_H_ */