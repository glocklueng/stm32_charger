#include "stm32f10x.h"
#include "global.h"
#include "para.h"
#include "menu.h"
#include "lcd5110.h"
#include "adc.h"
#include "key.h"
#include "charge.h"
#include "string.h"
#include "logic.h"
#include "eeprom.h"

// ����ѡ��
INT8U   func_ptr,func1_ptr;   // ����ָ��,�ӹ���ָ��
INT8U   disp_ptr;             // ��һ��ʾ��ָ��
INT8U   sel_ptr;              // ��ǰѡ����ָ��
INT16U  para_val,edit_val;
INT8U   Menu_times_count;   // �˵�����������

#define bittst(x,y)     x[y >> 3] & (1 << (y & 7))

INT8S const menu[][15] = {
    "===ϵͳ����===",
    "===״̬����===",
    "===��繦��===",
    "===�����趨===",
    "===��������===",
    "===���Թ���===",
    "===�汾��Ϣ===",};

INT8S const menu0[][15] = {
    "  ״̬����    ",
    "  ��繦��    ",
    "  �����趨    ",
    "  ��������    ",
    "  ϵͳ����    ",
    "  �汾��Ϣ    ",
    "              ",};

INT8S const menu1[][15] = {
    "  ��ص�ѹ    ",
    "  ��ص���    ",
    "  ����¶�    ",
    "  STM32 Vref  ",
    "  оƬ�¶�    ",};

INT8S const menu2[][15] = {
    "���һ����    ",
    "��ض�����    ",
    "LED ��λ����  ",
    "LCD ����ģʽ  ",
    "ϵͳ��������  ",
    "LCD �Աȶ�    ",
    "�����ʾģʽ  ",};

INT8S const menu3[][15] = {
    "F0: P=     mAH",
    "F1: P=     mAH",
    "F2: M=        ",
    "F3: M=        ",
    "F4: M=        ",
    "F5: P=        ",
    "F6: M=        ",};

INT8S const menu4[][15] = {
    "  ��ѹ����    ",
    "  ��������    ",};

INT8S const menu5[][15] = {
    "  ��׼���    ",
    "  ���ٳ��    ",
    "  ������    ",
    "  ��طŵ�    ",
    "  �޸����    ",};

INT8U const para_dotx[] = 
{
    0,0,

};

// ����������
INT8U const mptr[] = {0,0,0,0,0,0,0,0,0,0,0,
                      1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                      2,2,2,2,2,2,2,2,2,2,2,2,2,2,
                      3,3,3,3,3,3,3,3,3,3,3,
                      4,4,4,4,4,4,4,4,4,4,4,4,4,4,
                      5,5,5,
                      6,6,6,6,
                      7,7,7,
                      8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
                      8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
                      9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
                      9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
                      10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
                      10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
                      11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
                      11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
                      12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,
                      12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,
                      13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
                      13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
                      14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,
                      14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,
                      14,14,14,14,14,14,14,14,14,14,
                      15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
                      16,16,16,16,16,16,16,16,16,16,16,16,
                      17,17,17,17,17,17,17,17};

INT16U mptr0[20];     // �������ݵĵ�һ�����ݱ��

INT8U const pdot[] = {0,0,0,0,0,0,0,0,0,0,0,                                // 11
                      3,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3,3,3,3,3,    // 25
                      2,2,2,2,2,2,2,2,2,2,2,2,2,3,                          // 14
                      3,3,3,1,3,3,3,3,1,1,1,                                // 11
                      0,0,0,0,2,0,2,1,0,0,2,0,0,0,                          // 14
                      0,0,0,                                                // 3
                      0,0,1,1,                                              // 4
                      0,0,0,                                                // 3
                      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                      3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
                      3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
                      3,3,3,3,3,3,3,3,3,3,
                      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                      0,0,0,0,0,0,0,0,0,0,0,0,
                      0,0,0,0,0,0,0,0,0};

INT8U const dbar[] = {14,11,8};



void display_menu(void)
{
    INT16U  i,j,dot;
    INT16U  cur_disp;
    INT8S  buff[17];
    INT8S  *p;
	INT32S  temp;

    if (menu_ptr == 0)  // ��һ��˵� -----  ״̬����
    { 
        set_cursor(0xff,0xff);       // ��겻��ʾ
        display_str(0,0,(INT8S *)"  �� ˼ ��    ");
        display_str(0,2,(INT8S *)"   mySTM32    ");
        display_str(0,4,(INT8S *)" ���ܳ����   ");
    }

    if (menu_ptr == 1)  // �ڶ���˵� ----- ����ѡ��
    {
        set_cursor(0xff,0xff);       // ��겻��ʾ
        p = (INT8S *)menu[0];
        display_str(0,0,p);
        j = 2;
        cur_disp = disp_ptr;
        p = (INT8S *)menu0[0];
        for (i = 0; i < 2; i++)
        {
            strcpy((char*)buff,(char *)(p + cur_disp * 15));
            if (cur_disp == sel_ptr)      // ��ǰ��ѡ��Ĺ������ü�ͷָʾ
            {
                buff[0] = '=';
                buff[1] = '>';
            }
            display_str(0,j,buff);
            cur_disp++;
            j = j + 2;
        }
    }

    if (menu_ptr == 2)   //  ������˵� ----- �ӹ���ѡ��
    {
        set_cursor(0xff,0xff); // ��겻��ʾ
        Led0ChargeStat = AllOff;
        Led1ChargeStat = AllOff;

        p = (INT8S *)menu[func_ptr+1];
        display_str(0,0,p);
		switch (func_ptr)
		{
            case 0: // ״̬����
                j = 2;
                cur_disp = disp_ptr;
                p = (INT8S *)menu1[0];
                for (i = 0; i < 2; i++)
                {
                    strcpy((char*)buff,(char *)(p + cur_disp * 15));
                    if (cur_disp == sel_ptr)      // ��ǰ��ѡ��Ĺ������ü�ͷָʾ
                    {
                        buff[0] = '=';
                        buff[1] = '>';
                    }
                    display_str(0,j,buff);
                    cur_disp++;
                    j = j + 2;
                }
                break;
            case 1: // ��繦��
                j = 2;
                cur_disp = disp_ptr;
                p = (INT8S *)menu5[0];
                for (i = 0; i < 2; i++)
                {
                    strcpy((char*)buff,(char *)(p + cur_disp * 15));
                    if (cur_disp == sel_ptr)      // ��ǰ��ѡ��Ĺ������ü�ͷָʾ
                    {
                        buff[0] = '=';
                        buff[1] = '>';
                    }
                    display_str(0,j,buff);
                    cur_disp++;
                    j = j + 2;
                }
                break;
            case 2: // �������
			    dot = para_dotx[para_ptr];
                
                // ��ʾ���
                if ((bit_ptr >= dot) || (dot == 0))
                {
                    set_cursor(10-bit_ptr,4);  // �����ʾ
                }
                else
                {
                    set_cursor(11-bit_ptr,4);  // �����ʾ
                }
                
                // ��ʾ����
                strcpy((char*)buff,(char*)menu3[para_ptr]);
                sprfu(&buff[6],edit_val,dot,5);
                display_str(0,2,buff);
                // ����˵��
                set_cursor(0xff,0xff); // ��겻��ʾ
                display_str(0,4,(INT8S *)menu2[para_ptr]);				

                break;
            case 4: // ���Թ���
                j = 2;
                cur_disp = disp_ptr;
                p = (INT8S *)menu4[0];
                for (i = 0; i < 2; i++)
                {
                    strcpy((char*)buff,(char*)(p + cur_disp * 15));
                    if (cur_disp == sel_ptr)      // ��ǰ��ѡ��Ĺ������ü�ͷָʾ
                    {
                        buff[0] = '=';
                        buff[1] = '>';
                    }
                    display_str(0,j,buff);
                    cur_disp++;
                    j = j + 2;
                }
                break;
            case 5: // ϵͳ��Ϣ
                display_str(0,2,(INT8S*)"Ӳ��: V1.20 ");
                display_str(0,4,(INT8S*)"���: V0.08"); 
                break;
                 
        }
    }

    if ( (menu_ptr == 3) || (menu_ptr == 4) )      // ���Ĳ�˵� ----- �������,  // �����˵� ----- ���ݱ༭
    {
        if (func_ptr == 0)   // ����״̬
        {
            switch(func1_ptr)
            {
                case 0:      // ��ص�ѹ
                    strcpy((char*)buff,"V1:         V ");
                    sprfi(&buff[5],Batt0_Vol_Seconds,3,4);
                    display_str(0,2,buff);
                    strcpy((char*)buff,"V2:         V ");
                    sprfi(&buff[5],Batt1_Vol_Seconds,3,4);
                    display_str(0,4,buff);
                    break;
                case 1:      // ��ص���
                    strcpy((char*)buff,"I1:         A ");
                    sprfi(&buff[5],Batt0_Cur_Seconds,3,4);
                    display_str(0,2,buff);
                    strcpy((char*)buff,"I2:         A ");
                    sprfi(&buff[5],Batt1_Cur_Seconds,3,4);
                    display_str(0,4,buff);
                    break;
                 case 2:      // ����¶�
                    strcpy((char*)buff,"T1:        V ");
                    sprfu(&buff[5],Batt0_Temperature_Seconds,3,4);
                    display_str(0,2,buff);
                    strcpy((char*)buff,"T2:        V ");
                    sprfu(&buff[5],Batt1_Temperature_Seconds,3,4);
                    display_str(0,4,buff);
                    break;
                 case 3:      // STM32 Vref
                    strcpy((char*)buff,"STM32 Vref:  ");
                    display_str(0,2,buff);
                    strcpy((char*)buff,"           V ");
                    sprfu(&buff[5],STM32_vref_Seconds,3,4);
                    display_str(0,4,buff);
                    break;
                 case 4:      // STM32 оƬ�¶�
                    strcpy((char*)buff,"оƬ�¶�:    ");
                    display_str(0,2,buff);
                    strcpy((char*)buff,"           C ");
						// ��ȥ10.9 ��ƫ��Ϊ�����¶�				
                    sprfu(&buff[7],STM32_Temperature_Seconds,1,3);
                    display_str(0,4,buff);
                    break;
                default:
                    break;

            }
        }
        if (func_ptr == 1)   // ��繦��
        {
            switch(func1_ptr)
            {
                case 0:      // ��׼���
                    Bat0_State_Machine(Battery_Normal_Charge_Mode);  //���0 ���״̬��             
                    Bat1_State_Machine(Battery_Normal_Charge_Mode);  //���1 ���״̬��

                    if( Menu_times_count == 10  )
                    {
                        memset(buff,' ',17);
                        buff[14]='\0';
                        if( Battery_Discharging == Bat0_StateMachine ||Battery_Normal_Charging== Bat0_StateMachine )
                        {
                            sprfu(&buff[0],Batt0_Vol_Seconds,0,4);
                            sprfu(&buff[5],Batt0_Cur_Seconds,0,4);
                            sprfu(&buff[10],(INT16U)Bat0_Charge_Discharge_Capacity,0,4);
                            display_str(0,2,buff);
                        }
                        else if( Battery_Wait_For_Remove == Bat0_StateMachine )
                        {
                            sprfu(&buff[0],Batt0_Vol_Seconds,0,4);
                            sprfu(&buff[5],0,0,4);
                            sprfu(&buff[10],(INT16U)Bat0_Charge_Discharge_Capacity,0,4);
                            display_str(0,2,buff);
                        }                        
                        
                        if( Battery_Discharging == Bat1_StateMachine  ||Battery_Normal_Charging== Bat1_StateMachine )
                        {                        
                            sprfu(&buff[0],Batt1_Vol_Seconds,0,4);
                            sprfu(&buff[5],Batt1_Cur_Seconds,0,4);
                            sprfu(&buff[10],(INT16U)Bat1_Charge_Discharge_Capacity,0,4);
                            display_str(0,4,buff);
                        }
                        else if( Battery_Wait_For_Remove == Bat1_StateMachine )
                        {
                            sprfu(&buff[0],Batt1_Vol_Seconds,0,4);
                            sprfu(&buff[5],0,0,4);
                            sprfu(&buff[10],(INT16U)Bat1_Charge_Discharge_Capacity,0,4);
                            display_str(0,4,buff);
                        }       
                        
                        Menu_times_count = 0;
                    }
                    else
                        Menu_times_count++;
                    break;
										
                case 1:      // ���ٳ��
                    Bat0_State_Machine(Battery_Rapid_Charge_Mode);  //���0 ���״̬��             
                    Bat1_State_Machine(Battery_Rapid_Charge_Mode);  //���1 ���״̬��

                    if( Menu_times_count == 10  )
                    {
                        memset(buff,' ',17);
                        buff[14]='\0';
                        if( Battery_Rapid_Charging == Bat0_StateMachine )
                        {
                            sprfu(&buff[0],Batt0_Vol_Seconds,0,4);
                            sprfu(&buff[5],Batt0_Cur_Seconds,0,4);
                            sprfu(&buff[10],(INT16U)Bat0_Charge_Discharge_Capacity,0,4);
                            display_str(0,2,buff);
                        }
                        else if( Battery_Wait_For_Remove == Bat0_StateMachine )
                        {
                            sprfu(&buff[0],Batt0_Vol_Seconds,0,4);
                            sprfu(&buff[5],0,0,4);
                            sprfu(&buff[10],(INT16U)Bat0_Charge_Discharge_Capacity,0,4);
                            display_str(0,2,buff);
                        }
                        
                        if( Battery_Rapid_Charging == Bat1_StateMachine )
                        {                        
                            sprfu(&buff[0],Batt1_Vol_Seconds,0,4);
                            sprfu(&buff[5],Batt1_Cur_Seconds,0,4);
                            sprfu(&buff[10],(INT16U)Bat1_Charge_Discharge_Capacity,0,4);
                            display_str(0,4,buff);
                        }
                        else if( Battery_Wait_For_Remove == Bat1_StateMachine )
                        {
                            sprfu(&buff[0],Batt1_Vol_Seconds,0,4);
                            sprfu(&buff[5],0,0,4);
                            sprfu(&buff[10],(INT16U)Bat1_Charge_Discharge_Capacity,0,4);
                            display_str(0,4,buff);
                        }                             
                        Menu_times_count = 0;
                    }
                    else
                        Menu_times_count++;
                    break;
										
                 case 2:      // ������
                    if( 0 == Menu_times_count )
                    { 
                        memset(buff,0,17);
                        sprintf((char*)buff,"    Test...  ");
                        display_str(0,2,buff);	                        
                        display_str(0,4,buff);								
                    }
                    
                    if( Menu_times_count > 160 )
                    { 
                        Menu_times_count = 1;

                        if( 1 == Bat0_Insert_Test() )
                        {
                            Bat0_Recharge_Test();
                            memset(buff,0,17);
                            sprintf((char*)buff,"bat0:%4d Omh",Bat0_Resistance);
                            display_str(0,2,buff);								
                            Bat0_Resistance = 0;
                        }	
                        else 
                        {
                            sprintf((char*)buff,"  Not input   ");
                            display_str(0,2,buff);								
                        }

                        if( 1 == Bat1_Insert_Test() )
                        {
                            Bat1_Recharge_Test();
                            memset(buff,0,17);
                            sprintf((char*)buff,"bat1:%4d Omh",Bat1_Resistance);
                            display_str(0,4,buff);								
                            Bat1_Resistance = 0;
                        }	
                        else 
                        {
                            sprintf((char*)buff,"  Not input   ");
                            display_str(0,4,buff);								
                        }
                    }
                    else
                        Menu_times_count++;
                    break;
                    
                 case 3:      // ��طŵ�

                    Bat0_Discharge_Machine();
                    Bat1_Discharge_Machine();

                    if( Menu_times_count == 10  )
                    {
                        memset(buff,' ',17);
                        buff[14]='\0';
                        if( Battery_Discharging == Bat0_DischargeMachine )
                        {
                            sprfu(&buff[0],Batt0_Vol_Seconds,0,4);
                            sprfu(&buff[5],Batt0_Cur_Seconds,0,4);
                            sprfu(&buff[10],(INT16U)Bat0_Charge_Discharge_Capacity,0,4);
                            display_str(0,2,buff);
                        }
                        else if( Battery_Wait_For_Remove == Bat0_DischargeMachine )
                        {
                            sprfu(&buff[0],Batt0_Vol_Seconds,0,4);
                            sprfu(&buff[5],0,0,4);
                            sprfu(&buff[10],(INT16U)Bat0_Charge_Discharge_Capacity,0,4);
                            display_str(0,2,buff);
                        }
                        
                        if( Battery_Discharging == Bat1_DischargeMachine )
                        {                        
                            sprfu(&buff[0],Batt1_Vol_Seconds,0,4);
                            sprfu(&buff[5],Batt1_Cur_Seconds,0,4);
                            sprfu(&buff[10],(INT16U)Bat1_Charge_Discharge_Capacity,0,4);
                            display_str(0,4,buff);
                        }
                        else if( Battery_Wait_For_Remove == Bat1_DischargeMachine )
                        {
                            sprfu(&buff[0],Batt1_Vol_Seconds,0,4);
                            sprfu(&buff[5],0,0,4);
                            sprfu(&buff[10],(INT16U)Bat1_Charge_Discharge_Capacity,0,4);
                            display_str(0,4,buff);
                        }                        
                        Menu_times_count = 0;
                    }
                    else
                        Menu_times_count++;
                    break;
                    
                 case 4:      // �޸����
                    display_str(0,2,(INT8S*)"����δ���..  ");
                    display_str(0,4,(INT8S*)"              ");
                    break;
                default:
                    break;

            }
        }
        if (func_ptr == 2)   // ��������
        {
		    dot = para_dotx[para_ptr];
            
            // ��ʾ���
            if ((bit_ptr >= dot) || (dot == 0))
            {
                set_cursor(10-bit_ptr,2);  // �����ʾ
            }
            else
            {
                set_cursor(11-bit_ptr,2);  // �����ʾ
            }
            
            // ��ʾ����
            strcpy((char*)buff,(char*)menu3[para_ptr]);
            sprfu(&buff[6],edit_val,dot,5);
            display_str(0,2,buff);

            switch(para_ptr) // ����˵��
            {
                case 2:  // LED ˳�����
                    switch(edit_val)
                    {
                        case 0:
                            display_str(0,4,"LED ��λ��ȷ  ");
                            break;
                        case 1:
                            display_str(0,4,"LED1 ��λ��   ");
                            break;
                        case 2:
                            display_str(0,4,"LED2 ��λ��   ");
                            break;
                        case 3:
                            display_str(0,4,"LED1,LED2 ����");
                            break;
                        default:
                            display_str(0,4,"������ı༭ֵ");
                            break;
                    }
                    break;

                case 3:   // LCD ����ģʽ
                    switch(edit_val)
                    {
                        case 0:
                            display_str(0,4,"LCD ʡ��ģʽ  ");
                            break;
                        case 1:
                            display_str(0,4,"LCD ���ⳣ��  ");
                            break;
                        default:
                            display_str(0,4,"������ı༭ֵ");
                            break;
                    }
                    break;

                case 4:   // ϵͳ��������
                    switch(edit_val)
                    {
                        case 0:
                            display_str(0,4,"����          ");
                            break;
                        case 1:
                            display_str(0,4,"English       ");
                            break;
                        default:
                            display_str(0,4,"������ı༭ֵ");
                            break;
                    }
                    break;

                case 6:   // �������µ���ʾģʽ
                    switch(edit_val)
                    {
                        case 0:
                            display_str(0,4,"״̬��̬��ʾ  ");
                            break;
                        case 1:
                            display_str(0,4,"״̬ѭ����ʾ  ");
                            break;
                        default:
                            display_str(0,4,"������ı༭ֵ");
                            break;
                    }
                    break;

                default:
                    break;
            }
            
            	

        }


        if (func_ptr == 4)   // ϵͳ���� 
        {
            switch(func1_ptr)
            {
                case 0:      // ��ѹ����
                    if( 0 != Menu_times_count )
                    {
                        if( 10 == Menu_times_count )
                        {
                            Menu_times_count = 1;
                            strcpy((char*)buff,"V1:        V ");
                            sprfi(&buff[5],Batt0_Vol_Seconds,3,4);
                            display_str(0,2,buff);
                            strcpy((char*)buff,"V2:        V ");
                            sprfi(&buff[5],Batt1_Vol_Seconds,3,4);
                            display_str(0,4,buff);
                        }
                        else Menu_times_count ++;
                    }
                    else
                    {
                        display_str(0,2,(INT8S*)"Adjusting...");
                        display_str(0,4,(INT8S*)"              ");
                        Adjust_Battery_vol(0);			
                        Menu_times_count = 1;
                    }
                    break;
                case 1:      // ��������
                    if( 0 != Menu_times_count )
                    {
                        if( 10 == Menu_times_count )
                        {                    
                            Menu_times_count = 1;
                            strcpy((char*)buff,"I1:        A ");
							if(CURRENT_ZERO_0 -ADC_ConvertedValue[2]*3300/4096 >=4 &&  CURRENT_ZERO_0 -ADC_ConvertedValue[2]*3300/4096 <=4	)	
								temp = 0;
							else temp = ADC_ConvertedValue[2]*3300/4096 - CURRENT_ZERO_0;
                            sprfi(&buff[4],temp,3,4);
                            display_str(0,2,buff);
                            strcpy((char*)buff,"I2:        A ");
							if(CURRENT_ZERO_1 -ADC_ConvertedValue[3]*3300/4096 >=4 &&  CURRENT_ZERO_1 -ADC_ConvertedValue[3]*3300/4096 <=4	)	
								temp = 0;
							else temp = ADC_ConvertedValue[3]*3300/4096 - CURRENT_ZERO_1;														
                            sprfi(&buff[4],temp,3,4);
                            display_str(0,4,buff);
                        }
                        else Menu_times_count ++;					
                    }
                    else
                    {
                        display_str(0,2,(INT8S*)"Adjusting...");
                        display_str(0,4,(INT8S*)"              ");
                        Adjust_Battery_Cur(0);
                        Menu_times_count = 1;
                    }
                    break;
                default:
                    break;

            }
        }
    }

}

