#ifndef	_CHARGE__H
#define	_CHARGE__H


// Bat0_StateMachine ��ѡ��
#define Battery_Wait_For_Remove           1
#define Battery_Insert_Test               2
#define Battery_Chargeable_Test           3
#define Battery_Charge_Discharge          4
#define Battery_Normal_Charging           5
#define Battery_Rapid_Charging            6
#define Battery_Discharging               7
#define Battery_Init	                  0

// Bat_Charge_Discharge_Choise ��ѡ��
#define Battery_Normal_Charge_Mode        0
#define Battery_Rapid_Charge_Mode		  1
#define Battery_Discharge_Mode		      2 

// Bat_Fast_Charge_StateMachine ��ѡ��
#define Battery_Precharge_Mode	          1	 // Ԥ��״̬
#define Battery_Not_Monitor_Mode          2	 // �������״̬
#define Battery_Monitor_Charge_Mode	      3	 // �����״̬
#define Battery_Charge_Over_Mode          4	 // ���ֹͣ


// Bat_Status ��ѡ��
#define Battery_Not_input                 0  // ���û��װ��
#define Battery_Have_Input                1  // ����Ѿ�װ��
#define Battery_Chargeable                2  // �ǿɳ����
#define Battery_Not_chargeable            3  // ���ǿɳ����
#define Battery_Normal_Charge             4  // ��׼���ģʽ
#define Battery_Fast_Charge               5  // ���ٳ��ģʽ
#define Battery_DisCharge                 6  // �ŵ�ģʽ
#define Battery_Charge_Over               7  // ������
#define Battery_Discharge_Over            8  // �ŵ����

// ��ѹ�������ĳ���
#define delt_v_counts   12
// ��ѹ��ֵ
#define delt_v_value    5

extern INT16U para[256];
extern INT16U status[64];

extern volatile  INT32U  Bat0_Charge_Discharge_Time_Count;  // ��� 0 ���ʱ�������
extern volatile  INT32U  Bat1_Charge_Discharge_Time_Count;  // ��� 1 ���ʱ�������

extern FP32 Bat0_Charge_Discharge_Capacity ; // ��� 0 ����ŵ����� 
extern FP32 Bat1_Charge_Discharge_Capacity ; // ��� 1 ����ŵ�����

void InitChargePara(void);
void GetChargeMeasure(void);
INT8U Bat0_Insert_Test(void);
INT8U Bat1_Insert_Test(void);
INT8U Bat0_Recharge_Test(void);
INT8U Bat1_Recharge_Test(void);
void Battery_Resistance_Test(void);
INT8U Bat0_NormalCharge(void);
INT8U Bat1_NormalCharge(void);
INT8U Bat0_RapidCharge(void); //���ٳ��ģʽ
INT8U Bat1_RapidCharge(void); //���ٳ��ģʽ
void Bat0_State_Machine(INT8U mode);
void Bat1_State_Machine(INT8U mode);

void Bat0_Discharge_Machine(void);
INT8U Bat0_DisCharge(void);
void Bat1_Discharge_Machine(void);
INT8U Bat1_DisCharge(void);

void ReinitChargePara(	INT8U para_ptr);


#endif
