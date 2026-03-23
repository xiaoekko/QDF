/***********************************************************************************************
**
**	Define to prevent recursive inclusion 
**
***********************************************************************************************/
#ifndef __ADDRCONST_H__ 
#define __ADDRCONST_H__


#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************************************************
**
**	Macros definition
**
***********************************************************************************************/
#define 	ADR_SYSTEM_FIRST_RUN				(5)

#define		ADD_CONST							(6)

#define		ADR_MODELID							(1+ADD_CONST)	

#define		ADR_TIMERTYPE						(2+ADD_CONST)	
#define		ADR_DEVICEID						(3+ADD_CONST)	

#define		ADR_LIMITUPPRESS					(4+ADD_CONST)	
#define		ADR_LIMITDNPRESS					(8+ADD_CONST)	

#define		ADR_OPENTIMERCTR					(12+ADD_CONST)	
#define		ADR_CLOSETIMERCTR					(16+ADD_CONST)	

#define		ADR_MAXPRESSURE						(20+ADD_CONST)	
#define		ADR_MINPRESSURE						(24+ADD_CONST)	

#define		ADR_I2CCOUNTER						(28+ADD_CONST)

#define		ADR_CLOSE_COUNT						(32+ADD_CONST)	

#define		ADR_ZHONGAO_COUNT					(36+ADD_CONST)	
#define		ADR_ZHONGAO_PRESS					(40+ADD_CONST)	

#define		ADR_OPEN_PERCENT					(44+ADD_CONST)	

#define		ADR_ALARM_TEMP						(48+ADD_CONST)	

#define		ADDR_SPI_FLASH_ADDR					(52+ADD_CONST)

#define		ADDR_DEEP_SLEEP_ADDR				(56+ADD_CONST)

#define		ADDR_OVER_PRESS_SET_ADDR			(60+ADD_CONST)
#define		ADDR_UNDER_PRESS_SET_ADDR			(64+ADD_CONST)

//以下地址必须在最后，否则数据库会覆盖
//================================================================
#define		ADR_DATE_MON						(100+ADD_CONST)
#define		ADR_DATE_DAY						(104+ADD_CONST)

#define		ADR_TIME_HOUR						(108+ADD_CONST)
#define		ADR_TIME_MIN						(112+ADD_CONST)

#define		ADR_UP_PRESS						(116+ADD_CONST)	
#define		ADR_DN_PRESS						(120+ADD_CONST)	

#define		ADR_CLOSE_TYPE						(124+ADD_CONST)	

#define		EE_ADDR_WIFI_LEVEL					(128+ADD_CONST)

#define		EE_ADDR_NETIP0_LEVEL				(132+ADD_CONST)
#define		EE_ADDR_NETIP1_LEVEL				(136+ADD_CONST)
#define		EE_ADDR_NETIP2_LEVEL				(140+ADD_CONST)
#define		EE_ADDR_NETIP3_LEVEL				(144+ADD_CONST)
	
#define		EE_ADDR_NETPORT_LEVEL				(148+ADD_CONST)


#define 	ADR_SYSTEMDEVICENUM					(152+ADD_CONST)	

		
#define		ADD_DEVICENAME						(156+ADD_CONST)

#define		ADR_HISTORY_FORMAT_VERSION			(220+ADD_CONST)

//#define		ADD_DEVICENAME						(170+ADD_CONST)


#ifdef __cplusplus
	}
#endif



#endif



