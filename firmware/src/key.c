/****************************************Copyright (c)**************************************************
**                              智 能 充 电 器 开 发 小 组
**                                      麦思网 论坛
**                                     QQ 群: 7095643 
**
**                               http://www.mystm32.com/bbs
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: Key.c
**创   建   人: 安哥
**最后修改日期: 2009年09月20日 22:10
**描        述: 智能充电器 Key
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
** 描　述: 历史
**
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 蟲子
** 版  本: v0.071
** 日　期: 2010年1月5日 11:36
** 描　述: 当前版本 增加各个菜单下 cancel退出的函数处理
**
**------------------------------------------------------------------------------------------------------
**--------------当前版本信息----------------------------------------------------------------------------
** 创建人: 安哥
** 版  本: v0.08
** 日　期: 2010年2月10日 00:27
** 描　述: 增加电池容量设置
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
INT8U menu_ptr; //菜单指针
INT8U para_ptr; //参数指针

INT16U batt0_capacity = 1800;
INT16U batt1_capacity = 1800;
INT16U ModeLED      = 3;  // LED 的红绿灯脚位配置, 在 LED 脚位焊接错误后设置
INT16U ModeLCDLight = 0;  // LCD 背光模式   0: 省电模式; 1: 测试模式,背光常亮 
INT16U ModeLaguage  = 1;  // 系统语言  0: 中文;  1: 英文
INT16U ModeLCDContrast = 0; // LCD 的对比度设置
INT16U ModeLCDShow = 1;   // LCD 在充电时显示模式  0: 静态模式  1: 状态自动翻页

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

// 40 毫秒扫描一次
INT8U ReadKey(void)
{
    INT32U KeyCode,i;
    static INT8U KeyDly[10];

   // 扫描键值
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


   // 形成按键扫描码，先取得组合键

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
                if(StatBLWake) // 背光睡眠状态
                    return(WAKE_UP_BL); // 省电模式下做唤醒
                else
                    return(i);
		    }
	    }
    }
	return(0);
}


//处理充电器键盘
void process_key(INT8U keycode)
{
    INT16U const bit_val[] = {1,10,100,1000,10000};

    if (keycode == 0) 
        return;

    
	//screen_flg = 0; //屏幕保护

    
	//LCD5110_CLS();
	
    switch(menu_ptr)
    {
        case 0:  // 状态显示
            if (keycode == KEY_ENTER)
            {
                func_ptr = 0;
                menu_ptr = 1;
                sel_ptr = 0;
                disp_ptr = 0;
            }   
            break;
        case 1:  // 功能选择
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
            if ((keycode == KEY_UP) || (keycode == KEY_LEFT))   // 选择上一功能
            {
                if (sel_ptr > 0) sel_ptr--;					   	
                while (sel_ptr < disp_ptr) disp_ptr--;
            }    
            if ((keycode == KEY_DN) || (keycode == KEY_RIGHT))  // 选择下一功能
            {
                if (sel_ptr < 5) sel_ptr++; // 数字5代表功能选择菜单的总数					                        
                while (sel_ptr > disp_ptr + 1) disp_ptr++;
            }    
            break;

        case 2:  //子功能选择
            if (keycode == KEY_ESC)
            {
                               
                menu_ptr = 1;
                sel_ptr = func_ptr;
                disp_ptr = func_ptr;
                func1_ptr = 0;
		        //cursor_mode = 0; // 光标以下划线方式显示: 呼梯设置为方块显示，退出时恢复
		        //settime_flg = 0; // 退出时将时间设置标志清0

        		bit_ptr = 0;     // 重新登陆退出时，变量清0
        		//edit_val = 0;
                display_str(0,4,"            "); 
                	
            }  
            if (keycode == KEY_ENTER)
            {
               
                switch (func_ptr)
                {
                    case 0:  // 子功能选择: 状态监视                  
                        bitedit = 0;
                        para_ptr = 0;
                        menu_ptr = 3;
                        func1_ptr = sel_ptr;
                        switch (func1_ptr)
                        {
                            case 0:     // 电池电压
                                break;
                                   
                            default:
                                break;
                        }  
                        break;
                    case 1:  // 子功能选择: 充电功能
                        bitedit = 0;
                        para_ptr = 0;
                        menu_ptr = 3;
                        func1_ptr = sel_ptr;
                        break;
                    case 2:  // 子功能选择: 参数设定 
                        bit_ptr = 0;
                        edit_val = para_val;
                        menu_ptr = 3;             
                        break;
                    case 3:  // 子功能选择: 参数复制
                        break;
                    case 4:  // 子功能选择: 测试功能
                        bitedit = 0;
                        para_ptr = 0;
                        menu_ptr = 3;
                        func1_ptr = sel_ptr;
                        break;
                    case 5:  // 子功能选择: 系统信息
                        break;
                    default:
                        break;
                }
            }

            
            if (keycode == KEY_UP)   // 选择上一功能
            {
                if (func_ptr == 2)     // 参数设定
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
            if (keycode == KEY_DN)  // 选择下一功能
            {
                switch (func_ptr)
                {
                    case 0:    // 状态监视
                        if (sel_ptr < 4) sel_ptr++;									    		   
                        break;
                    case 1:    // 充电功能 
                        if (sel_ptr < 4) sel_ptr++;
                        break;
                    case 2:    // 参数设定
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
					case 4:    // 测试功能
                        if (sel_ptr < 1) sel_ptr++;
                        break;
                }
                while (sel_ptr > disp_ptr + 1) disp_ptr++;
            } 
            break;      

         case 3:     // 数据浏览
            if (keycode == KEY_ESC)
            {
            	switch(func_ptr)
            	{
            		case 0:
						break;
				
            		case 1:
						ChargeFuncExit();	//充电功能退出时的处理	
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
                    set_cursor(0xff,0xff); // 光标不显示
	                if (bitedit) //位操作参数
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
	                else  //非位操作参数
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
                                    ReinitChargePara(para_ptr); // 重新设定新的参数	
                                    break;
                                case 2: // LED 模式
                                    if(edit_val <= 3)
                                    {
                                        EE_WriteVariable(VirtAddVarTab[para_ptr+4], edit_val);
                                    }
                                    EE_ReadVariable(VirtAddVarTab[para_ptr+4], &para_val);
                                    ModeLED = para_val;
                                    break;
                                case 3: // LCD 背光模式
                                    if((edit_val == 0) || (edit_val == 1))
                                    {
                                        EE_WriteVariable(VirtAddVarTab[para_ptr+4], edit_val);
                                    }
                                    EE_ReadVariable(VirtAddVarTab[para_ptr+4], &para_val);
                                    ModeLCDLight = para_val;
                                    break;
                                case 4: // 系统语言
                                    if(edit_val <= 1)
                                    {
                                        EE_WriteVariable(VirtAddVarTab[para_ptr+4], edit_val);
                                    }
                                    EE_ReadVariable(VirtAddVarTab[para_ptr+4], &para_val);
                                    ModeLaguage = para_val;
                                    break;
                                case 5: // LCD 对比度
                                    if(edit_val <= 255)
                                    {
                                        EE_WriteVariable(VirtAddVarTab[para_ptr+4], edit_val);
                                    }
                                    EE_ReadVariable(VirtAddVarTab[para_ptr+4], &para_val);
                                    ModeLCDContrast = para_val;
                                    break;
                                case 6: // LCD 在充电是状态时候翻页
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
            if (keycode == KEY_UP)    // 参数增量
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
            if (keycode == KEY_DN)    // 参数减量
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
            // 标准充电功能退出
            TIM2->CCR1 = 0; // 停止充电
            Bat0_Status = Battery_Charge_Over;
            // 状态机恢复到Battery_Insert_Test 状态
            Bat0_StateMachine = Battery_Init; 
            TIM2->CCR2 = 0; // 停止充电
            Bat1_Status = Battery_Charge_Over;
            // 状态机恢复到Battery_Insert_Test 状态
            Bat1_StateMachine = Battery_Init; 
            Bat0_Charge_Discharge_Capacity = 0;
            Bat1_Charge_Discharge_Capacity = 0;
            Bat0_Charge_Discharge_Time_Count = 0;
            Bat1_Charge_Discharge_Time_Count = 0;

            ChargerPrintf(" Exit from normal charge  ...\r\n"); 

            break;

        case 1:  // 快速充电功能退出
            TIM2->CCR1 = 0; // 停止充电
            Bat0_Status = Battery_Charge_Over;
            // 状态机恢复到Battery_Insert_Test 状态
            Bat0_StateMachine = Battery_Init; 
            TIM2->CCR2 = 0; // 停止充电
            Bat1_Status = Battery_Charge_Over;
            // 状态机恢复到Battery_Insert_Test 状态
            Bat1_StateMachine = Battery_Init; 
            Bat0_Charge_Discharge_Capacity = 0;
            Bat1_Charge_Discharge_Capacity = 0;
            Bat0_Charge_Discharge_Time_Count = 0;
            Bat1_Charge_Discharge_Time_Count = 0;

            ChargerPrintf(" Exit from normal charge  ...\r\n"); 		
            break;

        case 2: // 检测内阻功能退出
            break;

        case 3: //放电功能退出，恢复环境变量
            TIM3->CCR1 = 0; // 停止放电
            Bat0_Status = Battery_Discharge_Over;    
            Bat0_DischargeMachine = Battery_Init; // 放电状态机初始化为1			
            TIM3->CCR2 = 0; // 停止放电
            Bat1_Status = Battery_Discharge_Over;
            Bat1_DischargeMachine = Battery_Init; // 放电状态机初始化为1
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
    InitPID(); // PID 初始化    
	 
}



/**************************************************************************************×*****************
**                                 END OF FILE
********************************************************************************************************/

