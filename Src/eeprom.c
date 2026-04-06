#include "eeprom.h"
#include <avr/eeprom.h>

// Array of 20 cards
const uint8_t default_uids[MAX_USERS][UID_LENGTH] = {
	{96, 150, 169, 33},    // Card 1
	{181, 226, 42, 19},    // Card 2
	{25, 144, 76, 210},    // Card 3
	{89, 201, 15, 63},     // Card 4
	{112, 45, 230, 99},    // Card 5
	{44, 180, 55, 12},     // Card 6
	{77, 22, 190, 88},     // Card 7
	{200, 10, 60, 30},     // Card 8
	{5, 99, 120, 240},     // Card 9
	{66, 175, 33, 105},    // Card 10
	{140, 50, 80, 195},    // Card 11
	{210, 85, 14, 75},     // Card 12
	{36, 160, 225, 40},    // Card 13
	{95, 250, 115, 55},    // Card 14
	{130, 70, 48, 165},    // Card 15
	{185, 110, 90, 25},    // Card 16
	{15, 205, 170, 85},    // Card 17
	{105, 35, 215, 145},   // Card 18
	{230, 65, 100, 20},    // Card 19
	{50, 195, 8, 135}      // Card 20
};

//Initialize EEPROM with default UIDs 
void EEPROM_Init_UIDs(void) {
	// Read the very first byte of the EEPROM (address 0x00)
	uint8_t first_byte = eeprom_read_byte((const uint8_t*)0);
	
	// If the first byte does not match Card 1's first byte (96),
	// we assume the EEPROM is blank or has old data.
	if (first_byte != 96) {
		eeprom_update_block((const void*)default_uids, (void*)0, MAX_USERS * UID_LENGTH);
	}
}

// Check if the scanned UID exists in the EEPROM whitelist 
int8_t EEPROM_Get_UID_Index(uint8_t *scanned_uid) {
	uint8_t stored_uid[UID_LENGTH];
	
	for (uint8_t i = 0; i < MAX_USERS; i++) {
		eeprom_read_block((void*)stored_uid, (const void*)(i * UID_LENGTH), UID_LENGTH);
		
		bool match = true;
		for (uint8_t j = 0; j < UID_LENGTH; j++) {
			if (scanned_uid[j] != stored_uid[j]) {
				match = false;
				break;
			}
		}
		
		if (match) {
			return i; // Match found! Return the index of this user (0 to 19)
		}
	}
	return -1; // No match found
}