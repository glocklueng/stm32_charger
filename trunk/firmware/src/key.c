/****************************************Copyright (c)**************************************************
**                              �� �� �� �� �� �� �� С ��
**                                      ��˼�� ��̳
**                                     QQ Ⱥ: 7095643 
**
**                               http://www.mystm32.com/bbs
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: Key.c
**��   ��   ��: ����
**����޸�����: 2009��09��20�� 22:10
**��        ��: ���ܳ���� Key
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
** �衡��: ��ʷ
**
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �x��
** ��  ��: v0.071
** �ա���: 2010��1��5�� 11:36
** �衡��: ��ǰ�汾 ���Ӹ����˵��� cancel�˳��ĺ�������
**
**------------------------------------------------------------------------------------------------------
**--------------��ǰ�汾��Ϣ----------------------------------------------------------------------------
** ������: ����
** ��  ��: v0.08
** �ա���: 2010��2��10�� 00:27
** �衡��: ���ӵ����������
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/

#include "stm32f10x.h"
#include "global.h"
#include "Logic.h"
#include "Charge.h"
#include "para.h"
#include "key.h"
#include "menu.h"
#include "lcd5110.h"
#include "eeprom.h"

INT8U state_ptr,bit_ptr;
INT8U bitedit;
INT8U menu_ptr; //�˵�ָ��
INT8U para_ptr; //����ָ��

INT16U batt0_capacity = 1800;
INT16U batt1_capacity = 1800;
INT16U ModeLED      = 3;  // LED �ĺ��̵ƽ�λ����, �� LED ��λ���Ӵ��������
INT16U ModeLCDLight = 0;  // LCD ����ģʽ   0: ʡ��ģʽ; 1: ����ģʽ,���ⳣ�� 
INT16U ModeLaguage  = 1;  // ϵͳ����  0: ����;  1: Ӣ��
INT16U ModeLCDContrast = 0; // LCD �ĶԱȶ�����
INT16U ModeLCDShow = 1;   // LCD �ڳ��ʱ��ʾģʽ  0: ��̬ģʽ  1: ״̬�Զ���ҳ

/*******************************************************************************
* Function Name  : Delay
* Description    : Inserts a delay time.
* Input          : nCount: specifies the delay time length.
* Output         : None
* Return         : None
*******************************************************************************/
void Delay(INT32U nCount)
{
    for(; nCount != 0; nCount--);
}	

// 40 ����ɨ��һ��
INT8U ReadKey(void)
{
    INT32U KeyCode,i;
    static INT8U KeyDly[10];

   // ɨ���ֵ
    KeyCode = 0;


    Row0_0;
    Delay(1);
    if (~Col0) KeyCode = 9;
    if (~Col1) KeyCode = 8;
    if (~Col2) KeyCode = 7;
    Row0_1;

    Row1_0;
    Delay(1);
    if (~Col0) KeyCode = 6;
    if (~Col1) KeyCode = 5;
    if (~Col2) KeyCode = 4;
    Row1_1;


   // �γɰ���ɨ���룬��ȡ����ϼ�

    for (i = 1; i < 10; i++)
    {
        if (KeyCode != i)
        {
            KeyDly[i] = 0;
        }
	    else
	    {
		    if (KeyDly[i] < 20)
		    {
		        KeyDly[i]++;
		    }
		    else
		    {
		        KeyDly[i] = 19;
		    }
		    if ((KeyDly[i] == 2) || (KeyDly[i] == 19))
		    {
                if(StatBLWake) // ����˯��״̬
                    return(WAKE_UP_BL); // ʡ��ģʽ��������
                else
                    return(i);
		    }
	    }
    }
	return(0);
}


//������������
void process_key(INT8U keycode)
{
    INT16U const bit_val[] = {1,10,100,1000,10000};

    if (keycode == 0) 
        return;

    
	//screen_flg = 0; //��Ļ����

    
	//LCD5110_CLS();
	
    switch(menu_ptr)
    {
        case 0:  // ״̬��ʾ
            if (keycode == KEY_ENTER)
            {
                func_ptr = 0;
                menu_ptr = 1;
                sel_ptr = 0;
                disp_ptr = 0;
            }   
            break;
        case 1:  // ����ѡ��
	        if (keycode == KEY_ESC)
            {
                menu_ptr = 0;
                func_ptr = 0;
            }
            if (keycode == KEY_ENTER)
            {   
                func_ptr = sel_ptr;            
                menu_ptr = 2;
                sel_ptr = 0;
                disp_ptr = 0;				
				switch (func_ptr)
				{
                    case 2:
                        EE_ReadVariable(VirtAddVarTab[para_ptr+4], &para_val);
                        edit_val = para_val; 
						break;
					default:
						break;
				}
            }                
            if ((keycode == KEY_UP) || (keycode == KEY_LEFT))   // ѡ����һ����
            {
                if (sel_ptr > 0) sel_ptr--;					   	
                while (sel_ptr < disp_ptr) disp_ptr--;
            }    
            if ((keycode == KEY_DN) || (keycode == KEY_RIGHT))  // ѡ����һ����
            {
                if (sel_ptr < 5) sel_ptr++; // ����5������ѡ��˵�������					                        
                while (sel_ptr > disp_ptr + 1) disp_ptr++;
            }    
            break;

        case 2:  //�ӹ���ѡ��
            if (keycode == KEY_ESC)
            {
                               
                menu_ptr = 1;
                sel_ptr = func_ptr;
                disp_ptr = func_ptr;
                func1_ptr = 0;
		        //cursor_mode = 0; // ������»��߷�ʽ��ʾ: ��������Ϊ������ʾ���˳�ʱ�ָ�
		        //settime_flg = 0; // �˳�ʱ��ʱ�����ñ�־��0

        		bit_ptr = 0;     // ���µ�½�˳�ʱ��������0
        		//edit_val = 0;
                display_str(0,4,"            "); 
                	
            }  
            if (keycode == KEY_ENTER)
            {
               
                switch (func_ptr)
                {
                    case 0:  // �ӹ���ѡ��: ״̬����                  
                        bitedit = 0;
                        para_ptr = 0;
                        menu_ptr = 3;
                        func1_ptr = sel_ptr;
                        switch (func1_ptr)
                        {
                            case 0:     // ��ص�ѹ
                                break;
                                   
                            default:
                                break;
                        }  
                        break;
                    case 1:  // �ӹ���ѡ��: ��繦��
                        bitedit = 0;
                        para_ptr = 0;
                        menu_ptr = 3;
                        func1_ptr = sel_ptr;
                        break;
                    case 2:  // �ӹ���ѡ��: �����趨 
                        bit_ptr = 0;
                        edit_val = para_val;
                        menu_ptr = 3;             
                        break;
                    case 3:  // �ӹ���ѡ��: ��������
                        break;
                    case 4:  // �ӹ���ѡ��: ���Թ���
                        bitedit = 0;
                        para_ptr = 0;
                        menu_ptr = 3;
                        func1_ptr = sel_ptr;
                        break;
                    case 5:  // �ӹ���ѡ��: ϵͳ��Ϣ
                        break;
                    default:
                        break;
                }
            }

            
            if (keycode == KEY_UP)   // ѡ����һ����
            {
                if (func_ptr == 2)     // �����趨
                {
                    if (bitedit == 0)
                    {
                        if ((para_ptr > 0) && (func_ptr != 0))
                        { 
                            para_ptr--;
                            EE_ReadVariable(VirtAddVarTab[para_ptr+4], &para_val);
                            edit_val = para_val;
                        }                
                    }

                }
				else 
				{
	                if (sel_ptr > 0) sel_ptr--;
	                while (sel_ptr < disp_ptr) disp_ptr--;
				}
            }    
            if (keycode == KEY_DN)  // ѡ����һ����
            {
                switch (func_ptr)
                {
                    case 0:    // ״̬����
                        if (sel_ptr < 4) sel_ptr++;									    		   
                        break;
                    case 1:    // ��繦�� 
                        if (sel_ptr < 4) sel_ptr++;
                        break;
                    case 2:    // �����趨
                        if (bitedit == 0)
                        { 
                            if ((para_ptr < 6) && (func1_ptr == 0)) 
                            {
                                para_ptr++;
                                EE_ReadVariable(VirtAddVarTab[para_ptr+4], &para_val);
                                edit_val = para_val;
                            } 
                        }
                        break;
					case 3:                       
                        break;
					case 4:    // ���Թ���
                        if (sel_ptr < 1) sel_ptr++;
                        break;
                }
                while (sel_ptr > disp_ptr + 1) disp_ptr++;
            } 
            break;      

         case 3:     // �������
            if (keycode == KEY_ESC)
            {
            	switch(func_ptr)
            	{
            		case 0:
						break;
				
            		case 1:
						ChargeFuncExit();	//��繦���˳�ʱ�Ĵ���	
						break;
				
            		case 2:
                        EE_ReadVariable(VirtAddVarTab[para_ptr+4], &para_val);
						edit_val = para_val;	
						break;
                    case 3:
                        break;
                        
                    case 4:
                        Menu_times_count = 0;
                        break;

					default:
						break;
				
            	}
				menu_ptr = 2;			

            }
			
            if (keycode == KEY_ENTER)
            {
                if (func_ptr == 2)
                {
                    set_cursor(0xff,0xff); // ��겻��ʾ
	                if (bitedit) //λ��������
	                {
	                    if (edit_val) edit_val = para_val;
	                    else edit_val = para_val;
                        EE_WriteVariable(VirtAddVarTab[para_ptr+4], edit_val);
                        EE_ReadVariable(VirtAddVarTab[para_ptr+4], &para_val);					
	                    //set_para(para_ptr,edit_val);
	                    //get_para(para_ptr,&para_val);
	                    if (para_val) edit_val = 1;
	                    else edit_val = 0;
	                }
	                else  //��λ��������
	                {
	                    if (edit_val != para_val)
	                    {   
                            switch(para_ptr)
                            {
                                case 0:
                                case 1:
                                    if(edit_val <= 3000)
                                    {
                                        EE_WriteVariable(VirtAddVarTab[para_ptr+4], edit_val);
                                    }
                                    EE_ReadVariable(VirtAddVarTab[para_ptr+4], &para_val);
                                    ReinitChargePara(para_ptr); // �����趨�µĲ���	
                                    break;
                                case 2: // LED ģʽ
                                    if(edit_val <= 3)
                                    {
                                        EE_WriteVariable(VirtAddVarTab[para_ptr+4], edit_val);
                                    }
                                    EE_ReadVariable(VirtAddVarTab[para_ptr+4], &para_val);
                                    ModeLED = para_val;
                                    break;
                                case 3: // LCD ����ģʽ
                                    if((edit_val == 0) || (edit_val == 1))
                                    {
                                        EE_WriteVariable(VirtAddVarTab[para_ptr+4], edit_val);
                                    }
                                    EE_ReadVariable(VirtAddVarTab[para_ptr+4], &para_val);
                                    ModeLCDLight = para_val;
                                    break;
                                case 4: // ϵͳ����
                                    if(edit_val <= 1)
                                    {
                                        EE_WriteVariable(VirtAddVarTab[para_ptr+4], edit_val);
                                    }
                                    EE_ReadVariable(VirtAddVarTab[para_ptr+4], &para_val);
                                    ModeLaguage = para_val;
                                    break;
                                case 5: // LCD �Աȶ�
                                    if(edit_val <= 255)
                                    {
                                        EE_WriteVariable(VirtAddVarTab[para_ptr+4], edit_val);
                                    }
                                    EE_ReadVariable(VirtAddVarTab[para_ptr+4], &para_val);
                                    ModeLCDContrast = para_val;
                                    break;
                                case 6: // LCD �ڳ����״̬ʱ��ҳ
                                    if(edit_val <= 1)
                                    {
                                        EE_WriteVariable(VirtAddVarTab[para_ptr+4], edit_val);
                                    }
                                    EE_ReadVariable(VirtAddVarTab[para_ptr+4], &para_val);
                                    ModeLCDShow = para_val;
                                    break;

                                
                                default:
                                    break;
                            }
                            
                            //set_para(para_ptr,edit_val);
	                        //get_para(para_ptr,&para_val);
							if( para_val != edit_val ) edit_val = para_val;
							else edit_val = edit_val;
	                    }
	                } 
                }
                menu_ptr = 2;
            }   
            if (keycode == KEY_LEFT)
            {
                if (func_ptr == 2)
                {
                    if (bit_ptr < 4) bit_ptr++;
                    else bit_ptr = 0;
                }
            }
            if (keycode == KEY_RIGHT)
            {
                if (func_ptr == 2)
                {
                    if (bit_ptr > 0) bit_ptr--;
                    else bit_ptr = 4;
                }
            }
            if (keycode == KEY_UP)    // ��������
            {
                if (func_ptr == 2)
                {
                    if (bitedit) edit_val = 1;
                    else
                    {
                        if (0xffff - edit_val >= bit_val[bit_ptr])
                        {
                            edit_val += bit_val[bit_ptr];
                        }
                    }
                }
            }
            if (keycode == KEY_DN)    // ��������
            {
                if (func_ptr == 2)
                {
                    if (bitedit) edit_val = 0;
                    else
                    {
                        if (edit_val >= bit_val[bit_ptr])
                        {
                            edit_val -= bit_val[bit_ptr];
                        }
                        else
                        {
                            edit_val = 0;
                        }
                    }
                }
            }
            break;  
            
        default:
            break;
    }
}


void ChargeFuncExit(void)
{
    switch(func1_ptr)
    {
        case 0: 
            // ��׼��繦���˳�
            TIM2->CCR1 = 0; // ֹͣ���
            Bat0_Status = Battery_Charge_Over;
            // ״̬���ָ���Battery_Insert_Test ״̬
            Bat0_StateMachine = Battery_Init; 
            TIM2->CCR2 = 0; // ֹͣ���
            Bat1_Status = Battery_Charge_Over;
            // ״̬���ָ���Battery_Insert_Test ״̬
            Bat1_StateMachine = Battery_Init; 
            Bat0_Charge_Discharge_Capacity = 0;
            Bat1_Charge_Discharge_Capacity = 0;
            Bat0_Charge_Discharge_Time_Count = 0;
            Bat1_Charge_Discharge_Time_Count = 0;

            ChargerPrintf(" Exit from normal charge  ...\r\n"); 

            break;

        case 1:  // ���ٳ�繦���˳�
            TIM2->CCR1 = 0; // ֹͣ���
            Bat0_Status = Battery_Charge_Over;
            // ״̬���ָ���Battery_Insert_Test ״̬
            Bat0_StateMachine = Battery_Init; 
            TIM2->CCR2 = 0; // ֹͣ���
            Bat1_Status = Battery_Charge_Over;
            // ״̬���ָ���Battery_Insert_Test ״̬
            Bat1_StateMachine = Battery_Init; 
            Bat0_Charge_Discharge_Capacity = 0;
            Bat1_Charge_Discharge_Capacity = 0;
            Bat0_Charge_Discharge_Time_Count = 0;
            Bat1_Charge_Discharge_Time_Count = 0;

            ChargerPrintf(" Exit from normal charge  ...\r\n"); 		
            break;

        case 2: // ������蹦���˳�
            break;

        case 3: //�ŵ繦���˳����ָ���������
            TIM3->CCR1 = 0; // ֹͣ�ŵ�
            Bat0_Status = Battery_Discharge_Over;    
            Bat0_DischargeMachine = Battery_Init; // �ŵ�״̬����ʼ��Ϊ1			
            TIM3->CCR2 = 0; // ֹͣ�ŵ�
            Bat1_Status = Battery_Discharge_Over;
            Bat1_DischargeMachine = Battery_Init; // �ŵ�״̬����ʼ��Ϊ1
            ChargerPrintf(" Exit from discharge  ...\r\n"); 
            Bat0_Charge_Discharge_Capacity = 0;
            Bat1_Charge_Discharge_Capacity = 0;
            Bat0_Charge_Discharge_Time_Count = 0;
            Bat1_Charge_Discharge_Time_Count = 0;
            break;

        case 4:
            break;

        default:
            break;
    }
    Menu_times_count = 0;
    InitPID(); // PID ��ʼ��    
	 
}



/**************************************************************************************��*****************
**                                 END OF FILE
********************************************************************************************************/

