#define RESET 1
#include "mfrc522.h"

// Write data to a specific MFRC522 register 
void MFRC522_write_data(MFRC522_reg address_register, uint8_t data_trans){
	SPI_start();
	// Address format: [0][address_register (6 bits)][0]
	uint8_t addr = (address_register << 1) & 0x7E;
	SPI_transmit_byte(addr);
	SPI_transmit_byte(data_trans);
	SPI_stop();
}

// Read data from a specific MFRC522 register 
uint8_t MFRC522_read_data(MFRC522_reg address_register){
	SPI_start();
	uint8_t data_received;
	// Address format: [1 (Read)][address_register (6 bits)][0]
	address_register = ((address_register << 1) & 0x7E) | 0x80;
	SPI_transmit_byte(address_register);
	// send dummy for received data
	data_received = SPI_transmit_byte(0x00); 
	SPI_stop();
	return data_received;
}

// Software reset for the MFRC522 chip 
void PCD_soft_reset(void){
	// Command 0x0F: SoftReset
	MFRC522_write_data(CommandReg, 0x0F);
	_delay_ms(50); // Wait for oscillator to stabilize
}

// Initialize the PCD (Proximity Coupling Device - MFRC522) 
void PCD_init(){
	// Hardware Reset
	DDRB |= (1 << RESET);
	PORTB &= ~(1 << RESET);
	_delay_us(2);
	PORTB |= (1 << RESET);
	_delay_ms(50);
	
	PCD_soft_reset();
	
	// Configure communication speed and timers
	MFRC522_write_data(TxModeReg, 0x00);
	MFRC522_write_data(RxModeReg, 0x00);
	MFRC522_write_data(ModWidthReg, 0x26);
	
	// Timer configuration for 25ms timeout
	MFRC522_write_data(TModeReg, 0x80);
	MFRC522_write_data(TPrescalerReg, 0xA9);
	MFRC522_write_data(TReloadRegH, 0x03);
	MFRC522_write_data(TReloadRegL, 0xE8);
	MFRC522_write_data(TxASKReg, 0x40);
	MFRC522_write_data(ModeReg, 0x3D);
	PCD_AntennaOn();
}

// Enable Antenna 
void PCD_AntennaOn() {
	uint8_t value = MFRC522_read_data(TxControlReg);
	if ((value & 0x03) != 0x03) {
		MFRC522_write_data(TxControlReg, value | 0x03);
	}
}

// Search for PICC (Proximity Integrated Circuit Card) in the field 
StatusCode PICC_RequestA(uint8_t *ATQA_length, uint8_t *ATQA_cmd) {
	uint8_t n;
	
	MFRC522_write_data(CommandReg, 0x00);   // Idle
	MFRC522_write_data(ComIrqReg, 0x7F);    // Clear IRQs
	MFRC522_write_data(FIFOLevelReg, 0x80); // Flush FIFO
	
	// Disable CRC for REQA/WUPA
	MFRC522_write_data(TxModeReg, MFRC522_read_data(TxModeReg) & (~0x80));
	MFRC522_write_data(RxModeReg, MFRC522_read_data(RxModeReg) & (~0x80));
	
	// Use 0x26 for REQA 
	MFRC522_write_data(FIFODataReg, 0x26);
	// Send 7 bits (REQA frame)   
	MFRC522_write_data(BitFramingReg, 0x07); 
	// Transceive command
	MFRC522_write_data(CommandReg, 0x0C);    
	// StartSend
	MFRC522_write_data(BitFramingReg, MFRC522_read_data(BitFramingReg) | 0x80); 
	
	
	uint16_t timeout_counter = 2000;
	do {
		n = MFRC522_read_data(ComIrqReg);
		timeout_counter--;
		if (n & 0x01) { MFRC522_write_data(BitFramingReg, 0x00); return STATUS_TIMEOUT; }
		if (n & 0x02) { MFRC522_write_data(BitFramingReg, 0x00); return STATUS_ERROR; }
			// RxIRq success
		if (n & 0x20) break; 
	} while (timeout_counter > 0);
	
	MFRC522_write_data(BitFramingReg, 0x00);
	
	n = MFRC522_read_data(FIFOLevelReg);
	if (n == 0 || n > 2) return STATUS_NO_ROOM;
	
	*ATQA_length = n;
	for (uint8_t i = 0; i < n; i++) {
		ATQA_cmd[i] = MFRC522_read_data(FIFODataReg);
	}
	
	return STATUS_OK;
}

// Anticollision loop and Select 1 PICC 
StatusCode PICC_Anticollision_Select(uint8_t *uid_out, uint8_t *sak_out) {
	uint8_t buffer[9];
	uint8_t currentLevelKnownBits = 0; // Bits of UID we know
	uint8_t selectDone = 0;            // Flag to break the loop when card is selected
	uint8_t n, irq, txLastBits, rxAlign, bytesToSend;
	
	uint16_t timeout_counter;

	// Clear collision flag 
	MFRC522_write_data(CollReg, MFRC522_read_data(CollReg) & (~0x80));

	
	while (!selectDone) {
		buffer[0] = 0x93; 

		// ==========================================
		// BRANCH 1: SETUP PARAMETERS 
		// ==========================================
		if (currentLevelKnownBits >= 32) {
			// PATH A: SELECT PHASE
			// Known 32 bits (4 bytes), prepare to select the card
			buffer[1] = 0x70; // NVB = 7 bytes (SEL + NVB + 4xUID + BCC)
			
			// Calculate BCC (Block Check Character) - XOR sum of the 4 UID bytes
			buffer[6] = buffer[2] ^ buffer[3] ^ buffer[4] ^ buffer[5];
			
			// Enable hardware CRC (Mandatory for the Select command)
			MFRC522_write_data(TxModeReg, MFRC522_read_data(TxModeReg) | 0x80);
			MFRC522_write_data(RxModeReg, MFRC522_read_data(RxModeReg) | 0x80);
			
			txLastBits = 0; // Send full, complete bytes
			bytesToSend = 7;
			} else {
			// PATH B: ANTICOLLISION PHASE
			//Still missing bits, let PCD ask the PICCs to send the rest.
			
			// Disable hardware CRC (Anticollision frames often have incomplete/fractional bytes)
			MFRC522_write_data(TxModeReg, MFRC522_read_data(TxModeReg) & (~0x80));
			MFRC522_write_data(RxModeReg, MFRC522_read_data(RxModeReg) & (~0x80));
			
			txLastBits = currentLevelKnownBits % 8;
			uint8_t count = currentLevelKnownBits / 8; // Number of fully known bytes
			bytesToSend = 2 + count + (txLastBits ? 1 : 0);
			
			// Set NVB (Number of Valid Bits) to tell the card what we already know
			buffer[1] = (bytesToSend << 4) + txLastBits;
		}

		// Configure BitFramingReg so the chip knows how to handle fractional bits
		rxAlign = txLastBits;
		MFRC522_write_data(BitFramingReg, (rxAlign << 4) | txLastBits);

		// ==========================================
		// BRANCH 2: TRANSCEIVE 
		// ==========================================
		MFRC522_write_data(CommandReg, 0x00);   // Idle command
		MFRC522_write_data(ComIrqReg, 0x7F);    // Clear all interrupt flags
		MFRC522_write_data(FIFOLevelReg, 0x80); // Flush the FIFO buffer

		// Load our assembled buffer into the chip's FIFO
		for (uint8_t i = 0; i < bytesToSend; i++) {
			MFRC522_write_data(FIFODataReg, buffer[i]);
		}

		// config PCD to Trancieve state
		MFRC522_write_data(CommandReg, 0x0C);
		MFRC522_write_data(BitFramingReg, MFRC522_read_data(BitFramingReg) | 0x80); // StartSend

		// Timeout for Trancieve state
		timeout_counter = 2000;
		do {
			irq = MFRC522_read_data(ComIrqReg);
			timeout_counter--;
			if (irq & 0x33) break; // Break if RxIRq, ErrIRq, or TimerIRq occurs
		} while (timeout_counter > 0);

		MFRC522_write_data(BitFramingReg, MFRC522_read_data(BitFramingReg) & (~0x80)); // Stop StartSend

		if (timeout_counter == 0 || (irq & 0x01)) {
			return STATUS_TIMEOUT; // No response from card
		}

		// ==========================================
		// BRANCH 3: COLLISION RESOLUTION 
		// ==========================================
		uint8_t errorRegValue = MFRC522_read_data(ErrorReg);

		if (errorRegValue & 0x08) {
			// Collision Occurred 
			uint8_t collRegValue = MFRC522_read_data(CollReg);
			uint8_t collPos = collRegValue & 0x1F; // Find the exact bit position of the crash
			if (collPos == 0) collPos = 32;
			
			// Update known bits up to the crash point
			currentLevelKnownBits = collPos;
			
			// The Fix: Arbitrarily force the bit at the collision position to '1'.
			// In the next loop iteration, we will only talk to the card that has a '1' here.
			uint8_t count = (currentLevelKnownBits - 1) / 8;
			uint8_t bitMask = 1 << ((currentLevelKnownBits - 1) % 8);
			buffer[2 + count] |= bitMask;
			
			} else {
			//  No Collision
			if (currentLevelKnownBits >= 32) {
				//  Select successful
				// Received sak, out anticollision loop
				*sak_out = MFRC522_read_data(FIFODataReg);
				selectDone = 1; 
				
				} else {
				//  Anticollision successful
				n = MFRC522_read_data(FIFOLevelReg);
				uint8_t index = 2 + (currentLevelKnownBits / 8);
				
				// Read remaining bits to the bufer
				for(uint8_t i = 0; i < n; i++) {
					uint8_t fifo_data = MFRC522_read_data(FIFODataReg);
					if (i == 0 && rxAlign > 0) {
						buffer[index] |= fifo_data;
						} else {
						buffer[index] = fifo_data;
					}
					index++;
				}
				
				// Begin the select phase because we know all the bytes in buffer
				currentLevelKnownBits = 32;
			}
		}
	}

	// Get the UID bytes begin form buffer[2]
	for (uint8_t i = 0; i < 4; i++) {
		uid_out[i] = buffer[2 + i];
	}

	return STATUS_OK;
}

//PICC_HaltA
StatusCode PICC_HaltA(void) {
	// According to ISO/IEC 14443-3, the Halt command consists of two specific bytes: 0x50 and 0x00.
	uint8_t buffer[2] = {0x50, 0x00}; 
	uint8_t irq;
	uint16_t timeout_counter;

	//  Enable Hardware CRC
	MFRC522_write_data(TxModeReg, MFRC522_read_data(TxModeReg) | 0x80);
	MFRC522_write_data(RxModeReg, MFRC522_read_data(RxModeReg) | 0x80);
	
	//Config Bit Framing, send full byte
	MFRC522_write_data(BitFramingReg, 0x00);
	

    // Stop any ongoing command (Idle) 
    MFRC522_write_data(CommandReg, 0x00);   
	// Clear interrupt flags
	MFRC522_write_data(ComIrqReg, 0x7F);  
	// Flush FIFO
	MFRC522_write_data(FIFOLevelReg, 0x80); 
	
	// Load the 2-byte HLTA command into the FIFO
	MFRC522_write_data(FIFODataReg, buffer[0]);
	MFRC522_write_data(FIFODataReg, buffer[1]);

	// config PCD to Trancieved state
	MFRC522_write_data(CommandReg, 0x0C);
	
	// Start trasnmit to Antenna
	MFRC522_write_data(BitFramingReg, MFRC522_read_data(BitFramingReg) | 0x80);

	// Timeout 
	timeout_counter = 2000;
	do {
		irq = MFRC522_read_data(ComIrqReg);
		timeout_counter--;
		if (irq & 0x33) break;
	} while (timeout_counter > 0);

	// Stop transmit
	MFRC522_write_data(BitFramingReg, 0x00);

	// if timeout return STATUS_OK because HALTA command do not received any respond according to ISO 14443-3
	if (timeout_counter == 0 || (irq & 0x01)) {
		return STATUS_OK;     // Success: The card went to sleep silently
	}
	
	return STATUS_ERROR;      // Error: The card sent unexpected data back
}