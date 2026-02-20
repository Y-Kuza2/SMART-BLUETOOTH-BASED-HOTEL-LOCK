#include "oled.h"
#include "system_config.h"
#include "stm32f4xx.h"

uint8_t OLED_Buffer[1024]; 

/* =============================================
   INTERNAL I2C FUNCTIONS
   ============================================= */
void Internal_I2C_Init(void) {
    RCC->AHB1ENR |= (1 << 1); // GPIOB Clock
    RCC->APB1ENR |= (1 << 21); // I2C1 Clock
    
    // PB6 (SCL), PB7 (SDA)
    GPIOB->MODER &= ~((3 << 12) | (3 << 14));
    GPIOB->MODER |=  ((2 << 12) | (2 << 14)); 
    GPIOB->OTYPER |= ((1 << 6)  | (1 << 7));  
    GPIOB->OSPEEDR|= ((3 << 12) | (3 << 14)); 
    GPIOB->PUPDR  |= ((1 << 12) | (1 << 14)); 
    GPIOB->AFR[0] &= ~((0xF << 24) | (0xF << 28));
    GPIOB->AFR[0] |=  ((4 << 24)   | (4 << 28));
    
    I2C1->CR1 |= (1 << 15); // Reset I2C
    I2C1->CR1 &= ~(1 << 15);
    I2C1->CR2 = 16; 
    I2C1->CCR = 80; 
    I2C1->TRISE = 17; 
    I2C1->CR1 |= 1; // Enable I2C
}

void Internal_I2C_Write(uint8_t addr, uint8_t* data, int len) {
    I2C1->CR1 |= (1 << 8); // Start
    while(!(I2C1->SR1 & (1 << 0))); 
    I2C1->DR = addr;
    while(!(I2C1->SR1 & (1 << 1)));
    (void)I2C1->SR2; 
    for(int i=0; i<len; i++) {
        while(!(I2C1->SR1 & (1 << 7))); 
        I2C1->DR = data[i];
    }
    while(!(I2C1->SR1 & (1 << 7)));
    while(!(I2C1->SR1 & (1 << 2))); 
    I2C1->CR1 |= (1 << 9); // Stop
}

void OLED_Command(uint8_t cmd) {
    uint8_t data[2] = {0x00, cmd};
    Internal_I2C_Write(OLED_ADDR, data, 2);
}

/* =============================================
   OLED LOGIC
   ============================================= */
// Font Data 
const uint8_t Font5x7[][5] = {
    {0x00,0x00,0x00,0x00,0x00}, // SPACE
    {0x7E,0x11,0x11,0x11,0x7E}, // A
    {0x7F,0x49,0x49,0x49,0x36}, // B
    {0x3E,0x41,0x41,0x41,0x22}, // C
    {0x7F,0x41,0x41,0x22,0x1C}, // D
    {0x7F,0x49,0x49,0x49,0x41}, // E
    {0x7F,0x09,0x09,0x09,0x01}, // F
    {0x3E,0x41,0x49,0x49,0x7A}, // G
    {0x7F,0x08,0x08,0x08,0x7F}, // H
    {0x00,0x41,0x7F,0x41,0x00}, // I
    {0x20,0x40,0x41,0x3F,0x01}, // J
    {0x7F,0x08,0x14,0x22,0x41}, // K
    {0x7F,0x40,0x40,0x40,0x40}, // L
    {0x7F,0x02,0x0C,0x02,0x7F}, // M
    {0x7F,0x04,0x08,0x10,0x7F}, // N
    {0x3E,0x41,0x41,0x41,0x3E}, // O
    {0x7F,0x09,0x09,0x09,0x06}, // P
    {0x3E,0x41,0x51,0x21,0x5E}, // Q
    {0x7F,0x09,0x19,0x29,0x46}, // R
    {0x46,0x49,0x49,0x49,0x31}, // S
    {0x01,0x01,0x7F,0x01,0x01}, // T
    {0x3F,0x40,0x40,0x40,0x3F}, // U
    {0x1F,0x20,0x40,0x20,0x1F}, // V
    {0x3F,0x40,0x38,0x40,0x3F}, // W
    {0x63,0x14,0x08,0x14,0x63}, // X
    {0x07,0x08,0x70,0x08,0x07}, // Y
    {0x61,0x51,0x49,0x45,0x43}, // Z
    {0x3E,0x51,0x49,0x45,0x3E}, // 0
    {0x00,0x42,0x7F,0x40,0x00}, // 1
    {0x42,0x61,0x51,0x49,0x46}, // 2
    {0x21,0x41,0x45,0x4B,0x31}, // 3
    {0x18,0x14,0x12,0x7F,0x10}, // 4
    {0x27,0x45,0x45,0x45,0x39}, // 5
    {0x3C,0x4A,0x49,0x49,0x30}, // 6
    {0x01,0x71,0x09,0x05,0x03}, // 7
    {0x36,0x49,0x49,0x49,0x36}, // 8
    {0x06,0x49,0x49,0x29,0x1E}, // 9
};

void OLED_Init(void) {
    Internal_I2C_Init(); // Initialize the embedded I2C driver
    Delay_Rough(1000000); 
    OLED_Command(0xAE); OLED_Command(0x20); OLED_Command(0x00); 
    OLED_Command(0xB0); OLED_Command(0xC8); OLED_Command(0x00); 
    OLED_Command(0x10); OLED_Command(0x40); OLED_Command(0x81); 
    OLED_Command(0xFF); OLED_Command(0xA1); OLED_Command(0xA6); 
    OLED_Command(0xA8); OLED_Command(0x3F); OLED_Command(0xA4); 
    OLED_Command(0xD3); OLED_Command(0x00); OLED_Command(0xD5); 
    OLED_Command(0xF0); OLED_Command(0xD9); OLED_Command(0x22); 
    OLED_Command(0xDA); OLED_Command(0x12); OLED_Command(0xDB); 
    OLED_Command(0x20); OLED_Command(0x8D); OLED_Command(0x14); 
    OLED_Command(0xAF); 
    OLED_Clear(); OLED_Update();
}

void OLED_Clear(void) {
    memset(OLED_Buffer, 0x00, 1024);
}

void OLED_Update(void) {
    OLED_Command(0x21); OLED_Command(0); OLED_Command(127); 
    OLED_Command(0x22); OLED_Command(0); OLED_Command(7);   
    for(int i=0; i<1024; i++) {
         uint8_t byte = OLED_Buffer[i];
         uint8_t data[2] = {0x40, byte};
         Internal_I2C_Write(OLED_ADDR, data, 2);
    }
}

void OLED_DrawChar(char c, int x, int y) {
    if(c >= 'a' && c <= 'z') c -= 32; 
    int charIndex = 0;
    if(c == ' ') charIndex = 0;
    else if(c >= 'A' && c <= 'Z') charIndex = c - 'A' + 1;
    else if(c >= '0' && c <= '9') charIndex = c - '0' + 27;
    else return; 
    for (int i = 0; i < 5; i++) {
        int bufIdx = x + i + (y * 128);
        if(bufIdx < 1024) OLED_Buffer[bufIdx] = Font5x7[charIndex][i];
    }
}

void OLED_DrawString(char* str, int x, int y) {
    int cursorX = x;
    while (*str) {
        OLED_DrawChar(*str++, cursorX, y);
        cursorX += 6; 
        if(cursorX > 120) break;
    }
}

void OLED_ShowLocked(void) {
    OLED_Clear();
    OLED_DrawString("SYSTEM LOCKED", 10, 2);
    OLED_DrawString("WAITING...", 10, 4);
    OLED_Update();
}

void OLED_ShowUnlocked(void) {
    OLED_Clear();
    OLED_DrawString("ACCESS GRANTED", 10, 2);
    OLED_DrawString("DOOR OPEN", 10, 4);
    OLED_Update();
}

void OLED_ShowError(void) {
    OLED_Clear();
    OLED_DrawString("ACCESS DENIED", 10, 2);
    OLED_DrawString("WRONG INPUT", 10, 4);
    OLED_Update();
}

void OLED_ShowWelcome(char* name) {
    OLED_Clear();
    OLED_DrawString("WELCOME", 10, 2);
    if (name[0] != '\0') OLED_DrawString(name, 10, 4);
    OLED_Update();
}