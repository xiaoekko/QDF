#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdint.h>

// ??ID???
typedef enum _enumModelID
{
    NoModel = 0,
    HandModel,
    AutoModel
} enumModelID;

// ????????
typedef enum _enumErrorAlarm
{
    NoError = 0,
    CloseError,
    OpenError,
    SensorError,
    Opening,
    Closing,
    Opened,
    Closed,
    OverPressClose,
    UnderPressClose,
    OverTempClose,
    RemoteClose,
    MotorStallError
} enumErrorAlarm;

typedef enum _enumHistoryActionCause
{
    HistoryActionNone = 0,
    HistoryActionManual,
    HistoryActionRemoteClose,
    HistoryActionOverPressClose,
    HistoryActionUnderPressClose,
    HistoryActionOverTempClose
} enumHistoryActionCause;

typedef enum _enumHistoryResult
{
    HistoryResultUnknown = 0,
    HistoryResultOpened,
    HistoryResultClosed,
    HistoryResultOpenError,
    HistoryResultCloseError,
    HistoryResultSensorError,
    HistoryResultMotorStallError
} enumHistoryResult;

/* ????? */
typedef enum {
    PCF8563_OK = 0,                          // ???????
    PCF8563_ERROR_DEVICE_NOT_FOUND,         // ?õô¦Ä???
    PCF8563_ERROR_INVALID_PARAMETER,        // ??????§¹
    PCF8563_ERROR_COMMUNICATION,            // ??????
    PCF8563_ERROR_VOLTAGE_LOW               // ???????
} PCF8563_Status_t;

/* ???? */
typedef struct {
    uint8_t second;     // ?? (0-59)
    uint8_t minute;     // ?? (0-59)
    uint8_t hour;       // ? (0-23)
} PCF8563_Time_t;

/* ????? */
typedef struct {
    uint8_t year;       // ?? (0-99, ???20xx??)
    uint8_t month;      // ?? (1-12)
    uint8_t day;        // ?? (1-31)
    uint8_t weekday;    // ???? (0-6, 0=??????)
} PCF8563_Date_t;

/* ???????? */
typedef struct {
    PCF8563_Time_t time;
    PCF8563_Date_t date;
} PCF8563_DateTime_t;


typedef struct tagI2CData
{

	unsigned char 	I2C_Months;  //?¡¤?
	unsigned char 	I2C_Days;  //????
	unsigned char 	I2C_Hours;  //§³?
	unsigned char 	I2C_Minutes;  //????

	unsigned char  	I2C_UP_Pressure;
	unsigned char  	I2C_DN_Pressure;

	unsigned char	CloseType;
	
}I2CData;


















#endif

