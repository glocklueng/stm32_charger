/****************************************Copyright (c)**************************************************
**                              �� �� �� �� �� �� �� С ��
**                                      ��˼�� ��̳
**                                     QQ Ⱥ: 7095643
**
**                               http://www.mystm32.com/bbs
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: GPIO.c
**��   ��   ��: ����
**����޸�����: 2009��09��20�� 22:10
**��        ��: ���ܳ���� GPIO
**
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: ����
** ��  ��: v0.01
** �ա���: 2009��09��20�� 22:10
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾��Ϣ----------------------------------------------------------------------------
** ������: ����
** ��  ��: v0.03
** �ա���: 2009��9��22�� 11:36
** �衡��: ��ǰ�汾
**
**------------------------------------------------------------------------------------------------------
**--------------��ǰ�汾��Ϣ----------------------------------------------------------------------------
** ������: ����
** ��  ��: v0.04
** �ա���: 2009��9��24�� 11:36
** �衡��: ����pwm�ŵ� ��ʼ��discharge pwm����
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#include "stm32f10x.h"
#include "gpio.h"







void InitGpio(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    /* TIM2 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /* TIM2 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    /* Enable DMA clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* Enable ADC1 and GPIOC clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                           RCC_APB2Periph_GPIOC, ENABLE);
    /*
        // ���в�ʹ�õĶ˿�����Ϊģ������,������ EMI/EMC
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                               RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
                               RCC_APB2Periph_AFIO, ENABLE);

        GPIO_StructInit(&GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin = GPAll;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        GPIO_Init(GPIOC, &GPIO_InitStructure);
        GPIO_Init(GPIOD, &GPIO_InitStructure);
     */

    // GPIOA.00 Configuration: TIM2 channel 0 as alternate function push-pull Charger1
    // GPIOA.01 Configuration: TIM2 channel 1 as alternate function push-pull Charger2
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // GPIOA.06 Configuration: TIM3 channel 0 as alternate function push-pull Charger1
    // GPIOA.07 Configuration: TIM3 channel 1 as alternate function push-pull Charger2
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Configure PA.02 (ADC Channel2) as analog input, Temperatrue_1
    // Configure PA.03 (ADC Channel3) as analog input, Temperatrue_2
    // Configure PA.04 (ADC Channel4) as analog input, Current_1
    // Configure PA.05 (ADC Channel5) as analog input, Current_2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Configure PB.00 (ADC Channel8) as analog input, Voltage_1
    // Configure PB.01 (ADC Channel9) as analog input, Voltage_2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // Configure PA8 as Output push-pull, LED1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Configure PB8 as Output push-pull, Key R0
    // Configure PB5 as Output push-pull, Key R1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // Configure PC.13  as  input, L0
    // Configure PC.14  as  input, L1
    // Configure PC.15  as  input, L2
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // Configure PB6 as Output push-pull, LED2
    // Configure PB2 as Output push-pull, LED3
    // Configure PB10 as Output push-pull, LED4
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_6 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // Configure PB9 as Output push-pull, LCD_BL
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**************************************************************************************��*****************
**                                 END OF FILE
********************************************************************************************************/
