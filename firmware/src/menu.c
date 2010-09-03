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

// 功能选择
INT8U   func_ptr,func1_ptr;   // 功能指针,子功能指针
INT8U   disp_ptr;             // 第一显示项指针
INT8U   sel_ptr;              // 当前选择项指针
INT16U  para_val,edit_val;
INT8U   Menu_times_count;   // 菜单操作计数器

#define bittst(x,y)     x[y >> 3] & (1 << (y & 7))

INT8S const menu[][15] = {
    "===系统功能===",
    "===状态监视===",
    "===充电功能===",
    "===参数设定===",
    "===参数复制===",
    "===测试功能===",
    "===版本信息===",};

INT8S const menu0[][15] = {
    "  状态监视    ",
    "  充电功能    ",
    "  参数设定    ",
    "  参数复制    ",
    "  系统矫正    ",
    "  版本信息    ",
    "              ",};

INT8S const menu1[][15] = {
    "  电池电压    ",
    "  电池电流    ",
    "  电池温度    ",
    "  STM32 Vref  ",
    "  芯片温度    ",};

INT8S const menu2[][15] = {
    "电池一容量    ",
    "电池二容量    ",
    "LED 脚位调整  ",
    "LCD 背光模式  ",
    "系统语言设置  ",
    "LCD 对比度    ",
    "充电显示模式  ",};

INT8S const menu3[][15] = {
    "F0: P=     mAH",
    "F1: P=     mAH",
    "F2: M=        ",
    "F3: M=        ",
    "F4: M=        ",
    "F5: P=        ",
    "F6: M=        ",};

INT8S const menu4[][15] = {
    "  电压矫零    ",
    "  电流矫零    ",};

INT8S const menu5[][15] = {
    "  标准充电    ",
    "  快速充电    ",
    "  内阻检测    ",
    "  电池放电    ",
    "  修复电池    ",};

INT8U const para_dotx[] = 
{
    0,0,

};

// 参数分组编号
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

INT16U mptr0[20];     // 分组数据的第一个数据编号

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

    if (menu_ptr == 0)  // 第一层菜单 -----  状态监视
    { 
        set_cursor(0xff,0xff);       // 光标不显示
        display_str(0,0,(INT8S *)"  麦 思 网    ");
        display_str(0,2,(INT8S *)"   mySTM32    ");
        display_str(0,4,(INT8S *)" 智能充电器   ");
    }

    if (menu_ptr == 1)  // 第二层菜单 ----- 功能选择
    {
        set_cursor(0xff,0xff);       // 光标不显示
        p = (INT8S *)menu[0];
        display_str(0,0,p);
        j = 2;
        cur_disp = disp_ptr;
        p = (INT8S *)menu0[0];
        for (i = 0; i < 2; i++)
        {
            strcpy((char*)buff,(char *)(p + cur_disp * 15));
            if (cur_disp == sel_ptr)      // 当前被选择的功能相用箭头指示
            {
                buff[0] = '=';
                buff[1] = '>';
            }
            display_str(0,j,buff);
            cur_disp++;
            j = j + 2;
        }
    }

    if (menu_ptr == 2)   //  第三层菜单 ----- 子功能选择
    {
        set_cursor(0xff,0xff); // 光标不显示
        Led0ChargeStat = AllOff;
        Led1ChargeStat = AllOff;

        p = (INT8S *)menu[func_ptr+1];
        display_str(0,0,p);
		switch (func_ptr)
		{
            case 0: // 状态监视
                j = 2;
                cur_disp = disp_ptr;
                p = (INT8S *)menu1[0];
                for (i = 0; i < 2; i++)
                {
                    strcpy((char*)buff,(char *)(p + cur_disp * 15));
                    if (cur_disp == sel_ptr)      // 当前被选择的功能相用箭头指示
                    {
                        buff[0] = '=';
                        buff[1] = '>';
                    }
                    display_str(0,j,buff);
                    cur_disp++;
                    j = j + 2;
                }
                break;
            case 1: // 充电功能
                j = 2;
                cur_disp = disp_ptr;
                p = (INT8S *)menu5[0];
                for (i = 0; i < 2; i++)
                {
                    strcpy((char*)buff,(char *)(p + cur_disp * 15));
                    if (cur_disp == sel_ptr)      // 当前被选择的功能相用箭头指示
                    {
                        buff[0] = '=';
                        buff[1] = '>';
                    }
                    display_str(0,j,buff);
                    cur_disp++;
                    j = j + 2;
                }
                break;
            case 2: // 电池容量
			    dot = para_dotx[para_ptr];
                
                // 显示光标
                if ((bit_ptr >= dot) || (dot == 0))
                {
                    set_cursor(10-bit_ptr,4);  // 光标显示
                }
                else
                {
                    set_cursor(11-bit_ptr,4);  // 光标显示
                }
                
                // 显示数据
                strcpy((char*)buff,(char*)menu3[para_ptr]);
                sprfu(&buff[6],edit_val,dot,5);
                display_str(0,2,buff);
                // 参数说明
                set_cursor(0xff,0xff); // 光标不显示
                display_str(0,4,(INT8S *)menu2[para_ptr]);				

                break;
            case 4: // 测试功能
                j = 2;
                cur_disp = disp_ptr;
                p = (INT8S *)menu4[0];
                for (i = 0; i < 2; i++)
                {
                    strcpy((char*)buff,(char*)(p + cur_disp * 15));
                    if (cur_disp == sel_ptr)      // 当前被选择的功能相用箭头指示
                    {
                        buff[0] = '=';
                        buff[1] = '>';
                    }
                    display_str(0,j,buff);
                    cur_disp++;
                    j = j + 2;
                }
                break;
            case 5: // 系统信息
                display_str(0,2,(INT8S*)"硬件: V1.20 ");
                display_str(0,4,(INT8S*)"软件: V0.08"); 
                break;
                 
        }
    }

    if ( (menu_ptr == 3) || (menu_ptr == 4) )      // 第四层菜单 ----- 浏览数据,  // 第五层菜单 ----- 数据编辑
    {
        if (func_ptr == 0)   // 监视状态
        {
            switch(func1_ptr)
            {
                case 0:      // 电池电压
                    strcpy((char*)buff,"V1:         V ");
                    sprfi(&buff[5],Batt0_Vol_Seconds,3,4);
                    display_str(0,2,buff);
                    strcpy((char*)buff,"V2:         V ");
                    sprfi(&buff[5],Batt1_Vol_Seconds,3,4);
                    display_str(0,4,buff);
                    break;
                case 1:      // 电池电流
                    strcpy((char*)buff,"I1:         A ");
                    sprfi(&buff[5],Batt0_Cur_Seconds,3,4);
                    display_str(0,2,buff);
                    strcpy((char*)buff,"I2:         A ");
                    sprfi(&buff[5],Batt1_Cur_Seconds,3,4);
                    display_str(0,4,buff);
                    break;
                 case 2:      // 电池温度
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
                 case 4:      // STM32 芯片温度
                    strcpy((char*)buff,"芯片温度:    ");
                    display_str(0,2,buff);
                    strcpy((char*)buff,"           C ");
						// 减去10.9 是偏移为环境温度				
                    sprfu(&buff[7],STM32_Temperature_Seconds,1,3);
                    display_str(0,4,buff);
                    break;
                default:
                    break;

            }
        }
        if (func_ptr == 1)   // 充电功能
        {
            switch(func1_ptr)
            {
                case 0:      // 标准充电
                    Bat0_State_Machine(Battery_Normal_Charge_Mode);  //电池0 充电状态机             
                    Bat1_State_Machine(Battery_Normal_Charge_Mode);  //电池1 充电状态机

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
										
                case 1:      // 快速充电
                    Bat0_State_Machine(Battery_Rapid_Charge_Mode);  //电池0 充电状态机             
                    Bat1_State_Machine(Battery_Rapid_Charge_Mode);  //电池1 充电状态机

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
										
                 case 2:      // 内阻检测
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
                    
                 case 3:      // 电池放电

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
                    
                 case 4:      // 修复电池
                    display_str(0,2,(INT8S*)"功能未完成..  ");
                    display_str(0,4,(INT8S*)"              ");
                    break;
                default:
                    break;

            }
        }
        if (func_ptr == 2)   // 参数设置
        {
		    dot = para_dotx[para_ptr];
            
            // 显示光标
            if ((bit_ptr >= dot) || (dot == 0))
            {
                set_cursor(10-bit_ptr,2);  // 光标显示
            }
            else
            {
                set_cursor(11-bit_ptr,2);  // 光标显示
            }
            
            // 显示数据
            strcpy((char*)buff,(char*)menu3[para_ptr]);
            sprfu(&buff[6],edit_val,dot,5);
            display_str(0,2,buff);

            switch(para_ptr) // 参数说明
            {
                case 2:  // LED 顺序调整
                    switch(edit_val)
                    {
                        case 0:
                            display_str(0,4,"LED 脚位正确  ");
                            break;
                        case 1:
                            display_str(0,4,"LED1 脚位反   ");
                            break;
                        case 2:
                            display_str(0,4,"LED2 脚位反   ");
                            break;
                        case 3:
                            display_str(0,4,"LED1,LED2 都反");
                            break;
                        default:
                            display_str(0,4,"不合理的编辑值");
                            break;
                    }
                    break;

                case 3:   // LCD 背光模式
                    switch(edit_val)
                    {
                        case 0:
                            display_str(0,4,"LCD 省电模式  ");
                            break;
                        case 1:
                            display_str(0,4,"LCD 背光常亮  ");
                            break;
                        default:
                            display_str(0,4,"不合理的编辑值");
                            break;
                    }
                    break;

                case 4:   // 系统语言设置
                    switch(edit_val)
                    {
                        case 0:
                            display_str(0,4,"中文          ");
                            break;
                        case 1:
                            display_str(0,4,"English       ");
                            break;
                        default:
                            display_str(0,4,"不合理的编辑值");
                            break;
                    }
                    break;

                case 6:   // 充电界面下的显示模式
                    switch(edit_val)
                    {
                        case 0:
                            display_str(0,4,"状态静态显示  ");
                            break;
                        case 1:
                            display_str(0,4,"状态循环显示  ");
                            break;
                        default:
                            display_str(0,4,"不合理的编辑值");
                            break;
                    }
                    break;

                default:
                    break;
            }
            
            	

        }


        if (func_ptr == 4)   // 系统矫正 
        {
            switch(func1_ptr)
            {
                case 0:      // 电压矫零
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
                case 1:      // 电流矫零
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

