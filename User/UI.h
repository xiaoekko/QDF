
#include "./SYSTEM/sys/sys.h"
#include "types.h"


//extern	float  		UP_Pressure;  // 上限实时压力值

//extern	float  		DN_Pressure;

extern	float  		MaxPressure;
extern	float  		MinPressure;


extern	uint8_t 	OverLoadFlag;
extern	uint8_t		StopFlag;


extern  uint8_t 	MenuID;

extern	uint8_t		SelectMenuID;
extern	uint8_t		MaxSelectMenuID;

extern	uint8_t 	E2CWriteFlag;

extern 	uint8_t 	DeviceID;

extern 	float		AlarmTemperatureValue;
extern  uint8_t 	OverTemperatureFlag;

extern uint32_t 	InterimState;



//运行模式
//typedef enum 	_enumModelID
//			{	NoModel=0,
//				HandModel,
//				AutoModel
//			}enumModelID ;
extern enumModelID				ModelID;

//////运行模式
////typedef enum 	_enumRunModel{NoRunModel=0,HandRunModel,AutoRunModel}enumRunModel ;
////extern enumRunModel				RunModel;
//			
//故障报警
//typedef enum 	_enumErrorAlarm
//            {   NoError=0,
//                CloseError,
//                OpenError,
//                SensorError,
//                Opening,
//                Closing,
//                Opened,
//                Closed,
//                OverPressClose,
//                UnderPressClose,
//                OverTempClose,
//                RemoteClose,
//                MotorStallError
//            }enumErrorAlarm ;
extern enumErrorAlarm			ErrorAlarm;

extern 	uint32_t 	PrevModelValue;	
extern 	uint32_t 	NextModelValue;	


extern uint8_t 			TmpStateFlag;	
extern uint8_t 			TmpStateXFlag;

extern uint8_t 			TmpCloseStateXFlag;
extern uint8_t 			TmpCloseStateFlag;

extern uint8_t 			CloseErrorCount;

extern uint16_t    		DeepSleepCount;

extern	uint8_t		OverPressDisFlag;
extern	uint8_t		UnderPressDisFlag;
extern 	uint32_t 	SpiAddrTemp;
extern	uint16_t		I2C_Counter;		// 添加计数器定义

// 添加时间和电池电压变量声明
extern PCF8563_Time_t current_time;
extern PCF8563_Date_t current_date;	
extern float battery_voltage;  // 添加电池电压变量
extern float temperature;

extern I2CData ArrayI2CData[9];

#define		MAX_TEMP_VALUE					(150)	//最大温度值
#define		MIN_TEMP_VALUE					(0)	//最小温度值


#define		MAX_DEV_ID					247

#define  	PCF8563EN					1
#define 	SIZE 						32	

///====================================================================================
#define		AUTO_HIGH_MAX				25.0F			//自动模式高压最大值
#define		AUTO_HIGH_MIN				3.0F			//自动模式高压最小值

#define		AUTO_LOW_MAX				2.5F			//自动模式低压最大值
#define		AUTO_LOW_MIN				0.0F			//自动模式低压最小值



/*****************************************************************************
**
**		函数声明
**
*****************************************************************************/
void 		SetupModel(enumModelID mModelID);

void		SystemInitFunc(void);
void 		AdcManageFunc(void);

void		FirstInterfaceFunc(void);
void		SecondInterfaceFunc(void);
void		ThirdInterfaceFunc(void);
void		ForthInterfaceFunc(void);


void 		CheckSystemFirstRun(void);
void		ValueManageFunc(void);

uint32_t FlashRead_Uint32(uint32_t adr);

//void 		SetupState(enumErrorAlarm mErrorAlarm);

void 		Flash_History_Write(uint8_t model, uint8_t action_cause, uint8_t result_state, float pressure, float temperature);

void 		MotorStartup(void);
void 		CheckMotorStall(void);


// 页面重置标志
extern uint8_t g_page_reset_flag;
extern uint8_t g_history_page_refresh_flag;
extern enumHistoryActionCause CurrentActionCause;


