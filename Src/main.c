#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <stdbool.h>
#include "uart.h"
#include "spi.h"
#include "mfrc522.h"
#include "i2c.h"
#include "lcd.h"
#include "eeprom.h" 

// user_status[i]==0: OUT (Absent or not checked in yet)  ; user_status[i]=1 IN (Present / Currently inside the premises)
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
	
	//
	MIFARE_UID user_card;
	StatusCode status;
	
	while (1)
	{
		status = PICC_RequestA(&user_card.ATQA_length, &user_card.ATQA_cmd[0]);
		if(status == STATUS_OK){
			status = PICC_Anticollision_Select(&user_card.UID[0], &user_card.sak);
			if(status == STATUS_OK){
				
				LCD_Clear();
				
				// Get the user index (0-19) from EEPROM
				int8_t user_index = EEPROM_Get_UID_Index(&user_card.UID[0]);
				
				if (user_index != -1) {
					// VALID CARD
					LCD_SetCursor(0, 0); // Row 0
					
					// Check user_status 
					if (user_status[user_index] == 0) {
						// User was OUT, now checking IN
						user_status[user_index] = 1; // Update status to IN
						LCD_print_String("Check-IN OK");
						} else {
						// User was IN, now checking OUT
						user_status[user_index] = 0; // Update status to OUT
						LCD_print_String("Check-OUT OK");
					}
					
					} else {
					// INVALID CARD
					LCD_SetCursor(0, 0);
					LCD_print_String("No Information");
					
				}
				
				status = PICC_HaltA();
				
				// Delay so user can read the LCD
				_delay_ms(1000);
				
				LCD_Clear();
				
			}
		}
		_delay_ms(50);
	}
		}