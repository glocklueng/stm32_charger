/****************************************Copyright (c)**************************************************
**                              智 能 充 电 器 开 发 
**                                     mystm32 论坛
**                                   QQ 群:  
**
**                               http://www.mystm32.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: Para.h
**创   建   人: 吕海安
**最后修改日期: 2008年03月08日
**描        述: 内存参数配置头文件
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 吕海安
** 版  本: v0.01
** 日　期: 2008年03月08日
** 描　述: 原始版本
**
**--------------当前版本修订------------------------------------------------------------------------------
** 创建人: 吕海安
** 版  本: v0.01
** 日　期: 2008年03月15日
** 描　述: 拟定参数表
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/

#ifndef	__PARA__H
#define	__PARA__H   



#define Bat0_Vol                    status[0]    // 电池 0 的电压值
#define Bat0_Cur                    status[1]    // 电池 0 的电流值         
#define Bat0_Temperature            status[2]    // 电池 0 的温度值
#define Bat0_Set_Capacity           status[3]    // 电池 0 设定容量 mAH         
#define Bat0_Test_Capacity          status[4]    // 电池 0 充电或放电容量 mAH 
#define Bat0_ReChargeable_Vol       status[5]    // 电池 0 没有充电时电压
#define Bat0_Resistance             status[6]    // 电池 0 内阻  
#define Bat0_StateMachine           status[7]    // 电池 0 状态机
#define Bat0_Rapid_Charge_StateMachine           status[8]    // 电池 0 状态机
#define Bat0_DischargeMachine	status[9]    // 电池 0 放电状态机
#define Bat0_Status			status[10]  // 电池0 所处状态
#define Bat0_Charge_Discharge_Choise	status[11]  // 电池0  选择的冲放模式

#define Bat1_Vol                    status[20]   // 电池 1 的电压值
#define Bat1_Cur                    status[21]   // 电池 1 的电流值         
#define Bat1_Temperature            status[22]   // 电池 1 的温度值
#define Bat1_Set_Capacity           status[23]   // 电池 1 设定容量 mAH         
#define Bat1_Test_Capacity          status[24]   // 电池 1 充电或放电容量 mAH 
#define Bat1_ReChargeable_Vol       status[25]   // 电池 1 没有充电时电压 
#define Bat1_Resistance             status[26]   // 电池 1 内阻
#define Bat1_StateMachine           status[27]   // 电池 1 状态机
#define Bat1_Rapid_Charge_StateMachine           status[28]    // 电池 0 状态机
#define Bat1_DischargeMachine	status[29]    // 电池 1 放电状态机
#define Bat1_Status			status[30]  // 电池1 所处状态
#define Bat1_Charge_Discharge_Choise	status[31]  // 电池0  选择的冲放模式

////////////////////////////////////////////////////////////////////////////////////////////////
// BAT0 & BAT1 共同参数

#define BAT_MAX_TEMPERATURE         para[0]  //最大截止温度
#define BAT_INSERT_TEST_CCR         para[1]  //检测电池是否插入 使用占空比
#define BAT_Vol_CHARGE_MAX      para[2]  //标准充电最高电压     mV
#define BAT_RECHARGE_TEST_CCR       para[3]  //检测是否是可冲电电池 使用占空比
#define BAT_MAX_RESISTANCE          para[4]  //最大可充电电池内阻
#define BAT_NOR_CHARGE_MAX_TIME_H   para[5]  //最大标冲时间        sec
#define BAT_NOR_CHARGE_MAX_TIME_L   para[6]  //最大标冲时间        sec
#define BAT_DISCHARGE_STOP_VOL  	 para[7] // 放电截止电压
#define BAT_VOL_PRE_CHARGE_OVER  para[8]  // 预冲截止电压
#define BAT_VOL_RAPID_CHARGE_MAX  para[9]  // 预冲截止电压

////////////////////////////////////////////////////////////////////////////////////////////////
// BAT0 参数
#define Bat0_Chargeable             para[32]  //检测是否是可充电电池
#define Bat0_Test_Insert            para[33]  //检测电池是否插入
#define Bat0_Vol_Repair_Min         para[34]  //需修复电池电压下限   mV
#define Bat0_Vol_Charge_Min         para[35]  //可充电电压范围下限   mV

#define Bat0_Vol_PowerFull_Min      para[37]  //满电电压下限         mV
#define Bat0_Charge_On_Off          para[38]  //电池状态             mV

#define Bat0_Cur_DisCharge          para[39]   //放电电流            mA
#define Bat0_Vol_DisCharge_OVER     para[40]  //放电结束电压        mV
#define Bat0_Cur_Repair             para[41]  //电池修复电流        mA
#define Bat0_Vol_Repair_OVER        para[42]  //修复结束电压        mV
#define Bat0_Time_Repair_MAX        para[43]  //最大修复时间        Min

#define Bat0_Cur_Pre_CHARGE         para[44]  //预充电流            mA 
#define Bat0_Vol_Rre_CHARGE_OVER    para[45]  //预充结束电压        mV
#define Bat0_Cur_Rapid_CHARGE        para[46]  //快充电流            mA
#define Bat0_Vol_Rapid_CHARGE_MAX    para[47]  //快冲最高电压        mV
#define Bat0_Time_Fast_CHARGE_MAX   para[48]  //最高电压保持时间    S
#define Bat0_Mode_Fast_CHARGE_OVER  para[49]  //结束方式            1:转为涓流 0:直接结束
#define Bat0_DeltaV_Fast_VHARGE     para[50]  //快冲结束-△V        *10 mV    
#define Bat0_Cur_Add_CHARGE         para[51]  //补充充电电流        mA
#define Bat0_Time_Add_CHARGE        para[52]  //补充维持时间        Min
#define Bat0_PowerFull_Retain       para[53]  //满电保持功能        1:开启  0:关闭
#define Bat0_Time_CHARGE_MAX        para[54]  //单次充电最大时间    Hour

#define Bat0_Cur_Nor_CHARGE         para[55]  //标准充电电流

#define Bat0_Charge_Mode             para[57]  //标准充电电流
#define Bat0_DisCharge_On_Off          para[58]  //电池状态             mV




////////////////////////////////////////////////////////////////////////////////////////////////
// BAT1 参数

#define Bat1_Chargeable             para[132]  //检测是否是可充电电池
#define Bat1_Test_Insert            para[133]  //检测电池是否插入
#define Bat1_Vol_Repair_Min         para[134]  //需修复电池电压下限   mV
#define Bat1_Vol_Charge_Min         para[135]  //可充电电压范围下限   mV
#define Bat1_Vol_Discharge_Min      para[136]  //需放电电压范围下限   mV
#define Bat1_Vol_PowerFull_Min      para[137]  //满电电压下限         mV
#define Bat1_VOL_ClassErr_MIN       para[138]  //电池种类错误下限     mV

#define Bat1_Cur_DisCharge          para[139]  //放电电流             mA
#define Bat1_Vol_DisCharge_OVER     para[140]  //放电结束电压         mV
#define Bat1_Cur_Repair             para[141]  //电池修复电流        mA
#define Bat1_Vol_Repair_OVER        para[142]  //修复结束电压        mV
#define Bat1_Time_Repair_MAX        para[143]  //最大修复时间        Min

#define Bat1_Cur_Pre_CHARGE         para[144]  //预充电流            mA 
#define Bat1_Vol_Rre_CHARGE_OVER    para[145]  //预充结束电压        mV
#define Bat1_Cur_Rapid_CHARGE        para[146]  //快充电流            mA
#define Bat1_Vol_Fast_CHARGE_MAX    para[147]  //快冲最高电压        mV
#define Bat1_Fast_Top_Vol_Time      para[148]  //最高电压保持时间    S
#define Bat1_Mode_Fast_CHARGE_OVER  para[149]  //结束方式            1:转为涓流 0:直接结束
#define Bat1_DeltaV_Fast_VHARGE     para[150]  //快冲结束-△V        *10 mV    
#define Bat1_Cur_Add_CHARGE         para[151]  //补充充电电流        mA
#define Bat1_Time_Add_CHARGE        para[152]  //补充维持时间        Min
#define Bat1_PowerFull_Retain       para[153]  //满电保持功能        1:开启  0:关闭
#define Bat1_Time_Fast_CHARGE_MAX   para[154]  //单次快充最大时间    sec

#define Bat1_Cur_Nor_CHARGE         para[155]  //标准充电电流
#define Bat1_Charge_On_Off          para[156]  //电池状态             mV
#define Bat1_Charge_Mode             para[157]  //标准充电电流

#define Bat1_DisCharge_On_Off          para[158]  //电池状态             mV

#endif

/**************************************************************************************×*****************
**                                 END OF FILE
********************************************************************************************************/
