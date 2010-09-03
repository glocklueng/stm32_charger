/****************************************Copyright (c)**************************************************
**                              �� �� �� �� �� �� �� С ��
**                                      ��˼�� ��̳
**                                     QQ Ⱥ: 7095643
**
**                               http://www.mystm32.com/bbs
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: lcd5110.c
**��   ��   ��: �x��
**����޸�����: 2009��09��21�� 11:26
**��        ��: ���ܳ����
**
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �x��
** ��  ��: v0.01
** �ա���: 2009��9��20��
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾��Ϣ----------------------------------------------------------------------------
** ������: �x��
** ��  ��: v0.04
** �ա���: 2009��9��21��
** �衡��: ��ǰ�汾
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

//Ӣ���ֿ�,5*8����,ÿһ���ַ�ռ��5���ֽ�,��94������ʾ�ַ�����
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

    //��λ LCD5110 RST��������
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
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex  ; //SPI����Ϊ˫��˫��ȫ˫��
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master; //����Ϊ�� SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; //SPI���ͽ��� 8 λ֡�ṹ
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;  //ʱ�����ո�
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; //���ݲ����ڵڶ���ʱ����
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//�ڲ� NSS �ź��� SSIλ����
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;//������Ԥ��ƵֵΪ 256
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//���ݴ���� MSB λ��ʼ
    SPI_InitStructure.SPI_CRCPolynomial = 7;//���������� CRCֵ����Ķ���ʽ 7
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
    //��λ LCD5110 RST��������
    LCD5110_RST_HIGH();
    LCD5110_SEND_CMD();

    lcd_cs_0();
    SPI2_SendByte(0x21);	//ʹ����չ��������LCDģʽ
    SPI2_SendByte(0xC8);	//����ƫ�õ�ѹ
    SPI2_SendByte(0x06);	//�¶�У��
    SPI2_SendByte(0x13);	//1:48
    SPI2_SendByte(0x20);	//ʹ�û�������
    SPI2_SendByte(0x0C);	//�趨��ʾģʽ��������ʾ
    lcd_cs_1();
}

//nokia5110����,��긴λ
void LCD5110_CLS(void)
{
    INT16U i=0;

    lcd_cs_0();
    LCD5110_SEND_CMD();
    //���ص�0��
    SPI2_SendByte( 128 );
    //���ص�0��
    SPI2_SendByte( 64 );
    LCD5110_SEND_DATA();

    //д504��0����,��������
    for( i=0; i<504; i++)
        SPI2_SendByte( 0 );
    lcd_cs_1();
}

//��궨λ,x(0-83)���е�ַ,y(0-5)���е�ַ
void LCD_GOTOXY( u8 x , u8 y )
{
    LCD5110_SEND_CMD();
    SPI2_SendByte( x + 128 );
    SPI2_SendByte( y + 64 );
}

// ���ù��
void set_cursor(INT8U x,INT8U y)
{
    cursor_x = x;
    cursor_y = y;
}


//��ASCIIֵ�ķ�ʽ��ʾһ������
void LCD5110_PUT_ASCII(u8 ASCII)
{
    u8 i;
    INT16U index;
    //��ģ�������ɿո�' '��ʼ,�ո��ASCII��ֵ����32
    index = ASCII - 32;
    //ÿ���ַ�����ģ��5���ֽ�
    index = index * 5;
    LCD5110_SEND_DATA();
    //һ���ַ�����ģ��5���ֽ�,����5*8����
    for(i=0; i<5; i++)
    {
        SPI2_SendByte( ASCLL_DATA[index++] );
    }
    //ÿ���ַ�֮���һ��
    SPI2_SendByte(0);
}

void LCD5110_PUT_STR(u8 *str , u8 n)
{
    while( n-- )
    {
        //˳����ʾ�ַ�
        LCD5110_PUT_ASCII( *str++ );
    }
}

void LCD5110_PUT_HZK16( u8 x , u8 y , u8 *str)
{
    u8 i;

    LCD_GOTOXY( x , y );

    LCD5110_SEND_DATA();
    //һ�����ֵ���ģ��16*16
    //��д�ϰ벿�ֵ�16�� 8*16
    for( i=0; i<16; i++ )
    {
        SPI2_SendByte( *str++ );
    }

    LCD_GOTOXY( x , y + 1 );

    LCD5110_SEND_DATA();
    //��д�°벿�ֵ�16�� 8*16
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
    //һ�����ֵ���ģ��12*12
    //��д�ϰ벿�ֵ�12�� 6*12

    p = (INT8U *)(Hzk[index]);
    for( i=0; i<12; i++ )
    {
        SPI2_SendByte( *p++ );
    }

    LCD_GOTOXY( x0 , y + 1 );

    LCD5110_SEND_DATA();
    //��д�°벿�ֵ�12�� 6*12
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
    //һ�����ֵ���ģ��12*12
    //��д�ϰ벿�ֵ�12�� 6*12
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
    //��д�°벿�ֵ�12�� 6*12
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
* ���ƣ�gethzindex
* ���ܣ���ú��ֵ�����
* ���룺
        buf ָ��Ҫ��ѯ�����ĺ���
* ���:
* ���أ�����ֵ
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
* ���ƣ�gethzindex
* ���ܣ���ú��ֵ�����
* ���룺
        buf ָ��Ҫ��ѯ�����ĺ���
* ���:
* ���أ�����ֵ
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
* ���ƣ�sprfi
* ���ܣ��з�������ʽ�����
* ���룺
        buf ���ָ��
        num Ҫ�������ֵ
        dot С�����λ�� ����1.00���� dot = 2
        len num�ĳ��ȣ�����: num = 128,�� len = 3
* ���: buf
* ���أ���
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
* ���ƣ�sprfu
* ���ܣ��޷�������ʽ�����
* ���룺
        buf ���ָ��
        num Ҫ�������ֵ
        dot С�����λ��,����1.00���� dot = 2
        len num�ĳ��ȣ�����: num = 128,�� len = 3
* ���: buf
* ���أ���
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

/* ����������ʽ�����
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
* ���ƣ�sprfb
* ���ܣ�����������ʽ�����
* ���룺
        buf ���ָ��
        num Ҫ����Ķ�������
        len num�ĳ��ȣ�����: num = 101,�� len = 3
* ���: text
* ���أ���
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
* ���ƣ�display_str
* ���ܣ��ַ�����ʽ�����
* ���룺
        x    ������λ��: ��ӦҺ����: 0-15
        y    ������λ��: ��ӦҺ����: 0-6
        text Ҫ������ַ�����ָ��
* ���: text
* ���أ���
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
        if((code0 >= 0xa0) && (code1 >= 0xa0)) // ����
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
        else // ASC �ֿ�
        {
            if ((x == cursor_x) && (y == cursor_y))   // ���λ��Ҫˢ��
            {
                scr[x][y0] = 0;
            }
            if (code0 != scr[x][y0])
            {
                LCD5110_PUT_ASC12(x,y,code0);
                if ((x == cursor_x) && (y == cursor_y))   // ���λ��Ҫˢ��
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
        if (Code0 >= 0x20)    // ��ʾ�ַ�
        {
            /*
            Index = x + y * 8;
            if ((Txt != DBufB[Index])                // ��ͬ���ַ�Ҫˢ��
                || ((x == CursorX) && (y == CursorY) && CursorOn))      // ���λ��Ҫˢ��
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
            if((Code0 >= 0xa0) && (Code1 >= 0xa0)) // ����
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
            else // ASC �ֿ�
            {
                LCD5110_PUT_ASC12(x,y,Code0);
                DBufB[Index] = Code0;
                Code0 = Code1;
                x += 6;
            }

        }
        else                // �����ַ�
        {
            if (Code0 == 0x0d)    // �س�
            {
                x  = 0;
            }
            if (Code0 == 0x0a)    // ����
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
        if (Txt >= 0x20)    // ��ʾ�ַ�
        {
            Index = x + y * 20;
            if (Txt != DBufB[Index])                // ��ͬ���ַ�Ҫˢ��
            {
                PutAsc8(x,y,Txt);
                DBufB[Index] = Txt;
            }
            x++;
        }
        else                // �����ַ�
        {
            if (Txt == 0x0d)    // �س�
            {
                x  = 0;
            }
            if (Txt == 0x0a)    // ����
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
// ��ʾMCU�����¶��Լ���ѹ��׼

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
    u8 hzk12_buf[24]; //12*12���ֵ���ģ
    //�������12�ı��� ����ʾ
    //if( x%12 !=0 || 84 == x )
    //  return;

    while(1)
    {
        //���������2���ֽڶ�����0xA0 ������Ǻ���
        if( *str >0xA0 && *(str+1) >0xA0 )
        {
            Get_Single_HZK12( str, hzk12_buf );
            LCD5110_PUT_HZK12( x , y , hzk12_buf );
        }
        else
            break;
        x += 12;
        //���һ���Ѿ�д�� ���Զ�����
        if( 84 == x )
        {
            x = 0;
            y += 2;
        }
        str += 2;
    }
} */



