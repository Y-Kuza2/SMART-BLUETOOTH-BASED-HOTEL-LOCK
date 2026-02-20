#include "bluetooth.h"
#include "system_config.h"

// Initialize USART1 for HC-05
void Bluetooth_Init(void) {
    RCC->APB2ENR |= (1 << 4); // Enable USART1 Clock
    
    // Configure PA9 (TX) and PA10 (RX)
    GPIOA->MODER &= ~( (3<<(9*2)) | (3<<(10*2)) );
    GPIOA->MODER |=  ( (2<<(9*2)) | (2<<(10*2)) );
    GPIOA->AFR[1] &= ~( (0xF<<4) | (0xF<<8) ); 
    GPIOA->AFR[1] |=  ( (7<<4)   | (7<<8)   ); 
    
    // Baud Rate 9600 @ 16MHz
    USART1->BRR = 0x0683; 
    // Enable USART, RX, TX, RX Interrupt
    USART1->CR1 = (1<<13) | (1<<5) | (1<<2) | (1<<3); 
    
    NVIC_EnableIRQ(USART1_IRQn);
}

// ISR for HC-05
void USART1_IRQHandler(void) {
    if (USART1->SR & (1 << 5)) { 
        char rx = USART1->DR;
        
        // Check for APP_CMD_UNLOCK ('1') and if system is enabled
        if (rx == APP_CMD_UNLOCK && bluetooth_enabled == 1) {
            Grant_Access();
        }
        // Basic error handling/noise rejection
        else if (rx != '\n' && rx != '\r' && rx != '0') {
             Deny_Access();
        }
    }
}