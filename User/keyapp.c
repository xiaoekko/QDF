#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"

#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./BSP/PRESS/press.h"
#include "./BSP/Brushless Motor/brushless motor.h"
#include "./BSP/PushMotor/pushmotor.h"
#include "./BSP/MAX31865/max31865.h"  
#include "./BSP/DI/Intput.h"
#include "./BSP/DO/Output.h"
#include "./BSP/PCF8563/pcf8563.h"
#include "./BSP/MB85RC16/mb85rc16.h"
#include "./BSP/W25QXX/w25qxx.h"
#include "./BSP/SPI/spi.h"
#include "./BSP/ADC/adc.h"

#include "task.h"
#include "keyapp.h"
#include "UI.h"

#include "GUI.h"

#include "./BSP/MB85RC16/AddrConst.h"

uint8_t 	NetSetFlag=0;
uint32_t 	CurrRS485PressValue;

//uint32_t	LowFreqCount=0;
//uint8_t		LowFreqFlag=0;

//uint32_t	WakeupCount=0;
extern uint8_t		WakeupFlag;

//uint8_t		ExCloseFlag=0;
//uint8_t		OverTempFlag=0;
//uint8_t		OverPressFlag=0;



uint16_t	MenuIDCount=0;


float		McuTemperature;

uint8_t		OverPressDisFlag=0;
uint8_t		UnderPressDisFlag=0;

// 在文件开头添加参数初始化函数
void InitParametersFromEEPROM(void)
{
    // 从EEPROM读取设备ID
    DeviceID = AT24CXX_ReadUchar(ADR_DEVICEID);
    if(DeviceID > MAX_DEV_ID)
        DeviceID = MAX_DEV_ID;
    else if(DeviceID < 1)
        DeviceID = 1;
    
    // 从EEPROM读取报警温度
    AlarmTemperatureValue = AT24CXX_ReadFloat(ADR_ALARM_TEMP);
    if(AlarmTemperatureValue > MAX_TEMP_VALUE)
        AlarmTemperatureValue = MAX_TEMP_VALUE;
    else if(AlarmTemperatureValue < MIN_TEMP_VALUE)
        AlarmTemperatureValue = MIN_TEMP_VALUE;
    
    // 从EEPROM读取最大压力
    MaxPressure = AT24CXX_ReadFloat(ADR_MAXPRESSURE);
    if(MaxPressure > AUTO_HIGH_MAX)
        MaxPressure = AUTO_HIGH_MAX;
    else if(MaxPressure < AUTO_HIGH_MIN)
        MaxPressure = AUTO_HIGH_MIN;
    
    // 从EEPROM读取最小压力
    MinPressure = AT24CXX_ReadFloat(ADR_MINPRESSURE);
    if(MinPressure > AUTO_LOW_MAX)
        MinPressure = AUTO_LOW_MAX;
    else if(MinPressure < AUTO_LOW_MIN)
        MinPressure = AUTO_LOW_MIN;
}


void 	ModelIncreaseKeyFunc(void)			
{
 
		    if(MenuID==0)    
		    	{

				ModelID++;
		    		//if(ModelID>AutoModel)
					ModelID=HandModel;
				//else if(ModelID<HandModel)
				//	ModelID=AutoModel;
				AT24CXX_WriteUchar(ADR_MODELID,ModelID);
		    	}
			else if(MenuID==1)
				{
					SelectMenuID++;
					if(SelectMenuID>MaxSelectMenuID)
						SelectMenuID=1;
				}
			else if(MenuID==2)
				{

					// SelectMenuID++;
			 		// if(SelectMenuID>3)
					// 	SelectMenuID=1;

				}

}

void 	ModelDecreaseKeyFunc(void)  		
{

				if(MenuID==0)
		    	{
		    	//if(ModelID>1)
				//	ModelID--;
	    		//if(ModelID>AutoModel)
					ModelID=HandModel;
				//else if(ModelID<HandModel)
				//	ModelID=AutoModel;
				AT24CXX_WriteUchar(ADR_MODELID,ModelID);
		    	}
			else if(MenuID==1)
				{
					SelectMenuID++;
					if(SelectMenuID>MaxSelectMenuID)
						SelectMenuID=1;
				}
			else if(MenuID==2)
				{
					// if(SelectMenuID>1)
					// 	SelectMenuID--;
					// if(SelectMenuID<1)
					// 	SelectMenuID=3;

				}

}


void KeyIncreaseFunc(void)      			//参数增加键函数
{

	if(MenuID==0){}      			//主界面模式
		
		else if(MenuID==1)        //设置界面
			{																//在 MenuID 为1的情况下，根据不同的选择菜单项执行相应的操作
				if(SelectMenuID==1)
					{
						current_date.year++;  //年份增加，范围限制在 23 到 99 之间
						if(current_date.year>99)
						current_date.year=99;
						else if(current_date.year<23)
						current_date.year=23;
						SyncTimeToRTC();  //同步时间到实时时钟，调用 SyncTimeToRTC 函数将修改后的时间同步到硬件
					}
				else if(SelectMenuID==2)
	  	 			{
	  					current_date.month++;  //月份增加，范围限制在 1 到 12 之间
						if(current_date.month>12)
						current_date.month=12;
						else if(current_date.month<1)
						current_date.month=1;
						SyncTimeToRTC();
	  	 			}
				else if(SelectMenuID==3)
	  	 			{
	  					current_date.day++;  //日期增加，范围限制在 1 到 31 之间
						if(current_date.day>31)
						current_date.day=31;
						else if(current_date.day<1)
						current_date.day=1;
						SyncTimeToRTC();
	  	 			}
				else if(SelectMenuID==4)
	  	 			{
	  					current_time.hour++;  //小时增加，范围限制在 0 到 23 之间
						if(current_time.hour>23)
						current_time.hour=23;
						SyncTimeToRTC();
	  	 			}
				else if(SelectMenuID==5)
	  	 			{
	  					current_time.minute++;  //分钟增加，范围限制在 0 到 59 之间
						if(current_time.minute>59)
						current_time.minute=59;
						SyncTimeToRTC();
	  	 			}
				else if(SelectMenuID==6)
	  	 			{
	  					current_time.second++;  //秒钟增加，范围限制在 0 到 59 之间
						if(current_time.second>59)
						current_time.second=59;
						SyncTimeToRTC();
	  	 			}
				else if(SelectMenuID==7)						//设备ID
	  	 			{
	  					DeviceID++;														//增加设备ID，范围限制在 1 到 MAX_DEV_ID 之间，并写入 EEPROM。
						if(DeviceID>MAX_DEV_ID)
						DeviceID=MAX_DEV_ID;
						AT24CXX_WriteUchar(ADR_DEVICEID,DeviceID);
	  	 			}
				else if(SelectMenuID==8)						//报警温度
	  	 			{
	  					AlarmTemperatureValue++;			//增加报警温度，范围限制在 0 到 MAX_TEMP_VALUE 之间，并写入 EEPROM。
						if(AlarmTemperatureValue>MAX_TEMP_VALUE)
						AlarmTemperatureValue=MAX_TEMP_VALUE;
						AT24CXX_WriteFloat(ADR_ALARM_TEMP,AlarmTemperatureValue);
	  	 			}
		   		else if(SelectMenuID==9)						//超压压力
		   			{
						MaxPressure+=0.1F;												//增加最大压力，每次增加 0.1，范围限制在 0 到 AUTO_HIGH_MAX 之间，并写入 EEPROM。
						if(MaxPressure>AUTO_HIGH_MAX)			
						MaxPressure=AUTO_HIGH_MAX;			// 超压压力 不能 AUTO_HIGH_MAX，超过则设置为最大值
						AT24CXX_WriteFloat(ADR_MAXPRESSURE,MaxPressure);
		   			}
		   		else if(SelectMenuID==10)					//欠压压力
		   			{
						MinPressure+=0.1F;												//增加最小压力，每次增加 0.1，范围限制在 0 到 AUTO_LOW_MAX 之间，并写入 EEPROM。
						if(MinPressure>AUTO_LOW_MAX)    	
						MinPressure=AUTO_LOW_MAX;				// 欠压压力 不能 AUTO_LOW_MAX，超过则设置为最大值
						AT24CXX_WriteFloat(ADR_MINPRESSURE,MinPressure);
		   			}  
		}

		else if(MenuID==2)  								//第三页
		{
			// DeepSleepCount++;		//增加深度睡眠计数，根据当前值的大小选择不同的增量，范围限制在 1 到 3600 之间，并写入 EEPROM。
							
			if(DeepSleepCount>=1000)
				DeepSleepCount+=100;    //大于等于1000时增加100
			
			else if(DeepSleepCount>=100)
				DeepSleepCount+=10;			//大于等于100时增加10
			
			else 
				DeepSleepCount+=1;			//其他情况增加1
			
			if(DeepSleepCount>3600)
				DeepSleepCount=3600;
			
			else if(DeepSleepCount<1)
				DeepSleepCount=0;


			AT24CXX_WriteUint(ADDR_DEEP_SLEEP_ADDR,DeepSleepCount);
		}
		else if(MenuID==3)
		{
				uint32_t current_max = FlashRead_Uint32(0);
				uint32_t max_page_start = 1;
				if (current_max > 1) {
					max_page_start = ((current_max - 2) / 5) * 5 + 1;
				}
				number+=5;
				if(number > max_page_start)
				{
					number = max_page_start;
				}
				g_history_page_refresh_flag = 1;
		}

}

void LongKeyIncreaseFunc(void)   			//长按键增加函数
{

			if(MenuID==0)
			{
				
			}
			else if(MenuID==1)
			{
				if(SelectMenuID==1)
	  	 		{
	  				current_date.year++;
					if(current_date.year>99)
						current_date.year=99;
					else if(current_date.year<23)
						current_date.year=23;
					SyncTimeToRTC();
	  	 		}
			else if(SelectMenuID==2)
	  	 		{
	  				current_date.month++;
					if(current_date.month>12)
						current_date.month=12;
					else if(current_date.month<1)
						current_date.month=1;
					SyncTimeToRTC();
	  	 		}
			else if(SelectMenuID==3)
	  	 		{
	  				current_date.day++;
					if(current_date.day>31)
						current_date.day=31;
					else if(current_date.day<1)
						current_date.day=1;
					SyncTimeToRTC();
	  	 		}
			else if(SelectMenuID==4)
	  	 		{
	  				current_time.hour++;
					if(current_time.hour>23)
						current_time.hour=23;

					SyncTimeToRTC();
	  	 		}
			else if(SelectMenuID==5)
	  	 		{
	  				current_time.minute++;
					if(current_time.minute>59)
						current_time.minute=59;
					SyncTimeToRTC();
	  	 		}
			else if(SelectMenuID==6)
	  	 		{
	  				current_time.second++;
					if(current_time.second>59)
						current_time.second=59;
					SyncTimeToRTC();
	  	 		}
			else if(SelectMenuID==7)
	  	 		{
	  				DeviceID++;
					if(DeviceID>MAX_DEV_ID)
						DeviceID=MAX_DEV_ID;
					AT24CXX_WriteUchar(ADR_DEVICEID,DeviceID);
	  	 		}
			else if(SelectMenuID==8)
	  	 		{
	  				AlarmTemperatureValue++;
					if(AlarmTemperatureValue>MAX_TEMP_VALUE)
						AlarmTemperatureValue=MAX_TEMP_VALUE;
					AT24CXX_WriteFloat(ADR_ALARM_TEMP,AlarmTemperatureValue);
	  	 		}
		   else if(SelectMenuID==9)
		   	{
				MaxPressure+=0.1F;
				if(MaxPressure>AUTO_HIGH_MAX)
					MaxPressure=AUTO_HIGH_MAX;
				AT24CXX_WriteFloat(ADR_MAXPRESSURE,MaxPressure);//
		   	}
		   else if(SelectMenuID==10)
		   	{
				MinPressure+=0.1F;
				if(MinPressure>AUTO_LOW_MAX)
					MinPressure=AUTO_LOW_MAX;
				AT24CXX_WriteFloat(ADR_MINPRESSURE,MinPressure);//
		   	}
				  	 

			}
			else if(MenuID==2)
			{
				// DeepSleepCount+=10;
				// if(DeepSleepCount>3600)
				// 	DeepSleepCount=3600;

				if(DeepSleepCount>=1000)
					DeepSleepCount+=1000;
				else if(DeepSleepCount>=100)
					DeepSleepCount+=100;
				else if(DeepSleepCount>=10)
					DeepSleepCount+=10;
				else 
					DeepSleepCount+=1;
				
				if(DeepSleepCount>3600)
					DeepSleepCount=3600;
				else if(DeepSleepCount<1)
					DeepSleepCount=0;

				AT24CXX_WriteUint(ADDR_DEEP_SLEEP_ADDR,DeepSleepCount);
			}
			else if(MenuID==3)
		{
				uint32_t current_max = FlashRead_Uint32(0);
				uint32_t max_page_start = 1;
				if (current_max > 1) {
					max_page_start = ((current_max - 2) / 5) * 5 + 1;
				}
				number+=5;
				if(number > max_page_start)
				{
					number = max_page_start;
				}
				g_page_reset_flag = 1;
		}
}

void KeyDecreaseFunc(void)    				//参数减少键函数
{
			  	if(MenuID==0)
					{
						//UP_Pressure=0;
					}
				else if(MenuID==1)
					{
				  	if(SelectMenuID==1)
		  	 		{
						if(current_date.year>23)
							current_date.year--;
						if(current_date.year<23)
							current_date.year=23;
						SyncTimeToRTC();
		  	 		}
				else if(SelectMenuID==2)
		  	 		{
						if(current_date.month>1)
							current_date.month--;
						if(current_date.month<1)
							current_date.month=1;
						SyncTimeToRTC();
		  	 		}
				else if(SelectMenuID==3)
		  	 		{
						if(current_date.day>1)
							current_date.day--;
						if(current_date.day<1)
							current_date.day=1;
						SyncTimeToRTC();
		  	 		}
				else if(SelectMenuID==4)
		  	 		{
						if(current_time.hour>=1)
							current_time.hour--;
						if(current_time.hour>23)
							current_time.hour=23;
						SyncTimeToRTC();
		  	 		}
				else if(SelectMenuID==5)
		  	 		{
						if(current_time.minute>=1)
							current_time.minute--;
						if(current_time.minute>59)
							current_time.minute=59;
						SyncTimeToRTC();
		  	 		}
				else if(SelectMenuID==6)
		  	 		{
						if(current_time.second>=1)
							current_time.second--;
						if(current_time.second>59)
							current_time.second=59;
						SyncTimeToRTC();
		  	 		}
				else if(SelectMenuID==7)
		  	 		{
						if(DeviceID>1)
				    		DeviceID--;
						if(DeviceID<1)
							DeviceID=1;
						AT24CXX_WriteUchar(ADR_DEVICEID,DeviceID);
		  	 		}
				else if(SelectMenuID==8)
		  	 		{
						AlarmTemperatureValue--;
				   	if(AlarmTemperatureValue<MIN_TEMP_VALUE)
					   	AlarmTemperatureValue=MIN_TEMP_VALUE;
				   	AT24CXX_WriteFloat(ADR_ALARM_TEMP,AlarmTemperatureValue);
		  	 		}
			   	else if(SelectMenuID==9)
			   	{

					MaxPressure-=0.1F;
					if(MaxPressure<AUTO_HIGH_MIN)
						MaxPressure=AUTO_HIGH_MIN;
					AT24CXX_WriteFloat(ADR_MAXPRESSURE,MaxPressure);//
			   	}
			   	else if(SelectMenuID==10)
			   	{
					MinPressure-=0.1F;
					if(MinPressure<AUTO_LOW_MIN)
						MinPressure=AUTO_LOW_MIN;
					AT24CXX_WriteFloat(ADR_MINPRESSURE,MinPressure);//
			   	}
			  

					}
				else if(MenuID==2)
				{
					if(DeepSleepCount>=1000)
						DeepSleepCount-=100;
					else if(DeepSleepCount>=100)
						DeepSleepCount-=10;
					else if(DeepSleepCount>=1)
						DeepSleepCount--;
					
					if(DeepSleepCount>3600)
						DeepSleepCount=3600;
					else if(DeepSleepCount<1)
						DeepSleepCount=0;
					AT24CXX_WriteUint(ADDR_DEEP_SLEEP_ADDR,DeepSleepCount);
				}
		else if(MenuID==3)
		{
				if(number>5)
					number-=5;
				else
					number=1;
				g_history_page_refresh_flag = 1;
		}
}

void LongKeyDecreaseFunc(void)   			//长按键减少函数
{
				if(MenuID==0)
				{
						
				}
				else if(MenuID==1)
				{
				  	if(SelectMenuID==1)
		  	 		{
						if(current_date.year>15)
							current_date.year--;
						if(current_date.year<15)
							current_date.year=15;
						SyncTimeToRTC();
		  	 		}
				else if(SelectMenuID==2)
		  	 		{
						if(current_date.month>1)
							current_date.month--;
						if(current_date.month<1)
							current_date.month=1;
						SyncTimeToRTC();
		  	 		}
				else if(SelectMenuID==3)
		  	 		{
						if(current_date.day>1)
							current_date.day--;
						if(current_date.day<1)
							current_date.day=1;
						SyncTimeToRTC();
		  	 		}
				else if(SelectMenuID==4)
		  	 		{
						if(current_time.hour>=1)
							current_time.hour--;
						if(current_time.hour>23)
							current_time.hour=23;
						SyncTimeToRTC();
		  	 		}
				else if(SelectMenuID==5)
		  	 		{
						if(current_time.minute>=1)
							current_time.minute--;
						if(current_time.minute>59)
							current_time.minute=59;
						SyncTimeToRTC();
		  	 		}
				else if(SelectMenuID==6)
		  	 		{
						if(current_time.second>=1)
							current_time.second--;
						if(current_time.second>59)
							current_time.second=59;
						SyncTimeToRTC();
		  	 		}
				else if(SelectMenuID==7)
		  	 		{
						if(DeviceID>1)
				    		DeviceID--;
						if(DeviceID<1)
							DeviceID=1;
						AT24CXX_WriteUchar(ADR_DEVICEID,DeviceID);
		  	 		}
				else if(SelectMenuID==8)
		  	 		{
						AlarmTemperatureValue--;
				   	if(AlarmTemperatureValue<MIN_TEMP_VALUE)
					   	AlarmTemperatureValue=MIN_TEMP_VALUE;
				   	AT24CXX_WriteFloat(ADR_ALARM_TEMP,AlarmTemperatureValue);
		  	 		}
			   	else if(SelectMenuID==9)
			   	{

					MaxPressure-=0.1F;
					if(MaxPressure<AUTO_HIGH_MIN)
						MaxPressure=AUTO_HIGH_MIN;
					AT24CXX_WriteFloat(ADR_MAXPRESSURE,MaxPressure);//
			   	}
			   	else if(SelectMenuID==10)
			   	{
					MinPressure-=0.1F;
					if(MinPressure<AUTO_LOW_MIN)
						MinPressure=AUTO_LOW_MIN;
					AT24CXX_WriteFloat(ADR_MINPRESSURE,MinPressure);//
			   	}
					}
				
				else if(MenuID==2)
				{
						// if(DeepSleepCount>=10)
						// 			DeepSleepCount-=10;
						// 		if(DeepSleepCount<10)
						// 			DeepSleepCount=10;

						if(DeepSleepCount>=1000)
							DeepSleepCount-=100;
						else if(DeepSleepCount>=100)
							DeepSleepCount-=10;
						else if(DeepSleepCount>=1)
							DeepSleepCount-=1;

						if(DeepSleepCount>3600)
							DeepSleepCount=3600;
						else if(DeepSleepCount<1)
							DeepSleepCount=0;

						AT24CXX_WriteUint(ADDR_DEEP_SLEEP_ADDR,DeepSleepCount);
				}		
	else if(MenuID==3)
	{
		if(number>5)
			number-=5;
		else
			number=1;
		g_page_reset_flag = 1;
	}
}

void PageDownKeyFunc(void)   				//翻页向下按键函数
{
					MenuID++;
					if(MenuID>3)
						MenuID=0;

}

uint8_t IR_KEY_OPEN_FLAG;
uint8_t IR_KEY_CLOSE_FLAG;
uint8_t IR_KEY_STOP_FLAG;

void		KeyRespFunc(void)   			//按键响应函数
{
				uint8_t ucKeyCode;
			
				ucKeyCode = IR_GetKey();	/* 读按键, 无键按下时返回 KEY_NONE = 0 */
				if (ucKeyCode != 0)
				{

					LowFreqCount=0;		//清零低频计数器和菜单计数器，重置 LowFreqCount 和 MenuIDCount 计数器
					MenuIDCount=0;

					switch (ucKeyCode)
					{
						case IR_KEY_BACK:							//红外按键Back	
							if(MenuID==1)
							ModelIncreaseKeyFunc();
							break;
	
						case IR_KEY_UP://红外按键Up
							if(MenuID==1 || MenuID==2 || MenuID==3)
							KeyIncreaseFunc();
							break;	
							
						case IR_KEY_DOWN:							//红外按键Down
							if(MenuID==1 || MenuID==2 || MenuID==3)
							KeyDecreaseFunc();
							break;

						case IR_KEY_ENTER:							//红外按键Enter
							PageDownKeyFunc();
							if(MenuID==1)
							{
								SyncTimeToRTC();
							}
							else if(MenuID==2)
							{
								number=1;
								//Time_InitStructure=SetupTimeStructure;
								//Date_InitStructure=SetupDateStructure;
								SyncTimeToRTC();
							}
							break;

							// 在KeyRespFunc()中修改
							case IR_KEY_OPEN:                           //红外按键Open
								if(MenuID == 0)  // 只在主界面响应
								{
									IR_KEY_OPEN_FLAG = 1;  // 设置开阀按键标志
								}
								break;
							
							case IR_KEY_CLOSE:                          //红外按键Close
								if(MenuID == 0)  // 只在主界面响应
								{
									IR_KEY_CLOSE_FLAG = 1;  // 设置关阀按键标志
								}
								break;

						case IR_KEY_STOP:							//红外按键Stop
							if((WakeupFlag==0)&&(LowFreqFlag==1))
							{
								IR_KEY_STOP_FLAG = 1;
								WakeupFlag=1;
								LowFreqCount=0;
								LowFreqFlag=0;

								// LCDLowFreqCount=0;
								// LCDLowFreqFlag=0;
								// LCDWakeupCount=0;
								// LCDWakeupFlag=0;
			  		}
							break;

//						default:
//							if (ucKeyCode >= IR_KEY_STRAT)
//							{

//							}
//							break;
	
					}
				}

}
















