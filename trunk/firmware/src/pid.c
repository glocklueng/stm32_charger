/****************************************Copyright (c)**************************************************
**                              智 能 充 电 器 开 发 小 组
**                                      麦思网 论坛
**                                     QQ 群: 7095643 
**
**                               http://www.mystm32.com/bbs
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: PID.c
**创   建   人: Atmel Corporation: http://www.atmel.com
**最后修改日期: 2008年03月11日
**描        述: brief General PID implementation for AVR.
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: Atmel Corporation: http://www.atmel.com
** 版  本: V0.01
** 日　期: 2006年02月16日
** 描　述: 原始版本
**
**------------------------------------------------------------------------------------------------------
**--------------当前版本修订------------------------------------------------------------------------------
** 修改人: 吕海安
** 日　期: 2008年03月11日
** 描　述: Fro 智能充电器
**         
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#include "pid.h"

struct PID_DATA pidData0;
struct PID_DATA pidData1;

#define K_P     0.1
#define K_I     0.01
#define K_D     0

/*******************************************************************************************************
** 函数名称: InitPID
** 功能描述: PID 初始化, 主函数调用
** 输入参数: 无
** 输出参数: 无
********************************************************************************************************/
void InitPID(void)
{
    pid_Init(K_P * SCALING_FACTOR, K_I * SCALING_FACTOR , K_D * SCALING_FACTOR , &pidData0);  
    pid_Init(K_P * SCALING_FACTOR, K_I * SCALING_FACTOR , K_D * SCALING_FACTOR , &pidData1); 
}

/*******************************************************************************************************
** 函数名称: pid_Init
** 功能描述: PID 初始化
** 输入参数: int16_t p_factor: Proportional term. 
**           int16_t i_factor: Integral term.
**           int16_t d_factor: Derivate term.
**           struct PID_DATA *pid: Struct with PID status.
** 输出参数: 无
********************************************************************************************************/
void pid_Init(int16_t p_factor, int16_t i_factor, int16_t d_factor, struct PID_DATA *pid)
{
    // Start values for PID controller
    
    pid->sumError = 0;
    pid->lastProcessValue = 0;
    
    // Tuning constants for PID loop
    pid->P_Factor = p_factor;
    pid->I_Factor = i_factor;
    pid->D_Factor = d_factor;
    
    // Limits to avoid overflow
    pid->maxError = MAX_INT / (pid->P_Factor + 1);
    pid->maxSumError = MAX_I_TERM / (pid->I_Factor + 1.0); //sea
}



/*******************************************************************************************************
** 函数名称: pid_Controller
** 功能描述: Calculates output from setpoint, process value and PID status.
** 输入参数: int16_t setPoint: Desired value.
**           int16_t processValue: Measured value.
**           struct PID_DATA *pid_st: PID status struct.
** 输出参数: 无
********************************************************************************************************/
int16_t pid_Controller(int16_t setPoint, int16_t processValue, struct PID_DATA *pid_st)
{
    int16_t error, p_term, d_term;
    int32_t i_term, ret, temp; 

    
    error = setPoint - processValue;
    

    if (error > pid_st->maxError)        // Calculate Pterm and limit error overflow
    {
        p_term = MAX_INT;
    }
    else if (error < -pid_st->maxError)
    {
        p_term = -MAX_INT;
    }
    else
    {
        p_term = pid_st->P_Factor * error;
    }
    
      
    
    temp = pid_st->sumError + error;     // Calculate Iterm and limit integral runaway
    if(temp > pid_st->maxSumError)
    {
        i_term = MAX_I_TERM;
        pid_st->sumError = pid_st->maxSumError;
    }
    else if(temp < -pid_st->maxSumError)
    {
        i_term = -MAX_I_TERM;
        pid_st->sumError = -pid_st->maxSumError;
    }
    else
    {
        pid_st->sumError = temp;
        i_term = pid_st->I_Factor * pid_st->sumError;
    }
    
    
    d_term = pid_st->D_Factor * (pid_st->lastProcessValue - processValue);  // Calculate Dterm
    
    pid_st->lastProcessValue = processValue;
 
    ret = (p_term + i_term + d_term) / SCALING_FACTOR;
    if(ret > MAX_INT)
    {
        ret = MAX_INT;
    }
    else if(ret < -MAX_INT)
    {
        ret = -MAX_INT;
    } 
    
    //lcd_locate(5,1);  
    //lcd_print_number(ret,4,0);     
    
    return((int16_t)ret);
}

/*******************************************************************************************************
** 函数名称: pid_Controller
** 功能描述: Calling this function will reset the integrator in the PID regulator.
** 输入参数: pidData_t *pid_st: PID status struct.
** 输出参数: 无
********************************************************************************************************/
void pid_Reset_Integrator(pidData_t *pid_st)
{
    pid_st->sumError = 0;
}


/**************************************************************************************×*****************
**                                 END OF FILE
********************************************************************************************************/
