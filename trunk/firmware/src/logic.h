#ifndef	_LOGIC__H
#define	_LOGIC__H

#include "global.h"

#define AllOff           0
#define RedOn            1
#define GreenOn          2
#define RedFlash10HZ     3
#define GreenFlash10HZ   4
#define RedFlash20HZ     5
#define GreenFlash20HZ   6
#define RedFlash2HZ      7
#define GreenFlash2HZ    8
#define OrangeOn         9

// 电池各个参数 每秒累积平均
extern  INT16U Batt0_Vol_Seconds ;
extern  INT16U Batt0_Cur_Seconds ;
extern  INT16U Batt0_Temperature_Seconds ;
extern  INT16U Batt1_Vol_Seconds ;
extern  INT16U Batt1_Cur_Seconds ;
extern  INT16U Batt1_Temperature_Seconds ;
extern  INT16U STM32_vref_Seconds ;
extern  INT16U STM32_Temperature_Seconds ;

extern  INT8U Led0ChargeStat;
extern  INT8U Led1ChargeStat;

extern  INT8U StatBLWake;  // 背光睡眠标志  1:睡眠  0:正常

void Capacity_Count(void);
void Uart_Send_Batt_Statue(void);
void Logic(void);
void Batt_Vol_Accumulate(INT8U erase);


#endif
