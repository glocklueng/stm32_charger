/****************************************Copyright (c)**************************************************
**                              �� �� �� �� �� �� �� С ��
**                                      ��˼�� ��̳
**                                     QQ Ⱥ: 7095643 
**
**                               http://www.mystm32.com/bbs
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: ADC.c
**��   ��   ��: ����
**����޸�����: 2009��09��20�� 22:10
**��        ��: ���ܳ���� ADC
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
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �x��
** ��  ��: v0.04
** �ա���: 2009��10��08�� 19:36
** �衡��: ���ӵ�ѹ���� ��ֵ�˲�
**
**------------------------------------------------------------------------------------------------------
**--------------��ǰ�汾��Ϣ----------------------------------------------------------------------------
** ������: �x��
** ��  ��: v0.71
** �ա���: 2010��1��5�� 19:36
** �衡��: �޸ĵ�ѹ����������� ���浽flash  
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#include "stm32f10x.h"
#include "global.h"
#include "logic.h"
#include "adc.h"
#include "eeprom.h"
#include "lcd5110.h"
#include "menu.h"
#include "string.h"


#define ADC_Converted_len  66
#define ADC1_DR_Address    ((u32)0x4001244C)

INT16U Temperature_1  = 0; // ��һ·����¶�
INT16U Temperature_2  = 0; // �ڶ�·����¶�
INT16S Current_1      = 0; // ��һ·��ص���
INT16S Current_2      = 0; // �ڶ�·��ص���
INT16U Voltage_1      = 0; // ��һ·��ص�ѹ
INT16U Voltage_2      = 0; // �ڶ�·��ص�ѹ
INT16U Temperature    = 0; // STM32 оƬ�¶�
INT16U Vref           = 0; // STM32 �ڲ� Vref ��ѹ


//��¼��һ��MCU�¶���ֵ
INT16U OldMcuTempreture = 0; 

#define VREF_VOL     3778

//���������  
INT16U CURRENT_ZERO_0;  
INT16U CURRENT_ZERO_1;  

//�����ѹ��
INT16U VOLTAGE_ZERO_0;  
INT16U VOLTAGE_ZERO_1;  

// �����Ŵ��� 6.6 �����ʱ��ʮ��
#define CUR_AMP 66

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ADC_InitTypeDef ADC_InitStructure;
DMA_InitTypeDef DMA_InitStructure;

INT16U ADC_Raw_Value[ 8*ADC_Converted_len ]; //adc��ȡ��ԭʼ����
INT16U ADC_ConvertedValue[8]; //����ת���õ������û�����


void InitADC(void)
{
    // configuration ADC  ///////////////////////////////////////////////////////////////

    /* ADCCLK = PCLK2/8 */
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);
    
    /* Enable ADC1 clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    
    /* ADC1 configuration ------------------------------------------------------*/
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;   // ��������ģʽ
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;         // ɨ�跽ʽ
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;   // ����ת��
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // �ⲿ������ֹ
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;              // �����Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel = 8;              // ����ת����ͨ����
    ADC_Init(ADC1, &ADC_InitStructure);
    
    /* ADC1 regular channel16 configuration    
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2 , 1, ADC_SampleTime_239Cycles5  );
    ADC_RegularChannelConfig(ADC1, ADC_Channel_3 , 2, ADC_SampleTime_239Cycles5  );
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4 , 3, ADC_SampleTime_239Cycles5  );
    ADC_RegularChannelConfig(ADC1, ADC_Channel_5 , 4, ADC_SampleTime_239Cycles5  );
    ADC_RegularChannelConfig(ADC1, ADC_Channel_8 , 5, ADC_SampleTime_239Cycles5  );
    ADC_RegularChannelConfig(ADC1, ADC_Channel_9 , 6, ADC_SampleTime_239Cycles5  );
    ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 7, ADC_SampleTime_239Cycles5  );
    ADC_RegularChannelConfig(ADC1, ADC_Channel_17, 8, ADC_SampleTime_239Cycles5  );
*/
    /* ADC1 regular channel16 configuration */   // 55Cycles5  ����Ƶ��: 15.086us
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2 , 1, ADC_SampleTime_55Cycles5  );
    ADC_RegularChannelConfig(ADC1, ADC_Channel_3 , 2, ADC_SampleTime_55Cycles5  );
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4 , 3, ADC_SampleTime_55Cycles5  );
    ADC_RegularChannelConfig(ADC1, ADC_Channel_5 , 4, ADC_SampleTime_55Cycles5  );
    ADC_RegularChannelConfig(ADC1, ADC_Channel_8 , 5, ADC_SampleTime_55Cycles5  );
    ADC_RegularChannelConfig(ADC1, ADC_Channel_9 , 6, ADC_SampleTime_55Cycles5  );
    ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 7, ADC_SampleTime_55Cycles5  );
    ADC_RegularChannelConfig(ADC1, ADC_Channel_17, 8, ADC_SampleTime_55Cycles5  );
		
    /* Enable the temperature sensor and vref internal channel */ 
    ADC_TempSensorVrefintCmd(ENABLE); 
    
    /* Enable ADC1 */
    ADC_Cmd(ADC1, ENABLE);   


//// configuration DMA ///////////////////////////////////////////////////////////////
    
    /* Enable DMA1 clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* DMA channel1 configuration ----------------------------------------------*/
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;      // �����ַ
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_Raw_Value; // �ڴ��ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;               // DMA ���䷽����
    DMA_InitStructure.DMA_BufferSize = 8*ADC_Converted_len;         // ����DMA�ڴ���ʱ�������ĳ��� word
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // ����DMA���������ģʽ��һ������
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;         // ����DMA���ڴ����ģʽ
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  // ���������ֳ�
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;          // �ڴ������ֳ�
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;     // ����DMA�Ĵ���ģʽ���������ϵ�ѭ��ģʽ
    DMA_InitStructure.DMA_Priority = DMA_Priority_High; // ����DMA�����ȼ���
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;        // ����DMA��2��memory�еı����������
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    
    /* Enable DMA channel1 */
    DMA_Cmd(DMA1_Channel1, ENABLE);

    /* Enable ADC1 DMA */
    ADC_DMACmd(ADC1, ENABLE);

//////////////////////////////////////////////////////////////////////////////////////

    /* Enable ADC1 reset calibaration register */   
    ADC_ResetCalibration(ADC1);
    /* Check the end of ADC1 reset calibration register */
    while(ADC_GetResetCalibrationStatus(ADC1));
                            
    /* Start ADC1 calibaration */
    ADC_StartCalibration(ADC1);
    /* Check the end of ADC1 calibration */
    while(ADC_GetCalibrationStatus(ADC1));
     
    /* Start ADC1 Software Conversion */ 
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/*
flag = 0 �������ֵ���Ѳ������浽flash
flag = 1 ��flash��ȡ������ʼ������ֵ
*/
void Adjust_Battery_vol(INT8U flag)
{
    INT16U  bat0_Vol,bat1_Vol;
    INT8U i=0,sta;
    INT8S  buff[17];

    if(flag)
    {
        sta = EE_ReadVariable(VirtAddVarTab[0], &bat0_Vol);

        if(1==sta)
		{
			ChargerPrintf(" No voltage calibration. \r\n");
			i++;
        }
        else
            ChargerPrintf("Old adjust bat0 vol %4d mv \r\n", bat0_Vol);

        sta = EE_ReadVariable(VirtAddVarTab[1], &bat1_Vol);

        if(1==sta)  
		{
			ChargerPrintf(" No voltage calibration. \r\n");
			i++;
        }	
        else
            ChargerPrintf("Old adjust bat1 vol %4d mv \r\n", bat1_Vol);

		if(i>0)		
			Adjust_Battery_vol(0);
		else
		{
            VOLTAGE_ZERO_0 =  bat0_Vol ;
            VOLTAGE_ZERO_1 =  bat1_Vol ;	 
		}
    }
    else
    {
        //�Ȱѳ������˵ĵ��ݷŵ�
        TIM3->CCR1 = 0x0192;
        TIM3->CCR2 = 0x0192;
        // �ӳ�100ms��ʱ��,�Ա�ŵ�
        SysDelay(1000);
        //�رշŵ�
        TIM3->CCR1 = 0x00;
        TIM3->CCR2 = 0x00;

        // �ӳ�һ��ʱ��ȴ���·�ȶ�
        i = 36;
        do
        {
            set_cursor(0xff,0xff); // ��겻��ʾ
            display_str(0,0,"===��ѹ����===");
            memset(buff,0,17);
            strcpy((char*)buff," wait...    S ");
            sprfi(&buff[8],--i,0,2);
            display_str(0,2,buff);        
            SysDelay(2000);
        }while( i > 0 );
        
        
        bat0_Vol = 0;
        bat1_Vol = 0;

        for(i=0;i<128;i++)
        {
            ADC_Covert_Value();

            //��ѹ����
            bat0_Vol +=  ADC_ConvertedValue[4]*3300/4096;
            bat1_Vol +=  ADC_ConvertedValue[5]*3300/4096;

            // �ӳ�8ms��ʱ��,�Ա����ݸ���
            SysDelay(16);
        }

        bat0_Vol /= 128;
        bat1_Vol /= 128;
				
		// ���У׼��ѹ����һ����ֵ����Ϊ���Ѿ�װ����
		if( bat0_Vol > 64 | bat1_Vol > 64 )	
		{
			display_str(0,2,"Remove battery");
			display_str(0,4,"and Restart... ");
			ChargerPrintf(" Please remove battery and Restart.... \r\n");
			while(1);
		}
		
        VOLTAGE_ZERO_0 =  bat0_Vol ;
        VOLTAGE_ZERO_1 =  bat1_Vol ;	    

        sta = EE_ReadVariable(VirtAddVarTab[0], &bat0_Vol);

        if(1==sta)  ChargerPrintf(" No voltage calibration. \r\n");
        else
        {
            ChargerPrintf(" Old record bat0 vol %4d mv \r\n", bat0_Vol);
            ChargerPrintf(" New adjust bat0 vol %4d mv \r\n", VOLTAGE_ZERO_0);
        }

        sta = EE_ReadVariable(VirtAddVarTab[1], &bat1_Vol);

        if(1==sta)  ChargerPrintf(" No voltage calibration. \r\n");
        else
        {
            ChargerPrintf(" Old record bat1 vol %4d mv \r\n", bat1_Vol);
            ChargerPrintf(" New adjust bat0 vol %4d mv \r\n", VOLTAGE_ZERO_1);
        }


        // дBattery0 vol У�����ݵ�flash
        EE_WriteVariable(VirtAddVarTab[0], VOLTAGE_ZERO_0);
        // дBattery1 vol У�����ݵ�flash
        EE_WriteVariable(VirtAddVarTab[1], VOLTAGE_ZERO_1);
    }
}

void Adjust_Battery_Cur(INT8U flag)
{
    INT32U bat0_Cur,bat1_Cur;
    INT8U i=0,sta;
    INT8S  buff[17];

	bat0_Cur = bat1_Cur = 0;
    if(flag)
    {
        sta = EE_ReadVariable(VirtAddVarTab[2], (INT16U*)&bat0_Cur);

        if(1==sta)
		 {
		    ChargerPrintf(" No Current Calibration.  \r\n");
			i++;	
        }
        else
            ChargerPrintf("Old record bat0 cur %4d ma \r\n", bat0_Cur);


        sta = EE_ReadVariable(VirtAddVarTab[3], (INT16U*)&bat1_Cur);

        if(1==sta)  
		 {
		    ChargerPrintf(" No Current Calibration. \r\n");
			i++;	
        }					
        else
            ChargerPrintf("Old record bat1 cur %4d ma \r\n", bat1_Cur);
				
        if( i>0 )
            Adjust_Battery_Cur(0);
		else
		{
            CURRENT_ZERO_0 =  (INT16U)bat0_Cur ;
            CURRENT_ZERO_1 =  (INT16U)bat1_Cur ;	
		}
        
        
    }
    else
    {
        //�Ȱѳ������˵ĵ��ݷŵ�
        TIM3->CCR1 = 0x0192;
        TIM3->CCR2 = 0x0192;
        // �ӳ�1s��ʱ��,�Ա�ŵ�
        SysDelay(2000);
        //�رշŵ�
        TIM3->CCR1 = 0x00;
        TIM3->CCR2 = 0x00;


        // �ӳ�һ��ʱ��ȴ���·�ȶ�
        i = 36;
        do
        {
            set_cursor(0xff,0xff); // ��겻��ʾ
            display_str(0,0,"===��������===");
            memset(buff,0,17);
            strcpy((char*)buff," wait...    S ");
            sprfi(&buff[8],--i,0,2);
            display_str(0,2,buff);        
            SysDelay(2000);
        }while( i > 0 );
            
        
        bat0_Cur = 0;
        bat1_Cur = 0;

        for(i=0;i<128;i++)
        {
            ADC_Covert_Value();

            //��������
            bat0_Cur +=  ADC_ConvertedValue[2]*3300/4096;
            bat1_Cur +=  ADC_ConvertedValue[3]*3300/4096;

            // �ӳ�8ms��ʱ��,�Ա����ݸ���
            SysDelay(16);
        }

        bat0_Cur /= 128;
        bat1_Cur /= 128;


        CURRENT_ZERO_0 =  (INT16U)bat0_Cur ;
        CURRENT_ZERO_1 =  (INT16U)bat1_Cur ;	    

        sta = EE_ReadVariable(VirtAddVarTab[2], (INT16U*)&bat0_Cur);

        if(1==sta)  ChargerPrintf(" No Current Calibration. \r\n");
        else
        {
            ChargerPrintf(" Old record bat0 cur %4d ma \r\n", bat0_Cur);
            ChargerPrintf(" New adjust bat0 cur %4d ma \r\n", CURRENT_ZERO_0);
        }

        sta = EE_ReadVariable(VirtAddVarTab[3], (INT16U*)&bat1_Cur);

        if(1==sta)  ChargerPrintf(" No Current Calibration. \r\n");
        else
        {
            ChargerPrintf(" Old record bat1 cur %4d ma \r\n", bat1_Cur);
            ChargerPrintf(" New adjust bat1 cur %4d ma \r\n", CURRENT_ZERO_1);
        }


        // дBattery0 vol У�����ݵ�flash
        EE_WriteVariable(VirtAddVarTab[2], CURRENT_ZERO_0);
        // дBattery1 vol У�����ݵ�flash
        EE_WriteVariable(VirtAddVarTab[3], CURRENT_ZERO_1);
    }
}

void ADC_Covert_Value(void)
{
    INT8U i,j;
    INT16U MAX = 0;
    INT16U MIN = 0xffff;
    INT16U adc_temp = 0;
    INT32U adc_sum = 0;
	
    for(j=0;j<8;j++)
    {
        for(i=0;i<ADC_Converted_len;i++)
        {
            adc_temp = ADC_Raw_Value[j+i*8];
            adc_sum += adc_temp; //�ۼ�
            if( adc_temp > MAX ) MAX = adc_temp;
            if( adc_temp < MIN ) MIN = adc_temp;
        }	
        adc_sum -= MAX ; //ȥ�����ֵ
        adc_sum -= MIN ; //ȥ����Сֵ
        ADC_ConvertedValue[j] = adc_sum / (ADC_Converted_len - 2); //���ֵ
        MAX = 0;  //���ֵ��ʼ��
        MIN = 0xffff; //��Сֵ��ʼ��
        adc_sum = 0; //����ֵ��ʼ��
    }
}

void AdjustAdcStandard(INT16U Temperature)
{
    if( Temperature != OldMcuTempreture )
    {
        OldMcuTempreture = Temperature;

        ChargerPrintf("Adjust Adc Standard\r\n\n");

        /* Enable ADC1 reset calibaration register */   
        ADC_ResetCalibration(ADC1);
        /* Check the end of ADC1 reset calibration register */
        while(ADC_GetResetCalibrationStatus(ADC1));
                                
        /* Start ADC1 calibaration */
        ADC_StartCalibration(ADC1);
        /* Check the end of ADC1 calibration */
        while(ADC_GetCalibrationStatus(ADC1));
    }
}

/*********************************************************************************************************
** ��������: Zero_current_bat1
** ��������: �ŵ��ʱ�򣬵�ѹ�Ǹ�ֵ
**           �������裰����ŷ�� 
**           �ŵ�������� 
**           ���������� 
**           �Ŵ���Ϊ���������������������������� 
**           �Ŵ���ѹΪ������������������������������������ 
**           �Ŵ�����ƫ�õ�ѹ��ref������3 
**           ���ĵ�ѹΪ 0.66 ��5.94 V
**           ���ĵ�ѹΪ (0.66 �� 5.94��) / 2 = 0.33 V ~ 2.97 V  
**    
**           ( Vcur * 13.2 + 3.3 ) / 2 = Vadc
**       =>   6.6 * Vcur = Vadc - 3.3 / 2  (ע: ���� 0xFFF Ϊ 12 λ ADC �� 3.3V ��ֵ)
**       =>   CUR_AMP * Vcur = Vadc - VREF_ZERO
**
** �������: INT16U vol: ����������·���� AD ��������ֵ
** �������: INT16S temp: ʵ�ʵ��� mA
********************************************************************************************************/
INT16S Zero_current_bat0(INT16U vol)
{
    INT32S temp;
    INT16S result;	
    
    temp = vol;     

	// ������ֵ����׼���3����λ��ֱ���������Ϊ0	
	//if( CURRENT_ZERO_0 - temp <= 3 && CURRENT_ZERO_0 - temp >= -3 )
	//	temp = 0;
	//else		
    	temp -= CURRENT_ZERO_0;
        
    // CUR_AMP * Vcur = Vadc - VREF_ZERO
    // ���� CUR_AMP �Ŵ��� 10 ��
    // ��ѹ / 0.1 ŷķ = ����  => ��ѹ * 10 / 1 ŷķ = ���� 
     
    temp = (temp * 100) / (INT8U)CUR_AMP;

    result = (INT16S)(temp);
    
    return result;
}

INT16S Zero_current_bat1(INT16U vol)
{
    INT32S temp;
    INT16S result;	
		
    temp = vol;
		
	// ������ֵ����׼���3����λ��ֱ���������Ϊ0	
	//if( CURRENT_ZERO_1 - temp <= 3 && CURRENT_ZERO_1 - temp >= -3 )
	//	temp = 0;
	//else
    	temp -= CURRENT_ZERO_1;     
    
    // CUR_AMP * Vcur = Vadc - VREF_ZERO
    // ���� CUR_AMP �Ŵ��� 10 ��
    // ��ѹ / 0.1 ŷķ = ����  => ��ѹ * 10 / 1 ŷķ = ���� 
     
    temp = (temp * 100) / (INT8U)CUR_AMP;
    
    result = (INT16S)(temp);
    
    return result;
}

/*********************************************************************************************************
** ��������: Zero_voltage_bat1
** 
** 
** �������: INT16U vol ADC���ֵ
** �������: �����ĵ�ѹֵ
********************************************************************************************************/
INT16U Zero_voltage_bat0(INT16U vol)
{
	if( VOLTAGE_ZERO_0 > vol )
		vol = 0;
	else
		vol -= VOLTAGE_ZERO_0;
		
    return vol;
}

/*********************************************************************************************************
** ��������: Zero_voltage_bat2
** 
** 
** �������: INT16U vol ADC���ֵ
** �������: �����ĵ�ѹֵ
********************************************************************************************************/
INT16U Zero_voltage_bat1(INT16U vol)
{
	if( VOLTAGE_ZERO_1 > vol )
		vol = 0;
	else
		vol -= VOLTAGE_ZERO_1;
		
    return vol;
}

/**************************************************************************************��*****************
**                                 END OF FILE
********************************************************************************************************/
