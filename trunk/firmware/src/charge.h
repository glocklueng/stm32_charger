#ifndef	_CHARGE__H
#define	_CHARGE__H


// Bat0_StateMachine 的选项
#define Battery_Wait_For_Remove           1
#define Battery_Insert_Test               2
#define Battery_Chargeable_Test           3
#define Battery_Charge_Discharge          4
#define Battery_Normal_Charging           5
#define Battery_Rapid_Charging            6
#define Battery_Discharging               7
#define Battery_Init	                  0

// Bat_Charge_Discharge_Choise 的选项
#define Battery_Normal_Charge_Mode        0
#define Battery_Rapid_Charge_Mode		  1
#define Battery_Discharge_Mode		      2 

// Bat_Fast_Charge_StateMachine 的选项
#define Battery_Precharge_Mode	          1	 // 预冲状态
#define Battery_Not_Monitor_Mode          2	 // 不检测充电状态
#define Battery_Monitor_Charge_Mode	      3	 // 检测充电状态
#define Battery_Charge_Over_Mode          4	 // 充电停止


// Bat_Status 的选项
#define Battery_Not_input                 0  // 电池没有装入
#define Battery_Have_Input                1  // 电池已经装入
#define Battery_Chargeable                2  // 是可充电电池
#define Battery_Not_chargeable            3  // 不是可充电电池
#define Battery_Normal_Charge             4  // 标准充电模式
#define Battery_Fast_Charge               5  // 快速充电模式
#define Battery_DisCharge                 6  // 放电模式
#define Battery_Charge_Over               7  // 充电完毕
#define Battery_Discharge_Over            8  // 放电完毕

// 负压检测数组的长度
#define delt_v_counts   12
// 负压差值
#define delt_v_value    5

extern INT16U para[256];
extern INT16U status[64];

extern volatile  INT32U  Bat0_Charge_Discharge_Time_Count;  // 电池 0 充电时间计数器
extern volatile  INT32U  Bat1_Charge_Discharge_Time_Count;  // 电池 1 充电时间计数器

extern FP32 Bat0_Charge_Discharge_Capacity ; // 电池 0 充电或放电容量 
extern FP32 Bat1_Charge_Discharge_Capacity ; // 电池 1 充电或放电容量

void InitChargePara(void);
void GetChargeMeasure(void);
INT8U Bat0_Insert_Test(void);
INT8U Bat1_Insert_Test(void);
INT8U Bat0_Recharge_Test(void);
INT8U Bat1_Recharge_Test(void);
void Battery_Resistance_Test(void);
INT8U Bat0_NormalCharge(void);
INT8U Bat1_NormalCharge(void);
INT8U Bat0_RapidCharge(void); //快速充电模式
INT8U Bat1_RapidCharge(void); //快速充电模式
void Bat0_State_Machine(INT8U mode);
void Bat1_State_Machine(INT8U mode);

void Bat0_Discharge_Machine(void);
INT8U Bat0_DisCharge(void);
void Bat1_Discharge_Machine(void);
INT8U Bat1_DisCharge(void);

void ReinitChargePara(	INT8U para_ptr);


#endif
