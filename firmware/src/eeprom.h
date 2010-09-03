/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : eeprom.h
* Author             : MCD Application Team
* Version            : V2.0.0
* Date               : 06/16/2008
* Description        : This file contains all the functions prototypes for the
*                      EEPROM emulation firmware library.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EEPROM_H
#define __EEPROM_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "global.h"
//#include "stm32f10x_type.h"



/* Exported constants --------------------------------------------------------*/
/* EEPROM start address in Flash */
#define EEPROM_START_ADDRESS    ((u32)0x08010000) /* EEPROM emulation start address:
                                                  after 64KByte of used Flash memory */

/* Pages 0 and 1 base and end addresses */
#define PAGE0_BASE_ADDRESS      ((u32)(EEPROM_START_ADDRESS + 0x000))
#define PAGE0_END_ADDRESS       ((u32)(EEPROM_START_ADDRESS + (PAGE_SIZE - 1)))

#define PAGE1_BASE_ADDRESS      ((u32)(EEPROM_START_ADDRESS + PAGE_SIZE))
#define PAGE1_END_ADDRESS       ((u32)(EEPROM_START_ADDRESS + (2 * PAGE_SIZE - 1)))

/* Used Flash pages for EEPROM emulation */
#define PAGE0                   ((INT16U)0x0000)
#define PAGE1                   ((INT16U)0x0001)

/* No valid page define */
#define NO_VALID_PAGE           ((INT16U)0x00AB)

/* Page status definitions */
#define ERASED                  ((INT16U)0xFFFF)     /* PAGE is empty */
#define RECEIVE_DATA            ((INT16U)0xEEEE)     /* PAGE is marked to receive data */
#define VALID_PAGE              ((INT16U)0x0000)     /* PAGE containing valid data */

/* Valid pages in read and write defines */
#define READ_FROM_VALID_PAGE    ((u8)0x00)
#define WRITE_IN_VALID_PAGE     ((u8)0x01)

/* Page full define */
#define PAGE_FULL               ((u8)0x80)

/* Variables' number */  // 用户变量的个数
#define NumbOfVar               ((u8)12)
// 2个校正电压，2个校正电流


//littleworm///////////////////////////////////////////////////////////////////////
  
//#define PAGE_SIZE  (INT16U)0x400  /* Page size = 1KByte */
#define PAGE_SIZE  (INT16U)0x800  /* Page size = 2KByte */
  
#define _FLASH
/* Uncomment the line below to enable FLASH program/erase/protections functions,
   otherwise only FLASH configuration (latency, prefetch, half cycle) functions
   are enabled */
#define _FLASH_PROG 

#define Adjust_batt0_vol_addr 0x2001
#define Adjust_batt1_vol_addr 0x2002
#define Adjust_batt0_cur_addr 0x2003
#define Adjust_batt1_cur_addr 0x2004
#define batt0_capacity_addr   0x2005
#define batt1_capacity_addr   0x2006
#define LED_Mode              0x2007
#define LCD_Light_Mode        0x2008
#define Laguage               0x2009
#define LCD_contrast          0x2010
#define LCD_show_mode         0x2011
#define Eeprom_Flag           0x2012


 

 

extern INT16U VirtAddVarTab[NumbOfVar];

//littleworm///////////////////////////////////////////////////////////////////////



/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
INT16U EE_Init(void);
INT16U EE_ReadVariable(INT16U VirtAddress, INT16U* Data);
INT16U EE_WriteVariable(INT16U VirtAddress, INT16U Data);

#endif /* __EEPROM_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
