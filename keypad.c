#include "keypad.h"
#include "stm32f4xx.h"

char keyMap[4][4] = {
    {'1', '2', '3', 'A'}, 
    {'4', '5', '6', 'B'}, 
    {'7', '8', '9', 'C'}, 
    {'*', '0', '#', 'D'}
};

char Scan_Keypad(void) {
    for(int r = 0; r < 4; r++) {
        GPIOE->ODR |= (0x0F << 7);      
        GPIOE->ODR &= ~(1 << (7 + r)); 
        for(int c = 0; c < 4; c++) {
            int colPin = 11 + c; 
            if ( ! (GPIOE->IDR & (1 << colPin)) ) {
                while( ! (GPIOE->IDR & (1 << colPin)) ); 
                return keyMap[r][c];
            }
        }
    }
    return 0;
}