#ifndef	_MENU__H
#define	_MENU__H

#include "global.h"

extern INT8U   VERSION[20];     // ����汾��

extern INT16U    mptr0[20];       // �������ݵĵ�һ�����ݱ��
extern INT8U   const mptr[];
extern INT8U   LED_DISP;        // �������ʾ����
extern INT16U  LED_VAL;         // �������ʾ��ֵ


extern char   datetime[6];
extern INT8U  inst_light[24];
extern INT16U   group;    // ����

#define      max_para        20

extern INT8U FWD,REV,RUN;
extern INT16U  language;
//extern char  buff[20];
extern INT16U	 para_val,edit_val;
extern INT8U const mptr[];         // ��������
extern INT8U const para_dot[];     // ������С����, 0 ��ʾ����, 1 ����1λС����,2 ����2λС����...
extern INT8U const status_dot[];

extern  INT8U   disp_ptr;   // ��һ��ʾ��ָ��
extern  INT8U   sel_ptr;    // ��ǰѡ����ָ��
extern  INT8U   func_ptr,func1_ptr;   // ����ָ��,�ӹ���ָ��
extern  INT16U  para_val,edit_val;

extern  INT8U   Menu_times_count;   // �˵�����������
void  display_menu(void);



#endif

