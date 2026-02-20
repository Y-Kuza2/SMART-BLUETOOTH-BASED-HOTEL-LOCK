#ifndef OLED_H
#define OLED_H

void OLED_Init(void);
void OLED_ShowLocked(void);
void OLED_ShowUnlocked(void);
void OLED_ShowError(void);
void OLED_ShowWelcome(char* name);
void OLED_DrawString(char* str, int x, int y);
void OLED_Clear(void);
void OLED_Update(void);

#endif