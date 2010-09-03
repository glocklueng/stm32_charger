/****************************************Copyright (c)**************************************************
**                              智 能 充 电 器 开 发 小 组
**                                      麦思网 论坛
**                                     QQ 群: 7095643
**
**                               http://www.mystm32.com/bbs
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: lcd5110.h
**创   建   人: 蟲子
**最后修改日期: 2009年09月21日 11:26
**描        述: 智能充电器
**
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 蟲子
** 版  本: v0.01
** 日　期: 2009年9月20日
** 描　述: 原始版本
**
**--------------当前版本信息----------------------------------------------------------------------------
** 创建人: 蟲子
** 版  本: v0.03
** 日　期: 2009年9月21日
** 描　述: 当前版本
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _USER_SPI_H
#define _USER_SPI_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "global.h"

#define GPIO_DC                  GPIOB
#define RCC_APB2Periph_GPIO_DC   RCC_APB2Periph_GPIOB
#define GPIO_Pin_DC              GPIO_Pin_14

#define GPIO_RST                  GPIOB
#define RCC_APB2Periph_GPIO_RST   RCC_APB2Periph_GPIOB
#define GPIO_Pin_RST              GPIO_Pin_11

#define GPIO_CS                   GPIOB
#define RCC_APB2Periph_GPIO_CS   RCC_APB2Periph_GPIOB
#define GPIO_Pin_CS              GPIO_Pin_12
#define lcd_cs_0()               GPIO_ResetBits(GPIO_CS,GPIO_Pin_CS)
#define lcd_cs_1()               GPIO_SetBits(GPIO_CS,GPIO_Pin_CS)

/* Exported macro ------------------------------------------------------------*/
// 拉低DC引脚 发送命令
#define LCD5110_SEND_CMD()       GPIO_ResetBits(GPIO_DC, GPIO_Pin_DC)
// 拉高DC引脚 发送数据
#define LCD5110_SEND_DATA()      GPIO_SetBits(GPIO_DC, GPIO_Pin_DC)

//使用PE7引脚作为LCD5110的RST引脚
#define LCD5110_RST_LOW()       GPIO_ResetBits(GPIO_RST, GPIO_Pin_RST)
#define LCD5110_RST_HIGH()      GPIO_SetBits(GPIO_RST, GPIO_Pin_RST)

#define LCD_BLANK  "              "

#define DBufSize    100

/* Exported functions ------------------------------------------------------- */
/*----- High layer function -----*/
void SPI_LCD_Init(void);
void LCD5110_Init(void);
void LCD5110_CLS(void);
void LCD_GOTOXY( u8 x , u8 y );
void LCD5110_PUT_ASCII(u8 ASCII);
void LCD5110_PUT_STR(u8 *string , u8 n);

void TextOut16(INT16U x,INT16U y,INT8S *ch);
void display_str(INT16U x,INT16U y,INT8S *text);
void set_cursor(INT8U x,INT8U y);
void sprfu(INT8S *buf,INT32U num,INT8U dot,INT8U len);
void sprfi(INT8S *buf,INT16U num,INT8U dot,INT8U len);

void ShowLOGO(void); //演示使用
void LCD_ShowChargeStatus(void);

/*----- Low layer function -----*/
u8 SPI_ReadByte(void);
u8 SPI2_SendByte(u8 byte);

#endif /* _USER_SPI_H */

