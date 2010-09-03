#ifndef	_KEY__H
#define	_KEY__H

#include "global.h"

// Key.C 按键
//***********************************************
#define     KEY_ESC          4
#define     KEY_ENTER        5
#define     KEY_LEFT         7
#define     KEY_RIGHT        6
#define     KEY_UP           9
#define     KEY_DN           8
#define     WAKE_UP_BL       99
//************************************************


#define     Row0_0            GPIOB->BRR  |= GP8
#define     Row0_1            GPIOB->BSRR |= GP8
#define     Row1_0            GPIOB->BRR  |= GP5
#define     Row1_1            GPIOB->BSRR |= GP5

#define     Col0              GPIOC->IDR & GP13
#define     Col1              GPIOC->IDR & GP14
#define     Col2              GPIOC->IDR & GP15

extern INT8U state_ptr,bit_ptr;
extern INT8U menu_ptr; //菜单指针
extern INT8U para_ptr; //参数指针

void Delay(INT32U nCount);
void ChargeFuncExit(void);
INT8U ReadKey(void);
void process_key(INT8U keycode);


#endif
