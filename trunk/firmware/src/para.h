/****************************************Copyright (c)**************************************************
**                              �� �� �� �� �� �� �� 
**                                     mystm32 ��̳
**                                   QQ Ⱥ:  
**
**                               http://www.mystm32.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: Para.h
**��   ��   ��: ������
**����޸�����: 2008��03��08��
**��        ��: �ڴ��������ͷ�ļ�
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: ������
** ��  ��: v0.01
** �ա���: 2008��03��08��
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�------------------------------------------------------------------------------
** ������: ������
** ��  ��: v0.01
** �ա���: 2008��03��15��
** �衡��: �ⶨ������
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/

#ifndef	__PARA__H
#define	__PARA__H   



#define Bat0_Vol                    status[0]    // ��� 0 �ĵ�ѹֵ
#define Bat0_Cur                    status[1]    // ��� 0 �ĵ���ֵ         
#define Bat0_Temperature            status[2]    // ��� 0 ���¶�ֵ
#define Bat0_Set_Capacity           status[3]    // ��� 0 �趨���� mAH         
#define Bat0_Test_Capacity          status[4]    // ��� 0 ����ŵ����� mAH 
#define Bat0_ReChargeable_Vol       status[5]    // ��� 0 û�г��ʱ��ѹ
#define Bat0_Resistance             status[6]    // ��� 0 ����  
#define Bat0_StateMachine           status[7]    // ��� 0 ״̬��
#define Bat0_Rapid_Charge_StateMachine           status[8]    // ��� 0 ״̬��
#define Bat0_DischargeMachine	status[9]    // ��� 0 �ŵ�״̬��
#define Bat0_Status			status[10]  // ���0 ����״̬
#define Bat0_Charge_Discharge_Choise	status[11]  // ���0  ѡ��ĳ��ģʽ

#define Bat1_Vol                    status[20]   // ��� 1 �ĵ�ѹֵ
#define Bat1_Cur                    status[21]   // ��� 1 �ĵ���ֵ         
#define Bat1_Temperature            status[22]   // ��� 1 ���¶�ֵ
#define Bat1_Set_Capacity           status[23]   // ��� 1 �趨���� mAH         
#define Bat1_Test_Capacity          status[24]   // ��� 1 ����ŵ����� mAH 
#define Bat1_ReChargeable_Vol       status[25]   // ��� 1 û�г��ʱ��ѹ 
#define Bat1_Resistance             status[26]   // ��� 1 ����
#define Bat1_StateMachine           status[27]   // ��� 1 ״̬��
#define Bat1_Rapid_Charge_StateMachine           status[28]    // ��� 0 ״̬��
#define Bat1_DischargeMachine	status[29]    // ��� 1 �ŵ�״̬��
#define Bat1_Status			status[30]  // ���1 ����״̬
#define Bat1_Charge_Discharge_Choise	status[31]  // ���0  ѡ��ĳ��ģʽ

////////////////////////////////////////////////////////////////////////////////////////////////
// BAT0 & BAT1 ��ͬ����

#define BAT_MAX_TEMPERATURE         para[0]  //����ֹ�¶�
#define BAT_INSERT_TEST_CCR         para[1]  //������Ƿ���� ʹ��ռ�ձ�
#define BAT_Vol_CHARGE_MAX      para[2]  //��׼�����ߵ�ѹ     mV
#define BAT_RECHARGE_TEST_CCR       para[3]  //����Ƿ��ǿɳ���� ʹ��ռ�ձ�
#define BAT_MAX_RESISTANCE          para[4]  //���ɳ��������
#define BAT_NOR_CHARGE_MAX_TIME_H   para[5]  //�����ʱ��        sec
#define BAT_NOR_CHARGE_MAX_TIME_L   para[6]  //�����ʱ��        sec
#define BAT_DISCHARGE_STOP_VOL  	 para[7] // �ŵ��ֹ��ѹ
#define BAT_VOL_PRE_CHARGE_OVER  para[8]  // Ԥ���ֹ��ѹ
#define BAT_VOL_RAPID_CHARGE_MAX  para[9]  // Ԥ���ֹ��ѹ

////////////////////////////////////////////////////////////////////////////////////////////////
// BAT0 ����
#define Bat0_Chargeable             para[32]  //����Ƿ��ǿɳ����
#define Bat0_Test_Insert            para[33]  //������Ƿ����
#define Bat0_Vol_Repair_Min         para[34]  //���޸���ص�ѹ����   mV
#define Bat0_Vol_Charge_Min         para[35]  //�ɳ���ѹ��Χ����   mV

#define Bat0_Vol_PowerFull_Min      para[37]  //�����ѹ����         mV
#define Bat0_Charge_On_Off          para[38]  //���״̬             mV

#define Bat0_Cur_DisCharge          para[39]   //�ŵ����            mA
#define Bat0_Vol_DisCharge_OVER     para[40]  //�ŵ������ѹ        mV
#define Bat0_Cur_Repair             para[41]  //����޸�����        mA
#define Bat0_Vol_Repair_OVER        para[42]  //�޸�������ѹ        mV
#define Bat0_Time_Repair_MAX        para[43]  //����޸�ʱ��        Min

#define Bat0_Cur_Pre_CHARGE         para[44]  //Ԥ�����            mA 
#define Bat0_Vol_Rre_CHARGE_OVER    para[45]  //Ԥ�������ѹ        mV
#define Bat0_Cur_Rapid_CHARGE        para[46]  //������            mA
#define Bat0_Vol_Rapid_CHARGE_MAX    para[47]  //�����ߵ�ѹ        mV
#define Bat0_Time_Fast_CHARGE_MAX   para[48]  //��ߵ�ѹ����ʱ��    S
#define Bat0_Mode_Fast_CHARGE_OVER  para[49]  //������ʽ            1:תΪ��� 0:ֱ�ӽ���
#define Bat0_DeltaV_Fast_VHARGE     para[50]  //������-��V        *10 mV    
#define Bat0_Cur_Add_CHARGE         para[51]  //���������        mA
#define Bat0_Time_Add_CHARGE        para[52]  //����ά��ʱ��        Min
#define Bat0_PowerFull_Retain       para[53]  //���籣�ֹ���        1:����  0:�ر�
#define Bat0_Time_CHARGE_MAX        para[54]  //���γ�����ʱ��    Hour

#define Bat0_Cur_Nor_CHARGE         para[55]  //��׼������

#define Bat0_Charge_Mode             para[57]  //��׼������
#define Bat0_DisCharge_On_Off          para[58]  //���״̬             mV




////////////////////////////////////////////////////////////////////////////////////////////////
// BAT1 ����

#define Bat1_Chargeable             para[132]  //����Ƿ��ǿɳ����
#define Bat1_Test_Insert            para[133]  //������Ƿ����
#define Bat1_Vol_Repair_Min         para[134]  //���޸���ص�ѹ����   mV
#define Bat1_Vol_Charge_Min         para[135]  //�ɳ���ѹ��Χ����   mV
#define Bat1_Vol_Discharge_Min      para[136]  //��ŵ��ѹ��Χ����   mV
#define Bat1_Vol_PowerFull_Min      para[137]  //�����ѹ����         mV
#define Bat1_VOL_ClassErr_MIN       para[138]  //��������������     mV

#define Bat1_Cur_DisCharge          para[139]  //�ŵ����             mA
#define Bat1_Vol_DisCharge_OVER     para[140]  //�ŵ������ѹ         mV
#define Bat1_Cur_Repair             para[141]  //����޸�����        mA
#define Bat1_Vol_Repair_OVER        para[142]  //�޸�������ѹ        mV
#define Bat1_Time_Repair_MAX        para[143]  //����޸�ʱ��        Min

#define Bat1_Cur_Pre_CHARGE         para[144]  //Ԥ�����            mA 
#define Bat1_Vol_Rre_CHARGE_OVER    para[145]  //Ԥ�������ѹ        mV
#define Bat1_Cur_Rapid_CHARGE        para[146]  //������            mA
#define Bat1_Vol_Fast_CHARGE_MAX    para[147]  //�����ߵ�ѹ        mV
#define Bat1_Fast_Top_Vol_Time      para[148]  //��ߵ�ѹ����ʱ��    S
#define Bat1_Mode_Fast_CHARGE_OVER  para[149]  //������ʽ            1:תΪ��� 0:ֱ�ӽ���
#define Bat1_DeltaV_Fast_VHARGE     para[150]  //������-��V        *10 mV    
#define Bat1_Cur_Add_CHARGE         para[151]  //���������        mA
#define Bat1_Time_Add_CHARGE        para[152]  //����ά��ʱ��        Min
#define Bat1_PowerFull_Retain       para[153]  //���籣�ֹ���        1:����  0:�ر�
#define Bat1_Time_Fast_CHARGE_MAX   para[154]  //���ο�����ʱ��    sec

#define Bat1_Cur_Nor_CHARGE         para[155]  //��׼������
#define Bat1_Charge_On_Off          para[156]  //���״̬             mV
#define Bat1_Charge_Mode             para[157]  //��׼������

#define Bat1_DisCharge_On_Off          para[158]  //���״̬             mV

#endif

/**************************************************************************************��*****************
**                                 END OF FILE
********************************************************************************************************/
