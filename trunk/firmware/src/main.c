/****************************************Copyright (c)**************************************************
**                              �� �� �� �� �� �� �� С ��
**                                      ��˼�� ��̳
**                                     QQ Ⱥ: 7095643
**
**                               http://www.mystm32.com/bbs
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**Ӳ ��  �� ��: ChargerV1.PCB
**��   ��   ��: main.c
**��   ��   ��: ����
**����޸�����: 2009��09��29�� 11:26
**��        ��: ���ܳ����
**
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: ����
** ��  ��: V0.01
** �ա���: 2009��09��18�� 20:54
** �衡��: ԭʼ�汾
**
**------------------------------------------------------------------------------------------------------
**--------------��ʷ�汾�޶�------------------------------------------------------------------------------
** �޸���: ������
** ��  ��: V0.02
** �ա���: 2009��09��20�� 22:58
** ��  ��: ���� ADC  ����
**         ���� GPIO ����
**         ���� UART ����
**         ���� ���� ����
**
**------------------------------------------------------------------------------------------------------
**--------------��ʷ�汾�޶�----------------------------------------------------------------------------
** �޸���: ������
** ��  ��: V0.02
** �ա���: 2009��09��22�� 11:29
** ��  ��: ������еײ�, ���� LCD ��û�е������
**
**------------------------------------------------------------------------------------------------------
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: �x��
** ��  ��: V0.071
** �ա���: 2010��1��5�� 11:29
** ��  ��: ���ӿ����Զ���ѹ����������� ����10ms���е�һ�ε�ͳ�ƺ���
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
void InitGlobal(void);   //��ʼ��ȫ�ֲ���

INT16U  timeu[50];             // 16 λ��ʱ��

// ������ʱ��ʱ��
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

//  �½���ʱ��ʱ��
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

//  ���ڶ�ʱ��
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



// ��ʱ��λʱ���� SysTick �жϾ���
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

    // 5ms ����һ��
    switch (P5ms)
    {
        case 1:
            Logic();
            break;

        case 2:
            if(P10ms)
            {
                P10ms = 0;
                Batt_Vol_Accumulate(0); // 10ms ����һ�ε�ص�ѹ�Ȳ���
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

    SystemInit();                                      // ϵͳʱ��

    NVIC_Configuration();                              // �ж�����

    InitGpio();                                        // IO �˿�����

    InitPID();                                         // PID ��ʼ��

    SysTick_Config( SystemFrequency/2000 );            // ��ʱ����ʼ��

    InitUart();                                        // ���ڳ�ʼ��

    InitADC();                                         // AD ת����ʼ��

    InitPWM();                                         // PWM ��ʼ��

    InitGlobal();                                      // ��ʼ��ȫ�ֲ���

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

    InitPara();             // ��ʼ�� EEPROM ����

    //�����ѹ��
    Adjust_Battery_vol(1);

    //���������
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

void InitGlobal(void)   // ��ʼ��ȫ�ֲ���
{
    InitChargePara();
}

void InitPara(void)     // ��ʼ�� EEPROM ����
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

/**************************************************************************************��*****************
**                                 END OF FILE
********************************************************************************************************/
