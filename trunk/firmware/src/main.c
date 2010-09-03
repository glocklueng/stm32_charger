/****************************************Copyright (c)**************************************************
**                              智 能 充 电 器 开 发 小 组
**                                      麦思网 论坛
**                                     QQ 群: 7095643
**
**                               http://www.mystm32.com/bbs
**
**--------------文件信息--------------------------------------------------------------------------------
**硬 件  版 本: ChargerV1.PCB
**文   件   名: main.c
**创   建   人: 安哥
**最后修改日期: 2009年09月29日 11:26
**描        述: 智能充电器
**
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 安哥
** 版  本: V0.01
** 日　期: 2009年09月18日 20:54
** 描　述: 原始版本
**
**------------------------------------------------------------------------------------------------------
**--------------历史版本修订------------------------------------------------------------------------------
** 修改人: 吕海安
** 版  本: V0.02
** 日　期: 2009年09月20日 22:58
** 描  述: 增加 ADC  驱动
**         增加 GPIO 驱动
**         增加 UART 驱动
**         增加 按键 驱动
**
**------------------------------------------------------------------------------------------------------
**--------------历史版本修订----------------------------------------------------------------------------
** 修改人: 吕海安
** 版  本: V0.02
** 日　期: 2009年09月22日 11:29
** 描  述: 完成所有底层, 除了 LCD 还没有调试完成
**
**------------------------------------------------------------------------------------------------------
**--------------当前版本修订----------------------------------------------------------------------------
** 修改人: 蟲子
** 版  本: V0.071
** 日　期: 2010年1月5日 11:29
** 描  述: 增加开机自动电压调零电流调零 增加10ms运行的一次的统计函数
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "global.h"
#include "stm32f10x.h"
#include "lcd5110.h"
#include "Nokia5110Lcd.h"
#include "para.h"
#include "charge.h"
#include "gpio.h"
#include "adc.h"
#include "logic.h"
#include "uart.h"
#include "pwm.h"
#include "eeprom.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

//ErrorStatus HSEStartUpStatus;

INT8U P500usReq;
volatile  INT32U  SoftTimer;
#define   TimerCnt    SysTick->VAL


/* Private function prototypes -----------------------------------------------*/
void  NVIC_Configuration(void);
int   fputc(int ch, FILE *f);
void InitGlobal(void);   //初始化全局参数

INT16U  timeu[50];             // 16 位定时器

// 上升延时定时器
INT8U ptimeu(INT8U no,INT8U dat,INT16U time_delay)
{
    if (dat == 0)
    {
        timeu[no] = 0;
        return 0;
    }
    else
    {
        if (timeu[no] >= time_delay) return 0xFF;
        else timeu[no]++;
    }
    return 0xFF;
}

//  下降延时定时器
INT8U ptimed(INT8U no,INT8U dat,INT16U time_delay)
{
    if (dat != 0)
    {
        timeu[no] = time_delay;
        return 0xFF;
    }
    else
    {
        if (timeu[no] > 0)
        {
            if (timeu[no] < time_delay) timeu[no]--;
            else timeu[no] = time_delay - 1;
        }
        else return 0;
    }
    return 0xFF;
}

//  周期定时器
INT8U ptime(INT8U no,INT16U period)
{
    if (timeu[no] > 0)
    {
        timeu[no]--;
        return 0;
    }
    else
    {
        timeu[no] = period;
        return 0xFF;
    }
}



// 延时单位时间由 SysTick 中断决定
void  SysDelay(INT32U DlyTime)
{
    INT32U  OldTimer;

    OldTimer = SoftTimer;
    while (SoftTimer - OldTimer < DlyTime);
}


void  P500us(void)
{
    static INT8U   P5ms = 0;
    static INT8U   P10ms = 0;

    if (P5ms < 9) P5ms++;
    else P5ms = 0;

    // 5ms 运行一次
    switch (P5ms)
    {
        case 1:
            Logic();
            break;

        case 2:
            if(P10ms)
            {
                P10ms = 0;
                Batt_Vol_Accumulate(0); // 10ms 计算一次电池电压等参数
            }
            else
            {
                P10ms = 1;
            }
            break;
        default:
            break;
    }


}


int main(void)
{

    SystemInit();                                      // 系统时钟

    NVIC_Configuration();                              // 中断设置

    InitGpio();                                        // IO 端口设置

    InitPID();                                         // PID 初始化

    SysTick_Config( SystemFrequency/2000 );            // 定时器初始化

    InitUart();                                        // 串口初始化

    InitADC();                                         // AD 转换初始化

    InitPWM();                                         // PWM 初始化

    InitGlobal();                                      // 初始化全局参数

    LcdBacklightOn();

    SysDelay(1000);

    SPI_LCD_Init();
    SysDelay(50);
    LCD5110_Init();
    LCD5110_CLS();
    //ShowLOGO();

    //TestNOKIA5110();


    /* Unlock the Flash Program Erase controller */
    FLASH_Unlock();

    /* EEPROM Init */
    EE_Init();

    InitPara();             // 初始化 EEPROM 参数

    //调零电压档
    Adjust_Battery_vol(1);

    //调零电流档
    Adjust_Battery_Cur(1);



    Led1Off();
    Led2Off();
    Led3Off();
    Led4Off();

    ChargerPrintf("\r\n ChargerV1 \r\n");
    ChargerPrintf("\r\n www.mystm32.com/bbs \r\n");
    ChargerPrintf("\r\n ========================= \r\n");
    ChargerPrintf("\r\n                           \r\n");


    Led1On();
    //	SysDelay(1000);
    Led2On();
//		SysDelay(1000);
    Led3On();
    //	SysDelay(1000);
    Led4On();
//		SysDelay(1000);



    while (1)
    {

        if (P500usReq)
        {
            P500usReq = 0;
            P500us();
        }
    }
}

void InitGlobal(void)   // 初始化全局参数
{
    InitChargePara();
}

void InitPara(void)     // 初始化 EEPROM 参数
{
    INT16U EepromFlag = 0;

    EE_ReadVariable(VirtAddVarTab[7+4], &EepromFlag);
    if(EepromFlag != 0x55AA)
    {
        EepromFlag = 0x55AA;
        EE_WriteVariable(VirtAddVarTab[7+4], EepromFlag);
        EE_WriteVariable(VirtAddVarTab[0+4], batt0_capacity);
        EE_WriteVariable(VirtAddVarTab[1+4], batt1_capacity);
        EE_WriteVariable(VirtAddVarTab[2+4], ModeLED);
        EE_WriteVariable(VirtAddVarTab[3+4], ModeLCDLight);
        EE_WriteVariable(VirtAddVarTab[4+4], ModeLaguage);
        EE_WriteVariable(VirtAddVarTab[5+4], ModeLCDContrast);
        EE_WriteVariable(VirtAddVarTab[6+4], ModeLCDShow);
    }
    else
    {
        EE_ReadVariable(VirtAddVarTab[0+4], &batt0_capacity);
        EE_ReadVariable(VirtAddVarTab[1+4], &batt1_capacity);
        EE_ReadVariable(VirtAddVarTab[2+4], &ModeLED);
        EE_ReadVariable(VirtAddVarTab[3+4], &ModeLCDLight);
        EE_ReadVariable(VirtAddVarTab[4+4], &ModeLaguage);
        EE_ReadVariable(VirtAddVarTab[5+4], &ModeLCDContrast);
        EE_ReadVariable(VirtAddVarTab[6+4], &ModeLCDShow);
    }
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{

#ifdef  VECT_TAB_RAM
    // Set the Vector Table base location at 0x20000000
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  // VECT_TAB_FLASH
    // Set the Vector Table base location at 0x08000000
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif

    // Configure the NVIC Preemption Priority Bits
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
}


#ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{
    // User can add his own implementation to report the file name and line number,
    // ex: ChargerPrintf("Wrong parameters value: file %s on line %d\r\n", file, line)

    ChargerPrintf("\n\r Wrong parameter value detected on\r\n");
    ChargerPrintf("       file  %s\r\n", file);
    ChargerPrintf("       line  %d\r\n", line);

    /* Infinite loop */
    while (1)
    {
    }
}
#endif


/*******************************************************************************
* Function Name  : fputc
* Description    : Retargets the C library ChargerPrintf function to the USART.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int fputc(int ch, FILE *f)
{
    /* Loop until the end of transmission */
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    USART_SendData(USART1, (u8) ch);

    return ch;
}

/**************************************************************************************×*****************
**                                 END OF FILE
********************************************************************************************************/
