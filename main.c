#include "system_config.h"
#include "bluetooth.h"
#include "oled.h"
#include "keypad.h"
#include "esp32.h"

/* =============================================
   GLOBAL FLAGS & VARIABLES
   ============================================= */
volatile uint8_t bluetooth_enabled = 0; 
char keyBuffer[5] = {0};               
int keyIdx = 0;
const char SYS_PIN[] = "1234";

// Name Protocol Variables
char guest_name[21] = {0}; 
volatile uint8_t update_screen = 0; 
volatile uint8_t receiving_name = 0; 
int name_idx = 0;

/* =============================================
   LOCAL PROTOTYPES
   ============================================= */
void System_Init(void);

/* =============================================
   MAIN FUNCTION
   ============================================= */
int main(void) {
    System_Init();      
    
    // Initialize Component Modules
    OLED_Init();        // Handles I2C internally
    Bluetooth_Init();   // Handles USART1 internally
    ESP32_Init();       // Handles USART2 internally
    
    // Note: Keypad is just GPIO, initialized in System_Init or on the fly in scan

    OLED_ShowLocked(); 

    while(1) {
        // Visual Status
        if (bluetooth_enabled) {
            GPIOD->ODR |= PIN_STATUS; 
        } else {
            GPIOD->ODR &= ~PIN_STATUS; 
        }

        // Screen Updates (Triggered by ISRs in Bluetooth/ESP32 modules)
        if (update_screen) {
            if (bluetooth_enabled) {
                OLED_ShowWelcome(guest_name);
            } else {
                OLED_ShowLocked();
            }
            update_screen = 0; 
        }

        // Reset Button
        if (GPIOA->IDR & PIN_RESET) {
            bluetooth_enabled = 0;
            keyIdx = 0;
            memset(keyBuffer, 0, 5);
            memset(guest_name, 0, 21);
            OLED_ShowLocked();
            while(GPIOA->IDR & PIN_RESET);
            Delay_Rough(200000); 
        }

        // Keypad Logic
        char key = Scan_Keypad();
        if (key != 0) {
            if (key == 'C') { 
                keyIdx = 0; memset(keyBuffer, 0, 5); 
                OLED_ShowLocked();
            }
            else if (key != '*' && key != '#') {
                keyBuffer[keyIdx++] = key;
                char starStr[6] = {0};
                for(int i=0; i<keyIdx; i++) starStr[i] = '*';
                OLED_Clear();
                OLED_DrawString("ENTER PIN:", 10, 0);
                OLED_DrawString(starStr, 10, 2);
                OLED_Update();
                Delay_Rough(200000); 

                if (keyIdx >= 4) {
                    if (strncmp(keyBuffer, SYS_PIN, 4) == 0) {
                        Grant_Access();
                    } else {
                        Deny_Access();
                    }
                    keyIdx = 0; memset(keyBuffer, 0, 5);
                }
            }
        }
    }
}

/* =============================================
   RELAY & LOGIC FUNCTIONS
   ============================================= */
void Grant_Access(void) {
    OLED_ShowUnlocked(); 
    GPIOD->ODR &= ~PIN_RELAY; // Unlock (Active Low)
    Delay_Rough(3000000);     
    GPIOD->ODR |= PIN_RELAY;  // Lock
    memset(guest_name, 0, 21);
    OLED_ShowLocked(); 
}

void Deny_Access(void) {
    OLED_ShowError(); 
    for(int i=0; i<5; i++) {
        GPIOD->ODR |= PIN_ERROR; Delay_Rough(100000);
        GPIOD->ODR &= ~PIN_ERROR; Delay_Rough(100000);
    }
    OLED_ShowLocked(); 
}

void Delay_Rough(volatile int count) {
    while(count--) __NOP();
}

/* =============================================
   SYSTEM INITIALIZATION
   ============================================= */
void System_Init(void) {
    RCC->AHB1ENR |= (1<<0) | (1<<3) | (1<<4); 
    
    // Relay & LEDs (PD12, PD13, PD14)
    GPIOD->MODER &= ~( (3<<(12*2)) | (3<<(13*2)) | (3<<(14*2)) );
    GPIOD->MODER |=  ( (1<<(12*2)) | (1<<(13*2)) | (1<<(14*2)) );
    GPIOD->ODR |= PIN_RELAY; 
    GPIOD->ODR &= ~(PIN_STATUS | PIN_ERROR);
    
    // Button (PA0)
    GPIOA->MODER &= ~(3 << 0); 
    GPIOA->PUPDR &= ~(3 << 0); GPIOA->PUPDR |=  (2 << 0); 
    
    // Keypad Rows (Output)
    for(int i=7; i<=10; i++) {
        GPIOE->MODER &= ~(3<<(i*2)); GPIOE->MODER |= (1<<(i*2));
    }
    // Keypad Cols (Input)
    for(int i=11; i<=14; i++) {
        GPIOE->MODER &= ~(3<<(i*2)); 
        GPIOE->PUPDR &= ~(3<<(i*2)); GPIOE->PUPDR |= (1<<(i*2));
    }
}