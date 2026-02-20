	#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

#include "stm32f4xx.h"
#include <string.h>
#include <stdlib.h>

/* =============================================
   HARDWARE & PIN DEFINITIONS
   ============================================= */
// Relay (Active Low) -> PD12
#define PIN_RELAY    (1 << 12) 
// Status LED (Orange) -> PD13
#define PIN_STATUS   (1 << 13)
// Error LED (Red) -> PD14
#define PIN_ERROR    (1 << 14)
// Reset Button (User Button) -> PA0
#define PIN_RESET    (1 << 0)

// I2C OLED Address
#define OLED_ADDR    0x78 

// Protocol Definitions
#define CMD_ENABLE_SYS  '1' // From ESP32
#define CMD_DISABLE_SYS '0' // From ESP32
#define CMD_START_NAME  '$' // From ESP32
#define APP_CMD_UNLOCK  '1' // From Bluetooth App

/* =============================================
   GLOBAL FLAGS & VARIABLES (Externs)
   ============================================= */
extern volatile uint8_t bluetooth_enabled; 
extern char keyBuffer[5];               
extern int keyIdx;
extern const char SYS_PIN[];

// Name Protocol Variables
extern char guest_name[21]; 
extern volatile uint8_t update_screen; 
extern volatile uint8_t receiving_name; 
extern int name_idx;

/* =============================================
   SHARED PROTOTYPES
   ============================================= */
void Grant_Access(void);
void Deny_Access(void);
void Delay_Rough(volatile int count);

#endif /* SYSTEM_CONFIG_H */