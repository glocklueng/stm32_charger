/****************************************Copyright (c)**************************************************
**                              �� �� �� �� �� �� �� С ��
**                                      ��˼�� ��̳
**                                     QQ Ⱥ: 7095643
**
**                               http://www.mystm32.com/bbs
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: Nokia5110Lcd.c
**��   ��   ��: xylch101
**����޸�����: 2008��06��27��
**��        ��: ŵ���� 5110 LCD ����Դ�ļ�
**
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: xylch101
** ��  ��: v0.01
** �ա���: 2008��06��27��
** �衡��: ԭʼ�汾, ŵ���� 5110 LCD ����Դ�ļ�
**
**--------------��ǰ�汾��Ϣ----------------------------------------------------------------------------
** ������: ����
** ��  ��: v0.08
** �ա���: 2010��02��11�� 15:30
** �衡��: �޸�֧�����ܳ����
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#include "Nokia5110Lcd.h"
#include "PixelStuff.h" // �С�Ӣ���֡�ͼƬ��ģ
#include "lcd5110.h"

//ȫ������ ( 6x84 bytes )
INT8U LcdPixelBuffer[ LCD_Y_RES / 8 ][ 84 ] = { {0}, {0} };  

/*********************************************
������    :   LcdInit
����      :   LCD SPI ��ʼ��
����      :   ��
����      :   ��
�༭��    :   Notepad 2
*********************************************/
void LcdInit( void )
{
/*
    PORTB |= LCD_RST_PIN;
    DDRB |= LCD_RST_PIN | LCD_DC_PIN | LCD_CE_PIN | SPI_MOSI_PIN | SPI_CLK_PIN;

    PORTB &= ~LCD_RST_PIN;
    PORTB |= LCD_RST_PIN;

    //��ʼ��SPI
    SPCR = 0x50;

    PORTB |= LCD_CE_PIN;

    //����LCD
    LcdExec( 0x21, DC_CMD );
    LcdExec( 0xC8, DC_CMD );
    LcdExec( 0x06, DC_CMD );
    LcdExec( 0x13, DC_CMD );
    LcdExec( 0x20, DC_CMD );
    LcdExec( 0x0C, DC_CMD );
 */   
    //����
    ClearBuffer( );
    //CommitBuffer( );
}

/*********************************************
������    :   LcdSetContrast
����      :   ���� LCD �Աȶ�
����      :   INT8U contrast   �Աȶ�ֵ��0~127
����      :   ��
�༭��    :   Notepad 2
*********************************************/
void LcdSetContrast( INT8U contrast )
{
    LcdExec( 0x21, DC_CMD );
    LcdExec( 0x80 | contrast, DC_CMD );
    LcdExec( 0x20, DC_CMD );
}

/*********************************************
������    :   LcdSetXY
����      :   ���� LCD ��ǰ����
����      :   INT8U X  �����꣺0~83
              INT8U Y  �����꣺0~05
����      :   ��
�༭��    :   Notepad 2
*********************************************/
void LcdSetXY(INT8U X, INT8U Y)
{
    LcdExec( 0x40 | Y, DC_CMD);    // ��
    LcdExec( 0x80 | X, DC_CMD);    // ��
}

/*********************************************
������    :   LcdExec
����      :   ִ�� LCD ָ��/����
����      :   INT8U data       ָ��/����ֵ
              LcdExecType dc  ָ��/����ѡ��(DC_CMD/DC_DATA)
����      :   ��
�༭��    :   Notepad 2
*********************************************/
void LcdExec( INT8U data, LcdExecType dc )
{

    if ( dc == DC_DATA )
    {
        LCD5110_SEND_DATA();
    }
    else
    {
        LCD5110_SEND_CMD();
    }

    SPI2_SendByte(data);
} 

/*********************************************
������    :   UnClear
����      :   ��nλ����
����      :   INT8U data   8λ����
              INT8U n      �������λ
����      :   INT8U        �����Ľ��
����      :   dataȡ11111000 nȡ3 --> 00011000
�༭��    :   Notepad 2
*********************************************/
INT8U UnClear( INT8U data, INT8U n )
{
    INT8U tmpData, i;
    tmpData = 0;
    for( i = 0; i < n; i++ )
    {
        tmpData |= ( 1 << ( 7 - i ) );
    }
    
    return ( data & ( ~tmpData ) );
}

/*********************************************
������    :   LnClear
����      :   ��nλ����
����      :   INT8U data   8λ����
              INT8U n      �������λ
����      :   INT8U        �����Ľ��
����      :   dataȡ00011111 nȡ3 --> 00011000
�༭��    :   Notepad 2
*********************************************/
INT8U LnClear( INT8U data, INT8U n )
{
    INT8U tmpData, i;
    tmpData = 0;
    for( i = 0; i < n; i++ )
    {
        tmpData |= ( 1 << i );
    }
    
    return ( data & ( ~tmpData ) );
}

/*********************************************
������    :   SplitByte
����      :   һ���ֽڲ�������ֽ�
����      :   INT8U data       8λ����
              INT8U nByLow     �ӵ�λ�����λ��
              INT8U bytes[ 2 ] ��ŷ��ص������ֽ�
����      :   ��
����      :   dataȡ11111111 nByLowȡ2 --> bytes[0]=11111100��bytes[1]=00000011
�༭��    :   Notepad 2
*********************************************/
void SplitByte( INT8U data, INT8U nByLow, INT8U bytes[ 2 ] )
{
    bytes[ 0 ] = data << nByLow;
    bytes[ 1 ] = data >> ( 8 - nByLow );
}

/*********************************************
������    :   BufferPixelStuff
����      :   ��ͼƬ�����ֵ���д��ȫ������(��ȷ��λ�����Ͻ��������ΪLCD������λ�á�
              �ַ���С���ޣ������޸�����������ݣ������ַ��Ŀ�߲�����Ҫ�����ˣ�16��
              ��ģ��Ҫ����12��12�ˣ�12����ģ��Ҫ����16��16��)
����      :   INT8U type       type=1-->ͼƬ���� / type=2-->ascii�ַ� / type=3-->�����ַ�
              INT8U index      �����������е�λ��(��type=2ʱ��ֱ�Ӵ�ascii�ַ�,��:'A')
              INT8U x          ������:0~83
              INT8U y          ������:0~47
              INT8U width      �������ݵĿ��(λ��) ������ַ���Ϊһ���ַ��Ŀ�ȡ������ͼƬ��Ϊ����ͼƬ�Ŀ��
              INT8U height     �������ݵĸ߶�(λ��) ������ַ���Ϊһ���ַ��ĸ߶ȡ������ͼƬ��Ϊ����ͼƬ�ĸ߶�
����      :   ��
�༭��    :   Notepad 2
*********************************************/
void BufferPixelStuff( INT8U type, INT8U index, INT8U x, INT8U y, INT8U width, INT8U height)
{
    INT8U i, j, tmpRow;
    INT8U row = y / 8;
    INT8U offset = y % 8;
    INT8U level = ( height % 8 == 0 ? height / 8 : ( height / 8 + 1 ) );
    
    INT8U bytes[ 2 ] = { 0 };
    INT8U byteTmp;
    
    for( j=0; j < width; j++ )
    {
        tmpRow = row;
        byteTmp = UnClear( LcdPixelBuffer[ tmpRow ][ x + j ], 8 - offset );
        
        for( i=0; i < level; i++ )
        {
            if( type == 1 )
            {
                SplitByte( PIXEL_BMP[ index ][ i * width + j ] , offset, bytes);
            }
            else if( type == 2 )
            {
                SplitByte( PIXEL_ENGLISH_CHAR[ index - 32 ][ i * width + j ] , offset, bytes);
            }
            else if( type == 3 )
            {
                SplitByte( PIXEL_CHINESE_CHAR[ index ][ i * width + j ] , offset, bytes);
            }
            else
            {
                return;
            }
    
            LcdPixelBuffer[ tmpRow++ ][ x + j ] = byteTmp | bytes[ 0 ];
            byteTmp = bytes[ 1 ];
        }       
        LcdPixelBuffer[ tmpRow ][ x + j ] = byteTmp | LnClear( LcdPixelBuffer[ tmpRow ] [x + j ], ( offset + height ) % 8 );
    }
}

/*********************************************
������    :   CacheEnglishString
����      :   ��ascii�ַ���д��ȫ������(��ȷ��λ�����Ͻ��������ΪLCD������λ��)
����      :   INT8U x          ������:0~83
              INT8U y          ������:0~47
              INT8U width      һ���ַ��Ŀ��(λ��)
              INT8U height     һ���ַ��ĸ߶�(λ��)
              INT8U bWidth     �ַ����
              INT8U *dataPtr   Ҫ��ʾ���ַ��� (��:"Hello world!")
����      :   ��
�༭��    :   Notepad 2
*********************************************/
void CacheEnglishString( INT8U x, INT8U y, INT8U width, INT8U height, INT8U bWidth, INT8U *dataPtr )
{
    INT8U i = 0;
    while ( *dataPtr )
    {
        BufferPixelStuff( 2, *dataPtr++, x + i * ( width + bWidth ), y, width, height );
        i++;
    }
}

/*********************************************
������    :   CachePixel
����      :   ��һ���㵽��ȫ��������(��ȷ��λ���������ΪLCD������λ��)
����      :   INT8U x              ������:0~83
            INT8U y              ������:0~47
            INT8U width          һ���ַ��Ŀ��(λ��)
            INT8U height         һ���ַ��ĸ߶�(λ��)
            LcdPixelMode mode   ���㷽ʽ (PIXEL_CLEAR-->������PIXEL_DRAW-->���㡢PIXEL_XOR-->ȡ��)
����      :   ��
�༭��    :   Notepad 2
*********************************************/
void CachePixel( INT8U x, INT8U y, LcdPixelMode mode )
{
    INT8U  row;
    INT8U  offset;
    INT8U  data;

    if ( x > LCD_X_RES ) return;
    if ( y > LCD_Y_RES ) return;

    row = y / 8;
    offset  = y % 8;

    data = LcdPixelBuffer[ row ][ x ];

    if ( mode == PIXEL_CLEAR )
    {
        data &= (~(0x01 << offset));
    }
    else if ( mode == PIXEL_DRAW )
    {
        data |= (0x01 << offset);
    }
    else if ( mode  == PIXEL_XOR )
    {
        data ^= (0x01 << offset);
    }

    LcdPixelBuffer[ row ][ x ] = data;
}

/*********************************************
������    :   CommitBuffer
����      :   �ύ���棬�ѵ�ǰ�����ϵ�����д��LCD��
����      :   ��
����      :   ��
�༭��    :   Notepad 2
*********************************************/
void CommitBuffer( void )
{
    INT8U row, col;

    LcdSetXY( 0x00, 0x00 );
    
    for ( row = 0; row < LCD_Y_RES / 8; row++ )
    {
        for ( col = 0; col < LCD_X_RES; col++ )
        {
            LcdExec( LcdPixelBuffer[ row ][ col ], DC_DATA );
        }
     }
}

/*********************************************
������    :   ClearBuffer
����      :   ��ջ���
����      :   ��
����      :   ��
�༭��    :   Notepad 2
*********************************************/
void ClearBuffer( void )
{
    INT8U row, col;
    for( row = 0; row < LCD_Y_RES / 8; row++ )
    {
        for( col=0; col < LCD_X_RES; col++ )
        {
            LcdPixelBuffer[ row ][ col ] = 0x00;
        }
    }
}

/*********************************************
������    :   TestNOKIA5110
����      :   ��������ʾ����
����      :   ��
����      :   ��
�༭��    :   Notepad 2
*********************************************/
void TestNOKIA5110(void)
{
    INT8U i, j, k, l;

    ClearBuffer( );

    /***********************
        дascii�ַ���
        λ��(0,0)
        ���������ַ���ģ��С5X7
        �ַ����2
        �ַ���"Hello everyone!"
    ***********************/
    CacheEnglishString( 0, 0, 5, 7, 2, "Hello world!" );    
    
    /***********************
        д'O','u','r'�����ַ� + "���ǵ�"������ + "AVR"48x24ͼƬ
        λ��   'O'(12,10)   'u'(19,10)   'r'(26,10)   "��"(0,8)   "��"(0,20)    ͼƬ(31,10)
        ���������ַ���ģ��С12X12
    ***********************/
    BufferPixelStuff( 2, 'O', 12, 10, 5, 7);    //'O'
    BufferPixelStuff( 2, 'u', 19, 10, 5, 7);    //'u'
    BufferPixelStuff( 2, 'r', 26, 10, 5, 7);    //'r'
    
    BufferPixelStuff( 3, 0, 0, 8, 12, 12);      //"��"
    BufferPixelStuff( 3, 1, 0, 20, 12, 12); //"��"
    
    BufferPixelStuff( 2, 'D', 13, 22, 5, 7);    //'D'
    BufferPixelStuff( 2, 'E', 21, 22, 5, 7);    //'E'
    
    BufferPixelStuff( 1, 0, 31, 10, 48, 24);    //ͼƬ
    
    
    /***********************
        ������
    ***********************/

    for( j = 0; j<6; j++ )
    {
        for( i = 0; i<70; i++ )
        {
            CachePixel( 7 + i, 34 + j*2, PIXEL_DRAW );
        }
    }
    
    /******��ʾЧ��******
    
    Hello everyone!     
    ��Our /\ \  / |_)
    ��DE /  \ \/  | \
     ---------------
     ---------------
     ---------------
    ******************/ 
    CommitBuffer( ); // �ύ����

    SysDelay(10000);

    // ���ⲿ��ע��ȥ�ˣ���ʾ����ʫ�䡣
    ClearBuffer( ); 
    for( k = 0; k < 4; k++ )
    {
        for( l = 0; l < 6; l++ )
        {
            BufferPixelStuff( 3, 2 + k * 6 + l, l * 12, k * 12, 12, 12);
        }
    }
       
    CommitBuffer( ); // �ύ����

    SysDelay(10000);
}
