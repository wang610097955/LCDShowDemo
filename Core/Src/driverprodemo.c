#include "gpio.h"
#include "stdint.h"
#include "driverprodemo.h"
#include "sys.h"

#define WRITE_CMD	0xF8
#define WRITE_DATA	0xFA

#define SID  PBout(5)
#define SCLK PBout(3)
#define CS   PDout(2)


void LCD_RST();
void Lcd_ClearGRAM(void);

void Delayus(uint32_t us){
  while(us--);
}

void SendByte(uint8_t byte)
{  
  uint8_t i;
  for (i = 0; i < 8; i++)
  {
    if ((byte<<i)&0x80)
    {
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
    }
    else
    {
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
    }
    
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
    Delayus(100);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);    
    Delayus(100);
  }
}

void Lcd_WriteCmd(uint8_t Cmd)
{  
  LCD_CS(LCD_CS_H);
  SendByte(WRITE_CMD);
  SendByte(0xf0&Cmd);
  SendByte((Cmd<<4)&0xf0);
  LCD_CS(LCD_CS_H);
}

void Lcd_WriteData(uint8_t data)
{
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
  SendByte(WRITE_DATA); 
  SendByte(0xf0&data);
  SendByte((data<<4)&0xf0);
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
}

void Lcd_SetLocation(unsigned char x,unsigned char y,char data)
{
  if(y==0x00)
    x+=0x80;
  if(y==0x01)
    x+=0x90;
  if(y==0x02)
    x+=0x88;
  if(y==0x03)
    x+0x98;
  Lcd_WriteCmd(x);
  Lcd_WriteData(data);
}

void Lcd_WriteStr(unsigned char addr_x,unsigned char addr_y,unsigned char *str)
{
//  Lcd_SetLocation(addr_x,addr_y);
  while(*str != 0x00)
  {
    Lcd_WriteData(*str++);
  }
}


uint8_t LCD_addr[4][8]={
  {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87},  		//第一行
  {0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97},		//第二行
  {0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F},		//第三行
  {0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F}		//第四行
};

uint8_t const num_lcd[]={"0123456789 :.-"};

void LCD_Display_Number(uint8_t Num)
{  
  Lcd_WriteData(num_lcd[Num]);
  HAL_Delay(1);
}

void LCD_SetLoca(uint8_t X, uint8_t Y)
{
  uint8_t address;
  switch(X)
  {
  case 0: 
    address = 0x80 + Y; 
    break;    
  case 1: 
    address = 0x80 + Y; 
    break; 
  case 2: 
    address = 0x90 + Y; 
    break; 
  case 3: 
    address = 0x88 + Y; 
    break;
  case 4: 
    address = 0x98 + Y; 
    break; 
  default:
    address = 0x80 + Y; 
    break;
  }
  Lcd_WriteCmd(address);
}

void LCD_Display_Words(uint8_t x,uint8_t y,uint8_t*str)
{ 
  Lcd_WriteCmd(LCD_addr[x][y]); //写初始光标位置
  while(*str>0)
  { 
    Lcd_WriteData(*str);    //写数据
    str++;     
  }
}

void Lcd_Init(void)
{

  LCD_RST();
  
#if(1)
  
  Lcd_WriteCmd(0x30);         //一次送八位数据
  Lcd_WriteCmd(0x0c);         //整体显示，游标关
  Lcd_WriteCmd(0x01);         // clear
#else 
  Lcd_WriteCmd(0x30);         //一次送八位数据
  Lcd_WriteCmd(0x0c);         //整体显示，游标关
  Lcd_WriteCmd(0x01);         // clear
  Lcd_WriteCmd(0x02);         //DRAM 地址归位
  Lcd_WriteCmd(0x80);         //设定DRAM 7位地址000 0000到地址计数器AC
  Lcd_WriteCmd(0x04);         //点设定，显示字符/光标从左到右移位，DRAM地址+1
//  Lcd_WriteCmd(0x0f);         //显示设定，开显示，显示光标，当前显示位反白闪动
#endif
}

void Lcd_Clear(void)
{
  Lcd_WriteCmd(0x01);
  HAL_Delay(10);
}

void LCD12864_choose(unsigned char x,unsigned char y)
{
    switch(y)
    {
        case 1:
            Lcd_WriteCmd(0x80+x);
        break;
        case 2:
            Lcd_WriteCmd(0x90+x);        
        break;
        case 3:
            Lcd_WriteCmd(0x88+x);        
        break;
        case 4:
            Lcd_WriteCmd(0x98+x);        
        break;
        default:
            Lcd_WriteCmd(0x82);        
        break;
    }
}

void LCD12864_show(unsigned char *str)
{
    while(*str!='\0')
    {
        Lcd_WriteData(*str);
        str++;
    }
}

void Lcd_DrawPicture(const unsigned char *ptr)
{
  unsigned int i,j,k;
  if(ptr==(void*)0)   //强制转换成空字符NULL
    Lcd_Clear();
  Lcd_WriteCmd(0x34);      //打开扩展指令集
  for(i=0x80,j=0;j<32;j++)
  {
    Lcd_WriteCmd(i++);
    Lcd_WriteCmd(0x80);
    for(k=0;k<16;k++)
    {
      if(ptr==(void*)0)
        Lcd_WriteData(*ptr++);
      else
        Lcd_WriteCmd(0x00);
    }
  }
  for(i=0x80,j=0;j<32;j++)
  {
    Lcd_WriteCmd(i++);
    Lcd_WriteCmd(0x88);
    for(k=0;k<16;k++)
    {
      if(ptr==(void*)0)
        Lcd_WriteData(*ptr++);
      else
        Lcd_WriteCmd(0x00);
    }
  }
  Lcd_WriteCmd(0x36); //打开扩充指令集动作，打开绘图显示Off
  Lcd_WriteCmd(0x30); //回到基本指令集
}

void Lcd_ClearGRAM(void)
{
  Lcd_DrawPicture((void*)0);
}

void LCD_RST()
{
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
  HAL_Delay(5);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
  HAL_Delay(5);
}

void LCD_CS(uint8_t Status)
{
  if(Status == 0)
  {    
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
  }else{
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
  }
}