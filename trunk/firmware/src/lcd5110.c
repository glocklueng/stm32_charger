/****************************************Copyright (c)**************************************************
**                              智 能 充 电 器 开 发 小 组
**                                      麦思网 论坛
**                                     QQ 群: 7095643
**
**                               http://www.mystm32.com/bbs
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: lcd5110.c
**创   建   人: 蟲子
**最后修改日期: 2009年09月21日 11:26
**描        述: 智能充电器
**
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 蟲子
** 版  本: v0.01
** 日　期: 2009年9月20日
** 描　述: 原始版本
**
**--------------当前版本信息----------------------------------------------------------------------------
** 创建人: 蟲子
** 版  本: v0.04
** 日　期: 2009年9月21日
** 描　述: 当前版本
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#include <stm32f10x.h>
#include "lcd5110.h"
#include "ziku.h"
#include "string.h"
#include "global.h"
#include "para.h"
#include "charge.h"
#include "adc.h"


INT8U cursor_x,cursor_y,cursor_mode = 1;

INT8U const hex2asc[]  = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
INT8U const hex2asc1[] = {' ','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

INT8S   DBuf[DBufSize],DBuf1[DBufSize],DBufB[DBufSize];

INT8U scr[13][3];

//英文字库,5*8点阵,每一个字符占用5个字节,共94个可显示字符数据
u8 ASCLL_DATA[]=
{
    0x00, 0x00, 0x00, 0x00, 0x00,   // sp
    0x00, 0x00, 0x2f, 0x00, 0x00,   // !
    0x00, 0x07, 0x00, 0x07, 0x00,   // "
    0x14, 0x7f, 0x14, 0x7f, 0x14,   // #
    0x24, 0x2a, 0x7f, 0x2a, 0x12,   // $
    0x62, 0x64, 0x08, 0x13, 0x23,   // %
    0x36, 0x49, 0x55, 0x22, 0x50,   // &
    0x00, 0x05, 0x03, 0x00, 0x00,   // '
    0x00, 0x1c, 0x22, 0x41, 0x00,   // (
    0x00, 0x41, 0x22, 0x1c, 0x00,   // )
    0x14, 0x08, 0x3E, 0x08, 0x14,   // *
    0x08, 0x08, 0x3E, 0x08, 0x08,   // +
    0x00, 0x00, 0xA0, 0x60, 0x00,   // ,
    0x08, 0x08, 0x08, 0x08, 0x08,   // -
    0x00, 0x60, 0x60, 0x00, 0x00,   // .
    0x20, 0x10, 0x08, 0x04, 0x02,   // /
    0x3E, 0x51, 0x49, 0x45, 0x3E,   // 0
    0x00, 0x42, 0x7F, 0x40, 0x00,   // 1
    0x42, 0x61, 0x51, 0x49, 0x46,   // 2
    0x21, 0x41, 0x45, 0x4B, 0x31,   // 3
    0x18, 0x14, 0x12, 0x7F, 0x10,   // 4
    0x27, 0x45, 0x45, 0x45, 0x39,   // 5
    0x3C, 0x4A, 0x49, 0x49, 0x30,   // 6
    0x01, 0x71, 0x09, 0x05, 0x03,   // 7
    0x36, 0x49, 0x49, 0x49, 0x36,   // 8
    0x06, 0x49, 0x49, 0x29, 0x1E,   // 9
    0x00, 0x36, 0x36, 0x00, 0x00,   // :
    0x00, 0x56, 0x36, 0x00, 0x00,   // ;
    0x08, 0x14, 0x22, 0x41, 0x00,   // <
    0x14, 0x14, 0x14, 0x14, 0x14,   // =
    0x00, 0x41, 0x22, 0x14, 0x08,   // >
    0x02, 0x01, 0x51, 0x09, 0x06,   // ?
    0x32, 0x49, 0x59, 0x51, 0x3E,   // @
    0x7C, 0x12, 0x11, 0x12, 0x7C,   // A
    0x7F, 0x49, 0x49, 0x49, 0x36,   // B
    0x3E, 0x41, 0x41, 0x41, 0x22,   // C
    0x7F, 0x41, 0x41, 0x22, 0x1C,   // D
    0x7F, 0x49, 0x49, 0x49, 0x41,   // E
    0x7F, 0x09, 0x09, 0x09, 0x01,   // F
    0x3E, 0x41, 0x49, 0x49, 0x7A,   // G
    0x7F, 0x08, 0x08, 0x08, 0x7F,   // H
    0x00, 0x41, 0x7F, 0x41, 0x00,   // I
    0x20, 0x40, 0x41, 0x3F, 0x01,   // J
    0x7F, 0x08, 0x14, 0x22, 0x41,   // K
    0x7F, 0x40, 0x40, 0x40, 0x40,   // L
    0x7F, 0x02, 0x0C, 0x02, 0x7F,   // M
    0x7F, 0x04, 0x08, 0x10, 0x7F,   // N
    0x3E, 0x41, 0x41, 0x41, 0x3E,   // O
    0x7F, 0x09, 0x09, 0x09, 0x06,   // P
    0x3E, 0x41, 0x51, 0x21, 0x5E,   // Q
    0x7F, 0x09, 0x19, 0x29, 0x46,   // R
    0x46, 0x49, 0x49, 0x49, 0x31,   // S
    0x01, 0x01, 0x7F, 0x01, 0x01,   // T
    0x3F, 0x40, 0x40, 0x40, 0x3F,   // U
    0x1F, 0x20, 0x40, 0x20, 0x1F,   // V
    0x3F, 0x40, 0x38, 0x40, 0x3F,   // W
    0x63, 0x14, 0x08, 0x14, 0x63,   // X
    0x07, 0x08, 0x70, 0x08, 0x07,   // Y
    0x61, 0x51, 0x49, 0x45, 0x43,   // Z
    0x00, 0x7F, 0x41, 0x41, 0x00,   // [
    0x55, 0x2A, 0x55, 0x2A, 0x55,   // 55
    0x00, 0x41, 0x41, 0x7F, 0x00,   // ]
    0x04, 0x02, 0x01, 0x02, 0x04,   // ^
    0x40, 0x40, 0x40, 0x40, 0x40,   // _
    0x00, 0x01, 0x02, 0x04, 0x00,   // '
    0x20, 0x54, 0x54, 0x54, 0x78,   // a
    0x7F, 0x48, 0x44, 0x44, 0x38,   // b
    0x38, 0x44, 0x44, 0x44, 0x20,   // c
    0x38, 0x44, 0x44, 0x48, 0x7F,   // d
    0x38, 0x54, 0x54, 0x54, 0x18,   // e
    0x08, 0x7E, 0x09, 0x01, 0x02,   // f
    0x18, 0xA4, 0xA4, 0xA4, 0x7C,   // g
    0x7F, 0x08, 0x04, 0x04, 0x78,   // h
    0x00, 0x44, 0x7D, 0x40, 0x00,   // i
    0x40, 0x80, 0x84, 0x7D, 0x00,   // j
    0x7F, 0x10, 0x28, 0x44, 0x00,   // k
    0x00, 0x41, 0x7F, 0x40, 0x00,   // l
    0x7C, 0x04, 0x18, 0x04, 0x78,   // m
    0x7C, 0x08, 0x04, 0x04, 0x78,   // n
    0x38, 0x44, 0x44, 0x44, 0x38,   // o
    0xFC, 0x24, 0x24, 0x24, 0x18,   // p
    0x18, 0x24, 0x24, 0x18, 0xFC,   // q
    0x7C, 0x08, 0x04, 0x04, 0x08,   // r
    0x48, 0x54, 0x54, 0x54, 0x20,   // s
    0x04, 0x3F, 0x44, 0x40, 0x20,   // t
    0x3C, 0x40, 0x40, 0x20, 0x7C,   // u
    0x1C, 0x20, 0x40, 0x20, 0x1C,   // v
    0x3C, 0x40, 0x30, 0x40, 0x3C,   // w
    0x44, 0x28, 0x10, 0x28, 0x44,   // x
    0x1C, 0xA0, 0xA0, 0xA0, 0x7C,   // y
    0x44, 0x64, 0x54, 0x4C, 0x44,   // z
    0x00, 0x08, 0x36, 0x41, 0x00,  // {
    0x00, 0x00, 0x7F, 0x00, 0x00,   // |
    0x00, 0x41, 0x36, 0x08, 0x00,   // }
    0x08, 0x10, 0x08, 0x04, 0x08    // ~
};



//=======================================================================================


/*******************************************************************************
* Function Name  : SPI_LCD_Init
* Description    : SPI_LCD_Init
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_LCD_Init(void)
{
    volatile unsigned int u;
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2 , ENABLE);

    /* Enable GPIOE for LCD5110 RST pin */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_RST, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_RST;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIO_RST, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CS;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIO_CS, &GPIO_InitStructure);

    GPIO_SetBits(GPIO_CS, GPIO_Pin_CS);

    //复位 LCD5110 RST引脚拉低
    LCD5110_RST_LOW();
    for (u=0; u<(14*(20000)); u++);

    /* Configure SPI1 pins: SCK, MISO and MOSI */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Configure I/O for LCD5110 DC */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_DC;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIO_DC, &GPIO_InitStructure);


    /* SPI1 configuration */
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex  ; //SPI设置为双线双向全双工
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master; //设置为主 SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; //SPI发送接收 8 位帧结构
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;  //时钟悬空高
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; //数据捕获于第二个时钟沿
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//内部 NSS 信号有 SSI位控制
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;//波特率预分频值为 256
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//数据传输从 MSB 位开始
    SPI_InitStructure.SPI_CRCPolynomial = 7;//定义了用于 CRC值计算的多项式 7
    SPI_Init(SPI2, &SPI_InitStructure);

    /* Enable SPI1  */
    SPI_Cmd(SPI2, ENABLE);
}

/*******************************************************************************
* Function Name  : SPI_ReadByte
* Description    : SPI_ReadByte
* Input          : None
* Output         : None
* Return         : Byte Read from the SPI .
*******************************************************************************/
u8 SPI2_ReadByte(void)
{
    return (SPI2_SendByte(0x00));
}

/*******************************************************************************
* Function Name  : SPI_SendByte
* Description    : Sends a byte through the SPI interface and return the byte
*                  received from the SPI bus.
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
u8 SPI2_SendByte(u8 byte)
{
    /* Loop while DR register in not emplty */
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

    /* Send byte through the SPI1 peripheral */
    SPI_I2S_SendData(SPI2, byte);

    /* Wait to receive a byte */
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

    /* Return the byte read from the SPI bus */
    return SPI_I2S_ReceiveData(SPI2);
}


////////////////////////////////////////////////////////////////////////////////
void LCD5110_Init(void)
{
    //复位 LCD5110 RST引脚拉高
    LCD5110_RST_HIGH();
    LCD5110_SEND_CMD();

    lcd_cs_0();
    SPI2_SendByte(0x21);	//使用扩展命令设置LCD模式
    SPI2_SendByte(0xC8);	//设置偏置电压
    SPI2_SendByte(0x06);	//温度校正
    SPI2_SendByte(0x13);	//1:48
    SPI2_SendByte(0x20);	//使用基本命令
    SPI2_SendByte(0x0C);	//设定显示模式，正常显示
    lcd_cs_1();
}

//nokia5110清屏,光标复位
void LCD5110_CLS(void)
{
    INT16U i=0;

    lcd_cs_0();
    LCD5110_SEND_CMD();
    //光标回到0列
    SPI2_SendByte( 128 );
    //光标回到0行
    SPI2_SendByte( 64 );
    LCD5110_SEND_DATA();

    //写504个0数据,就是清屏
    for( i=0; i<504; i++)
        SPI2_SendByte( 0 );
    lcd_cs_1();
}

//光标定位,x(0-83)是列地址,y(0-5)是行地址
void LCD_GOTOXY( u8 x , u8 y )
{
    LCD5110_SEND_CMD();
    SPI2_SendByte( x + 128 );
    SPI2_SendByte( y + 64 );
}

// 设置光标
void set_cursor(INT8U x,INT8U y)
{
    cursor_x = x;
    cursor_y = y;
}


//以ASCII值的方式显示一个变量
void LCD5110_PUT_ASCII(u8 ASCII)
{
    u8 i;
    INT16U index;
    //字模数据是由空格' '开始,空格的ASCII的值就是32
    index = ASCII - 32;
    //每个字符的字模是5个字节
    index = index * 5;
    LCD5110_SEND_DATA();
    //一个字符的字模是5个字节,就是5*8点阵
    for(i=0; i<5; i++)
    {
        SPI2_SendByte( ASCLL_DATA[index++] );
    }
    //每个字符之间空一列
    SPI2_SendByte(0);
}

void LCD5110_PUT_STR(u8 *str , u8 n)
{
    while( n-- )
    {
        //顺序显示字符
        LCD5110_PUT_ASCII( *str++ );
    }
}

void LCD5110_PUT_HZK16( u8 x , u8 y , u8 *str)
{
    u8 i;

    LCD_GOTOXY( x , y );

    LCD5110_SEND_DATA();
    //一个汉字的字模是16*16
    //先写上半部分的16行 8*16
    for( i=0; i<16; i++ )
    {
        SPI2_SendByte( *str++ );
    }

    LCD_GOTOXY( x , y + 1 );

    LCD5110_SEND_DATA();
    //再写下半部分的16行 8*16
    for( i=0; i<16; i++ )
    {
        SPI2_SendByte( *str++ );
    }
}


void LCD5110_PUT_HZK12( u8 x , u8 y , INT16U index)
{
    u8 i;
    u8 *p;


    u8 x0;

    lcd_cs_0();
    x0 = x * 6;
    LCD_GOTOXY( x0 , y );

    LCD5110_SEND_DATA();
    //一个汉字的字模是12*12
    //先写上半部分的12行 6*12

    p = (INT8U *)(Hzk[index]);
    for( i=0; i<12; i++ )
    {
        SPI2_SendByte( *p++ );
    }

    LCD_GOTOXY( x0 , y + 1 );

    LCD5110_SEND_DATA();
    //再写下半部分的12行 6*12
    for( i=0; i<12; i++ )
    {
        SPI2_SendByte( *p++ );
    }
    lcd_cs_1();
}


void LCD5110_PUT_ASC12( u8 x , u8 y , INT16U index )
{
    u8 i;
    u8 *p;

    u8 x0;

    lcd_cs_0();
    x0 = x * 6;
    LCD_GOTOXY( x0 , y );

    LCD5110_SEND_DATA();
    //一个汉字的字模是12*12
    //先写上半部分的12行 6*12
    p = (INT8U *)(ascii[index]);

    if ((cursor_x == x) && (cursor_y == y) && cursor_mode)
    {
        for( i=0; i<6; i++ )
        {
            SPI2_SendByte( ~ ( *p++ ));
        }
    }
    else
    {
        for( i=0; i<6; i++ )
        {
            SPI2_SendByte( *p++ );
        }
    }

    LCD_GOTOXY( x0 , y + 1 );

    LCD5110_SEND_DATA();
    //再写下半部分的12行 6*12
    if ((cursor_x == x) && (cursor_y == y) && cursor_mode)
    {
        for( i=0; i<6; i++ )
        {
            SPI2_SendByte( ~ ( *p++ ));
        }
    }
    else
    {
        for( i=0; i<6; i++ )
        {
            SPI2_SendByte( *p++ );
        }
    }
    lcd_cs_1();
}


/*************************************************
* 名称：gethzindex
* 功能：获得汉字的索引
* 输入：
        buf 指向要查询索引的汉字
* 输出:
* 返回：索引值
**************************************************/
INT16U GetHzIndex(INT8U  Code0,INT8U Code1)
{
    INT16U   i = 0, j = 0, k= 0;

    k = strlen((char *)HzkIndex)-2;

    for(i=0,j=0; i < k; i += 2,j++)
    {
        if ((Code0 == HzkIndex[i]) && (Code1 == HzkIndex[i + 1]))
        {
            return j;
        }
    }

    return 0;
}

/*************************************************
* 名称：gethzindex
* 功能：获得汉字的索引
* 输入：
        buf 指向要查询索引的汉字
* 输出:
* 返回：索引值
**************************************************/
INT16U gethzindex(INT8S *buf)
{
    INT16U i=0, j=0, k=0;

    k = strlen((char*)HzkIndex)-2;
    for(i=0,j=0; i < k; i += 2,j++)
    {
        if ( ((INT8U)*buf == HzkIndex[i]) && ((INT8U)*(buf + 1) == HzkIndex[i + 1]) )
        {
            return j;
        }
    }

    return 0;
}

/*************************************************
* 名称：sprfi
* 功能：有符号数格式化输出
* 输入：
        buf 输出指针
        num 要输出的数值
        dot 小数点的位置 例如1.00，则 dot = 2
        len num的长度，例如: num = 128,则 len = 3
* 输出: buf
* 返回：无
**************************************************/
void sprfi(INT8S *buf,INT16U num,INT8U dot,INT8U len)
{
    INT8U sign;
    INT8U i,j,k;

    k = len;
    if (dot == 0) buf[0] = ' ';

    sign = 1;
    if (num > 0x7fff)
    {
        k++;
        sign = 0;
        num = ~num + 1;
    }

    for (i = 1; i <= len; i++)
    {
        j = num % 10;
        num = num / 10;
        if ((i > dot + 1) && (num == 0)) buf[k] = hex2asc1[j];
        else buf[k] = hex2asc[j];
        k--;
        if (i == dot)
        {
            buf[k] = '.';
            k--;
        }
    }

    if (sign == 0)
    {
        buf[k] = '-';
    }
}

/*************************************************
* 名称：sprfu
* 功能：无符号数格式化输出
* 输入：
        buf 输出指针
        num 要输出的数值
        dot 小数点的位置,例如1.00，则 dot = 2
        len num的长度，例如: num = 128,则 len = 3
* 输出: buf
* 返回：无
**************************************************/
void sprfu(INT8S *buf,INT32U num,INT8U dot,INT8U len)
{
    INT8U i,j,k;

    if (dot == 0) k = len - 1;
    else k = len;

    for (i = 1; i <= len; i++)
    {
        j = num % 10;
        num = num / 10;
        if ((i > dot + 1) && (num == 0)) buf[k] = hex2asc1[j];
        else buf[k] = hex2asc[j];
        k--;
        if (i == dot)
        {
            buf[k] = '.';
            k--;
        }
    }

}

/* 二进制数格式化输出
void sprfb(char *buf,uint num,uchar len)
{
    uchar i,j,k;

    k = len - 1;

	for (i = 0; i < len; i++)
    {
       buf[k] = hex2asc[num & 1];
       num >>= 1;
       k--;
    }
} */

/*************************************************
* 名称：sprfb
* 功能：二进制数格式化输出
* 输入：
        buf 输出指针
        num 要输出的二进制数
        len num的长度，例如: num = 101,则 len = 3
* 输出: text
* 返回：无
**************************************************/
void sprfb(INT8S *buf,INT16U num,INT8U len)
{
    INT8U i,k;

    k = 0;

    for (i = 0; i < len; i++)
    {
        buf[k] = hex2asc[num & 1];
        num >>= 1;
        k++;
    }
}


/*************************************************
* 名称：display_str
* 功能：字符串格式化输出
* 输入：
        x    横坐标位置: 对应液晶长: 0-15
        y    横坐标位置: 对应液晶宽: 0-6
        text 要输出的字符串的指针
* 输出: text
* 返回：无
**************************************************/
void display_str(INT16U x,INT16U y,INT8S *text)
{
    INT8S        *p;
    INT16U        index,y0;
    INT8U       code0,code1;

    p = text;
    y0 = y / 2;

    while(*p)
    {
        code0 = *p;
        code1 = *(p+1);
        if((code0 >= 0xa0) && (code1 >= 0xa0)) // 汉字
        {
            if ((code0 != scr[x][y0]) || (code1 != scr[x+1][y0]))
            {
                index = gethzindex(p);
                LCD5110_PUT_HZK12(x,y,index);
                scr[x][y0] = code0;
                scr[x+1][y0] = code1;
            }
            x += 2;
            p += 2;
        }
        else // ASC 字库
        {
            if ((x == cursor_x) && (y == cursor_y))   // 光标位置要刷新
            {
                scr[x][y0] = 0;
            }
            if (code0 != scr[x][y0])
            {
                LCD5110_PUT_ASC12(x,y,code0);
                if ((x == cursor_x) && (y == cursor_y))   // 光标位置要刷新
                {
                    scr[x][y0] = 1;
                }
                else
                {
                    scr[x][y0] = code0;
                }
            }
            x += 1 ;
            p++;
        }
    }
}

void TextOut16(INT16U x,INT16U y,INT8S *ch)
{
    INT8U       Code0,Code1;
    INT16U      Index;

    Code0 = *ch;
    while(Code0)
    {
        if (Code0 >= 0x20)    // 显示字符
        {
            /*
            Index = x + y * 8;
            if ((Txt != DBufB[Index])                // 不同的字符要刷新
                || ((x == CursorX) && (y == CursorY) && CursorOn))      // 光标位置要刷新
            {
                PutAsc16(x,y,Txt);
                DBufB[Index] = Txt;
            }
            x++;
            */
            Code0 = *ch;
            ch++;
            Code1 = *ch;
            Index = x + y * 6;
            if((Code0 >= 0xa0) && (Code1 >= 0xa0)) // 汉字
            {
                if ((Code0 != DBufB[Index]) || (Code1 != DBufB[Index+1]))
                {
                    DBufB[Index] = Code0;
                    Index++;
                    DBufB[Index] = Code1;
                    Index = GetHzIndex(Code0,Code1);
                    LCD5110_PUT_HZK12(x,y,Index);
                }
                x += 12;
                ch++;
                Code0 = *ch;
            }
            else // ASC 字库
            {
                LCD5110_PUT_ASC12(x,y,Code0);
                DBufB[Index] = Code0;
                Code0 = Code1;
                x += 6;
            }

        }
        else                // 控制字符
        {
            if (Code0 == 0x0d)    // 回车
            {
                x  = 0;
            }
            if (Code0 == 0x0a)    // 换行
            {
                y += 2;
            }
            ch++;
            Code0 = *ch;
        }
    }
}
/*
void TextOut8(INT16U x,INT16U y,INT8S *ch)
{
    INT8S       Txt;
    INT16U      Index;

    Txt = *ch;
    while(Txt && (y < 5))
    {
        if (Txt >= 0x20)    // 显示字符
        {
            Index = x + y * 20;
            if (Txt != DBufB[Index])                // 不同的字符要刷新
            {
                PutAsc8(x,y,Txt);
                DBufB[Index] = Txt;
            }
            x++;
        }
        else                // 控制字符
        {
            if (Txt == 0x0d)    // 回车
            {
                x  = 0;
            }
            if (Txt == 0x0a)    // 换行
            {
                y++;
            }
        }
        ch++;
        Txt = *ch;
    }
}
  */



void LCD_ShowChargeStatus(void)
{
    u8 str[36];
    INT16U Temperature, Vref;

    LCD5110_CLS();

    memset(str,0,36);
    sprintf((char*)str,"V0: %4d mv",Bat0_Vol);
    LCD_GOTOXY( 0 , 0 );
    LCD5110_PUT_STR( str , 11);

    memset(str,0,36);
    if( 0x8000 & Bat0_Cur )
    {
        Bat0_Cur = 0x8000 - Bat0_Cur & 0x7FFF ;
        sprintf((char*)str,"I0:-%4d ma", Bat0_Cur );
    }
    else
        sprintf((char*)str,"I0: %4d ma",Bat0_Cur);

    LCD_GOTOXY( 0 , 1 );
    LCD5110_PUT_STR( str , 11);

    memset(str,0,36);
    sprintf((char*)str,"V1: %4d mv",Bat1_Vol);
    LCD_GOTOXY( 0 , 2 );
    LCD5110_PUT_STR( str , 11);

    memset(str,0,36);
    if( 0x8000 & Bat1_Cur )
    {
        Bat1_Cur = 0x8000 - Bat1_Cur & 0x7FFF ;
        sprintf((char*)str,"I1:-%4d ma", Bat1_Cur );
    }
    else
        sprintf((char*)str,"I1: %4d ma",Bat1_Cur);
    LCD_GOTOXY( 0 , 3 );
    LCD5110_PUT_STR( str , 11);

/////////////////////////////////////////////////////////////////////////////
// 显示MCU工作温度以及电压基准

    Temperature= ((1.42 - ADC_ConvertedValue[6]*3.3/4096)*1000/4.35 + 25)*10;

    Vref = ADC_ConvertedValue[7]*3300/4096;

    memset(str,0,36);
    sprintf((char*)str,"%d%d.%dC  %4dmv",Temperature/100,Temperature%100/10,Temperature%100%10,Vref);
    LCD_GOTOXY( 0 , 5 );
    LCD5110_PUT_STR( str , 13);

}

/*
void LCD5110_PUT_MULUTI_HZK12( u8 x , u8 y , u8 *str )
{
    u8 hzk12_buf[24]; //12*12汉字的字模
    //如果不是12的倍数 则不显示
    //if( x%12 !=0 || 84 == x )
    //  return;

    while(1)
    {
        //如果连续的2个字节都大于0xA0 则标明是汉字
        if( *str >0xA0 && *(str+1) >0xA0 )
        {
            Get_Single_HZK12( str, hzk12_buf );
            LCD5110_PUT_HZK12( x , y , hzk12_buf );
        }
        else
            break;
        x += 12;
        //如果一行已经写满 则自动换行
        if( 84 == x )
        {
            x = 0;
            y += 2;
        }
        str += 2;
    }
} */



