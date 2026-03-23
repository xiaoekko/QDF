#ifndef __FREEMODBUSAPP_H__
#define __FREEMODBUSAPP_H__

#include 		"mb.h"
#include 		"mbutils.h"

#pragma anon_unions

typedef union{
              unsigned char Byte;
              struct {
                                unsigned char Bit0:1;
                                unsigned char Bit1:1;
                                unsigned char Bit2:1;
                                unsigned char Bit3:1;        
                                unsigned char Bit4:1;
                                unsigned char Bit5:1;
                                unsigned char Bit6:1;
                                unsigned char Bit7:1;
                                                  
                      } ;
}bitfield8;

extern bitfield8  uCoilsStatusHI;
extern bitfield8  uCoilsStatusLO;

extern bitfield8  uInputStatusHI;
extern bitfield8  uInputStatusLO;


typedef union{
              unsigned short Word;
              struct {
                                unsigned char Bit0:1;
                                unsigned char Bit1:1;
                                unsigned char Bit2:1;
                                unsigned char Bit3:1;        
                                unsigned char Bit4:1;
                                unsigned char Bit5:1;
                                unsigned char Bit6:1;
                                unsigned char Bit7:1;
                                unsigned char Bit8:1;
                                unsigned char Bit9:1;
                                unsigned char Bit10:1;
                                unsigned char Bit11:1;        
                                unsigned char Bit12:1;
                                unsigned char Bit13:1;
                                unsigned char Bit14:1;
                                unsigned char Bit15:1;                                                        
                      } ;
}bitfield16;

//输入寄存器起始地址
#define REG_INPUT_START       0x0000
//输入寄存器数量
#define REG_INPUT_NREGS       200
//保持寄存器起始地址
#define REG_HOLDING_START     0x0000
//保持寄存器数量
#define REG_HOLDING_NREGS     200

//线圈起始地址
#define REG_COILS_START       0x0000
//线圈数量
#define REG_COILS_SIZE        16

//开关寄存器起始地址
#define REG_DISCRETE_START    0x0000
//开关寄存器数量
#define REG_DISCRETE_SIZE     16


/* Private variables ---------------------------------------------------------*/

extern unsigned char RegHoldingOKFlag;
//输入寄存器内容   0x04  模拟量输入
extern	uint16_t usRegInputBuf[REG_INPUT_NREGS] ;
//寄存器起始地址
//uint16_t usRegInputStart ;

//保持寄存器内容  0x03 0x06 0x10  模拟量输出设定值，PID参数
extern	uint16_t usRegHoldingBuf[REG_HOLDING_NREGS] ;
//保持寄存器起始地址
//uint16_t usRegHoldingStart ;

//读写线圈状态 0x01 0x05 0x0F  电磁阀输出，LED输出
extern	uint8_t ucRegCoilsBuf[REG_COILS_SIZE/8] ;
//读开关输入状态	0x02 开关，按键等
extern	uint8_t ucRegDiscreteBuf[REG_DISCRETE_SIZE/8];



eMBErrorCode 
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs );

eMBErrorCode 
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,eMBRegisterMode eMode );

eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,eMBRegisterMode eMode );

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete );


#endif




