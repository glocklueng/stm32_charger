/****************************************Copyright (c)**************************************************
**                              智 能 充 电 器 开 发 小 组
**                                      麦思网 论坛
**                                     QQ 群: 7095643
**
**                               http://www.mystm32.com/bbs
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: Nokia5110Lcd.c
**创   建   人: xylch101
**最后修改日期: 2008年06月27日
**描        述: 诺基亚 5110 LCD 驱动源文件
**
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: xylch101
** 版  本: v0.01
** 日　期: 2008年06月27日
** 描　述: 原始版本, 诺基亚 5110 LCD 驱动源文件
**
**--------------当前版本信息----------------------------------------------------------------------------
** 创建人: 安哥
** 版  本: v0.08
** 日　期: 2010年02月11日 15:30
** 描　述: 修改支持智能充电器
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#include "Nokia5110Lcd.h"
#include "PixelStuff.h" // 中、英文字、图片字模
#include "lcd5110.h"

//全屏缓存 ( 6x84 bytes )
INT8U LcdPixelBuffer[ LCD_Y_RES / 8 ][ 84 ] = { {0}, {0} };  

/*********************************************
函数名    :   LcdInit
功能      :   LCD SPI 初始化
参数      :   无
返回      :   无
编辑器    :   Notepad 2
*********************************************/
void LcdInit( void )
{
/*
    PORTB |= LCD_RST_PIN;
    DDRB |= LCD_RST_PIN | LCD_DC_PIN | LCD_CE_PIN | SPI_MOSI_PIN | SPI_CLK_PIN;

    PORTB &= ~LCD_RST_PIN;
    PORTB |= LCD_RST_PIN;

    //初始化SPI
    SPCR = 0x50;

    PORTB |= LCD_CE_PIN;

    //设置LCD
    LcdExec( 0x21, DC_CMD );
    LcdExec( 0xC8, DC_CMD );
    LcdExec( 0x06, DC_CMD );
    LcdExec( 0x13, DC_CMD );
    LcdExec( 0x20, DC_CMD );
    LcdExec( 0x0C, DC_CMD );
 */   
    //清屏
    ClearBuffer( );
    //CommitBuffer( );
}

/*********************************************
函数名    :   LcdSetContrast
功能      :   设置 LCD 对比度
参数      :   INT8U contrast   对比度值：0~127
返回      :   无
编辑器    :   Notepad 2
*********************************************/
void LcdSetContrast( INT8U contrast )
{
    LcdExec( 0x21, DC_CMD );
    LcdExec( 0x80 | contrast, DC_CMD );
    LcdExec( 0x20, DC_CMD );
}

/*********************************************
函数名    :   LcdSetXY
功能      :   设置 LCD 当前坐标
参数      :   INT8U X  横坐标：0~83
              INT8U Y  纵坐标：0~05
返回      :   无
编辑器    :   Notepad 2
*********************************************/
void LcdSetXY(INT8U X, INT8U Y)
{
    LcdExec( 0x40 | Y, DC_CMD);    // 行
    LcdExec( 0x80 | X, DC_CMD);    // 列
}

/*********************************************
函数名    :   LcdExec
功能      :   执行 LCD 指令/数据
参数      :   INT8U data       指令/数据值
              LcdExecType dc  指令/数据选择(DC_CMD/DC_DATA)
返回      :   无
编辑器    :   Notepad 2
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
函数名    :   UnClear
功能      :   高n位清零
参数      :   INT8U data   8位数据
              INT8U n      清零多少位
返回      :   INT8U        清零后的结果
举例      :   data取11111000 n取3 --> 00011000
编辑器    :   Notepad 2
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
函数名    :   LnClear
功能      :   低n位清零
参数      :   INT8U data   8位数据
              INT8U n      清零多少位
返回      :   INT8U        清零后的结果
举例      :   data取00011111 n取3 --> 00011000
编辑器    :   Notepad 2
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
函数名    :   SplitByte
功能      :   一个字节拆成两个字节
参数      :   INT8U data       8位数据
              INT8U nByLow     从低位起多少位拆开
              INT8U bytes[ 2 ] 存放返回的两个字节
返回      :   无
举例      :   data取11111111 nByLow取2 --> bytes[0]=11111100、bytes[1]=00000011
编辑器    :   Notepad 2
*********************************************/
void SplitByte( INT8U data, INT8U nByLow, INT8U bytes[ 2 ] )
{
    bytes[ 0 ] = data << nByLow;
    bytes[ 1 ] = data >> ( 8 - nByLow );
}

/*********************************************
函数名    :   BufferPixelStuff
功能      :   把图片、文字点阵写入全屏缓存(精确定位，右上角坐标可以为LCD上任意位置。
              字符大小不限，自行修改数组点阵数据，单个字符的宽高参数不要传错了，16号
              字模不要传成12，12了，12号字模不要传成16，16了)
参数      :   INT8U type       type=1-->图片点阵 / type=2-->ascii字符 / type=3-->中文字符
              INT8U index      数据在数组中的位置(当type=2时，直接传ascii字符,如:'A')
              INT8U x          横坐标:0~83
              INT8U y          纵坐标:0~47
              INT8U width      点阵数据的宽度(位数) 如果是字符就为一个字符的宽度、如果是图片就为整个图片的宽度
              INT8U height     点阵数据的高度(位数) 如果是字符就为一个字符的高度、如果是图片就为整个图片的高度
返回      :   无
编辑器    :   Notepad 2
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
函数名    :   CacheEnglishString
功能      :   把ascii字符串写入全屏缓存(精确定位，右上角坐标可以为LCD上任意位置)
参数      :   INT8U x          横坐标:0~83
              INT8U y          纵坐标:0~47
              INT8U width      一个字符的宽度(位数)
              INT8U height     一个字符的高度(位数)
              INT8U bWidth     字符间距
              INT8U *dataPtr   要显示的字符串 (如:"Hello world!")
返回      :   无
编辑器    :   Notepad 2
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
函数名    :   CachePixel
功能      :   画一个点到入全屏缓存中(精确定位，坐标可以为LCD上任意位置)
参数      :   INT8U x              横坐标:0~83
            INT8U y              纵坐标:0~47
            INT8U width          一个字符的宽度(位数)
            INT8U height         一个字符的高度(位数)
            LcdPixelMode mode   画点方式 (PIXEL_CLEAR-->擦除、PIXEL_DRAW-->画点、PIXEL_XOR-->取反)
返回      :   无
编辑器    :   Notepad 2
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
函数名    :   CommitBuffer
功能      :   提交缓存，把当前缓存上的数据写到LCD上
参数      :   无
返回      :   无
编辑器    :   Notepad 2
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
函数名    :   ClearBuffer
功能      :   清空缓存
参数      :   无
返回      :   无
编辑器    :   Notepad 2
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
函数名    :   TestNOKIA5110
功能      :   测试屏显示函数
参数      :   无
返回      :   无
编辑器    :   Notepad 2
*********************************************/
void TestNOKIA5110(void)
{
    INT8U i, j, k, l;

    ClearBuffer( );

    /***********************
        写ascii字符串
        位置(0,0)
        单个点阵字符字模大小5X7
        字符间距2
        字符串"Hello everyone!"
    ***********************/
    CacheEnglishString( 0, 0, 5, 7, 2, "Hello world!" );    
    
    /***********************
        写'O','u','r'三个字符 + "我们的"三个字 + "AVR"48x24图片
        位置   'O'(12,10)   'u'(19,10)   'r'(26,10)   "我"(0,8)   "们"(0,20)    图片(31,10)
        单个点阵字符字模大小12X12
    ***********************/
    BufferPixelStuff( 2, 'O', 12, 10, 5, 7);    //'O'
    BufferPixelStuff( 2, 'u', 19, 10, 5, 7);    //'u'
    BufferPixelStuff( 2, 'r', 26, 10, 5, 7);    //'r'
    
    BufferPixelStuff( 3, 0, 0, 8, 12, 12);      //"我"
    BufferPixelStuff( 3, 1, 0, 20, 12, 12); //"们"
    
    BufferPixelStuff( 2, 'D', 13, 22, 5, 7);    //'D'
    BufferPixelStuff( 2, 'E', 21, 22, 5, 7);    //'E'
    
    BufferPixelStuff( 1, 0, 31, 10, 48, 24);    //图片
    
    
    /***********************
        画横线
    ***********************/

    for( j = 0; j<6; j++ )
    {
        for( i = 0; i<70; i++ )
        {
            CachePixel( 7 + i, 34 + j*2, PIXEL_DRAW );
        }
    }
    
    /******显示效果******
    
    Hello everyone!     
    我Our /\ \  / |_)
    们DE /  \ \/  | \
     ---------------
     ---------------
     ---------------
    ******************/ 
    CommitBuffer( ); // 提交缓存

    SysDelay(10000);

    // 把这部分注释去了，显示四行诗句。
    ClearBuffer( ); 
    for( k = 0; k < 4; k++ )
    {
        for( l = 0; l < 6; l++ )
        {
            BufferPixelStuff( 3, 2 + k * 6 + l, l * 12, k * 12, 12, 12);
        }
    }
       
    CommitBuffer( ); // 提交缓存

    SysDelay(10000);
}
