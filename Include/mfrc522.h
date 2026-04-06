/*
 * IncFile1.h
 *
 * Created: 13/02/2026 15:58:18
 *  Author: admin
 */ 


#ifndef MFRC522_H_
#define MFRC522_H_






#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <util/delay.h>
#include "spi.h" 

typedef struct  MIFARE_UID
{
	uint8_t UID[4];
	uint8_t sak;
}MIFARE_UID;

typedef enum MFRC522_reg{
	CommandReg      = 0x01,
	ComIrqReg       = 0x04,
	ErrorReg        = 0x06,
	FIFODataReg     = 0x09,
	FIFOLevelReg    = 0x0A,
	BitFramingReg   = 0x0D,
	CollReg         = 0x0E,
	ModeReg         = 0x11,
	TxModeReg       = 0x12,
	RxModeReg       = 0x13,
	TxControlReg    = 0x14,
	TxASKReg        = 0x15,
	ModWidthReg     = 0x24,
	TModeReg        = 0x2A,
	TPrescalerReg   = 0x2B,
	TReloadRegH     = 0x2C,
	TReloadRegL     = 0x2D
	}MFRC522_reg;

typedef enum StatusCode{
	STATUS_OK				,	// Success
	STATUS_ERROR			,	// Error in communication
	STATUS_COLLISION		,	// Collission detected
	STATUS_TIMEOUT			,	// Timeout in communication.
	STATUS_NO_ROOM			,	// A buffer is not big enough.
	STATUS_INVALID			,	// Invalid argument.
	STATUS_CRC_WRONG		,	// The CRC_A does not match
	STATUS_MIFARE_NACK		= 0xff // A MIFARE PICC responded with NAK.
}StatusCode;

void MFRC522_write_data(MFRC522_reg address_register, uint8_t data_trans);
uint8_t MFRC522_read_data(MFRC522_reg address_register);
void PCD_init();
void PCD_AntennaOn();
StatusCode PICC_RequestA(uint8_t *RTQA_lenght,uint8_t *RTQA_cmd);
StatusCode PICC_Anticollision_Select(uint8_t *uid_out, uint8_t *sak_out);
StatusCode PICC_HaltA();



#endif 