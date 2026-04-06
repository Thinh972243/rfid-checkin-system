

#ifndef EEPROM_H_
#define EEPROM_H_

#include <stdint.h>
#include <stdbool.h>

#define MAX_USERS 20
#define UID_LENGTH 4

//  Init EEPROM and stores 20 default UID value
void EEPROM_Init_UIDs(void);

// Checks if the scanned UID exists in EEPROM and returns its index
int8_t EEPROM_Get_UID_Index(uint8_t *scanned_uid);



#endif /* EEPROM_H_ */