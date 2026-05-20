#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h>    // 1. THÊM th? vi?n này ?? dùng hàm sprintf
#include "uart.h"
#include "spi.h"
#include "mfrc522.h"
#include "i2c.h"
#include "lcd.h"
#include "eeprom.h"

// user_status[i]==0: OUT (Absent) ; user_status[i]=1 IN (Present)
uint8_t user_status[MAX_USERS] = {0};

int main(void)
{
	// Init LCD and low layer protocol(I2C)
	I2C_Init();
	LCD_Init();
	LCD_Clear();
	
	// Init PCD (MFRC522) and low layer protocol(SPI)
	SPI_init();
	SPI_flush();
	PCD_init();
	
	// Init EEPROM and stores 20 default UID value
	EEPROM_Init_UIDs();
	
	// Set cursor and print "Ready" if LCD is ready
	LCD_SetCursor(0,0);
	LCD_print_String("Ready");
	
	// Parameter
	MIFARE_UID user_card;
	StatusCode status;
	
	// 2. KHAI BÁO m?ng ??m ?? ch?a chu?i text tr??c khi in ra LCD
	char lcd_buffer[16];
	
	while (1)
	{
		status = PICC_RequestA(&user_card.ATQA_length, &user_card.ATQA_cmd[0]);
		if(status == STATUS_OK){
			status = PICC_Anticollision_Select(&user_card.UID[0], &user_card.sak);
			if(status == STATUS_OK){
				status = PICC_HaltA();
				LCD_Clear();
				
				// Get the user index (0-19) from EEPROM
				int8_t user_index = EEPROM_Get_UID_Index(&user_card.UID[0]);
				
				// 3. ?Ã S?A: Thêm d?u { ?? bao tr?n kh?i x? lý th? h?p l?
				if (user_index != -1) {
					
					// --- HÀNG 0: IN TR?NG THÁI ---
					LCD_SetCursor(0, 0);
					if (user_status[user_index] == 0) {
						// User was OUT, now checking IN
						user_status[user_index] = 1;
						LCD_print_String("Check-IN");
						} else {
						// User was IN, now checking OUT
						user_status[user_index] = 0;
						LCD_print_String("Check-OUT");
					}
					
					// --- HÀNG 1: IN S? TH? T? NHÂN VIÊN ---
					
					// Ghép ch? "Nhan vien " và bi?n user_index vào lcd_buffer
					sprintf(lcd_buffer, " NV %d", user_index);
					LCD_print_String(lcd_buffer);
					
					} else {
					// INVALID CARD (Th? không có trong h? th?ng)
					LCD_SetCursor(0, 0);
					LCD_print_String("No Information");
				} // K?t thúc if (user_index != -1)
				
				// Delay so user can read the LCD
				_delay_ms(500);
				
				// (Tùy ch?n) Xóa màn hình và hi?n l?i ch? Ready ch? ng??i ti?p theo
				LCD_Clear();
				LCD_SetCursor(0,0);
				LCD_print_String("Ready");
			}
		}
		_delay_ms(50);
	}
}