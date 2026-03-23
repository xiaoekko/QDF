

#ifndef __BSP_MB85RC16_H__
#define __BSP_MB85RC16_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "./BSP/MB85RC16/iic.h"

/*****************************************************************************
**
**      宏定义
**
*****************************************************************************/
#define AT24C01         127
#define AT24C02         255
#define AT24C04         511
#define AT24C08         1023
#define AT24C16         2047
#define AT24C32         4095
#define AT24C64         8191
#define AT24C128        16383
#define AT24C256        32767

#define EE_TYPE         AT24C02

extern uint8_t AT24CXX_Check(void);        // 检查器件
extern void AT24CXX_Init(void);             // 初始化IIC

extern uint8_t AT24CXX_ReadUchar(uint16_t I2C_ADDR);
extern uint16_t AT24CXX_ReadUint(uint16_t I2C_ADDR);
extern int16_t AT24CXX_ReadInt(uint16_t I2C_ADDR);
extern float AT24CXX_ReadFloat(uint16_t I2C_ADDR);
extern void AT24CXX_WriteUchar(uint16_t I2C_ADDR, uint8_t ucData);
extern void AT24CXX_WriteUint(uint16_t I2C_ADDR, uint16_t ucData);
extern void AT24CXX_WriteInt(uint16_t I2C_ADDR, int16_t ucData);
extern void AT24CXX_WriteFloat(uint16_t I2C_ADDR, float flData);

extern uint32_t MyI2C_ReadUlong(uint16_t I2C_ADDR);
extern void MyI2C_WriteUlong(uint16_t I2C_ADDR, uint32_t ucData);

extern uint8_t MB85RCXX_ReadString(uint16_t I2C_ADDR, uint8_t i);
extern void MB85RCXX_WriteString(uint16_t I2C_ADDR, uint8_t i, uint8_t ucData);

extern void WrToROM(unsigned int Data[], unsigned char Address, unsigned char Num);
extern void RdFromROM(uint16_t Data[], unsigned char Address, unsigned char Num);

#ifdef __cplusplus
}
#endif

#endif












