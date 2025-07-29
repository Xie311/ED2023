//
// Created by 86189 on 2024/2/5.
//
/*
 * from CSDN码上芯路人
版权声明：本文为博主原创文章，遵循 CC 4.0 BY-SA 版权协议，转载请附上原文出处链接和本声明。
原文链接：https://blog.csdn.net/qq_52158753/article/details/130173425
 */
//IIC1->PB6--SCL      PB7--SDA
//IIC2->PB10--SCL      PB11--SDA

#include "oled.h"
#include "i2c.h"
#include "oledfont.h"

uint8_t CMD_Data[]={
        0xAE, 0x00, 0x10, 0x40, 0xB0, 0x81, 0xFF, 0xA1, 0xA6, 0xA8, 0x3F,

        0xC8, 0xD3, 0x00, 0xD5, 0x80, 0xD8, 0x05, 0xD9, 0xF1, 0xDA, 0x12,

        0xD8, 0x30, 0x8D, 0x14, 0xAF};      // 初始化命令


void WriteCmd(void)
{
    uint8_t i = 0;
    for(i=0; i<27; i++)
    {
        HAL_I2C_Mem_Write(&hi2c1 ,0x78,0x00,I2C_MEMADD_SIZE_8BIT,CMD_Data+i,1,0x100);
    }
}
//向设备写控制命令
void OLED_WR_CMD(uint8_t cmd)
{
    HAL_I2C_Mem_Write(&hi2c1 ,0x78,0x00,I2C_MEMADD_SIZE_8BIT,&cmd,1,0x100);
}
//向设备写数据
void OLED_WR_DATA(uint8_t data)
{
    HAL_I2C_Mem_Write(&hi2c1 ,0x78,0x40,I2C_MEMADD_SIZE_8BIT,&data,1,0x100);
}
//初始化oled屏幕
/*函数内部是void WriteCmd(void)函数，这个函数是将27个字节的初始化命令写入OLED设备中的控制寄存器。
 * 在使用OLED显示屏时，需要先向控制寄存器写入一些特定的命令来设置显示模式、亮度等参数，然后才能向数据寄存器写入实际的数据。*/
void OLED_Init(void)
{
    HAL_Delay(200);

    WriteCmd();
}
//清屏
void OLED_Clear(void)
{
    uint8_t i,n;
    for(i=0;i<8;i++)
    {
        OLED_WR_CMD(0xb0+i);
        OLED_WR_CMD (0x00);
        OLED_WR_CMD (0x10);
        for(n=0;n<128;n++)
            OLED_WR_DATA(0);
    }
}
//开启OLED屏幕的DCDC电路，开启OLED显示，实现方式是使用OLED_WR_CMD向控制寄存器写入几个对应的命令
void OLED_Display_On(void)
{
    OLED_WR_CMD(0X8D);  //SET DCDC命令
    OLED_WR_CMD(0X14);  //DCDC ON
    OLED_WR_CMD(0XAF);  //DISPLAY ON
}
//关闭OLED屏幕的DCDC电路，关闭OLED显示，原理与开启一致
void OLED_Display_Off(void)
{
    OLED_WR_CMD(0X8D);  //SET DCDC命令
    OLED_WR_CMD(0X10);  //DCDC OFF
    OLED_WR_CMD(0XAE);  //DISPLAY OFF
}
void OLED_Set_Pos(uint8_t x, uint8_t y)
{
    OLED_WR_CMD(0xb0+y);
    OLED_WR_CMD(((x&0xf0)>>4)|0x10);
    OLED_WR_CMD(x&0x0f);
}

//在OLED显示功能打开的情况下，将整个屏幕点亮（填充为白色），与清屏的作用是相对的
//实现原理都是用OLED_WR_CMD向控制寄存器写入几个对应的命令并使用OLED_WR_DATA传递参数1（清屏为0），实现将整个屏幕填充为白色
void OLED_On(void)
{
    uint8_t i,n;
    for(i=0;i<8;i++)
    {
        OLED_WR_CMD(0xb0+i);    //设置页地址（0~7）
        OLED_WR_CMD(0x00);      //设置显示位置—列低地址
        OLED_WR_CMD(0x10);      //设置显示位置—列高地址
        for(n=0;n<128;n++)
            OLED_WR_DATA(1);
    } //更新显示
}

//用于使字符在特定位置点亮
unsigned int oled_pow(uint8_t m,uint8_t n)
{
    unsigned int result=1;
    while(n--)result*=m;
    return result;
}

//显示数字
//x,y :起点坐标，x为0-127,y为0-7
//unsigned int num：要显示的数字(0~4294967295)，对应ASCII码表中的数值，具体数据在oledfont.h中
//len :数字的位数
//size:字体大小，取值为8或16；为8时每个字符横向x占用8，纵向占用1，为16时x，y都乘2
void OLED_ShowNum(uint8_t x,uint8_t y,unsigned int num,uint8_t len,uint8_t size2)
{
    uint8_t t,temp;
    uint8_t enshow=0;
    for(t=0;t<len;t++)
    {
        temp=(num/oled_pow(10,len-t-1))%10;
        if(enshow==0&&t<(len-1))
        {
            if(temp==0)
            {
                OLED_ShowChar(x+(size2/2)*t,y,' ',size2);  //此处空格表示空余位数填充，比如改成0时前面少于len的位数补0
                continue;
            }else enshow=1;

        }
        OLED_ShowChar(x+(size2/2)*t,y,temp+'0',size2);
    }
}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//size:选择字体 16/12
//uint8_t chr是要显示的字符，对应ASCII码表中的值，具体数据在oledfont.h中
//uint8_t Char_Size是字符的大小，取值为8或16；为8时每个字符横向x占用8，纵向占用1，为16时x，y都乘2
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size)
{
    unsigned char c=0,i=0;
    c=chr-' ';//得到偏移后的值
    if(x>128-1){x=0;y=y+2;}
    if(Char_Size ==16)
    {
        OLED_Set_Pos(x,y);
        for(i=0;i<8;i++)
            OLED_WR_DATA(F8X16[c*16+i]);
        OLED_Set_Pos(x,y+1);
        for(i=0;i<8;i++)
            OLED_WR_DATA(F8X16[c*16+i+8]);
    }
    else {
        OLED_Set_Pos(x,y);
        for(i=0;i<6;i++)
            OLED_WR_DATA(F6x8[c][i]);

    }
}

//显示一个字符串
//x,y同上
//uint8_t *chr是待显示的字符数组，是一个指向字符数组的指针
//uint8_t Char_Size是字符串的大小，取值为8或16；为8时每行最多显示16个字符，纵向占用1，为16时每行最多显示8个字符，纵向占用2
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t Char_Size)
{
    unsigned char j=0;
    while (chr[j]!='\0')
    {		OLED_ShowChar(x,y,chr[j],Char_Size);
        x+=8;
        if(x>120){x=0;y+=2;}
        j++;
    }
}
//显示汉字
//Hzk 用取模软件得出的数组
//x,y同上
//uint8_t no是汉字编号，汉字对应的数据是通过取字模软件生成的，被存放在oledfont.h中一个数组中const unsigned char Hzk[][32]，
//数组的横下标即为这个编号。（说明：汉字在8像素情况下基本看不清，因此汉字的大小默认为16）
void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no)
{
    uint8_t t,adder=0;
    OLED_Set_Pos(x,y);
    for(t=0;t<16;t++)
    {
        OLED_WR_DATA(Hzk[2*no][t]);
        adder+=1;
    }
    OLED_Set_Pos(x,y+1);
    for(t=0;t<16;t++)
    {
        OLED_WR_DATA(Hzk[2*no+1][t]);
        adder+=1;
    }
}

//显示图片
//BMP 用取模软件得出的数组
/*
uint8_t x0：要显示的 BMP 图片左上角在 OLED 显示屏上的 x 坐标，通常为0。
uint8_t y0: 表示要显示的 BMP 图片左上角在 OLED 显示屏上的 y 坐标，通常为0。
uint8_t x1: 表示要显示的 BMP 图片右下角在 OLED 显示屏上的 x 坐标，通常为128。
uint8_t y1: 表示要显示的 BMP 图片右下角在 OLED 显示屏上的 y 坐标，通常为8。
uint8_t no：图片编号，图片对应的数据是通过取字模软件生成的，被存放在oledfont.h的
 一个数组中const unsigned char BMP[][1024] ，数组的横下标即为这个编号。
*/
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t no)
{
    uint32_t j = 0;
    uint8_t x, y;

    if (y1 % 8 == 0) y = y1 / 8;
    else y = y1 / 8 + 1;
    for (y = y0; y < y1; y++)
    {
        OLED_Set_Pos(x0, y);
        for (x = x0; x < x1; x++)
        {
            OLED_WR_DATA(BMP[no][j++]);
        }
    }
}

