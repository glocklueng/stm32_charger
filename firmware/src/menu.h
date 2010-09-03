#ifndef	_MENU__H
#define	_MENU__H

#include "global.h"

extern INT8U   VERSION[20];     // 程序版本号

extern INT16U    mptr0[20];       // 分组数据的第一个数据编号
extern INT8U   const mptr[];
extern INT8U   LED_DISP;        // 数码管显示内容
extern INT16U  LED_VAL;         // 数码管显示数值


extern char   datetime[6];
extern INT8U  inst_light[24];
extern INT16U   group;    // 分组

#define      max_para        20

extern INT8U FWD,REV,RUN;
extern INT16U  language;
//extern char  buff[20];
extern INT16U	 para_val,edit_val;
extern INT8U const mptr[];         // 参数分组
extern INT8U const para_dot[];     // 参数的小数点, 0 表示整数, 1 代表1位小数点,2 代表2位小数点...
extern INT8U const status_dot[];

extern  INT8U   disp_ptr;   // 第一显示项指针
extern  INT8U   sel_ptr;    // 当前选择项指针
extern  INT8U   func_ptr,func1_ptr;   // 功能指针,子功能指针
extern  INT16U  para_val,edit_val;

extern  INT8U   Menu_times_count;   // 菜单操作计数器
void  display_menu(void);



#endif

