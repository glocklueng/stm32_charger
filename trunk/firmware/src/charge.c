/****************************************Copyright (c)**************************************************
**                              �� �� �� �� �� �� �� С ��
**                                      ��˼�� ��̳
**                                     QQ Ⱥ: 7095643 
**
**                               http://www.mystm32.com/bbs
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: charge.c
**��   ��   ��: ����
**����޸�����: 2009��09��20�� 22:10
**��        ��: ���ܳ���� 
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: ����
** ��  ��: v0.01
** �ա���: 2009��09��20�� 22:10
** �衡��: ԭʼ�汾
**
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: ����
** ��  ��: v0.04
** �ա���: 2009��10��08�� 19:36
** �衡��: ��ɵ�ز����� ����Ƿ��ǿɳ���ؼ�� ��������� ��س��״̬��  ��׼���ģʽ���
**
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: ����
** ��  ��: v0.06
** �ա���: 2009��11��07�� 19:36
** �衡��: ���ӵ�طŵ繦�� ���ӿ��ٳ�繦�ܣ������ƣ�
**
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: ����
** ��  ��: v0.071
** �ա���: 2010��1��5�� 11:36
** �衡��: �޸ĳ��״̬�� �޸ĳ��������غ���
**
**------------------------------------------------------------------------------------------------------
**--------------��ǰ�汾��Ϣ----------------------------------------------------------------------------
** ������: ����
** ��  ��: v0.08
** �ա���: 2010��2��12�� 0:21
** �衡��: �޸����ͷŵ�һ��ʼʱ�Ĵ���������ƿ��ٳ������
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
volatile  INT32U  Bat0_Charge_Discharge_Time_Count;  // ��� 0 ���ʱ�������
volatile  INT32U  Bat1_Charge_Discharge_Time_Count;  // ��� 1 ���ʱ�������

FP32 Bat0_Charge_Discharge_Capacity ; // ��� 0 ����ŵ����� 
FP32 Bat1_Charge_Discharge_Capacity ; // ��� 1 ����ŵ�����

INT32U BAT_NOR_CHARGE_MAX_TIME;

/*****************************************************************************
    ���0 ���״̬�� �ȼ�����Ƿ���� Ȼ�������Ƿ��ǿɳ����
    Ȼ��ȴ��û�ѡ����ģʽ ����ֱ��ʹ��Ĭ�ϳ�緽ʽ���г��
*****************************************************************************/
void Bat0_State_Machine(INT8U mode)
{
    INT8U TempStatus;
    static INT8U timecount = 0;

    switch(Bat0_StateMachine)
    {
        case Battery_Wait_For_Remove:
            //ÿ��3S�Ӳż��һ��	
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
            //ÿ��3S�Ӳż��һ��	
            if( timecount <30 )
            {
                timecount++;
                break;
            }
            else timecount = 0;

            // ����Ƿ��е�ز���״̬ ����1 �����е�ر�����
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
            // ������Ƿ��ǿɳ����, ����1 ����ɳ����
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
                //����0 �õ�ز��ǿɳ���� �ܳ�
                Bat0_StateMachine = Battery_Wait_For_Remove;
                ChargerPrintf(" Battery 0 refuse charge,wait take off ...\r\n"); 
            }
            break;

        // ��ش��ڳ��״̬ �ȴ��û�ѡ���緽ʽ����һ��Ĭ�����ã�
        case Battery_Charge_Discharge:
            Bat0_Charge_Discharge_Choise = mode;
            if( Battery_Normal_Charge_Mode == Bat0_Charge_Discharge_Choise )
            {
                // ��׼���ǰ�ȷŵ�
                ChargerPrintf(" Battery 0 Normal Discharging ...\r\n"); 
                Bat0_Status = Battery_DisCharge;
                Bat0_StateMachine = Battery_Discharging;	
                Batt_Vol_Accumulate(1);
            }
            else if( Battery_Rapid_Charge_Mode == Bat0_Charge_Discharge_Choise )
            {
                Bat0_Status = Battery_Fast_Charge;
                // �ÿ��ٳ��ΪԤ��״̬
                Bat0_Rapid_Charge_StateMachine = Battery_Precharge_Mode;
                Bat0_StateMachine = Battery_Rapid_Charging;	
                Batt_Vol_Accumulate(1);
            }
            Bat0_Charge_Discharge_Time_Count = 0; 
            Bat0_Charge_Discharge_Capacity = 0;            
            break;

         case  Battery_Normal_Charging:
            // ��׼���ģʽ
            TempStatus = Bat0_NormalCharge(); 
            if(1 == TempStatus)
            {
                Bat0_Status = Battery_Charge_Over;
                Bat0_StateMachine = Battery_Wait_For_Remove;
                ChargerPrintf(" Battery 0 charge full ,wait take off ...\r\n"); 
            }		
            break;

        case Battery_Rapid_Charging:
            //���ٳ��ģʽ
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
                // �ŵ����ת����׼���
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
    ���1 ���״̬�� �ȼ�����Ƿ���� Ȼ�������Ƿ��ǿɳ����
    Ȼ��ȴ��û�ѡ����ģʽ ����ֱ��ʹ��Ĭ�ϳ�緽ʽ���г��
*****************************************************************************/
void Bat1_State_Machine(INT8U mode)
{
    INT8U TempStatus;
    static INT8U timecount = 0;

    switch(Bat1_StateMachine)
    {
        case Battery_Wait_For_Remove:
            //ÿ��3S�Ӳż��һ��	
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
            //ÿ��3S�Ӳż��һ��	
            if( timecount <30 )
            {
                timecount++;
                break;
            }
            else timecount = 0;

            // ����Ƿ��е�ز���״̬ ����1 �����е�ر�����
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
            // ������Ƿ��ǿɳ����, ����1 ����ɳ����
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
                //����0 �õ�ز��ǿɳ���� �ܳ�
                Bat1_StateMachine = Battery_Wait_For_Remove;
                ChargerPrintf(" Battery 1 refuse charge,wait take off ...\r\n"); 
            }
            break;

        // ��ش��ڳ��״̬ �ȴ��û�ѡ���緽ʽ����һ��Ĭ�����ã�
        case Battery_Charge_Discharge:
            Bat1_Charge_Discharge_Choise = mode;
            if( Battery_Normal_Charge_Mode == Bat1_Charge_Discharge_Choise )
            {
                // ��׼���ǰ�ȷŵ�
                ChargerPrintf(" Battery 1 Normal Discharging ...\r\n"); 
                Bat1_Status = Battery_DisCharge;
                Bat1_StateMachine = Battery_Discharging;		
                Batt_Vol_Accumulate(1);
            }
            else if( Battery_Rapid_Charge_Mode == Bat1_Charge_Discharge_Choise )
            {
                Bat1_Status = Battery_Fast_Charge;
                // �ÿ��ٳ��ΪԤ��״̬
                Bat1_Rapid_Charge_StateMachine = Battery_Precharge_Mode;                
                Bat1_StateMachine = Battery_Rapid_Charging;	
                Batt_Vol_Accumulate(1);
            }
            Bat1_Charge_Discharge_Capacity = 0;
            Bat1_Charge_Discharge_Time_Count = 0;   
            break;



         case  Battery_Normal_Charging:
            // ��׼���ģʽ
            TempStatus = Bat1_NormalCharge(); 
            if(1 == TempStatus)
            {
                Bat1_Status = Battery_Charge_Over;
                Bat1_StateMachine = Battery_Wait_For_Remove;
                ChargerPrintf(" Battery 1 charge full ,wait take off ...\r\n"); 
            }		
            break;

        case Battery_Rapid_Charging:
            //���ٳ��ģʽ
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
            		// �ŵ����ת����׼���
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
    // 2·��ع�ͬ��������
    BAT_Vol_CHARGE_MAX = 1800; // ��׼���ʱ��߽�ֹ��ѹ
    BAT_MAX_TEMPERATURE = 50; // ��س��ʱ��߽�ֹ�¶�
    BAT_MAX_RESISTANCE = 300; // �ɳ�����������ֵ
    BAT_NOR_CHARGE_MAX_TIME_H = 0x66F; // ��׼���ʱ��15h 15*3600*2 ��ÿ500us����һ��    
    BAT_NOR_CHARGE_MAX_TIME_L = 0xF300; // ��׼���ʱ��15h 15*3600*2 ��ÿ500us����һ��    
    BAT_INSERT_TEST_CCR = 300; // ������Ƿ����pwmռ�ձ�
    BAT_RECHARGE_TEST_CCR = 760; // ����Ƿ��ǿɳ����ʹ��pwmռ�ձ� 900ma����
    BAT_DISCHARGE_STOP_VOL = 1000; //ֹͣ�ŵ��ѹ
    BAT_VOL_PRE_CHARGE_OVER = 800; // Ԥ�������ѹ    
    BAT_VOL_RAPID_CHARGE_MAX = 1800;  // �����ߵ�ѹ    

    //ÿСʱ��ֵ��0x6DDD00
    //��׼0.1C�����Ҫʱ��0x6DDD000(16Сʱ)
    //��׼0.2C�����Ҫʱ��0x36EE800(8Сʱ)
    BAT_NOR_CHARGE_MAX_TIME = 0x36EE800;

    ReinitChargePara(0);
    ReinitChargePara(1);

}


void ReinitChargePara(  INT8U para_ptr)
{
    INT8U sta;

    if( 0 == para_ptr )
    {
        // ��eeprom��ȡ���0������
        sta = EE_ReadVariable(VirtAddVarTab[4], &Bat0_Set_Capacity);
        if( 1 == sta )
            Bat0_Set_Capacity = 1800; 

        if(Bat0_Set_Capacity > 1800)
            Bat0_Set_Capacity = 1800;

        // �� 0 ·��ز�������
        // Ĭ�ϵ������Ϊ1800 �ò����ڳ��ǰӦ����ʵ������޸�
        Bat0_Vol_Repair_Min  = 900;     // ��Ҫ�޸���ѹ����    
        Bat0_Vol_Charge_Min  = 1200;    // �ɳ���ѹ��Χ����


        Bat0_Cur_DisCharge   = Bat0_Set_Capacity * 0.1; // �ŵ����
        Bat0_Cur_Repair      = Bat0_Set_Capacity * 0.2; // �޸�����

        Bat0_Cur_Pre_CHARGE  = Bat0_Set_Capacity * 0.1; // Ԥ�����
        Bat0_Cur_Rapid_CHARGE = Bat0_Set_Capacity * 0.4; // ������
        Bat0_Cur_Add_CHARGE  = Bat0_Set_Capacity * 0.05; // �������
        Bat0_Cur_Nor_CHARGE  = Bat0_Set_Capacity * 0.2; // ��׼������

        Bat0_StateMachine = Battery_Init; 
        Bat0_Rapid_Charge_StateMachine = Battery_Precharge_Mode; //Ԥ��״̬
        Bat0_Charge_On_Off = 0; // 0 ���û�д��ڳ��״̬
        Bat0_Test_Insert = 0; // 0 ���û�в���
        Bat0_Charge_Discharge_Choise = Battery_Normal_Charge_Mode;  //Ĭ�ϵĳ��ģʽ
        Bat0_DischargeMachine = Battery_Init;  //�ŵ�״̬����ʼ��Ϊ1

        // ���1 ״̬��־λ
        Bat0_Status = Battery_Not_input;
    }
    else if( 1 == para_ptr )
    {
        // ��eeprom��ȡ���0������
        sta = EE_ReadVariable(VirtAddVarTab[5], &Bat1_Set_Capacity);
        if( 1 == sta )
            Bat1_Set_Capacity = 1800; 

        if(Bat1_Set_Capacity > 1800)
            Bat1_Set_Capacity = 1800;

        // �� 1 ·��ز�������
        //Bat1_Set_Capacity = 1800; 
        Bat1_Vol_Repair_Min  = 900;     // ��Ҫ�޸���ѹ����    
        Bat1_Vol_Charge_Min  = 1200;    // �ɳ���ѹ��Χ����
        Bat1_Vol_DisCharge_OVER = 1000; // �ŵ������ѹ
        Bat1_Vol_Rre_CHARGE_OVER = 1200; // Ԥ�������ѹ
        Bat1_Vol_Fast_CHARGE_MAX = 1800;  // �����ߵ�ѹ

        Bat1_Cur_DisCharge   = Bat1_Set_Capacity * 0.1; // �ŵ����
        Bat1_Cur_Repair      = Bat1_Set_Capacity * 0.2; // �޸�����
        Bat1_Cur_Pre_CHARGE  = Bat1_Set_Capacity * 0.1; // Ԥ�����
        Bat1_Cur_Rapid_CHARGE = Bat1_Set_Capacity * 0.4; // ������
        Bat1_Cur_Add_CHARGE  = Bat1_Set_Capacity * 0.05; // �������
        Bat1_Cur_Nor_CHARGE  = Bat1_Set_Capacity * 0.2; // ��׼������

        Bat1_StateMachine = Battery_Init; 
        Bat1_Charge_On_Off = 0; // 1 ���û�д��ڳ��״̬
        Bat1_Test_Insert = 0; // 1 ���û�в���
        Bat1_Charge_Discharge_Choise = Battery_Normal_Charge_Mode; //Ĭ�ϵĳ��ģʽ
        Bat1_DischargeMachine = Battery_Init;  //�ŵ�״̬����ʼ��Ϊ1    

        // ���1 ״̬��־λ
        Bat1_Status = Battery_Not_input;		   
    }
		
}

/*��ڲ���flag=1 ���У���������
��ڲ���flag=0 ���ûУ��������
*/
void GetChargeMeasure(void) 
{
    ADC_Covert_Value();
    Bat0_Temperature = ADC_ConvertedValue[0]*3300/4096;
    Bat1_Temperature = ADC_ConvertedValue[1]*3300/4096;

    //��������
    Bat0_Cur = Zero_current_bat0(ADC_ConvertedValue[2]*3300/4096);
    Bat1_Cur = Zero_current_bat1(ADC_ConvertedValue[3]*3300/4096);    

    if( Battery_Discharging == Bat0_DischargeMachine || Bat0_Status == Battery_DisCharge )  
    { 
        // �����������������
        if( Bat0_Cur < 8 ) Bat0_Cur = 0xffff;
        Bat0_Cur = 0xffff - Bat0_Cur;
    }
    else if( Bat0_Cur > 32768 ) Bat0_Cur = 0;
    
    if( Battery_Discharging == Bat1_DischargeMachine || Bat1_Status == Battery_DisCharge )
    { 
        // �����������������
        if( Bat1_Cur < 8 ) Bat1_Cur = 0xffff;
        Bat1_Cur = 0xffff - Bat1_Cur;
    }
    else if( Bat1_Cur > 32768 ) Bat1_Cur = 0;
    
    //��ѹ����
    Bat0_Vol = Zero_voltage_bat0( ADC_ConvertedValue[4]*3300/4096 );
    Bat1_Vol = Zero_voltage_bat1( ADC_ConvertedValue[5]*3300/4096 );
    Temperature= ((1.42 - ADC_ConvertedValue[6]*3.3/4096)*1000/4.35 + 25)*10; 
    Vref = ADC_ConvertedValue[7]*3300/4096;
}

/************************************************************** 
    ����Ƿ��е�طŲ������� ���һ����ʱ���pwm 
    ���û�в����� ��ô��⵽�ĵ�ѹӦ����3300mv
    �������3300mv  ��ô�����е�ز��� 
    �������õ��ǵ��� BAT_Vol_CHARGE_MAX
**************************************************************/
INT8U Bat0_Insert_Test(void)
{
    INT16U Bat0_Vol_temp;
        
    // ���pwm 
    TIM2->CCR1 = BAT_INSERT_TEST_CCR;

    // �ӳ�10ms��ʱ��,����һ�β�����Ҫ��ʱ����448us������һ�ֲ���16��
    SysDelay(30);

    // ��ȡ��ǰ�ļ������
    GetChargeMeasure();
    Bat0_Vol_temp = Bat0_Vol ;
    // �ر�pwm 
    TIM2->CCR1 = 0;
    SysDelay(20);

    // �������ѹ����3300mv ֤��û�е�ز���
    // ���� ����Ϊ�е�ز��� BAT_Vol_CHARGE_MAX ��׼���ʱ��߽�ֹ��ѹ
    if( Bat0_Vol_temp < BAT_Vol_CHARGE_MAX )
    {
        return 1;
    }
    return 0; 
}

INT8U Bat1_Insert_Test(void)
{
    INT16U Bat1_Vol_temp;

    // ���pwm 
    TIM2->CCR2 = BAT_INSERT_TEST_CCR;

    // �ӳ�10ms��ʱ��
    SysDelay(30);

    // ��ȡ��ǰ�ļ������
    GetChargeMeasure();
    Bat1_Vol_temp = Bat1_Vol ;

    // �ر�pwm 
    TIM2->CCR2 = 0;
    SysDelay(20);

    // �������ѹ����3300mv ֤��û�е�ز���
    // ���� ����Ϊ�е�ز��� BAT0_Vol_Nor_CHARGE_MAX ��׼���ʱ��߽�ֹ��ѹ
    if( Bat1_Vol_temp < BAT_Vol_CHARGE_MAX )
    {
        return 1;
    }
    return 0;
}

/************************************************************** 
    ��طŽ����� ������Ƿ�ɳ� 
    ��ⲻ���ʱ�ĵ�ص�ѹ v0
    �����ʱ�ĵ�ص�ѹ vc
    �������� i
    �������� r = (vc - v0)/i
**************************************************************/
INT8U Bat0_Recharge_Test(void)
{
    INT8U i;
    INT16U Resistance_tmp=0;
    INT16U charge_current_tmp;
			
    for(i=0;i<4;i++)
    {
		// ���pwm 
		TIM2->CCR1 = BAT_RECHARGE_TEST_CCR;
		//�ӳ�8ms��ʱ��
		SysDelay(16);

        // ��ȡ���ʱ��ص�ѹ�͵�������
        GetChargeMeasure();
        Bat0_ReChargeable_Vol = Bat0_Vol;
        charge_current_tmp = Bat0_Cur;

        // �ر�pwm 
        TIM2->CCR1 = 0;
        // �ӳ�8ms��ʱ�䣬�պ���adc��ֵ�ɼ�һ�ֵ�ʱ��
        SysDelay(4);

        GetChargeMeasure();
        Resistance_tmp += ( Bat0_ReChargeable_Vol  - Bat0_Vol )*1000 /charge_current_tmp; // ����Ŵ�1000�� 				
    }
    Bat0_Resistance = Resistance_tmp/4;
	ChargerPrintf(" Bat0_Resistance      %4d omh \r\n", Bat0_Resistance);	
    // ������������ BAT_MAX_RESISTANCE ���ɳ������������ ���ǿɳ����
    if( Bat0_Resistance < BAT_MAX_RESISTANCE )
    { 
        //ChargerPrintf(" Bat0_Resistance      %4d omh \r\n", Bat0_Resistance);
        return 1;
    }
    return 0;
}


/************************************************************** 
    ��طŽ����� ������Ƿ�ɳ� 
    ��ⲻ���ʱ�ĵ�ص�ѹ v0
    �����ʱ�ĵ�ص�ѹ vc
    �������� i
    �������� r = (vc - v0)/i
**************************************************************/
INT8U Bat1_Recharge_Test(void)
{
    INT8U i;
    INT16U Resistance_tmp=0;		
    INT16U charge_current_tmp;

    // ���pwm 
    for(i=0;i<4;i++)
    {
        // ���pwm 
        TIM2->CCR2 = BAT_RECHARGE_TEST_CCR;
        //�ӳ�8ms��ʱ��
        SysDelay(16);

        // ��ȡ���ʱ��ص�ѹ�͵�������
        GetChargeMeasure();
        Bat1_ReChargeable_Vol = Bat1_Vol;
        charge_current_tmp = Bat1_Cur;

        // �ر�pwm 
        TIM2->CCR2 = 0;
        // �ӳ�8ms��ʱ��
        SysDelay(4);

        GetChargeMeasure();
        Resistance_tmp += ( Bat1_ReChargeable_Vol - Bat1_Vol )*1000 /charge_current_tmp; // ����Ŵ�1000�� 				
    }

    // ����������/////////////////////////////////////////////
    Bat1_Resistance = Resistance_tmp/4;
    ChargerPrintf(" Bat1_Resistance      %4d omh \r\n", Bat1_Resistance);		
    // ������������ BAT_MAX_RESISTANCE ���ɳ������������ ���ǿɳ����
    if( Bat1_Resistance < BAT_MAX_RESISTANCE )
    { 
        return 1;
    }
    return 0;
}


/************************************************************** 
    ��׼��緽ʽ ʹ��0.1C�ĵ������ ���ʱ��15h
**************************************************************/
INT8U Bat0_NormalCharge()
{
    INT16U Charge0_CCR;

    // ȷ�ϵ�ش��ڳ��״̬
    if( Bat0_Status == Battery_Normal_Charge )
    {
        // ��ȡʵʱ�����
        GetChargeMeasure();

        // �������õĵ����Ǳ�׼������
        Charge0_CCR = pid_Controller(Bat0_Cur_Nor_CHARGE, Bat0_Cur, &pidData0);      
        if(Charge0_CCR > 900) 
            Charge0_CCR = 900;

        TIM2->CCR1 = Charge0_CCR;

        // �����ص�ѹ���� ��ֹͣ
        if( Batt0_Vol_Seconds > BAT_Vol_CHARGE_MAX )
        {
            ChargerPrintf(" Battery 0 chargeing over by vol ...\r\n");
            TIM2->CCR1 = 0; // ֹͣ���
            return 1;
        }
        // ������ʱ�䵽�� ��ֹͣ
        if( Bat0_Charge_Discharge_Time_Count > BAT_NOR_CHARGE_MAX_TIME )
            {
            ChargerPrintf(" Battery 0 chargeing over by time ...\r\n");
            TIM2->CCR1 = 0; // ֹͣ���
            return 1;
        }

        // �������¶ȹ��� ��ֹͣ
        //if( Bat0_Temperature > BAT_MAX_TEMPERATURE )
        //{
        //    ChargerPrintf(" Battery 0 chargeing over by time ...\r\n");
        //    return 1;
        //}
    }
    return 0;
}


/************************************************************** 
    ��׼��緽ʽ ʹ��0.1C�ĵ������ ���ʱ��15h
**************************************************************/
INT8U Bat1_NormalCharge(void)
{
    INT16U Charge1_CCR;

    // ȷ�ϵ�ش��ڳ��״̬
    if(  Bat1_Status == Battery_Normal_Charge  )
    {
        // ��ȡʵʱ�����
        GetChargeMeasure();

        // �������õĵ����Ǳ�׼������
        Charge1_CCR = pid_Controller(Bat1_Cur_Nor_CHARGE, Bat1_Cur, &pidData1);      
        if(Charge1_CCR > 900) 
            Charge1_CCR = 900;

        TIM2->CCR2 = Charge1_CCR;

        // �����ص�ѹ���� ��ֹͣ
        if( Batt1_Vol_Seconds > BAT_Vol_CHARGE_MAX )
        {
            ChargerPrintf("Battery 1 chargeing over by vol ...\r\n");
            TIM2->CCR2 = 0; // ֹͣ���
            return 1;
        }
        // ������ʱ�䵽�� ��ֹͣ
        if( Bat1_Charge_Discharge_Time_Count > BAT_NOR_CHARGE_MAX_TIME )
        {
            ChargerPrintf("Battery 1 chargeing over by time ...\r\n");
            TIM2->CCR2 = 0; // ֹͣ���
            return 1;
        }
    }
    return 0;
}

/************************************************************** 
    ��׼�ŵ�
**************************************************************/
INT8U Bat0_DisCharge()
{
    INT16U DisCharge0_CCR;

    // ��ȡʵʱ�����
    GetChargeMeasure();

    // �������õĵ����Ǳ�׼������
    DisCharge0_CCR = pid_Controller(Bat0_Cur_DisCharge, Bat0_Cur, &pidData0);      
    if(DisCharge0_CCR > 900) 
        DisCharge0_CCR = 900;
    
    TIM3->CCR1 = DisCharge0_CCR;

    // �����ص�ѹ�����ֹ����ѹ ��ֹͣ
    if( Batt0_Vol_Seconds < BAT_DISCHARGE_STOP_VOL )
    {
        ChargerPrintf(" Battery 0 discharge over by vol ...\r\n");
        TIM3->CCR1 = 0; // ֹͣ�ŵ�
        return 1;
    }
    // �����ѹ���ڷ�ֵ�����ʾ��ر�ȡ��
    else if( Batt0_Vol_Seconds > BAT_Vol_CHARGE_MAX )
    {
        ChargerPrintf(" Battery 0 have been take off  ...\r\n");
        TIM3->CCR1 = 0; // ֹͣ�ŵ�
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
			
        // ����Ƿ��е�ز���״̬ ����1 �����е�ر�����
        case Battery_Insert_Test :
            //ÿ��1S�Ӳż��һ��	
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
            //ÿ��3S�Ӳż��һ��	
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
    ��׼�ŵ�
**************************************************************/
INT8U Bat1_DisCharge()
{
    INT16U DisCharge1_CCR;

    // ��ȡʵʱ�����
    GetChargeMeasure();

    // �������õĵ����Ǳ�׼������
    DisCharge1_CCR = pid_Controller(Bat1_Cur_DisCharge, Bat1_Cur, &pidData1);      
    if(DisCharge1_CCR > 900) 
        DisCharge1_CCR = 900;

    TIM3->CCR2 = DisCharge1_CCR;

    // �����ص�ѹ�����ֹ����ѹ ��ֹͣ
    if( Batt1_Vol_Seconds < BAT_DISCHARGE_STOP_VOL )
    {
        ChargerPrintf(" Battery 1 discharge over by vol ...\r\n");
        TIM3->CCR2 = 0; // ֹͣ�ŵ�
        return 1;
    }
    // �����ѹ���ڷ�ֵ�����ʾ��ر�ȡ��
    else if( Batt1_Vol_Seconds > BAT_Vol_CHARGE_MAX )
    {
        ChargerPrintf(" Battery 1 have been take off  ...\r\n");
        TIM3->CCR1 = 0; // ֹͣ�ŵ�
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
			    
        // ����Ƿ��е�ز���״̬ ����1 �����е�ر�����
        case Battery_Insert_Test :
            //ÿ��1S�Ӳż��һ��	
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
            //ÿ��3S�Ӳż��һ��	
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
** ��������: Bat0_RapidCharge
** ��������: ���ٳ�纯��
** 
** Ԥ�����0.2C��Ԥ���ʱ��60min�������ʱ�������ѹ
** ��û�дﵽԤ���ֹ��ѹ����ֹͣ���
** 
** ���ٳ�����0.4C����ʼ���ʱ����5minʱ��Ϊ����⸺
** ѹ�׶Σ�����ʱ��210min���ﵽ�ʱ�����û�г�
** �ָ�ѹ����ֹͣ���
** 
** �ܵĿ���ʱ��Ϊ270min������ʱ����ֹͣ���
********************************************************************************************************/
INT8U Bat0_RapidCharge(void)  //���ٳ��ģʽ
{
    static INT32U timecount,timeold;
    static INT16U bat_vol_top = 0;
    INT16U Charge0_CCR;
    static INT8S delt_v[delt_v_counts]; // ��ѹ�������
    INT8U i;
    INT8S sum = 0;
        
    // ��ȡʵʱ�����
    GetChargeMeasure();
            
    switch(Bat0_Rapid_Charge_StateMachine)
    {
        case Battery_Init:
        {
            timecount = timeold = SoftTimer ; // ���������
        }
        break;
            
        case Battery_Precharge_Mode:
        {
            Led0ChargeStat = RedFlash10HZ;

            // �������õ�Ԥ�����
            Charge0_CCR = pid_Controller(Bat0_Cur_Pre_CHARGE, Bat0_Cur, &pidData0);      
            if(Charge0_CCR > 900) 
                Charge0_CCR = 900;

            TIM2->CCR1 = Charge0_CCR;

            Bat0_Status = Battery_Fast_Charge ;

            timeold = SoftTimer;
            if( timeold - timecount > 0x6DDD00 ) //sixty minutes
            {
                // ����Ԥ���ʱ��60min����û����Ԥ���ֹ��ѹ����ֹͣ���
                if( Batt0_Vol_Seconds < BAT_VOL_PRE_CHARGE_OVER )
                {
                    TIM2->CCR1 = 0;	
                    Bat0_Rapid_Charge_StateMachine = Battery_Charge_Over_Mode;
                    ChargerPrintf(" Battery 0 pre -charge over by the bad voltage ...\r\n");
                }
            }

            // �����ص�ѹ����Ԥ���ֹ��ѹ����ת
            if( Batt0_Vol_Seconds > BAT_VOL_PRE_CHARGE_OVER )
            {
                // ��ת������ظ�ѹ�Ŀ��ģʽ
                Bat0_Rapid_Charge_StateMachine = Battery_Not_Monitor_Mode;
                timecount = timeold = SoftTimer ; // ���������
                ChargerPrintf(" Battery 0 precharge over ...\r\n");
                ChargerPrintf(" Battery 0 jump to not monitor charge  ...\r\n");
            }
        }
        break;

        case Battery_Not_Monitor_Mode:
        {
            Led0ChargeStat = RedFlash20HZ;

            // ������
            Charge0_CCR = pid_Controller(Bat0_Cur_Rapid_CHARGE, Bat0_Cur, &pidData0);      
            if(Charge0_CCR > 900) 
                Charge0_CCR = 900;

            TIM2->CCR1 = Charge0_CCR;	

            // �����ص�ѹ���� ��ֹͣ���
            if( Batt0_Vol_Seconds > BAT_Vol_CHARGE_MAX )
            {
                TIM2->CCR1 = 0;	            
                Bat0_Rapid_Charge_StateMachine = Battery_Charge_Over_Mode;
                ChargerPrintf(" Battery 0 rapid charge over by voltage ...\r\n");
            }

            // �����ӳ�5���Ӻ�ʼ��⸺ѹ����ʵ���и��õ�һ��
            // �취�ǵȴ���ѹ������һ������ֵ����ȥ��ʼ��⸺ѹ
            timeold = SoftTimer;
            if( timeold - timecount > 0x927C0 ) //five minutes
            {
                Bat0_Rapid_Charge_StateMachine = Battery_Monitor_Charge_Mode;	
                // ��ʼ����ѹ�������
                for( i=0; i<delt_v_counts; i++)
                {
                    delt_v[i] = -1;
                }                
                timecount = timeold = SoftTimer ; // ���������
                ChargerPrintf(" Battery 0 not monitor charge over ...\r\n");
                ChargerPrintf(" Battery 0 jump to monitor charge  ...\r\n");
            }

        }	
        break;

        case Battery_Monitor_Charge_Mode:
        {

            Led0ChargeStat = RedFlash20HZ;

            // �������õĿ�����
            Charge0_CCR = pid_Controller(Bat0_Cur_Rapid_CHARGE, Bat0_Cur, &pidData0);      
            if(Charge0_CCR > 900) 
                Charge0_CCR = 900;

            TIM2->CCR1 = Charge0_CCR;	

            // �����ص�ѹ���� ��ֹͣ���
            if( Batt0_Vol_Seconds > BAT_Vol_CHARGE_MAX )
            {
                TIM2->CCR1 = 0;	
                Bat0_Rapid_Charge_StateMachine = Battery_Charge_Over_Mode;
                ChargerPrintf(" Battery 0 rapid - charge over by bad voltage ...\r\n");
            }

            timeold = SoftTimer;
            if( timeold - timecount > 0x1808580 ) //210 minutes
            {
                // �������ʱ��90min����û���ָ�ѹ����ֹͣ���
                TIM2->CCR1 = 0;	
                Bat0_Rapid_Charge_StateMachine = Battery_Charge_Over_Mode;
                ChargerPrintf(" Battery 0 rapid -charge over by the time ...\r\n");
            }
            
            // ��⸺ѹ/////////////////////////////////////////////////////
            // �����ߵ�ѹֵ��������ֵ����
            if( Batt0_Vol_Seconds > bat_vol_top )
            {
                bat_vol_top = Batt0_Vol_Seconds;
            }

            //���� �ƶ��������ֵ
            for( i=0; i<delt_v_counts -1; i++)
            {
                delt_v[i] = delt_v[i+1];
            }      

            // ���ָ�ѹ�����1���������-1                        
            if( bat_vol_top - Batt0_Vol_Seconds > delt_v_value )
                delt_v[delt_v_counts-1] = 1; 
            else 
                delt_v[delt_v_counts-1] = -1;

            //  ����б���ܺ�
            for( i=0; i<delt_v_counts; i++)
            {
                sum += delt_v[i] ;
            }      

             
            if( sum > delt_v_counts/2 )
            {
                // ��ת��������
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

    // �ܵĿ��ٳ��ʱ��
    if( Bat0_Charge_Discharge_Time_Count > 0x1EE6280) // 270min
    {
        TIM2->CCR1 = 0;	
        Bat0_Rapid_Charge_StateMachine = Battery_Charge_Over_Mode;
        ChargerPrintf(" Battery 0 rapid - charge over by total time ...\r\n");
    }
        
    return 0;
}


/*********************************************************************************************************
** ��������: Bat1_RapidCharge
** ��������: ���ٳ�纯��
** 
** Ԥ�����0.2C��Ԥ���ʱ��60min�������ʱ�������ѹ
** ��û�дﵽԤ���ֹ��ѹ����ֹͣ���
** 
** ���ٳ�����0.4C����ʼ���ʱ����5minʱ��Ϊ����⸺
** ѹ�׶Σ�����ʱ��210min���ﵽ�ʱ�����û�г�
** �ָ�ѹ����ֹͣ���
** 
** �ܵĿ���ʱ��Ϊ150min������ʱ����ֹͣ���
********************************************************************************************************/
INT8U Bat1_RapidCharge(void)  //���ٳ��ģʽ
{
    static INT32U timecount ,timeold;
    static INT16U bat_vol_top = 0;
    INT16U Charge1_CCR;
    static INT8S delt_v[delt_v_counts]; // ��ѹ�������
    INT8U i;
    INT8S sum = 0;
        
    // ��ȡʵʱ�����
    GetChargeMeasure();
            
    switch(Bat1_Rapid_Charge_StateMachine)
    {
        case Battery_Init:
        {
            timecount = timeold = SoftTimer ; // ���������
        }
        break;
        
        case Battery_Precharge_Mode:
        {
            Led1ChargeStat = RedFlash10HZ;

            // �������õ�Ԥ�����
            Charge1_CCR = pid_Controller(Bat1_Cur_Pre_CHARGE, Bat1_Cur, &pidData1);      
            if(Charge1_CCR > 900) 
                Charge1_CCR = 900;

            TIM2->CCR2 = Charge1_CCR;

            Bat1_Status = Battery_Fast_Charge ;

            timeold = SoftTimer;
            if( timeold - timecount > 0x6DDD00 ) //sixty minutes
            {
                // ����Ԥ���ʱ��60min����û����Ԥ���ֹ��ѹ����ֹͣ���
                if( Batt1_Vol_Seconds < BAT_VOL_PRE_CHARGE_OVER )
                {
                    TIM2->CCR2 = 0;	
                    Bat1_Rapid_Charge_StateMachine = Battery_Charge_Over_Mode;
                    ChargerPrintf(" Battery 1 pre -charge over by the bad voltage ...\r\n");
                }
            }            

            // �����ص�ѹ����Ԥ���ֹ��ѹ����ת
            if( Batt1_Vol_Seconds > BAT_VOL_PRE_CHARGE_OVER )
            {
                // ��ת������ظ�ѹ�Ŀ��ģʽ
                Bat1_Rapid_Charge_StateMachine = Battery_Not_Monitor_Mode;
                timecount = timeold = SoftTimer ; // ���������
                ChargerPrintf(" Battery 1 precharge over ...\r\n");
                ChargerPrintf(" Battery 1 jump to not monitor charge  ...\r\n");
            }
        }
        break;

        case Battery_Not_Monitor_Mode:
        {
            Led1ChargeStat = RedFlash20HZ;

            // ������
            Charge1_CCR = pid_Controller(Bat1_Cur_Rapid_CHARGE, Bat1_Cur, &pidData1);      
            if(Charge1_CCR > 900) 
                Charge1_CCR = 900;

            TIM2->CCR2 = Charge1_CCR;	

            // �����ص�ѹ���� ��ֹͣ���
            if( Batt1_Vol_Seconds > BAT_Vol_CHARGE_MAX )
            {
                Bat1_Rapid_Charge_StateMachine = Battery_Charge_Over_Mode;
                ChargerPrintf(" Battery 1 rapid - charge over by voltage ...\r\n");
                TIM2->CCR2 = 0;	
            }

            // �����ӳ�5���Ӻ�ʼ��⸺ѹ����ʵ���и��õ�һ��
            // �취�ǵȴ���ѹ������һ������ֵ����ȥ��ʼ��⸺ѹ
            timeold = SoftTimer;
            if( timeold - timecount > 0x927C0 ) //ten minutes
            {
                Bat1_Rapid_Charge_StateMachine = Battery_Monitor_Charge_Mode;	
                // ��ʼ����ѹ�������
                for( i=0; i<delt_v_counts; i++)
                {
                    delt_v[i] = -1;
                }                
                timecount = timeold = SoftTimer ; // ���������
                ChargerPrintf(" Battery 1 not monitor charge over ...\r\n");
                ChargerPrintf(" Battery 1 jump to monitor charge  ...\r\n");
            }

        }	
        break;

        case Battery_Monitor_Charge_Mode:
        {
            Led1ChargeStat = RedFlash20HZ;

            // �������õĿ�����
            Charge1_CCR = pid_Controller(Bat1_Cur_Rapid_CHARGE, Bat1_Cur, &pidData1);      
            if(Charge1_CCR > 900) 
                Charge1_CCR = 900;

            TIM2->CCR2 = Charge1_CCR;	

            // �����ص�ѹ���� ��ֹͣ���
            if( Batt1_Vol_Seconds > BAT_Vol_CHARGE_MAX )
            {
                Bat1_Rapid_Charge_StateMachine = Battery_Charge_Over_Mode;
                TIM2->CCR2 = 0;	
                ChargerPrintf(" Battery 1 rapid - charge over by voltage ...\r\n");
            }

            timeold = SoftTimer;
            if( timeold - timecount > 0x1808580 ) //210 minutes
            {
                // �������ʱ��90min����û���ָ�ѹ����ֹͣ���
                TIM2->CCR2 = 0;	
                Bat1_Rapid_Charge_StateMachine = Battery_Charge_Over_Mode;
                ChargerPrintf(" Battery 1 rapid -charge over by the time ...\r\n");
            }            

            // ��⸺ѹ/////////////////////////////////////////////////////
            // �����ߵ�ѹֵ��������ֵ����
            if( Batt1_Vol_Seconds > bat_vol_top )
            {
                bat_vol_top = Batt1_Vol_Seconds;
            }

            //���� �ƶ��������ֵ
            for( i=0; i<delt_v_counts -1; i++)
            {
                delt_v[i] = delt_v[i+1];
            }      

            // ���ָ�ѹ�����1���������-1                        
            if( bat_vol_top - Batt1_Vol_Seconds > delt_v_value )
                delt_v[delt_v_counts-1] = 1; 
            else 
                delt_v[delt_v_counts-1] = -1;

            //  ����б���ܺ�
            for( i=0; i<delt_v_counts; i++)
            {
                sum += delt_v[i] ;
            }      

             
            if( sum > delt_v_counts/2 )
            {
                // ��ת��������
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
    
    // �ܵĿ��ٳ��ʱ��
    if( Bat1_Charge_Discharge_Time_Count > 0x1EE6280) // 270min
    {
        TIM2->CCR2 = 0;	
        Bat1_Rapid_Charge_StateMachine = Battery_Charge_Over_Mode;
        ChargerPrintf(" Battery 1 rapid - charge over by total time ...\r\n");
    }
    return 0;
}

/**************************************************************************************��*****************
**                                 END OF FILE
********************************************************************************************************/
