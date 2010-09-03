/****************************************Copyright (c)**************************************************
**                              智 能 充 电 器 开 发 小 组
**                                      麦思网 论坛
**                                     QQ 群: 7095643 
**
**                               http://www.mystm32.com/bbs
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: ADC.c
**创   建   人: 安哥
**最后修改日期: 2009年09月20日 22:10
**描        述: 智能充电器 ADC
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 安哥
** 版  本: v0.01
** 日　期: 2009年09月20日 22:10
** 描　述: 原始版本
**
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 安哥
** 版  本: v0.03
** 日　期: 2009年9月22日 11:36
** 描　述: 当前版本
**
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 蟲子
** 版  本: v0.04
** 日　期: 2009年10月08日 19:36
** 描　述: 增加电压调零 均值滤波
**
**------------------------------------------------------------------------------------------------------
**--------------当前版本信息----------------------------------------------------------------------------
** 创建人: 蟲子
** 版  本: v0.71
** 日　期: 2010年1月5日 19:36
** 描　述: 修改电压调零电流调零 保存到flash  
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

INT16U Temperature_1  = 0; // 第一路电池温度
INT16U Temperature_2  = 0; // 第二路电池温度
INT16S Current_1      = 0; // 第一路电池电流
INT16S Current_2      = 0; // 第二路电池电流
INT16U Voltage_1      = 0; // 第一路电池电压
INT16U Voltage_2      = 0; // 第二路电池电压
INT16U Temperature    = 0; // STM32 芯片温度
INT16U Vref           = 0; // STM32 内部 Vref 电压


//记录上一次MCU温度数值
INT16U OldMcuTempreture = 0; 

#define VREF_VOL     3778

//调零电流档  
INT16U CURRENT_ZERO_0;  
INT16U CURRENT_ZERO_1;  

//调零电压档
INT16U VOLTAGE_ZERO_0;  
INT16U VOLTAGE_ZERO_1;  

// 电流放大倍数 6.6 运算的时候十倍
#define CUR_AMP 66

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ADC_InitTypeDef ADC_InitStructure;
DMA_InitTypeDef DMA_InitStructure;

INT16U ADC_Raw_Value[ 8*ADC_Converted_len ]; //adc获取的原始数据
INT16U ADC_ConvertedValue[8]; //计算转换好的最终用户数据


void InitADC(void)
{
    // configuration ADC  ///////////////////////////////////////////////////////////////

    /* ADCCLK = PCLK2/8 */
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);
    
    /* Enable ADC1 clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    
    /* ADC1 configuration ------------------------------------------------------*/
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;   // 独立工作模式
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;         // 扫描方式
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;   // 连续转换
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 外部触发禁止
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;              // 数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 8;              // 用于转换的通道数
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
    /* ADC1 regular channel16 configuration */   // 55Cycles5  采样频率: 15.086us
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
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;      // 外设地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_Raw_Value; // 内存地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;               // DMA 传输方向单向
    DMA_InitStructure.DMA_BufferSize = 8*ADC_Converted_len;         // 设置DMA在传输时缓冲区的长度 word
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 设置DMA的外设递增模式，一个外设
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;         // 设置DMA的内存递增模式
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  // 外设数据字长
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;          // 内存数据字长
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;     // 设置DMA的传输模式：连续不断的循环模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High; // 设置DMA的优先级别
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;        // 设置DMA的2个memory中的变量互相访问
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
flag = 0 检测修正值并把参数保存到flash
flag = 1 从flash读取参数初始化修正值
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
        //先把充电输出端的电容放电
        TIM3->CCR1 = 0x0192;
        TIM3->CCR2 = 0x0192;
        // 延迟100ms的时间,以便放电
        SysDelay(1000);
        //关闭放电
        TIM3->CCR1 = 0x00;
        TIM3->CCR2 = 0x00;

        // 延迟一定时间等待电路稳定
        i = 36;
        do
        {
            set_cursor(0xff,0xff); // 光标不显示
            display_str(0,0,"===电压矫零===");
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

            //电压调零
            bat0_Vol +=  ADC_ConvertedValue[4]*3300/4096;
            bat1_Vol +=  ADC_ConvertedValue[5]*3300/4096;

            // 延迟8ms的时间,以便数据更新
            SysDelay(16);
        }

        bat0_Vol /= 128;
        bat1_Vol /= 128;
				
		// 如果校准电压大于一定数值，认为是已经装入电池
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


        // 写Battery0 vol 校正数据到flash
        EE_WriteVariable(VirtAddVarTab[0], VOLTAGE_ZERO_0);
        // 写Battery1 vol 校正数据到flash
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
        //先把充电输出端的电容放电
        TIM3->CCR1 = 0x0192;
        TIM3->CCR2 = 0x0192;
        // 延迟1s的时间,以便放电
        SysDelay(2000);
        //关闭放电
        TIM3->CCR1 = 0x00;
        TIM3->CCR2 = 0x00;


        // 延迟一定时间等待电路稳定
        i = 36;
        do
        {
            set_cursor(0xff,0xff); // 光标不显示
            display_str(0,0,"===电流矫零===");
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

            //电流调零
            bat0_Cur +=  ADC_ConvertedValue[2]*3300/4096;
            bat1_Cur +=  ADC_ConvertedValue[3]*3300/4096;

            // 延迟8ms的时间,以便数据更新
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


        // 写Battery0 vol 校正数据到flash
        EE_WriteVariable(VirtAddVarTab[2], CURRENT_ZERO_0);
        // 写Battery1 vol 校正数据到flash
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
            adc_sum += adc_temp; //累加
            if( adc_temp > MAX ) MAX = adc_temp;
            if( adc_temp < MIN ) MIN = adc_temp;
        }	
        adc_sum -= MAX ; //去掉最大值
        adc_sum -= MIN ; //去掉最小值
        ADC_ConvertedValue[j] = adc_sum / (ADC_Converted_len - 2); //求均值
        MAX = 0;  //最大值初始化
        MIN = 0xffff; //最小值初始化
        adc_sum = 0; //总数值初始化
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
** 函数名称: Zero_current_bat1
** 功能描述: 放电的时候，电压是负值
**           采样电阻０．１欧． 
**           放电电流２Ａ 
**           充电电流２Ａ 
**           放大倍数为１＋１００／８．２＝１３．２ 
**           放大后电压为０．１＊２＊１３．２＝＋／－２．６４ 
**           放大器后偏置电压Ｖref＝３．3 
**           最后的电压为 0.66 ～5.94 V
**           最后的电压为 (0.66 ～ 5.94Ｖ) / 2 = 0.33 V ~ 2.97 V  
**    
**           ( Vcur * 13.2 + 3.3 ) / 2 = Vadc
**       =>   6.6 * Vcur = Vadc - 3.3 / 2  (注: 其中 0xFFF 为 12 位 ADC 的 3.3V 的值)
**       =>   CUR_AMP * Vcur = Vadc - VREF_ZERO
**
** 输入参数: INT16U vol: 电流采样电路上面 AD 采样出的值
** 输出参数: INT16S temp: 实际电流 mA
********************************************************************************************************/
INT16S Zero_current_bat0(INT16U vol)
{
    INT32S temp;
    INT16S result;	
    
    temp = vol;     

	// 测量数值跟基准相差3个单位，直接软件矫正为0	
	//if( CURRENT_ZERO_0 - temp <= 3 && CURRENT_ZERO_0 - temp >= -3 )
	//	temp = 0;
	//else		
    	temp -= CURRENT_ZERO_0;
        
    // CUR_AMP * Vcur = Vadc - VREF_ZERO
    // 其中 CUR_AMP 放大了 10 倍
    // 电压 / 0.1 欧姆 = 电流  => 电压 * 10 / 1 欧姆 = 电流 
     
    temp = (temp * 100) / (INT8U)CUR_AMP;

    result = (INT16S)(temp);
    
    return result;
}

INT16S Zero_current_bat1(INT16U vol)
{
    INT32S temp;
    INT16S result;	
		
    temp = vol;
		
	// 测量数值跟基准相差3个单位，直接软件矫正为0	
	//if( CURRENT_ZERO_1 - temp <= 3 && CURRENT_ZERO_1 - temp >= -3 )
	//	temp = 0;
	//else
    	temp -= CURRENT_ZERO_1;     
    
    // CUR_AMP * Vcur = Vadc - VREF_ZERO
    // 其中 CUR_AMP 放大了 10 倍
    // 电压 / 0.1 欧姆 = 电流  => 电压 * 10 / 1 欧姆 = 电流 
     
    temp = (temp * 100) / (INT8U)CUR_AMP;
    
    result = (INT16S)(temp);
    
    return result;
}

/*********************************************************************************************************
** 函数名称: Zero_voltage_bat1
** 
** 
** 输入参数: INT16U vol ADC检测值
** 输出参数: 调零后的电压值
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
** 函数名称: Zero_voltage_bat2
** 
** 
** 输入参数: INT16U vol ADC检测值
** 输出参数: 调零后的电压值
********************************************************************************************************/
INT16U Zero_voltage_bat1(INT16U vol)
{
	if( VOLTAGE_ZERO_1 > vol )
		vol = 0;
	else
		vol -= VOLTAGE_ZERO_1;
		
    return vol;
}

/**************************************************************************************×*****************
**                                 END OF FILE
********************************************************************************************************/
