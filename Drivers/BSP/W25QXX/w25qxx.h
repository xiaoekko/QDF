
#ifndef __BSP_W25QXX_H__
#define __BSP_W25QXX_H__	

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "../../User/types.h"  // ??????????????

// ??????????????
// typedef enum _enumModelID enumModelID;
// typedef enum _enumErrorAlarm enumErrorAlarm;

#include "UI.h"

/*****************************************************************************
**
**		????
**
*****************************************************************************/

// W25X???/Q???о??б?	   
#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
#define W25Q128	0XEF17

extern uint16_t W25QXX_TYPE;					// ????W25QXXо????		   

// CS???????
#define W25QXX_CS_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define W25QXX_CS_GPIO_PORT                GPIOA
#define W25QXX_CS_PIN                      GPIO_PIN_4

// CS???????
#define W25QXX_CS_LOW()                    HAL_GPIO_WritePin(W25QXX_CS_GPIO_PORT, W25QXX_CS_PIN, GPIO_PIN_RESET)
#define W25QXX_CS_HIGH()                   HAL_GPIO_WritePin(W25QXX_CS_GPIO_PORT, W25QXX_CS_PIN, GPIO_PIN_SET)

////////////////////////////////////////////////////////////////////////////////// 
// ????
#define 	W25X_WriteEnable			0x06 
#define 	W25X_WriteDisable			0x04 
#define 	W25X_ReadStatusReg			0x05 
#define 	W25X_WriteStatusReg			0x01 
#define 	W25X_ReadData				0x03 
#define 	W25X_FastReadData			0x0B 
#define 	W25X_FastReadDual			0x3B 
#define 	W25X_PageProgram			0x02 
#define 	W25X_BlockErase				0xD8 
#define 	W25X_SectorErase			0x20 
#define 	W25X_ChipErase				0xC7 
#define 	W25X_PowerDown				0xB9 
#define 	W25X_ReleasePowerDown		0xAB 
#define 	W25X_DeviceID				0xAB 
#define 	W25X_ManufactDeviceID		0x90 
#define 	W25X_JedecDeviceID			0x9F 

#define     HISTORY_RECORD_SIZE     18                // 历史记录数据长度
#define     FlashLocateAdr          0x0000            // ???????洢???

/*****************************************************************************
**
**		?????
**
*****************************************************************************/

typedef 	struct	_History
{
	uint8_t 				Year;					// ???
	uint8_t 				Month;					// ?·?
	uint8_t 				Day;					// ????
			
	uint8_t 				Hour;					// С?
	uint8_t 				Min;					// ????
	uint8_t 				Second; 				// ????
			
	enumModelID 			ModelID;				// ?????
	uint8_t 				DeviceID;				// ?豸??
	enumHistoryActionCause 	ActionCause;			// 动作来源
	enumHistoryResult 		ResultState;			// 最终结果
		
	float 					Pressure; 				// ????
	float 					Temperature;			// ????
	
}History;

extern History HistoryData;
extern uint8_t HistoryBuffWrite[20];
extern uint8_t HistoryBuffRead[20];
extern uint32_t FlashLocate;
extern uint32_t 	maxnumber;
extern uint32_t 	number;

/*****************************************************************************
**
**		函数声明
**
*****************************************************************************/

void 	W25QXX_Init(void);
uint16_t  	W25QXX_ReadID(void);				// 读取FLASH ID
uint8_t	 	W25QXX_ReadSR(void);				// 读取状态寄存器 
void 	W25QXX_Write_SR(uint8_t sr);				// 写状态寄存器
void 	W25QXX_Write_Enable(void); 			// 写使能 
void 	W25QXX_Write_Disable(void);			// 写禁止
void 	W25QXX_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
void 	W25QXX_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead);		// 读取flash
void 	W25QXX_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);		// 写入flash
void 	W25QXX_Erase_Chip(void);			// 擦除整个芯片
void 	W25QXX_Erase_Sector(uint32_t Dst_Addr); 	// 擦除扇区
void 	W25QXX_Wait_Busy(void);				// 等待空闲
void 	W25QXX_PowerDown(void);				// 进入掉电模式
void 	W25QXX_WAKEUP(void);				// 唤醒

void 		SetupModel(enumModelID mModelID);
void 		SetupState(enumErrorAlarm mErrorAlarm);
#ifdef __cplusplus
	}
#endif

#endif

/***********************************************************************************************
*************************************  文件结束  ***********************************************
***********************************************************************************************/





