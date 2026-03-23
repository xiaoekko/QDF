#include "./BSP/MB85RC16/mb85rc16.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/MB85RC16/AddrConst.h"

union
{
    float f;
    unsigned char c;
    unsigned short int i;
    unsigned int l;
} iic_data_buf;

// HAL库延时函数
void DelayNMS(unsigned int dly)
{
    HAL_Delay(dly);
}

// 初始化IIC接口
void AT24CXX_Init(void)
{
    IIC_Init();    // IIC初始化
}

uint8_t AT24CXX_ReadOneByte(uint16_t ReadAddr)
{
    uint8_t temp = 0;
    IIC_Start();
    if(EE_TYPE > AT24C16)
    {
        IIC_Send_Byte(0XA0);
        IIC_Wait_Ack();
        IIC_Send_Byte(ReadAddr >> 8);
    }
    else
        IIC_Send_Byte(0XA0 + ((ReadAddr / 256) << 1));

    IIC_Wait_Ack();
    IIC_Send_Byte(ReadAddr % 256);
    IIC_Wait_Ack();
    IIC_Start();
    IIC_Send_Byte(0XA1 + ((ReadAddr / 256) << 1));
    IIC_Wait_Ack();
    temp = IIC_Read_Byte(0);
    IIC_Stop();
    return temp;
}

// 在AT24CXX指定地址写入一个数据
// WriteAddr  :写入数据的目的地址
// DataToWrite:要写入的数据
void AT24CXX_WriteOneByte(uint16_t WriteAddr, uint8_t DataToWrite)
{
    IIC_Start();
    if(EE_TYPE > AT24C16)
    {
        IIC_Send_Byte(0XA0);
        IIC_Wait_Ack();
        IIC_Send_Byte(WriteAddr >> 8);
    }
    else
    {
        IIC_Send_Byte(0XA0 + ((WriteAddr / 256) << 1));
    }
    IIC_Wait_Ack();
    IIC_Send_Byte(WriteAddr % 256);
    IIC_Wait_Ack();
    IIC_Send_Byte(DataToWrite);
    IIC_Wait_Ack();
    IIC_Stop();
    HAL_Delay(10);    // 使用HAL库延时函数
}

void AT24CXX_WriteLenByte(uint16_t WriteAddr, uint32_t DataToWrite, uint8_t Len)
{
    uint8_t t;
    for(t = 0; t < Len; t++)
    {
        AT24CXX_WriteOneByte(WriteAddr + t, (DataToWrite >> (8 * t)) & 0xff);
    }
}

void WrToROM(unsigned int Data[], unsigned char Address, unsigned char Num)
{
    unsigned char i;
    unsigned int *PData;
    PData = Data;
    for(i = 0; i < Num; i++)
    {
        AT24CXX_WriteLenByte(Address + i, PData[i], 2);
    }
}

// 在AT24CXX指定地址读出连续数据，长度为Len个字节
// 该函数可以读出16bit或者32bit的数据.
// ReadAddr   :开始读出的地址
// 返回值     :数据
// Len        :要读出数据的长度2,4
uint32_t AT24CXX_ReadLenByte(uint16_t ReadAddr, uint8_t Len)
{
    uint8_t t;
    uint32_t temp = 0;
    for(t = 0; t < Len; t++)
    {
        temp <<= 8;
        temp += AT24CXX_ReadOneByte(ReadAddr + Len - t - 1);
    }
    return temp;
}

void RdFromROM(uint16_t Data[], unsigned char Address, unsigned char Num)
{
    unsigned char i;
    uint16_t *PData;
    PData = Data;
    for(i = 0; i < Num; i++)
    {
        PData[i] = AT24CXX_ReadLenByte(Address + i, 2);
    }
}

// 检查AT24CXX是否正常
// 这里用了24XX的最后一个地址(255)来存储标志字.
// 如果用其他24C系列,这个地址要修改
// 返回1:检测失败
// 返回0:检测成功
uint8_t AT24CXX_Check(void)
{
    uint8_t temp;
    temp = AT24CXX_ReadOneByte(255);    // 避免每次开机都写AT24CXX
    if(temp == 0X55) return 0;
    else    // 排除第一次初始化的情况
    {
        AT24CXX_WriteOneByte(255, 0X55);
        temp = AT24CXX_ReadOneByte(255);
        if(temp == 0X55) return 0;
    }
    return 1;
}

// 在AT24CXX里面的指定地址开始读出指定个数的数据
// ReadAddr :开始读出的地址 对24c02为0~255
// pBuffer  :数据数组首地址
// NumToRead:要读出数据的个数
void AT24CXX_Read(uint16_t ReadAddr, uint8_t *pBuffer, uint16_t NumToRead)
{
    while(NumToRead)
    {
        *pBuffer++ = AT24CXX_ReadOneByte(ReadAddr++);
        NumToRead--;
    }
}

// 在AT24CXX里面的指定地址开始写入指定个数的数据
// WriteAddr :开始写入的地址 对24c02为0~255
// pBuffer   :数据数组首地址
// NumToWrite:要写入数据的个数
void AT24CXX_Write(uint16_t WriteAddr, uint8_t *pBuffer, uint16_t NumToWrite)
{
    while(NumToWrite--)
    {
        AT24CXX_WriteOneByte(WriteAddr, *pBuffer);
        WriteAddr++;
        pBuffer++;
    }
}

uint8_t AT24CXX_ReadUchar(uint16_t I2C_ADDR)
{
    AT24CXX_Read(I2C_ADDR, (unsigned char *)&iic_data_buf.c, 1);
    return(iic_data_buf.c);
}

uint16_t AT24CXX_ReadUint(uint16_t I2C_ADDR)
{
    AT24CXX_Read(I2C_ADDR, (unsigned char *)&iic_data_buf.i, 2);
    return(iic_data_buf.i);
}

int16_t AT24CXX_ReadInt(uint16_t I2C_ADDR)
{
    AT24CXX_Read(I2C_ADDR, (unsigned char *)&iic_data_buf.i, 2);
    return(iic_data_buf.i);
}

float AT24CXX_ReadFloat(uint16_t I2C_ADDR)
{
    AT24CXX_Read(I2C_ADDR, (unsigned char *)&iic_data_buf.f, 4);
    return(iic_data_buf.f);
}

void AT24CXX_WriteUchar(uint16_t I2C_ADDR, uint8_t ucData)
{
    AT24CXX_Write(I2C_ADDR, &ucData, 1);
}

void AT24CXX_WriteUint(uint16_t I2C_ADDR, uint16_t ucData)
{
    AT24CXX_Write(I2C_ADDR, (uint8_t *)&ucData, 2);
}

void AT24CXX_WriteInt(uint16_t I2C_ADDR, int16_t ucData)
{
    AT24CXX_Write(I2C_ADDR, (uint8_t *)&ucData, 2);
}

void AT24CXX_WriteFloat(uint16_t I2C_ADDR, float flData)
{
    AT24CXX_Write(I2C_ADDR, (uint8_t *)&flData, 4);
}

uint32_t MyI2C_ReadUlong(uint16_t I2C_ADDR)
{
    AT24CXX_Read(I2C_ADDR, (unsigned char *)&iic_data_buf.l, 4);
    return(iic_data_buf.l);
}

void MyI2C_WriteUlong(uint16_t I2C_ADDR, uint32_t ucData)
{
    AT24CXX_Write(I2C_ADDR, (uint8_t *)&ucData, 4);
}

uint8_t MB85RCXX_ReadString(uint16_t I2C_ADDR, uint8_t i)
{
    AT24CXX_Read(I2C_ADDR + i, (unsigned char *)&iic_data_buf.c, 1);
    return(iic_data_buf.c);
}

void MB85RCXX_WriteString(uint16_t I2C_ADDR, uint8_t i, uint8_t ucData)
{
    AT24CXX_Write(I2C_ADDR + i, &ucData, 1);
}










