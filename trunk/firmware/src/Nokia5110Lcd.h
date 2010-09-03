/*******************************************************
文件名  :   Nokia5110Lcd.h
描述    :   诺基亚 5110 LCD 驱动头文件
作者    :   xylch101
编辑器  :   Notepad 2
日期    :   2008年06月27日
*******************************************************/

#ifndef NOKIA_5110_LCD_H
#define NOKIA_5110_LCD_H

#include "global.h"

//宽高像素
#define LCD_X_RES                  84
#define LCD_Y_RES                  48

//管脚配置 (atmega16L) 自行修改
#define LCD_DC_PIN                 0x08  //  PB3
#define LCD_CE_PIN                 0x10  //  PB4
#define SPI_MOSI_PIN               0x20  //  PB5
#define LCD_RST_PIN                0x40  //  PB6
#define SPI_CLK_PIN                0x80  //  PB7

//typedef unsigned char              INT8U;
//typedef unsigned int               INT16U;

//枚举 D/C 模式选择 
typedef enum
{
    DC_CMD  = 0,    //写命令
    DC_DATA = 1     //写数据

} LcdExecType;

//枚举 画点类型选择 
typedef enum
{
    PIXEL_CLEAR =  0,   //擦除
    PIXEL_DRAW  =  1,   //画点
    PIXEL_XOR =  2      //取反

} LcdPixelMode;

//驱动函数声明
void LcdInit            ( void );
void LcdSetContrast     ( INT8U contrast );
void LcdSetXY           (INT8U X, INT8U Y);
void LcdExec            ( INT8U data, LcdExecType cd );
INT8U UnClear           ( INT8U data, INT8U n );
INT8U LnClear           ( INT8U data, INT8U n );
void SplitByte          ( INT8U data, INT8U nByLow, INT8U bytes[ 2 ] );
void BufferPixelStuff   ( INT8U type, INT8U index, INT8U x, INT8U y, INT8U width, INT8U height);
void CacheEnglishString ( INT8U x, INT8U y, INT8U width, INT8U height, INT8U bWidth, INT8U *dataPtr );
void CachePixel         ( INT8U x, INT8U y, LcdPixelMode mode );
void CommitBuffer       ( void );
void ClearBuffer        ( void );
void TestNOKIA5110      ( void );

#endif
