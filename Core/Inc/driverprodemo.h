#ifndef  __driverprodemo_H_
#define __driverprodemo_H_

#include "sys.h"
#define WRITE_CMD	0xF8
#define WRITE_DAT	0xFA

#define LCD_CS_L        0
#define LCD_CS_H        1

#define SID PBout(5)
#define SCLK PBout(3)
#define CS   PDout(2)

void lcd_GPIO_init(void);
void Lcd_Init(void);

void SendByte(uint8_t Dbyte);
void Lcd_Clear(void);
void LCD_Display_Words(uint8_t x,uint8_t y,uint8_t*str);

void Lcd_WriteData(uint8_t Dat );
void Lcd_WriteCmd(uint8_t Cmd );

void Lcd_SetLocation(unsigned char x,unsigned char y,char data);



void LCD_SetLoca(uint8_t X, uint8_t Y);
void LCD_CS(uint8_t Status);



#endif