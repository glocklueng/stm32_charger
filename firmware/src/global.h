#ifndef _GLOBAL__H
#define _GLOBAL__H

#ifdef DEBUG

#define _TIM2
#define _TIM3
#define _TIM4
#define _TIM5
#define _TIM6
#define _TIM7
#define _RTC
#define _WWDG
#define _IWDG
#define _SPI2
#define _SPI3
#define _USART2
#define _USART3
#define _UART4
#define _UART5
#define _I2C1
#define _I2C2
#define _CAN
#define _BKP
#define _PWR
#define _DAC
#define _AFIO
#define _EXTI
#define _GPIOA
#define _GPIOB
#define _GPIOC
#define _GPIOD
#define _GPIOE
#define _GPIOF
#define _GPIOG
#define _ADC1
#define _ADC2
#define _TIM1
#define _SPI1
#define _TIM8
#define _USART1
#define _ADC3
#define _SDIO
#define _DMA
#define _DMA1_Channel1
#define _DMA1_Channel2
#define _DMA1_Channel3
#define _DMA1_Channel4
#define _DMA1_Channel5
#define _DMA1_Channel6
#define _DMA1_Channel7
#define _DMA2_Channel1
#define _DMA2_Channel2
#define _DMA2_Channel3
#define _DMA2_Channel4
#define _DMA2_Channel5
#define _RCC
#define _CRC
#define _FLASH
#define _FSMC
#define _DBGMCU
#define _SysTick
#define _NVIC

#endif // DEBUG

#include "stdint.h"
#include "PID.h"
//#include "stm32_dsp.h"
#include <stdio.h>

#define GP0                 ((u16)0x0001)  /* Pin 0 selected */
#define GP1                 ((u16)0x0002)  /* Pin 1 selected */
#define GP2                 ((u16)0x0004)  /* Pin 2 selected */
#define GP3                 ((u16)0x0008)  /* Pin 3 selected */
#define GP4                 ((u16)0x0010)  /* Pin 4 selected */
#define GP5                 ((u16)0x0020)  /* Pin 5 selected */
#define GP6                 ((u16)0x0040)  /* Pin 6 selected */
#define GP7                 ((u16)0x0080)  /* Pin 7 selected */
#define GP8                 ((u16)0x0100)  /* Pin 8 selected */
#define GP9                 ((u16)0x0200)  /* Pin 9 selected */
#define GP10                ((u16)0x0400)  /* Pin 10 selected */
#define GP11                ((u16)0x0800)  /* Pin 11 selected */
#define GP12                ((u16)0x1000)  /* Pin 12 selected */
#define GP13                ((u16)0x2000)  /* Pin 13 selected */
#define GP14                ((u16)0x4000)  /* Pin 14 selected */
#define GP15                ((u16)0x8000)  /* Pin 15 selected */
#define GPAll               ((u16)0xFFFF)  /* All pins selected */

#define Led3Off()              GPIOA->BSRR |= GP8
#define Led3On()               GPIOA->BRR  |= GP8
#define Led3Flash()            GPIOA->ODR  ^= GP8

#define Led4Off()              GPIOB->BSRR |= GP6
#define Led4On()               GPIOB->BRR  |= GP6
#define Led4Flash()            GPIOB->ODR  ^= GP6

#define Led1Off()              GPIOB->BSRR |= GP2
#define Led1On()               GPIOB->BRR  |= GP2
#define Led1Flash()            GPIOB->ODR  ^= GP2

#define Led2Off()              GPIOB->BSRR |= GP10
#define Led2On()               GPIOB->BRR  |= GP10
#define Led2Flash()            GPIOB->ODR  ^= GP10

#define LcdBacklightOn()       GPIOB->BSRR |= GP9
#define LcdBacklightOff()      GPIOB->BRR  |= GP9
#define LcdBacklightFlash()    GPIOB->ODR  ^= GP9

typedef unsigned char   INT8U;
typedef signed   char   INT8S;
typedef unsigned short  INT16U;
typedef signed   short  INT16S;
typedef unsigned int    INT32U;
typedef signed   int    INT32S;
typedef          int    INT32;
typedef unsigned long long       INT64U;
typedef signed   long long       INT64S;
typedef float                    FP32;
typedef double                   FP64;


// main.C 主函数
extern   volatile INT32U  SoftTimer;
extern   INT8U P500usReq;

extern   INT16U  timeu[50];             // 16 位定时器
extern   INT8U ptimeu(INT8U no,INT8U dat,INT16U time_delay);
extern   INT8U ptimed(INT8U no,INT8U dat,INT16U time_delay);
extern   INT8U ptimepw(INT8U no,INT8U dat,INT16U pw);
extern   INT8U ptime(INT8U no,INT16U period);


extern void  SysDelay(INT32U DlyTime);
#define  ChargerPrintf   printf         // 使用 printf 函数

extern INT16U batt0_capacity;
extern INT16U batt1_capacity;
extern INT16U ModeLCDLight;    // LCD 背光模式   0: 省电模式; 1: 测试模式,背光常亮
extern INT16U ModeLED;         // LED 的红绿灯脚位配置, 在 LED 脚位焊接错误后设置
extern INT16U ModeLaguage;     // 系统语言  0: 中文;  1: 英文
extern INT16U ModeLCDContrast; // LCD 的对比度设置
extern INT16U ModeLCDShow;     // LCD 在充电时显示模式  0: 静态模式  1: 状态自动翻页

extern void InitPara(void);   // 初始化 EEPROM 参数





#endif // Charger

