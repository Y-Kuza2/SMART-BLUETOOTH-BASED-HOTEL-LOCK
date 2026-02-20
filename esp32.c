#include "esp32.h"
#include "system_config.h"

// Initialize USART2 for ESP32
void ESP32_Init(void) {
    RCC->APB1ENR |= (1 << 17); // Enable USART2 Clock
    
    // Configure PA2 (TX) and PA3 (RX)
    GPIOA->MODER &= ~( (3<<(2*2)) | (3<<(3*2)) );
    GPIOA->MODER |=  ( (2<<(2*2)) | (2<<(3*2)) );
    GPIOA->AFR[0] &= ~( (0xF<<8) | (0xF<<12) );
    GPIOA->AFR[0] |=  ( (7<<8)   | (7<<12)   ); 
    
    // Baud Rate 9600
    USART2->BRR = 0x0683; 
    USART2->CR1 = (1<<13) | (1<<5) | (1<<2) | (1<<3); 
    
    NVIC_EnableIRQ(USART2_IRQn);
}

// ISR for ESP32
void USART2_IRQHandler(void) {
    if (USART2->SR & (1 << 5)) { 
        char rx = USART2->DR;
        if (rx == CMD_START_NAME) { 
            receiving_name = 1;
            name_idx = 0;
            memset(guest_name, 0, 21);
        }
        else if (receiving_name) {
            if (rx == '\n' || rx == '\r') {
                receiving_name = 0; 
                bluetooth_enabled = 1; 
                update_screen = 1; 
            }
            else if (name_idx < 20) {
                guest_name[name_idx++] = rx;
            }
        }
        else {
            if (rx == CMD_ENABLE_SYS) {
                bluetooth_enabled = 1; 
                guest_name[0] = '\0'; 
                update_screen = 1;
            }
            else if (rx == CMD_DISABLE_SYS) {
                bluetooth_enabled = 0; 
                update_screen = 1; 
            }
        }
    }
}