#ifndef	_ADC__H
#define	_ADC__H

#include "global.h"


#define ADJUST_TIMES    3

// ADC.C ģ������
extern    INT16U Temperature_1; // ��һ·����¶�
extern    INT16U Temperature_2; // �ڶ�·����¶�
extern    INT16S Current_1;     // ��һ·��ص���
extern    INT16S Current_2;     // �ڶ�·��ص���
extern    INT16U Voltage_1;     // ��һ·��ص�ѹ
extern    INT16U Voltage_2;     // �ڶ�·��ص�ѹ
extern    INT16U Temperature;   // STM32 оƬ�¶�
extern    INT16U Vref;          // STM32 �ڲ� Vref ��ѹ

//���������  
extern    INT16U CURRENT_ZERO_0;  
extern    INT16U CURRENT_ZERO_1;  

//�����ѹ��
extern    INT16U VOLTAGE_ZERO_0;  
extern    INT16U VOLTAGE_ZERO_1;  

extern    INT16U ADC_ConvertedValue[8];
void AdjustAdcStandard(INT16U Temperature);
INT16S vol_to_bat1_cur(INT16U vol);
INT16S vol_to_bat2_cur(INT16U vol);

INT16S Zero_current_bat0(INT16U vol);      
INT16S Zero_current_bat1(INT16U vol); 
INT16U Zero_voltage_bat0(INT16U vol); 
INT16U Zero_voltage_bat1(INT16U vol); 

void InitADC(void);
void ADC_Covert_Value(void);

void Adjust_Battery_vol(INT8U flag);
void Adjust_Battery_Cur(INT8U flag);

#endif
