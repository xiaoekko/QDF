#include "./SYSTEM/sys/sys.h"

extern 	uint8_t 	IPData[4];
extern 	uint16_t 	PortVal;
extern 	uint8_t 	WIFIDeviceID;
extern 	uint8_t 	NetSetFlag;
extern	uint32_t 	CurrRS485PressValue;

extern	uint32_t	LowFreqCount;
extern	uint32_t	WakeupCount;
extern	uint8_t		LowFreqFlag;
extern	float		McuTemperature;

extern	uint16_t	MenuIDCount;

extern	uint8_t		ExCloseFlag;
extern  uint8_t     OverTempFlag;
extern  uint8_t     OverPressFlag;
				
extern  uint8_t     IR_KEY_OPEN_FLAG;
extern  uint8_t     IR_KEY_CLOSE_FLAG;
extern  uint8_t 	IR_KEY_STOP_FLAG;

void KeyRespFunc(void);
void ModelIncreaseKeyFunc(void);
void ModelDecreaseKeyFunc(void);
void PageDownKeyFunc(void);
void KeyIncreaseFunc(void);
void KeyDecreaseFunc(void);
void LongKeyIncreaseFunc(void);
void LongKeyDecreaseFunc(void);

// 添加参数初始化函数声明
void InitParametersFromEEPROM(void);
