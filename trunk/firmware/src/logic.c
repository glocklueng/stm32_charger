/****************************************Copyright (c)**************************************************
**                              �� �� �� �� �� �� �� С ��
**                                      ��˼�� ��̳
**                                     QQ Ⱥ: 7095643
**
**                               http://www.mystm32.com/bbs
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: Logic.c
**��   ��   ��: ����
**����޸�����: 2009��09��20�� 22:10
**��        ��: ���ܳ���� Logic
**
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: ����
** ��  ��: v0.01
** �ա���: 2009��09��20�� 22:10
** �衡��: ԭʼ�汾
**
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: ����
** ��  ��: v0.03
** �ա���: 2009��9��22�� 11:36
** �衡��: ��ǰ�汾
**
**------------------------------------------------------------------------------------------------------
**--------------��ǰ�汾��Ϣ----------------------------------------------------------------------------
** ������: �x��
** ��  ��: v0.71
** �ա���: 2010��1��5�� 11:36
** �衡��: ��ǰ�汾 �����뼶�ĳ�����ͳ�ƺ��� ������λ��ͨѶ���� ����AD�������STM32 VREF����
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/

#include "stm32f10x.h"
#include "global.h"
#include "para.h"
#include "charge.h"
#include "lcd5110.h"
#include "logic.h"
#include "string.h"
#include "key.h"
#include "menu.h"
#include "adc.h"


INT16U TestVol1 = 30;
INT16U TestVol2 = 30;

INT16U TestCur1 = 0;
INT16U TestCur2 = 0;

// ��ظ������� ÿ���ۻ�ƽ��
INT16U Batt0_Vol_Seconds ;
INT16U Batt0_Cur_Seconds ;
INT16U Batt0_Temperature_Seconds ;
INT16U Batt1_Vol_Seconds ;
INT16U Batt1_Cur_Seconds ;
INT16U Batt1_Temperature_Seconds ;
INT16U STM32_vref_Seconds ;
INT16U STM32_Temperature_Seconds ;

INT8U Led0ChargeStat = 0;
INT8U Led1ChargeStat = 0;

INT8U PowerOn = 1;   // �ϵ��־
INT8U StatBLWake = 0;  // ����˯�߱�־  1:˯��  0:����


void Uart_Send_Batt_Statue(void);

void UART_SendSingleData(INT8U data)
{
    /* Loop until the end of transmission */
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    USART_SendData(USART1,data);
}

void Capacity_Count(void)
{
    INT16U batt0_current ;
    INT16U batt1_current ;

    // ֱ�ӻ�ȡ����ֵ
    batt0_current = Batt0_Cur_Seconds;
    batt1_current = Batt1_Cur_Seconds;

    // BATT0 ���ݵ�������������
    if( Battery_Rapid_Charging == Bat0_StateMachine ||
            Battery_Normal_Charging == Bat0_StateMachine ||
            Battery_Discharging== Bat0_StateMachine ||
            Battery_Discharging == Bat0_DischargeMachine )
    {
        Bat0_Charge_Discharge_Capacity += (FP32)batt0_current / 3600 ;
    }


    // BATT1 ���ݵ�������������
    if( Battery_Rapid_Charging == Bat1_StateMachine ||
            Battery_Normal_Charging == Bat1_StateMachine ||
            Battery_Discharging== Bat1_StateMachine ||
            Battery_Discharging == Bat1_DischargeMachine )
    {
        Bat1_Charge_Discharge_Capacity += (FP32)batt1_current / 3600 ;
    }
}

void Uart_Send_Batt_Statue(void)
{
    // ��һͨ������
    INT16U batt0_voltage ;
    INT16U batt0_current ;
    INT16U batt0_capacity = 0 ;
    INT16U batt0_time ;
    INT16U batt0_temperature = 0 ;

    // �ڶ�ͨ������
    INT16U batt1_voltage ;
    INT16U batt1_current ;
    INT16U batt1_capacity = 0  ;
    INT16U batt1_time ;
    INT16U batt1_temperature = 0 ;

    INT8U i;
    INT8U datatype = 0x01;
    INT8U batt_status = 0;



    INT8U str[50];
    INT8U batt_str[22];


    // battery 0
    if( Battery_Normal_Charge ==  Bat0_Status ||
            Battery_Fast_Charge == Bat0_Status  ||
            Battery_DisCharge == Bat0_Status )
        batt_status |= 0x10;

    // battery 1
    if( Battery_Normal_Charge ==  Bat1_Status ||
            Battery_Fast_Charge == Bat1_Status  ||
            Battery_DisCharge == Bat1_Status )
        batt_status |= 0x01;




    // ֱ�ӻ�ȡ��ѹֵ
    batt0_voltage = Batt0_Vol_Seconds;
    batt1_voltage = Batt1_Vol_Seconds;

    // ֱ�ӻ�ȡ����ֵ
    batt0_current = Batt0_Cur_Seconds;
    batt1_current = Batt1_Cur_Seconds;

    // �Ѹ���������ת����16λ���α���
    batt0_capacity = (INT16U)Bat0_Charge_Discharge_Capacity ;

    // �Ѹ���������ת����16λ���α���
    batt1_capacity = (INT16U)Bat1_Charge_Discharge_Capacity ;

    // ���ڽ��յ��ַ���ͷ
    memcpy( str,"little",6);

    memset(batt_str,0,20);

    // ��ȡ����ŵ�ʱ�� �������S
    batt0_time = Bat0_Charge_Discharge_Time_Count / 2000;
    batt1_time = Bat1_Charge_Discharge_Time_Count / 2000;

    // ��������
    batt_str[0] = datatype;

    // ״̬��Ϣ ÿһ·4bit
    batt_str[1] = batt_status;

    // ��һͨ��ʱ��
    batt_str[2] = batt0_time >> 8;
    batt_str[3] = batt0_time;

    // �ڶ�ͨ��ʱ��
    batt_str[4] = batt1_time >> 8;
    batt_str[5] = batt1_time;

    // ��һͨ����ѹ
    batt_str[6] = batt0_voltage >> 8;
    batt_str[7] = batt0_voltage;

    // �ڶ�ͨ����ѹ
    batt_str[8] = batt1_voltage >> 8;
    batt_str[9] = batt1_voltage;

    // ��һͨ������
    batt_str[10] = batt0_current >> 8;
    batt_str[11] = batt0_current;

    // �ڶ�ͨ������
    batt_str[12] = batt1_current >> 8;
    batt_str[13] = batt1_current;

    // ��һͨ���¶�
    batt_str[14] = batt0_temperature >> 8;
    batt_str[15] = batt0_temperature ;

    // �ڶ�ͨ���¶�
    batt_str[16] = batt1_temperature >> 8;
    batt_str[17] = batt1_temperature ;

    // ��һͨ������
    batt_str[18] = batt0_capacity >> 8;
    batt_str[19] = batt0_capacity;

    // �ڶ�ͨ������
    batt_str[20] = batt1_capacity >> 8;
    batt_str[21] = batt1_capacity;



    memcpy(str+6,batt_str,22);

    memcpy( str + 28,"worm",4);

    for(i=0; i<32; i++)
    {
        UART_SendSingleData(str[i]);
    }

    //ChargerPrintf("time %4d capacity %4d\r\n",batt1_time,batt1_capacity);
}


// �߼�����
void Logic(void)
{
    INT16U  CpuTimer;   // CPU_LED ���ڶ�ʱ��
    INT8U   KeyCode = 0;


    if (ptime(0,8)) // 40mS ����һ�ΰ���ɨ�����
    {
        KeyCode = ReadKey();
        process_key(KeyCode);
        display_menu();
    }

    if(ptimed(5,(KeyCode || PowerOn)  ,2000))  // 10S ��û���˰�����, LCD �����Լ���ʡ��
    {
        PowerOn = 0;
        LcdBacklightOn();
        StatBLWake = 0; // ��������ģʽ
    }
    else
    {
        if(0 == ModeLCDLight)
        {
            LcdBacklightOff();
            StatBLWake = 1; // ����˯��ģʽ
        }
    }


    /*
        if (ptime(0,10))
        {
            //���0 ���״̬��
            Bat0_State_Machine();
                                                                          1
            //���1 ���״̬��
            Bat1_State_Machine();
        }
        */
    CpuTimer = 199;
    if (ptime(2,CpuTimer))
    {
        //��ʱ�ڴ˼���
        //Bat0_Charge_Time_Count++;
        //Bat1_Charge_Time_Count++;

        // ��ȡʵʱ�����
        //GetChargeMeasure();
        //ChargerPrintf(" Charge_Time        %4d sec \r\n", Bat0_Charge_Time_Count);
        //ChargerPrintf(" Current_0          %4d mA \r\n", Bat0_Cur);
        //ChargerPrintf(" Voltage_0          %4d mV \r\n\r\n", Bat0_Vol);

        //LCD_ShowChargeStatus();
        Capacity_Count();

        if( // battery 0
            Battery_Normal_Charge ==  Bat0_Status ||
            Battery_Fast_Charge == Bat0_Status  ||
            Battery_DisCharge == Bat0_Status ||
            // battery 1
            Battery_Normal_Charge ==  Bat1_Status ||
            Battery_Fast_Charge == Bat1_Status  ||
            Battery_DisCharge == Bat1_Status)
            ;//Uart_Send_Batt_Statue();

    }

    switch(ModeLED)
    {
    case 0: // LED1 ��������; // LED2 ��������;
        switch(Led0ChargeStat)
        {
        case RedOn:
            Led1On();
            Led2Off();
            break;
        case GreenOn:
            Led1Off();
            Led2On();
            break;
        case RedFlash10HZ:
            if (ptime(3,19))
            {
                Led1Flash();
            }
            Led2Off();
            break;
        case GreenFlash10HZ:
            Led1Off();
            if (ptime(3,19))
            {
                Led2Flash();
            }
            break;
        case RedFlash20HZ:
            if (ptime(3,9))
            {
                Led1Flash();
            }
            Led2Off();
            break;
        case GreenFlash20HZ:
            Led1Off();
            if (ptime(3,9))
            {
                Led2Flash();
            }
            break;
        case RedFlash2HZ:
            if (ptime(3,99))
            {
                Led1Flash();
            }
            Led2Off();
            break;
        case GreenFlash2HZ:
            Led1Off();
            if (ptime(3,99))
            {
                Led2Flash();
            }
            break;
        case OrangeOn:
            Led1On();
            Led2On();
            break;
        case AllOff:
            Led1Off();
            Led2Off();
            break;
        default:
            Led1Off();
            Led2Off();
            break;
        }

        switch(Led1ChargeStat)
        {
        case RedOn:
            Led3On();
            Led4Off();
            break;
        case GreenOn:
            Led3Off();
            Led4On();
            break;
        case RedFlash10HZ:
            if (ptime(4,19))
            {
                Led3Flash();
            }
            Led4Off();
            break;
        case GreenFlash10HZ:
            Led3Off();
            if (ptime(4,19))
            {
                Led4Flash();
            }
            break;
        case RedFlash20HZ:
            if (ptime(4,9))
            {
                Led3Flash();
            }
            Led4Off();
            break;
        case GreenFlash20HZ:
            Led3Off();
            if (ptime(4,9))
            {
                Led4Flash();
            }
            break;
        case RedFlash2HZ:
            if (ptime(4,99))
            {
                Led3Flash();
            }
            Led4Off();
            break;
        case GreenFlash2HZ:
            Led3Off();
            if (ptime(4,99))
            {
                Led4Flash();
            }
            break;
        case OrangeOn:
            Led3On();
            Led4On();
            break;
        case AllOff:
            Led3Off();
            Led4Off();
            break;
        default:
            Led3Off();
            Led4Off();
            break;
        }
        break;

    case 1:  // LED1 ��������; // LED2 ������;
        switch(Led0ChargeStat)
        {
        case RedOn:
            Led2On();
            Led1Off();
            break;
        case GreenOn:
            Led2Off();
            Led1On();
            break;
        case RedFlash10HZ:
            if (ptime(3,19))
            {
                Led2Flash();
            }
            Led1Off();
            break;
        case GreenFlash10HZ:
            Led2Off();
            if (ptime(3,19))
            {
                Led1Flash();
            }
            break;
        case RedFlash20HZ:
            if (ptime(3,9))
            {
                Led2Flash();
            }
            Led1Off();
            break;
        case GreenFlash20HZ:
            Led2Off();
            if (ptime(3,9))
            {
                Led1Flash();
            }
            break;
        case RedFlash2HZ:
            if (ptime(3,99))
            {
                Led2Flash();
            }
            Led1Off();
            break;
        case GreenFlash2HZ:
            Led2Off();
            if (ptime(3,99))
            {
                Led1Flash();
            }
            break;
        case OrangeOn:
            Led2On();
            Led1On();
            break;
        case AllOff:
            Led2Off();
            Led1Off();
            break;
        default:
            Led2Off();
            Led1Off();
            break;
        }

        switch(Led1ChargeStat)
        {
        case RedOn:
            Led3On();
            Led4Off();
            break;
        case GreenOn:
            Led3Off();
            Led4On();
            break;
        case RedFlash10HZ:
            if (ptime(4,19))
            {
                Led3Flash();
            }
            Led4Off();
            break;
        case GreenFlash10HZ:
            Led3Off();
            if (ptime(4,19))
            {
                Led4Flash();
            }
            break;
        case RedFlash20HZ:
            if (ptime(4,9))
            {
                Led3Flash();
            }
            Led4Off();
            break;
        case GreenFlash20HZ:
            Led3Off();
            if (ptime(4,9))
            {
                Led4Flash();
            }
            break;
        case RedFlash2HZ:
            if (ptime(4,99))
            {
                Led3Flash();
            }
            Led4Off();
            break;
        case GreenFlash2HZ:
            Led3Off();
            if (ptime(4,99))
            {
                Led4Flash();
            }
            break;
        case OrangeOn:
            Led3On();
            Led4On();
            break;
        case AllOff:
            Led3Off();
            Led4Off();
            break;
        default:
            Led3Off();
            Led4Off();
            break;
        }
        break;

    case 2:    // LED1 ��������; // LED2 ��������;
        switch(Led0ChargeStat)
        {
        case RedOn:
            Led1On();
            Led2Off();
            break;
        case GreenOn:
            Led1Off();
            Led2On();
            break;
        case RedFlash10HZ:
            if (ptime(3,19))
            {
                Led1Flash();
            }
            Led2Off();
            break;
        case GreenFlash10HZ:
            Led1Off();
            if (ptime(3,19))
            {
                Led2Flash();
            }
            break;
        case RedFlash20HZ:
            if (ptime(3,9))
            {
                Led1Flash();
            }
            Led2Off();
            break;
        case GreenFlash20HZ:
            Led1Off();
            if (ptime(3,9))
            {
                Led2Flash();
            }
            break;
        case RedFlash2HZ:
            if (ptime(3,99))
            {
                Led1Flash();
            }
            Led2Off();
            break;
        case GreenFlash2HZ:
            Led1Off();
            if (ptime(3,99))
            {
                Led2Flash();
            }
            break;
        case OrangeOn:
            Led1On();
            Led2On();
            break;
        case AllOff:
            Led1Off();
            Led2Off();
            break;
        default:
            Led1Off();
            Led2Off();
            break;
        }
        switch(Led1ChargeStat)
        {
        case RedOn:
            Led4On();
            Led3Off();
            break;
        case GreenOn:
            Led4Off();
            Led3On();
            break;
        case RedFlash10HZ:
            if (ptime(4,19))
            {
                Led4Flash();
            }
            Led3Off();
            break;
        case GreenFlash10HZ:
            Led4Off();
            if (ptime(4,19))
            {
                Led3Flash();
            }
            break;
        case RedFlash20HZ:
            if (ptime(4,9))
            {
                Led4Flash();
            }
            Led3Off();
            break;
        case GreenFlash20HZ:
            Led4Off();
            if (ptime(4,9))
            {
                Led3Flash();
            }
            break;
        case RedFlash2HZ:
            if (ptime(4,99))
            {
                Led4Flash();
            }
            Led3Off();
            break;
        case GreenFlash2HZ:
            Led4Off();
            if (ptime(4,99))
            {
                Led3Flash();
            }
            break;
        case OrangeOn:
            Led4On();
            Led3On();
            break;
        case AllOff:
            Led4Off();
            Led3Off();
            break;
        default:
            Led4Off();
            Led3Off();
            break;
        }
        break;

    case 3:    // LED1 ��������; // LED2 ��������;
        switch(Led0ChargeStat)
        {
        case RedOn:
            Led2On();
            Led1Off();
            break;
        case GreenOn:
            Led2Off();
            Led1On();
            break;
        case RedFlash10HZ:
            if (ptime(3,19))
            {
                Led2Flash();
            }
            Led1Off();
            break;
        case GreenFlash10HZ:
            Led2Off();
            if (ptime(3,19))
            {
                Led1Flash();
            }
            break;
        case RedFlash20HZ:
            if (ptime(3,9))
            {
                Led2Flash();
            }
            Led1Off();
            break;
        case GreenFlash20HZ:
            Led2Off();
            if (ptime(3,9))
            {
                Led1Flash();
            }
            break;
        case RedFlash2HZ:
            if (ptime(3,99))
            {
                Led2Flash();
            }
            Led1Off();
            break;
        case GreenFlash2HZ:
            Led2Off();
            if (ptime(3,99))
            {
                Led1Flash();
            }
            break;
        case OrangeOn:
            Led2On();
            Led1On();
            break;
        case AllOff:
            Led2Off();
            Led1Off();
            break;
        default:
            Led2Off();
            Led1Off();
            break;
        }

        switch(Led1ChargeStat)
        {
        case RedOn:
            Led4On();
            Led3Off();
            break;
        case GreenOn:
            Led4Off();
            Led3On();
            break;
        case RedFlash10HZ:
            if (ptime(4,19))
            {
                Led4Flash();
            }
            Led3Off();
            break;
        case GreenFlash10HZ:
            Led4Off();
            if (ptime(4,19))
            {
                Led3Flash();
            }
            break;
        case RedFlash20HZ:
            if (ptime(4,9))
            {
                Led4Flash();
            }
            Led3Off();
            break;
        case GreenFlash20HZ:
            Led4Off();
            if (ptime(4,9))
            {
                Led3Flash();
            }
            break;
        case RedFlash2HZ:
            if (ptime(4,99))
            {
                Led4Flash();
            }
            Led3Off();
            break;
        case GreenFlash2HZ:
            Led4Off();
            if (ptime(4,99))
            {
                Led3Flash();
            }
            break;
        case OrangeOn:
            Led4On();
            Led3On();
            break;
        case AllOff:
            Led4Off();
            Led3Off();
            break;
        default:
            Led4Off();
            Led3Off();
            break;
        }
        break;

    default:
        break;

    }


}


/*
Batt_Vol_Accumulate ����ÿ10ms����һ�Σ������ص�ѹ���������¶Ȳ���
ÿ���Ӽ���100�Σ�Ȼ��ȡƽ��ֵ����ֵ��ȫ�ֲ�����
�Թ���������ʹ��
*/
void Batt_Vol_Accumulate(INT8U erase)
{
    static INT8U times_count = 0;

    static INT32U Batt0_Vol_Sum = 0;
    static INT32U Batt0_Cur_Sum = 0;
    static INT32U Batt0_Temperature_Sum = 0;
    static INT32U Batt1_Vol_Sum = 0;
    static INT32U Batt1_Cur_Sum = 0;
    static INT32U Batt1_Temperature_Sum = 0;
    static INT32U STM32_vref_Sum = 0;
    static INT32U STM32_Temperature_Sum = 0;

    if(erase)
    {
        times_count = 0;
        Batt0_Vol_Sum = 0;
        Batt0_Cur_Sum = 0;
        Batt0_Temperature_Sum = 0;
        Batt1_Vol_Sum = 0;
        Batt1_Cur_Sum = 0;
        Batt1_Temperature_Sum = 0;
        STM32_vref_Sum = 0;
        STM32_Temperature_Sum = 0;

        Batt0_Cur_Seconds = 0;
        Batt1_Cur_Seconds = 0;
    }

    if( 100 == times_count )
    {
        Batt0_Vol_Seconds = Batt0_Vol_Sum / 100;
        Batt0_Cur_Seconds = Batt0_Cur_Sum / 100;
        Batt0_Temperature_Seconds = Batt0_Temperature_Sum / 100;
        Batt1_Vol_Seconds = Batt1_Vol_Sum / 100;
        Batt1_Cur_Seconds = Batt1_Cur_Sum / 100;
        Batt1_Temperature_Seconds = Batt1_Temperature_Sum / 100;
        STM32_vref_Seconds = STM32_vref_Sum /100;
        STM32_Temperature_Seconds = STM32_Temperature_Sum /100;

        //У��
        //Vchx = Vrefint * (ADchx/ADrefint)
        //����VrefintΪ���յ�ѹ=1.20V
        Batt0_Vol_Seconds = 1200 * (((FP32)Batt0_Vol_Seconds) / STM32_vref_Seconds);
        Batt1_Vol_Seconds = 1200 * (((FP32)Batt1_Vol_Seconds) / STM32_vref_Seconds);
        Batt0_Cur_Seconds = 1200 * (((FP32)Batt0_Cur_Seconds) / STM32_vref_Seconds);
        Batt1_Cur_Seconds = 1200 * (((FP32)Batt1_Cur_Seconds) / STM32_vref_Seconds);
        Batt0_Temperature_Seconds = 1200 * (((FP32)Batt0_Temperature_Seconds) / STM32_vref_Seconds);
        Batt1_Temperature_Seconds = 1200 * (((FP32)Batt1_Temperature_Seconds) / STM32_vref_Seconds);
        STM32_Temperature_Seconds = 1200 * (((FP32)STM32_Temperature_Seconds) / STM32_vref_Seconds);

        //ChargerPrintf(" Voltage_0          %4d mV \r\n", Batt0_Vol_Seconds);

        // ���������
        Batt0_Vol_Sum = 0;
        Batt0_Cur_Sum = 0;
        Batt0_Temperature_Sum = 0;
        Batt1_Vol_Sum = 0;
        Batt1_Cur_Sum = 0;
        Batt1_Temperature_Sum = 0;
        times_count = 0;
        STM32_vref_Sum = 0;
        STM32_Temperature_Sum = 0;
    }
    else
    {
        // ��ȡʵʱ�����
        GetChargeMeasure();
        Batt0_Vol_Sum += Bat0_Vol;
        Batt0_Cur_Sum += Bat0_Cur;
        Batt0_Temperature_Sum += Bat0_Temperature;
        Batt1_Vol_Sum += Bat1_Vol;
        Batt1_Cur_Sum += Bat1_Cur;
        Batt1_Temperature_Sum += Bat1_Temperature;
        STM32_vref_Sum += Vref;
        STM32_Temperature_Sum += Temperature;
        times_count++;
    }

}

/**************************************************************************************��*****************
**                                 END OF FILE
********************************************************************************************************/
