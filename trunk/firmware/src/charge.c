/****************************************Copyright (c)**************************************************
**                              智 能 充 电 器 开 发 小 组
**                                      麦思网 论坛
**                                     QQ 群: 7095643 
**
**                               http://www.mystm32.com/bbs
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: charge.c
**创   建   人: 虫子
**最后修改日期: 2009年09月20日 22:10
**描        述: 智能充电器 
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 虫子
** 版  本: v0.01
** 日　期: 2009年09月20日 22:10
** 描　述: 原始版本
**
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 虫子
** 版  本: v0.04
** 日　期: 2009年10月08日 19:36
** 描　述: 完成电池插入检测 电池是否是可充电电池检测 电池内阻检测 电池充电状态机  标准充电模式充电
**
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 虫子
** 版  本: v0.06
** 日　期: 2009年11月07日 19:36
** 描　述: 增加电池放电功能 增加快速充电功能（待完善）
**
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 虫子
** 版  本: v0.071
** 日　期: 2010年1月5日 11:36
** 描　述: 修改充电状态机 修改充电所有相关函数
**
**------------------------------------------------------------------------------------------------------
**--------------当前版本信息----------------------------------------------------------------------------
** 创建人: 虫子
** 版  本: v0.08
** 日　期: 2010年2月12日 0:21
** 描　述: 修复充电和放电一开始时的大电流，完善快速充电流程
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/

#include "stm32f10x.h"
#include "global.h"
#include "charge.h"
#include "para.h"
#include "adc.h"
#include "logic.h"
#include "lcd5110.h"
#include "eeprom.h"

INT16U para[256];
INT16U status[64];
volatile  INT32U  Bat0_Charge_Discharge_Time_Count;  // 电池 0 充电时间计数器
volatile  INT32U  Bat1_Charge_Discharge_Time_Count;  // 电池 1 充电时间计数器

FP32 Bat0_Charge_Discharge_Capacity ; // 电池 0 充电或放电容量 
FP32 Bat1_Charge_Discharge_Capacity ; // 电池 1 充电或放电容量

INT32U BAT_NOR_CHARGE_MAX_TIME;

/*****************************************************************************
    电池0 充电状态机 先检测电池是否插入 然后检测电池是否是可充电电池
    然后等待用户选择充电模式 或者直接使用默认充电方式进行充电
*****************************************************************************/
void Bat0_State_Machine(INT8U mode)
{
    INT8U TempStatus;
    static INT8U timecount = 0;

    switch(Bat0_StateMachine)
    {
        case Battery_Wait_For_Remove:
            //每隔3S钟才检测一次	
            if( timecount <30 )
            {
                timecount++;
                break;
            }
            else timecount = 0;					
            TempStatus = Bat0_Insert_Test();
            if( 0 == TempStatus )
            {
                Bat0_Status= Battery_Not_input;
                Bat0_StateMachine = Battery_Insert_Test;  
                ChargerPrintf(" Battery 0 have been take off  ...\r\n"); 			
             }	
            break;

        case Battery_Init:    
            display_str(0,2,"  Input test ");
            Bat0_StateMachine = Battery_Insert_Test;
            break;

        case Battery_Insert_Test:
            //每隔3S钟才检测一次	
            if( timecount <30 )
            {
                timecount++;
                break;
            }
            else timecount = 0;

            // 检测是否有电池插入状态 返回1 代表有电池被插入
            TempStatus = Bat0_Insert_Test();
            if( 1 == TempStatus )
            {
                Bat0_Status= Battery_Have_Input;
                Bat0_StateMachine = Battery_Chargeable_Test;  
                ChargerPrintf(" Battery 0 have been inserted  ...\r\n"); 	
             }
            else display_str(0,2,"  Not input   ");
            break;

        case Battery_Chargeable_Test:
            // 检测电池是否是可充电电池, 返回1 代表可充电电池
            TempStatus = Bat0_Recharge_Test();
            if( 1 == TempStatus )
            {
                Bat0_Status = Battery_Chargeable;
                Bat0_StateMachine = Battery_Charge_Discharge;
                ChargerPrintf(" Battery 0 is rechargeable ...\r\n"); 
            }
            else
            {   
                Bat0_Status = Battery_Not_chargeable;
                //返回0 该电池不是可充电电池 拒冲
                Bat0_StateMachine = Battery_Wait_For_Remove;
                ChargerPrintf(" Battery 0 refuse charge,wait take off ...\r\n"); 
            }
            break;

        // 电池处于充电状态 等待用户选择充电方式（有一个默认设置）
        case Battery_Charge_Discharge:
            Bat0_Charge_Discharge_Choise = mode;
            if( Battery_Normal_Charge_Mode == Bat0_Charge_Discharge_Choise )
            {
                // 标准充电前先放电
                ChargerPrintf(" Battery 0 Normal Discharging ...\r\n"); 
                Bat0_Status = Battery_DisCharge;
                Bat0_StateMachine = Battery_Discharging;	
                Batt_Vol_Accumulate(1);
            }
            else if( Battery_Rapid_Charge_Mode == Bat0_Charge_Discharge_Choise )
            {
                Bat0_Status = Battery_Fast_Charge;
                // 置快速充电为预冲状态
                Bat0_Rapid_Charge_StateMachine = Battery_Precharge_Mode;
                Bat0_StateMachine = Battery_Rapid_Charging;	
                Batt_Vol_Accumulate(1);
            }
            Bat0_Charge_Discharge_Time_Count = 0; 
            Bat0_Charge_Discharge_Capacity = 0;            
            break;

         case  Battery_Normal_Charging:
            // 标准充电模式
            TempStatus = Bat0_NormalCharge(); 
            if(1 == TempStatus)
            {
                Bat0_Status = Battery_Charge_Over;
                Bat0_StateMachine = Battery_Wait_For_Remove;
                ChargerPrintf(" Battery 0 charge full ,wait take off ...\r\n"); 
            }		
            break;

        case Battery_Rapid_Charging:
            //快速充电模式
            TempStatus = Bat0_RapidCharge(); 
            if(1 == TempStatus)
            {
                Bat0_Status = Battery_Charge_Over;
                Bat0_StateMachine = Battery_Wait_For_Remove;
                ChargerPrintf(" Battery 0 charge full ,wait take off ...\r\n"); 
            }		
            break;

        case Battery_Discharging:
            TempStatus = Bat0_DisCharge();
            if(1 == TempStatus)
            {
                // 放电结束转到标准充电
                Bat0_Status = Battery_Normal_Charge;
                Bat0_StateMachine = Battery_Normal_Charging;
                Bat0_Charge_Discharge_Capacity = 0;
                ChargerPrintf(" Battery 0 Discharge over ,change to normal charging ...\r\n"); 
            }								
            break;
                

        default:
            Led0ChargeStat = AllOff;
            break;

    }
}

/*****************************************************************************
    电池1 充电状态机 先检测电池是否插入 然后检测电池是否是可充电电池
    然后等待用户选择充电模式 或者直接使用默认充电方式进行充电
*****************************************************************************/
void Bat1_State_Machine(INT8U mode)
{
    INT8U TempStatus;
    static INT8U timecount = 0;

    switch(Bat1_StateMachine)
    {
        case Battery_Wait_For_Remove:
            //每隔3S钟才检测一次	
            if( timecount <30 )
            {
                timecount++;
                break;
            }
            else timecount = 0;					
            TempStatus = Bat1_Insert_Test();
            if( 0 == TempStatus )
            {
                Bat1_Status= Battery_Not_input;
                Bat1_StateMachine = Battery_Insert_Test;  
                Bat1_Charge_Discharge_Capacity = 0;
                Bat1_Charge_Discharge_Time_Count = 0;                       
                ChargerPrintf(" Battery 1 have been take off  ...\r\n"); 			
             }			 
            break;

        case Battery_Init:    
            display_str(0,4,"  Input test ");
            Bat1_StateMachine = Battery_Insert_Test;
            break;            

        case Battery_Insert_Test:
            //每隔3S钟才检测一次	
            if( timecount <30 )
            {
                timecount++;
                break;
            }
            else timecount = 0;

            // 检测是否有电池插入状态 返回1 代表有电池被插入
            TempStatus = Bat1_Insert_Test();
            if( 1 == TempStatus )
            {
                Bat1_Status= Battery_Have_Input;
                Bat1_StateMachine = Battery_Chargeable_Test;  
                ChargerPrintf(" Battery 1 have been inserted  ...\r\n"); 	
             }
            else display_str(0,4,"  Not input   ");
            break;

        case Battery_Chargeable_Test:
            // 检测电池是否是可充电电池, 返回1 代表可充电电池
            TempStatus = Bat1_Recharge_Test();
            if( 1 == TempStatus )
            {
                Bat1_Status = Battery_Chargeable;
                Bat1_StateMachine = Battery_Charge_Discharge;
                ChargerPrintf(" Battery 1 is rechargeable ...\r\n"); 
            }
            else
            {   
                Bat1_Status = Battery_Not_chargeable;
                //返回0 该电池不是可充电电池 拒冲
                Bat1_StateMachine = Battery_Wait_For_Remove;
                ChargerPrintf(" Battery 1 refuse charge,wait take off ...\r\n"); 
            }
            break;

        // 电池处于充电状态 等待用户选择充电方式（有一个默认设置）
        case Battery_Charge_Discharge:
            Bat1_Charge_Discharge_Choise = mode;
            if( Battery_Normal_Charge_Mode == Bat1_Charge_Discharge_Choise )
            {
                // 标准充电前先放电
                ChargerPrintf(" Battery 1 Normal Discharging ...\r\n"); 
                Bat1_Status = Battery_DisCharge;
                Bat1_StateMachine = Battery_Discharging;		
                Batt_Vol_Accumulate(1);
            }
            else if( Battery_Rapid_Charge_Mode == Bat1_Charge_Discharge_Choise )
            {
                Bat1_Status = Battery_Fast_Charge;
                // 置快速充电为预冲状态
                Bat1_Rapid_Charge_StateMachine = Battery_Precharge_Mode;                
                Bat1_StateMachine = Battery_Rapid_Charging;	
                Batt_Vol_Accumulate(1);
            }
            Bat1_Charge_Discharge_Capacity = 0;
            Bat1_Charge_Discharge_Time_Count = 0;   
            break;



         case  Battery_Normal_Charging:
            // 标准充电模式
            TempStatus = Bat1_NormalCharge(); 
            if(1 == TempStatus)
            {
                Bat1_Status = Battery_Charge_Over;
                Bat1_StateMachine = Battery_Wait_For_Remove;
                ChargerPrintf(" Battery 1 charge full ,wait take off ...\r\n"); 
            }		
            break;

        case Battery_Rapid_Charging:
            //快速充电模式
            TempStatus = Bat1_RapidCharge(); 
            if(1 == TempStatus)
            {
                Bat1_Status = Battery_Charge_Over;
                Bat1_StateMachine = Battery_Wait_For_Remove;
                ChargerPrintf(" Battery 1 charge full ,wait take off ...\r\n"); 
            }		
            break;

        case Battery_Discharging:
            TempStatus = Bat1_DisCharge();
            if(1 == TempStatus)
            {
            		// 放电结束转到标准充电
                Bat1_Status = Battery_Normal_Charge;
                Bat1_StateMachine = Battery_Normal_Charging;
				 Bat1_Charge_Discharge_Capacity = 0;				
                ChargerPrintf(" Battery 1 Discharge over ,change to normal charging ...\r\n"); 
            }			
            break;
                

        default:
            Led1ChargeStat = AllOff;
            break;

    }
}

void InitChargePara(void)
{
    // 2路电池共同参数设置
    BAT_Vol_CHARGE_MAX = 1800; // 标准充电时最高截止电压
    BAT_MAX_TEMPERATURE = 50; // 电池充电时最高截止温度
    BAT_MAX_RESISTANCE = 300; // 可充电电池最大内阻值
    BAT_NOR_CHARGE_MAX_TIME_H = 0x66F; // 标准充电时间15h 15*3600*2 ，每500us自增一次    
    BAT_NOR_CHARGE_MAX_TIME_L = 0xF300; // 标准充电时间15h 15*3600*2 ，每500us自增一次    
    BAT_INSERT_TEST_CCR = 300; // 检测电池是否插入pwm占空比
    BAT_RECHARGE_TEST_CCR = 760; // 检测是否是可充电电池使用pwm占空比 900ma左右
    BAT_DISCHARGE_STOP_VOL = 1000; //停止放电电压
    BAT_VOL_PRE_CHARGE_OVER = 800; // 预充结束电压    
    BAT_VOL_RAPID_CHARGE_MAX = 1800;  // 快冲最高电压    

    //每小时数值是0x6DDD00
    //标准0.1C充电需要时间0x6DDD000(16小时)
    //标准0.2C充电需要时间0x36EE800(8小时)
    BAT_NOR_CHARGE_MAX_TIME = 0x36EE800;

    ReinitChargePara(0);
    ReinitChargePara(1);

}


void ReinitChargePara(  INT8U para_ptr)
{
    INT8U sta;

    if( 0 == para_ptr )
    {
        // 从eeprom读取电池0的容量
        sta = EE_ReadVariable(VirtAddVarTab[4], &Bat0_Set_Capacity);
        if( 1 == sta )
            Bat0_Set_Capacity = 1800; 

        if(Bat0_Set_Capacity > 1800)
            Bat0_Set_Capacity = 1800;

        // 第 0 路电池参数设置
        // 默认电池容量为1800 该参数在充电前应按照实质情况修改
        Bat0_Vol_Repair_Min  = 900;     // 需要修复电压下限    
        Bat0_Vol_Charge_Min  = 1200;    // 可充电电压范围下限


        Bat0_Cur_DisCharge   = Bat0_Set_Capacity * 0.1; // 放电电流
        Bat0_Cur_Repair      = Bat0_Set_Capacity * 0.2; // 修复电流

        Bat0_Cur_Pre_CHARGE  = Bat0_Set_Capacity * 0.1; // 预冲电流
        Bat0_Cur_Rapid_CHARGE = Bat0_Set_Capacity * 0.4; // 快冲电流
        Bat0_Cur_Add_CHARGE  = Bat0_Set_Capacity * 0.05; // 补充电流
        Bat0_Cur_Nor_CHARGE  = Bat0_Set_Capacity * 0.2; // 标准充电电流

        Bat0_StateMachine = Battery_Init; 
        Bat0_Rapid_Charge_StateMachine = Battery_Precharge_Mode; //预冲状态
        Bat0_Charge_On_Off = 0; // 0 电池没有处于充电状态
        Bat0_Test_Insert = 0; // 0 电池没有插入
        Bat0_Charge_Discharge_Choise = Battery_Normal_Charge_Mode;  //默认的充电模式
        Bat0_DischargeMachine = Battery_Init;  //放电状态机初始化为1

        // 电池1 状态标志位
        Bat0_Status = Battery_Not_input;
    }
    else if( 1 == para_ptr )
    {
        // 从eeprom读取电池0的容量
        sta = EE_ReadVariable(VirtAddVarTab[5], &Bat1_Set_Capacity);
        if( 1 == sta )
            Bat1_Set_Capacity = 1800; 

        if(Bat1_Set_Capacity > 1800)
            Bat1_Set_Capacity = 1800;

        // 第 1 路电池参数设置
        //Bat1_Set_Capacity = 1800; 
        Bat1_Vol_Repair_Min  = 900;     // 需要修复电压下限    
        Bat1_Vol_Charge_Min  = 1200;    // 可充电电压范围下限
        Bat1_Vol_DisCharge_OVER = 1000; // 放电结束电压
        Bat1_Vol_Rre_CHARGE_OVER = 1200; // 预充结束电压
        Bat1_Vol_Fast_CHARGE_MAX = 1800;  // 快冲最高电压

        Bat1_Cur_DisCharge   = Bat1_Set_Capacity * 0.1; // 放电电流
        Bat1_Cur_Repair      = Bat1_Set_Capacity * 0.2; // 修复电流
        Bat1_Cur_Pre_CHARGE  = Bat1_Set_Capacity * 0.1; // 预冲电流
        Bat1_Cur_Rapid_CHARGE = Bat1_Set_Capacity * 0.4; // 快冲电流
        Bat1_Cur_Add_CHARGE  = Bat1_Set_Capacity * 0.05; // 补充电流
        Bat1_Cur_Nor_CHARGE  = Bat1_Set_Capacity * 0.2; // 标准充电电流

        Bat1_StateMachine = Battery_Init; 
        Bat1_Charge_On_Off = 0; // 1 电池没有处于充电状态
        Bat1_Test_Insert = 0; // 1 电池没有插入
        Bat1_Charge_Discharge_Choise = Battery_Normal_Charge_Mode; //默认的充电模式
        Bat1_DischargeMachine = Battery_Init;  //放电状态机初始化为1    

        // 电池1 状态标志位
        Bat1_Status = Battery_Not_input;		   
    }
		
}

/*入口参数flag=1 输出校正后的数据
入口参数flag=0 输出没校正的数据
*/
void GetChargeMeasure(void) 
{
    ADC_Covert_Value();
    Bat0_Temperature = ADC_ConvertedValue[0]*3300/4096;
    Bat1_Temperature = ADC_ConvertedValue[1]*3300/4096;

    //电流调零
    Bat0_Cur = Zero_current_bat0(ADC_ConvertedValue[2]*3300/4096);
    Bat1_Cur = Zero_current_bat1(ADC_ConvertedValue[3]*3300/4096);    

    if( Battery_Discharging == Bat0_DischargeMachine || Bat0_Status == Battery_DisCharge )  
    { 
        // 软件调整电流零点误差
        if( Bat0_Cur < 8 ) Bat0_Cur = 0xffff;
        Bat0_Cur = 0xffff - Bat0_Cur;
    }
    else if( Bat0_Cur > 32768 ) Bat0_Cur = 0;
    
    if( Battery_Discharging == Bat1_DischargeMachine || Bat1_Status == Battery_DisCharge )
    { 
        // 软件调整电流零点误差
        if( Bat1_Cur < 8 ) Bat1_Cur = 0xffff;
        Bat1_Cur = 0xffff - Bat1_Cur;
    }
    else if( Bat1_Cur > 32768 ) Bat1_Cur = 0;
    
    //电压调零
    Bat0_Vol = Zero_voltage_bat0( ADC_ConvertedValue[4]*3300/4096 );
    Bat1_Vol = Zero_voltage_bat1( ADC_ConvertedValue[5]*3300/4096 );
    Temperature= ((1.42 - ADC_ConvertedValue[6]*3.3/4096)*1000/4.35 + 25)*10; 
    Vref = ADC_ConvertedValue[7]*3300/4096;
}

/************************************************************** 
    检测是否有电池放插入充电器 输出一个短时间的pwm 
    如果没有插入电池 那么检测到的电压应该是3300mv
    如果低于3300mv  那么就是有电池插入 
    这里设置的是低于 BAT_Vol_CHARGE_MAX
**************************************************************/
INT8U Bat0_Insert_Test(void)
{
    INT16U Bat0_Vol_temp;
        
    // 输出pwm 
    TIM2->CCR1 = BAT_INSERT_TEST_CCR;

    // 延迟10ms的时间,完整一次采样需要的时间是448us，这里一轮采样16次
    SysDelay(30);

    // 获取当前的检测数据
    GetChargeMeasure();
    Bat0_Vol_temp = Bat0_Vol ;
    // 关闭pwm 
    TIM2->CCR1 = 0;
    SysDelay(20);

    // 如果检测电压等于3300mv 证明没有电池插入
    // 否则 则认为有电池插入 BAT_Vol_CHARGE_MAX 标准充电时最高截止电压
    if( Bat0_Vol_temp < BAT_Vol_CHARGE_MAX )
    {
        return 1;
    }
    return 0; 
}

INT8U Bat1_Insert_Test(void)
{
    INT16U Bat1_Vol_temp;

    // 输出pwm 
    TIM2->CCR2 = BAT_INSERT_TEST_CCR;

    // 延迟10ms的时间
    SysDelay(30);

    // 获取当前的检测数据
    GetChargeMeasure();
    Bat1_Vol_temp = Bat1_Vol ;

    // 关闭pwm 
    TIM2->CCR2 = 0;
    SysDelay(20);

    // 如果检测电压等于3300mv 证明没有电池插入
    // 否则 则认为有电池插入 BAT0_Vol_Nor_CHARGE_MAX 标准充电时最高截止电压
    if( Bat1_Vol_temp < BAT_Vol_CHARGE_MAX )
    {
        return 1;
    }
    return 0;
}

/************************************************************** 
    电池放进来后 检测电池是否可冲 
    检测不充电时的电池电压 v0
    检测充电时的电池电压 vc
    检测充电电流 i
    则电池内阻 r = (vc - v0)/i
**************************************************************/
INT8U Bat0_Recharge_Test(void)
{
    INT8U i;
    INT16U Resistance_tmp=0;
    INT16U charge_current_tmp;
			
    for(i=0;i<4;i++)
    {
		// 输出pwm 
		TIM2->CCR1 = BAT_RECHARGE_TEST_CCR;
		//延迟8ms的时间
		SysDelay(16);

        // 获取充电时电池电压和电流参数
        GetChargeMeasure();
        Bat0_ReChargeable_Vol = Bat0_Vol;
        charge_current_tmp = Bat0_Cur;

        // 关闭pwm 
        TIM2->CCR1 = 0;
        // 延迟8ms的时间，刚好是adc数值采集一轮的时间
        SysDelay(4);

        GetChargeMeasure();
        Resistance_tmp += ( Bat0_ReChargeable_Vol  - Bat0_Vol )*1000 /charge_current_tmp; // 内阻放大1000倍 				
    }
    Bat0_Resistance = Resistance_tmp/4;
	ChargerPrintf(" Bat0_Resistance      %4d omh \r\n", Bat0_Resistance);	
    // 如果电池内阻在 BAT_MAX_RESISTANCE 最大可充电电池内阻以内 则是可充电电池
    if( Bat0_Resistance < BAT_MAX_RESISTANCE )
    { 
        //ChargerPrintf(" Bat0_Resistance      %4d omh \r\n", Bat0_Resistance);
        return 1;
    }
    return 0;
}


/************************************************************** 
    电池放进来后 检测电池是否可冲 
    检测不充电时的电池电压 v0
    检测充电时的电池电压 vc
    检测充电电流 i
    则电池内阻 r = (vc - v0)/i
**************************************************************/
INT8U Bat1_Recharge_Test(void)
{
    INT8U i;
    INT16U Resistance_tmp=0;		
    INT16U charge_current_tmp;

    // 输出pwm 
    for(i=0;i<4;i++)
    {
        // 输出pwm 
        TIM2->CCR2 = BAT_RECHARGE_TEST_CCR;
        //延迟8ms的时间
        SysDelay(16);

        // 获取充电时电池电压和电流参数
        GetChargeMeasure();
        Bat1_ReChargeable_Vol = Bat1_Vol;
        charge_current_tmp = Bat1_Cur;

        // 关闭pwm 
        TIM2->CCR2 = 0;
        // 延迟8ms的时间
        SysDelay(4);

        GetChargeMeasure();
        Resistance_tmp += ( Bat1_ReChargeable_Vol - Bat1_Vol )*1000 /charge_current_tmp; // 内阻放大1000倍 				
    }

    // 计算电池内阻/////////////////////////////////////////////
    Bat1_Resistance = Resistance_tmp/4;
    ChargerPrintf(" Bat1_Resistance      %4d omh \r\n", Bat1_Resistance);		
    // 如果电池内阻在 BAT_MAX_RESISTANCE 最大可充电电池内阻以内 则是可充电电池
    if( Bat1_Resistance < BAT_MAX_RESISTANCE )
    { 
        return 1;
    }
    return 0;
}


/************************************************************** 
    标准充电方式 使用0.1C的电流充电 充电时间15h
**************************************************************/
INT8U Bat0_NormalCharge()
{
    INT16U Charge0_CCR;

    // 确认电池处于充电状态
    if( Bat0_Status == Battery_Normal_Charge )
    {
        // 获取实时检测结果
        GetChargeMeasure();

        // 这里设置的电流是标准充电电流
        Charge0_CCR = pid_Controller(Bat0_Cur_Nor_CHARGE, Bat0_Cur, &pidData0);      
        if(Charge0_CCR > 900) 
            Charge0_CCR = 900;

        TIM2->CCR1 = Charge0_CCR;

        // 如果电池电压过高 则停止
        if( Batt0_Vol_Seconds > BAT_Vol_CHARGE_MAX )
        {
            ChargerPrintf(" Battery 0 chargeing over by vol ...\r\n");
            TIM2->CCR1 = 0; // 停止充电
            return 1;
        }
        // 如果充电时间到达 则停止
        if( Bat0_Charge_Discharge_Time_Count > BAT_NOR_CHARGE_MAX_TIME )
            {
            ChargerPrintf(" Battery 0 chargeing over by time ...\r\n");
            TIM2->CCR1 = 0; // 停止充电
            return 1;
        }

        // 如果电池温度过高 则停止
        //if( Bat0_Temperature > BAT_MAX_TEMPERATURE )
        //{
        //    ChargerPrintf(" Battery 0 chargeing over by time ...\r\n");
        //    return 1;
        //}
    }
    return 0;
}


/************************************************************** 
    标准充电方式 使用0.1C的电流充电 充电时间15h
**************************************************************/
INT8U Bat1_NormalCharge(void)
{
    INT16U Charge1_CCR;

    // 确认电池处于充电状态
    if(  Bat1_Status == Battery_Normal_Charge  )
    {
        // 获取实时检测结果
        GetChargeMeasure();

        // 这里设置的电流是标准充电电流
        Charge1_CCR = pid_Controller(Bat1_Cur_Nor_CHARGE, Bat1_Cur, &pidData1);      
        if(Charge1_CCR > 900) 
            Charge1_CCR = 900;

        TIM2->CCR2 = Charge1_CCR;

        // 如果电池电压过高 则停止
        if( Batt1_Vol_Seconds > BAT_Vol_CHARGE_MAX )
        {
            ChargerPrintf("Battery 1 chargeing over by vol ...\r\n");
            TIM2->CCR2 = 0; // 停止充电
            return 1;
        }
        // 如果充电时间到达 则停止
        if( Bat1_Charge_Discharge_Time_Count > BAT_NOR_CHARGE_MAX_TIME )
        {
            ChargerPrintf("Battery 1 chargeing over by time ...\r\n");
            TIM2->CCR2 = 0; // 停止充电
            return 1;
        }
    }
    return 0;
}

/************************************************************** 
    标准放电
**************************************************************/
INT8U Bat0_DisCharge()
{
    INT16U DisCharge0_CCR;

    // 获取实时检测结果
    GetChargeMeasure();

    // 这里设置的电流是标准充电电流
    DisCharge0_CCR = pid_Controller(Bat0_Cur_DisCharge, Bat0_Cur, &pidData0);      
    if(DisCharge0_CCR > 900) 
        DisCharge0_CCR = 900;
    
    TIM3->CCR1 = DisCharge0_CCR;

    // 如果电池电压到达截止充电电压 则停止
    if( Batt0_Vol_Seconds < BAT_DISCHARGE_STOP_VOL )
    {
        ChargerPrintf(" Battery 0 discharge over by vol ...\r\n");
        TIM3->CCR1 = 0; // 停止放电
        return 1;
    }
    // 如果电压大于阀值，则表示电池被取出
    else if( Batt0_Vol_Seconds > BAT_Vol_CHARGE_MAX )
    {
        ChargerPrintf(" Battery 0 have been take off  ...\r\n");
        TIM3->CCR1 = 0; // 停止放电
        return 1;
    }

    return 0;
}

void Bat0_Discharge_Machine(void)
{
    INT8U TempStatus;
    static INT8U timecount = 0;

    switch(Bat0_DischargeMachine)
    {

        case Battery_Init:
            display_str(0,2,"  Input test ");
            Bat0_DischargeMachine = Battery_Insert_Test;
        break;
			
        // 检测是否有电池插入状态 返回1 代表有电池被插入
        case Battery_Insert_Test :
            //每隔1S钟才检测一次	
            if( timecount <30 )
            {
                timecount++;
                break;
            }
            else timecount = 0;

            TempStatus = Bat0_Insert_Test();
            if( 1 == TempStatus )
            {
                Bat0_DischargeMachine = Battery_Discharging;  
                Bat0_Status= Battery_DisCharge;
                Bat0_Charge_Discharge_Capacity = 0;
                Bat0_Charge_Discharge_Time_Count = 0;       
                ChargerPrintf(" Battery 0 have been inserted  ...\r\n"); 
                ChargerPrintf(" Battery 0 start to discharge  ...\r\n"); 
                Batt_Vol_Accumulate(1);
            }
            else
                display_str(0,2,"  Not input   ");	
            break;

        case Battery_Discharging:
            Led0ChargeStat = GreenFlash20HZ;
            TempStatus = Bat0_DisCharge();
            if( 1 == TempStatus )
            {
                Bat0_DischargeMachine = Battery_Wait_For_Remove;
                Bat0_Status= Battery_Discharge_Over;
                ChargerPrintf(" Battery 0 discharge over ,wait take off ...\r\n"); 
            }   
            break;

        case Battery_Wait_For_Remove:
            Led0ChargeStat = GreenOn;
            //每隔3S钟才检测一次	
            if( timecount <30 )
            {
                timecount++;
                break;
            }
            else timecount = 0;					
            TempStatus = Bat0_Insert_Test();
            if( 0 == TempStatus )
            {
                Bat0_DischargeMachine = Battery_Insert_Test; 
                Bat0_Charge_Discharge_Capacity = 0;
                Bat0_Charge_Discharge_Time_Count = 0;
                ChargerPrintf(" Battery 0 have been take off  ...\r\n"); 	
            }		
            break;

        default:
        break;

    }
}


/************************************************************** 
    标准放电
**************************************************************/
INT8U Bat1_DisCharge()
{
    INT16U DisCharge1_CCR;

    // 获取实时检测结果
    GetChargeMeasure();

    // 这里设置的电流是标准充电电流
    DisCharge1_CCR = pid_Controller(Bat1_Cur_DisCharge, Bat1_Cur, &pidData1);      
    if(DisCharge1_CCR > 900) 
        DisCharge1_CCR = 900;

    TIM3->CCR2 = DisCharge1_CCR;

    // 如果电池电压到达截止充电电压 则停止
    if( Batt1_Vol_Seconds < BAT_DISCHARGE_STOP_VOL )
    {
        ChargerPrintf(" Battery 1 discharge over by vol ...\r\n");
        TIM3->CCR2 = 0; // 停止放电
        return 1;
    }
    // 如果电压大于阀值，则表示电池被取出
    else if( Batt1_Vol_Seconds > BAT_Vol_CHARGE_MAX )
    {
        ChargerPrintf(" Battery 1 have been take off  ...\r\n");
        TIM3->CCR1 = 0; // 停止放电
        return 1;
    }
    return 0;
}


void Bat1_Discharge_Machine(void)
{
    INT8U TempStatus;
    static INT8U timecount = 0;

    switch(Bat1_DischargeMachine)
    {
    
        case Battery_Init:
            display_str(0,4,"  Input test ");
            Bat1_DischargeMachine = Battery_Insert_Test;
            break;
			    
        // 检测是否有电池插入状态 返回1 代表有电池被插入
        case Battery_Insert_Test :
            //每隔1S钟才检测一次	
            if( timecount <30 )
            {
                timecount++;
                break;
            }
            else timecount = 0;

            TempStatus = Bat1_Insert_Test();
            if( 1 == TempStatus )
            {
                Bat1_DischargeMachine = Battery_Discharging;  
                Bat1_Status= Battery_DisCharge;
                Bat1_Charge_Discharge_Capacity = 0;
                Bat1_Charge_Discharge_Time_Count = 0;   
                ChargerPrintf(" Battery 1 have been inserted  ...\r\n"); 
                ChargerPrintf(" Battery 1 start to discharge  ...\r\n"); 
                Batt_Vol_Accumulate(1);
            }
			  else
					display_str(0,4,"  Not input   ");		
            break;

        case Battery_Discharging:
            Led1ChargeStat = GreenFlash20HZ;
            TempStatus = Bat1_DisCharge();
            if( 1 == TempStatus )
            {
                Bat1_DischargeMachine = Battery_Wait_For_Remove;
                Bat1_Status= Battery_Discharge_Over;
                ChargerPrintf(" Battery 1 discharge over ,wait take off ...\r\n"); 
            }   
            break;

        case Battery_Wait_For_Remove:
            Led1ChargeStat = GreenOn;
            //每隔3S钟才检测一次	
            if( timecount <30 )
            {
                timecount++;
                break;
            }
            else timecount = 0;								
            TempStatus = Bat1_Insert_Test();
            if( 0 == TempStatus )
            {
                Bat1_DischargeMachine = Battery_Insert_Test; 
                Bat1_Charge_Discharge_Capacity = 0;
                Bat1_Charge_Discharge_Time_Count = 0;                       
                ChargerPrintf(" Battery 1 have been take off  ...\r\n"); 
            }		
            break;

        default:
        break;

    }
}

/*********************************************************************************************************
** 函数名称: Bat0_RapidCharge
** 功能描述: 快速充电函数
** 
** 预充电流0.2C，预充最长时间60min，到达最长时间如果电压
** 还没有达到预充截止电压，则停止充电
** 
** 快速充电电流0.4C，开始快充时设置5min时间为不检测负
** 压阶段，快充最长时间210min，达到最长时间如果没有出
** 现负压，则停止充电
** 
** 总的快充最长时间为270min，到达时间则停止充电
********************************************************************************************************/
INT8U Bat0_RapidCharge(void)  //快速充电模式
{
    static INT32U timecount,timeold;
    static INT16U bat_vol_top = 0;
    INT16U Charge0_CCR;
    static INT8S delt_v[delt_v_counts]; // 负压检测数组
    INT8U i;
    INT8S sum = 0;
        
    // 获取实时检测结果
    GetChargeMeasure();
            
    switch(Bat0_Rapid_Charge_StateMachine)
    {
        case Battery_Init:
        {
            timecount = timeold = SoftTimer ; // 清零计数器
        }
        break;
            
        case Battery_Precharge_Mode:
        {
            Led0ChargeStat = RedFlash10HZ;

            // 这里设置的预冲电流
            Charge0_CCR = pid_Controller(Bat0_Cur_Pre_CHARGE, Bat0_Cur, &pidData0);      
            if(Charge0_CCR > 900) 
                Charge0_CCR = 900;

            TIM2->CCR1 = Charge0_CCR;

            Bat0_Status = Battery_Fast_Charge ;

            timeold = SoftTimer;
            if( timeold - timecount > 0x6DDD00 ) //sixty minutes
            {
                // 到达预充最长时间60min，还没到达预充截止电压，则停止充电
                if( Batt0_Vol_Seconds < BAT_VOL_PRE_CHARGE_OVER )
                {
                    TIM2->CCR1 = 0;	
                    Bat0_Rapid_Charge_StateMachine = Battery_Charge_Over_Mode;
                    ChargerPrintf(" Battery 0 pre -charge over by the bad voltage ...\r\n");
                }
            }

            // 如果电池电压到达预冲截止电压则跳转
            if( Batt0_Vol_Seconds > BAT_VOL_PRE_CHARGE_OVER )
            {
                // 跳转到不监控负压的快充模式
                Bat0_Rapid_Charge_StateMachine = Battery_Not_Monitor_Mode;
                timecount = timeold = SoftTimer ; // 清零计数器
                ChargerPrintf(" Battery 0 precharge over ...\r\n");
                ChargerPrintf(" Battery 0 jump to not monitor charge  ...\r\n");
            }
        }
        break;

        case Battery_Not_Monitor_Mode:
        {
            Led0ChargeStat = RedFlash20HZ;

            // 快充电流
            Charge0_CCR = pid_Controller(Bat0_Cur_Rapid_CHARGE, Bat0_Cur, &pidData0);      
            if(Charge0_CCR > 900) 
                Charge0_CCR = 900;

            TIM2->CCR1 = Charge0_CCR;	

            // 如果电池电压过高 则停止充电
            if( Batt0_Vol_Seconds > BAT_Vol_CHARGE_MAX )
            {
                TIM2->CCR1 = 0;	            
                Bat0_Rapid_Charge_StateMachine = Battery_Charge_Over_Mode;
                ChargerPrintf(" Battery 0 rapid charge over by voltage ...\r\n");
            }

            // 这里延迟5分钟后开始检测负压，其实还有更好的一个
            // 办法是等待电压上升到一个阀门值，再去开始检测负压
            timeold = SoftTimer;
            if( timeold - timecount > 0x927C0 ) //five minutes
            {
                Bat0_Rapid_Charge_StateMachine = Battery_Monitor_Charge_Mode;	
                // 初始化负压检测数组
                for( i=0; i<delt_v_counts; i++)
                {
                    delt_v[i] = -1;
                }                
                timecount = timeold = SoftTimer ; // 清零计数器
                ChargerPrintf(" Battery 0 not monitor charge over ...\r\n");
                ChargerPrintf(" Battery 0 jump to monitor charge  ...\r\n");
            }

        }	
        break;

        case Battery_Monitor_Charge_Mode:
        {

            Led0ChargeStat = RedFlash20HZ;

            // 这里设置的快充电流
            Charge0_CCR = pid_Controller(Bat0_Cur_Rapid_CHARGE, Bat0_Cur, &pidData0);      
            if(Charge0_CCR > 900) 
                Charge0_CCR = 900;

            TIM2->CCR1 = Charge0_CCR;	

            // 如果电池电压过高 则停止充电
            if( Batt0_Vol_Seconds > BAT_Vol_CHARGE_MAX )
            {
                TIM2->CCR1 = 0;	
                Bat0_Rapid_Charge_StateMachine = Battery_Charge_Over_Mode;
                ChargerPrintf(" Battery 0 rapid - charge over by bad voltage ...\r\n");
            }

            timeold = SoftTimer;
            if( timeold - timecount > 0x1808580 ) //210 minutes
            {
                // 到达快充最长时间90min，还没出现负压，则停止充电
                TIM2->CCR1 = 0;	
                Bat0_Rapid_Charge_StateMachine = Battery_Charge_Over_Mode;
                ChargerPrintf(" Battery 0 rapid -charge over by the time ...\r\n");
            }
            
            // 检测负压/////////////////////////////////////////////////////
            // 获得最高电压值，采用秒值运算
            if( Batt0_Vol_Seconds > bat_vol_top )
            {
                bat_vol_top = Batt0_Vol_Seconds;
            }

            //向左 移动数组的数值
            for( i=0; i<delt_v_counts -1; i++)
            {
                delt_v[i] = delt_v[i+1];
            }      

            // 出现负压，填充1，否则填充-1                        
            if( bat_vol_top - Batt0_Vol_Seconds > delt_v_value )
                delt_v[delt_v_counts-1] = 1; 
            else 
                delt_v[delt_v_counts-1] = -1;

            //  计算斜率总和
            for( i=0; i<delt_v_counts; i++)
            {
                sum += delt_v[i] ;
            }      

             
            if( sum > delt_v_counts/2 )
            {
                // 跳转到充电结束
                Bat0_Rapid_Charge_StateMachine = Battery_Charge_Over_Mode;
                TIM2->CCR1 = 0;	
                timecount = 0;
                ChargerPrintf(" Battery 0 rapid - charge over by delt-V ...\r\n");
            }


        }
        break;

        case Battery_Charge_Over_Mode:
        {
            Led0ChargeStat = RedFlash2HZ;
            return 1;
        }

        default:
        break;	
    }

    // 总的快速充电时间
    if( Bat0_Charge_Discharge_Time_Count > 0x1EE6280) // 270min
    {
        TIM2->CCR1 = 0;	
        Bat0_Rapid_Charge_StateMachine = Battery_Charge_Over_Mode;
        ChargerPrintf(" Battery 0 rapid - charge over by total time ...\r\n");
    }
        
    return 0;
}


/*********************************************************************************************************
** 函数名称: Bat1_RapidCharge
** 功能描述: 快速充电函数
** 
** 预充电流0.2C，预充最长时间60min，到达最长时间如果电压
** 还没有达到预充截止电压，则停止充电
** 
** 快速充电电流0.4C，开始快充时设置5min时间为不检测负
** 压阶段，快充最长时间210min，达到最长时间如果没有出
** 现负压，则停止充电
** 
** 总的快充最长时间为150min，到达时间则停止充电
********************************************************************************************************/
INT8U Bat1_RapidCharge(void)  //快速充电模式
{
    static INT32U timecount ,timeold;
    static INT16U bat_vol_top = 0;
    INT16U Charge1_CCR;
    static INT8S delt_v[delt_v_counts]; // 负压检测数组
    INT8U i;
    INT8S sum = 0;
        
    // 获取实时检测结果
    GetChargeMeasure();
            
    switch(Bat1_Rapid_Charge_StateMachine)
    {
        case Battery_Init:
        {
            timecount = timeold = SoftTimer ; // 清零计数器
        }
        break;
        
        case Battery_Precharge_Mode:
        {
            Led1ChargeStat = RedFlash10HZ;

            // 这里设置的预冲电流
            Charge1_CCR = pid_Controller(Bat1_Cur_Pre_CHARGE, Bat1_Cur, &pidData1);      
            if(Charge1_CCR > 900) 
                Charge1_CCR = 900;

            TIM2->CCR2 = Charge1_CCR;

            Bat1_Status = Battery_Fast_Charge ;

            timeold = SoftTimer;
            if( timeold - timecount > 0x6DDD00 ) //sixty minutes
            {
                // 到达预充最长时间60min，还没到达预充截止电压，则停止充电
                if( Batt1_Vol_Seconds < BAT_VOL_PRE_CHARGE_OVER )
                {
                    TIM2->CCR2 = 0;	
                    Bat1_Rapid_Charge_StateMachine = Battery_Charge_Over_Mode;
                    ChargerPrintf(" Battery 1 pre -charge over by the bad voltage ...\r\n");
                }
            }            

            // 如果电池电压到达预冲截止电压则跳转
            if( Batt1_Vol_Seconds > BAT_VOL_PRE_CHARGE_OVER )
            {
                // 跳转到不监控负压的快充模式
                Bat1_Rapid_Charge_StateMachine = Battery_Not_Monitor_Mode;
                timecount = timeold = SoftTimer ; // 清零计数器
                ChargerPrintf(" Battery 1 precharge over ...\r\n");
                ChargerPrintf(" Battery 1 jump to not monitor charge  ...\r\n");
            }
        }
        break;

        case Battery_Not_Monitor_Mode:
        {
            Led1ChargeStat = RedFlash20HZ;

            // 快充电流
            Charge1_CCR = pid_Controller(Bat1_Cur_Rapid_CHARGE, Bat1_Cur, &pidData1);      
            if(Charge1_CCR > 900) 
                Charge1_CCR = 900;

            TIM2->CCR2 = Charge1_CCR;	

            // 如果电池电压过高 则停止充电
            if( Batt1_Vol_Seconds > BAT_Vol_CHARGE_MAX )
            {
                Bat1_Rapid_Charge_StateMachine = Battery_Charge_Over_Mode;
                ChargerPrintf(" Battery 1 rapid - charge over by voltage ...\r\n");
                TIM2->CCR2 = 0;	
            }

            // 这里延迟5分钟后开始检测负压，其实还有更好的一个
            // 办法是等待电压上升到一个阀门值，再去开始检测负压
            timeold = SoftTimer;
            if( timeold - timecount > 0x927C0 ) //ten minutes
            {
                Bat1_Rapid_Charge_StateMachine = Battery_Monitor_Charge_Mode;	
                // 初始化负压检测数组
                for( i=0; i<delt_v_counts; i++)
                {
                    delt_v[i] = -1;
                }                
                timecount = timeold = SoftTimer ; // 清零计数器
                ChargerPrintf(" Battery 1 not monitor charge over ...\r\n");
                ChargerPrintf(" Battery 1 jump to monitor charge  ...\r\n");
            }

        }	
        break;

        case Battery_Monitor_Charge_Mode:
        {
            Led1ChargeStat = RedFlash20HZ;

            // 这里设置的快充电流
            Charge1_CCR = pid_Controller(Bat1_Cur_Rapid_CHARGE, Bat1_Cur, &pidData1);      
            if(Charge1_CCR > 900) 
                Charge1_CCR = 900;

            TIM2->CCR2 = Charge1_CCR;	

            // 如果电池电压过高 则停止充电
            if( Batt1_Vol_Seconds > BAT_Vol_CHARGE_MAX )
            {
                Bat1_Rapid_Charge_StateMachine = Battery_Charge_Over_Mode;
                TIM2->CCR2 = 0;	
                ChargerPrintf(" Battery 1 rapid - charge over by voltage ...\r\n");
            }

            timeold = SoftTimer;
            if( timeold - timecount > 0x1808580 ) //210 minutes
            {
                // 到达快充最长时间90min，还没出现负压，则停止充电
                TIM2->CCR2 = 0;	
                Bat1_Rapid_Charge_StateMachine = Battery_Charge_Over_Mode;
                ChargerPrintf(" Battery 1 rapid -charge over by the time ...\r\n");
            }            

            // 检测负压/////////////////////////////////////////////////////
            // 获得最高电压值，采用秒值运算
            if( Batt1_Vol_Seconds > bat_vol_top )
            {
                bat_vol_top = Batt1_Vol_Seconds;
            }

            //向左 移动数组的数值
            for( i=0; i<delt_v_counts -1; i++)
            {
                delt_v[i] = delt_v[i+1];
            }      

            // 出现负压，填充1，否则填充-1                        
            if( bat_vol_top - Batt1_Vol_Seconds > delt_v_value )
                delt_v[delt_v_counts-1] = 1; 
            else 
                delt_v[delt_v_counts-1] = -1;

            //  计算斜率总和
            for( i=0; i<delt_v_counts; i++)
            {
                sum += delt_v[i] ;
            }      

             
            if( sum > delt_v_counts/2 )
            {
                // 跳转到充电结束
                timecount = 0;
                TIM2->CCR2 = 0;	
                Bat1_Rapid_Charge_StateMachine = Battery_Charge_Over_Mode;
                ChargerPrintf(" Battery 1 rapid - charge over by delt-V ...\r\n");
            }


        }
        break;

        case Battery_Charge_Over_Mode:
        {
            Led1ChargeStat = RedFlash2HZ;
            return 1;
        }

        default:
        break;	
    }
    
    // 总的快速充电时间
    if( Bat1_Charge_Discharge_Time_Count > 0x1EE6280) // 270min
    {
        TIM2->CCR2 = 0;	
        Bat1_Rapid_Charge_StateMachine = Battery_Charge_Over_Mode;
        ChargerPrintf(" Battery 1 rapid - charge over by total time ...\r\n");
    }
    return 0;
}

/**************************************************************************************×*****************
**                                 END OF FILE
********************************************************************************************************/
