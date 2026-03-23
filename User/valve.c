
/*第二代切断阀逻辑代码*/

#include "valve.h"
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "GUI.h"
#include <string.h> 
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
#include "../Drivers/BSP/MB85RC16/AddrConst.h"
#include "./BSP/W25QXX/w25qxx.h"
#include "./BSP/SPI/spi.h"
#include "./BSP/ADC/adc.h"
#include "UI.h"
#include "keyapp.h"
#include "task.h"
#include "FreeModbusApp.h"

extern uint8_t IR_KEY_OPEN_FLAG;
extern uint8_t IR_KEY_CLOSE_FLAG;

uint32_t 			InterimState=0;
uint8_t 			TmpStateXFlag=0;
uint8_t 			TmpStateFlag=0;
uint8_t 			TmpCloseStateXFlag=0;
uint8_t 			TmpCloseStateFlag=0;
uint8_t 			CloseErrorCount=0;
uint8_t             RemoteCloseCauseFlag=0;
uint8_t				IsOpenSensorDetected=0;

static void SetManualActionCause(void)
{
	CurrentActionCause = HistoryActionManual;
}

static void UpdateEmergencyCloseActionCause(void)
{
	if((ExCloseSwitch == 1) || (ExCloseFlag == 1))
		CurrentActionCause = HistoryActionRemoteClose;
	else if((g_press_data.up_pressure > MaxPressure) || (OverPressFlag == 1))
		CurrentActionCause = HistoryActionOverPressClose;
	else if(g_press_data.up_pressure < MinPressure)
		CurrentActionCause = HistoryActionUnderPressClose;
	else if((OverTemperatureCheckFunc() == 1) || (OverTempFlag == 1))
		CurrentActionCause = HistoryActionOverTempClose;
	else
		CurrentActionCause = HistoryActionManual;
}

uint16_t 			TempCount=0;
uint16_t 			TempCount1=0;
uint16_t 			TempCount2=0;
uint16_t 			TempCount3=0;
uint16_t 			TempCount4=0;


/*
检查压力是否超过设定的上限或低于下限

OverpressureCheckFunc(1)  	压力超限
	
OverpressureCheckFunc(0) 	压力正常
*/
uint8_t OverpressureCheckFunc(void)
{
  	static uint8_t OverPreeCount=0;         //定义变量OverPreeCount=0，用于记录压力超限次数
	
	if ((g_press_data.up_pressure<MinPressure)||(g_press_data.up_pressure>MaxPressure))		//超压
		{
		  OverPreeCount++;            		//压力超限次数+1
		  if(OverPreeCount>2)               //如果 OverPreeCount 大于2，表示压力连续超限超过两次
		  	{
		  		OverPreeCount=5;            //此时将 OverPreeCount 设置为5，并返回1表示压力超限。
				return 1;	          
		  	}
			else
				return 0;                   //否则，返回0表示压力暂时超限但未达到连续超限的阈值。
		}
	else
		{
			OverPreeCount=0;                //如果压力在正常范围内，将 OverPreeCount 重置为0，并返回0表示压力正常。
			return 0;		           
		}
}


/*
检查当前温度是否超过设定的报警温度值

OverTemperatureCheckFunc(1)  	温度超限

OverTemperatureCheckFunc(0)		温度正常
*/
uint8_t OverTemperatureCheckFunc(void)
{
	if(temperature>AlarmTemperatureValue)
		{
			return 1;               	
		}
	else
		{
			return 0;              
		}
}




void HandModelFunc(void)	//开关阀逻辑，100ms执行一次
{
		switch(InterimState)
			{	
			   case 0: 		/* 初始/待机状态 */
			   	{					
					/* 清除所有临时计数器和状态标志 */
					TempCount=0;
					TempCount1=0;
					TempCount2=0;
					TempCount3=0;

					TmpStateXFlag=0;
					TmpStateFlag=0;

					TmpCloseStateXFlag=0;
					TmpCloseStateFlag=0;
					
					/* 1. 安全监测：优先检查是否存在紧急关阀条件 */
					if((OverpressureCheckFunc()==1)||(OverTemperatureCheckFunc()==1)||(ExCloseSwitch==1)||(ExCloseFlag==1)||(OverTempFlag==1)||(OverPressFlag==1))
						{                          
							  /* 如果不在关阀到位状态，则尝试紧急关阀 */
							  if(CloseSensor == 1)
							 {
								 if((ErrorAlarm!=CloseError))  		// 排除已经是关阀故障的情况，避免死循环
								 {	
									InterimState=9;	// 跳转至紧急关阀状态
									UpdateEmergencyCloseActionCause();
									
                                    RemoteCloseCauseFlag = ((ExCloseSwitch == 1) || (ExCloseFlag == 1)) ? 1 : 0;
									/* 复位部分标志 */
									ExCloseFlag=0; 
									OverTempFlag=0;
									OverPressFlag=0;
									
									/* 记录具体的故障类型 */
									if(OverpressureCheckFunc()==1)
										OverLoadFlag=1;
									if(OverTemperatureCheckFunc()==1)
										OverTemperatureFlag=1;
								 }
							 }   
						}
					else  /* 2. 正常待机逻辑：响应按键操作 */
						{
							OverLoadFlag=0;			// 清除超时标志
							OverTemperatureFlag=0;	// 清除超温标志
							
							/* 响应开阀按键 */
							if(IR_KEY_OPEN_FLAG)
								{					
									TmpStateFlag=0;
									SetManualActionCause();
									IR_KEY_OPEN_FLAG = 0; // 清除按键标志
									
									 	/* 情况 A: 传感器显示已开阀 */
									 	if(OpenSensor==0)
										  	{
												// 确认为开阀状态 (无需再检查 CloseSensor)
												InterimState=0;	// 保持当前状态
												ErrorAlarm=Opened;
										  	}
										  /* 情况 B: 传感器显示未开阀 */
										  else 
										  	{
										  	   /* B1: 既不开也不关 (中间状态) -> 执行强制复位开阀 */
										  	   if(CloseSensor==1)
										  	   	{
										  	   		InterimState=3;		// 跳转至"复位+开阀"流程
										  	   		ErrorAlarm=Opening;
											  		
													/* 初始化计数器 */
													TempCount=0;
													TempCount1=0;
													TempCount2=0;
													TempCount3=0;
													CloseErrorCount=0;
													IsOpenSensorDetected=0;
										  	   	}
											   /* B2: 处于关阀状态 -> 执行正常开阀 */
											   else
											   	{
											  		InterimState=1;		// 跳转至"正常开阀"流程
											  		ErrorAlarm=Opening;
											  		
													/* 初始化计数器 */
													TempCount=0;
													TempCount1=0;
													TempCount2=0;
													CloseErrorCount=0;
													IsOpenSensorDetected=0;
											   	}
										  	}
								}
							/* 响应关阀按键 */
							else if(IR_KEY_CLOSE_FLAG)
								{
									SetManualActionCause();
									IR_KEY_CLOSE_FLAG = 0; // 清除按键标志
									
									 /* 情况 A: 传感器显示已关阀 */
									 if(CloseSensor==0)
										{
											InterimState=0;		// 保持当前状态
											ErrorAlarm=Closed;
										}	
									 /* 情况 B: 未关阀 -> 执行正常关阀 */
									 else
									 	{
									 		InterimState=2;	// 跳转至"正常关阀"流程
											ErrorAlarm=Closing;
											
											/* 初始化计数器 */
											TempCount=0;
											TempCount1=0;
											TempCount2=0;
											TempCount3=0;
											CloseErrorCount=0;
									 	}
								}	
						}
			   	}
				break;

			   case 1:		/* 正常开阀 */
			   	{					
					//CheckMotorStall();//检测电机堵转
					
					/* 1. 安全监测：检查紧急关阀条件 (超压、超温、外部信号) */
					if((OverpressureCheckFunc()==1)||(OverTemperatureCheckFunc()==1)||(ExCloseSwitch==1)||(ExCloseFlag==1)||(OverTempFlag==1)||(OverPressFlag==1))
						{
							 /* 如果不在关阀到位状态，则尝试紧急关阀 */
							 if(CloseSensor==1)
							 {
								 if((ErrorAlarm!=CloseError)) // 排除已经是关阀故障的情况
								 {
									InterimState=9;			// 跳转至紧急关阀状态
                                    UpdateEmergencyCloseActionCause();
                                    RemoteCloseCauseFlag = ((ExCloseSwitch == 1) || (ExCloseFlag == 1)) ? 1 : 0;
									ExCloseFlag=0;
									OverTempFlag=0;
									OverPressFlag=0;
									
									if(OverpressureCheckFunc()==1)
										OverLoadFlag=1;
									if(OverTemperatureCheckFunc()==1)
										OverTemperatureFlag=1;
								 }
							 }

							TempCount=0;
							TempCount1=0;
							TempCount2=0;                   
						}
					else    /* 正常开阀逻辑 */
						{
								OverLoadFlag=0;
								OverTemperatureFlag=0;  // 重置故障标志
								LowFreqFlag=0;
								LowFreqCount=0;
								TempCount++;            // 全局开阀计时

								/* 情况 A: 检测到开阀信号 (OpenSensor=0) 或之前已检测到 */
								 if((OpenSensor==0)||(IsOpenSensorDetected==1))
									{
											TempCount1++;
											IsOpenSensorDetected=1;

											/* 延时 13秒 (130*100ms) 确保阀门完全打开 */
											if(TempCount1<130)  
												{
													MotorStartup();
													stallCheckTimer += 100; // 堵转检测计时

													ErrorAlarm=Opening;
												}
											else
												{
													/* 开阀完成 */
													ErrorAlarm=Opened;
											   		TempCount=0;
													TempCount1=0;
													TempCount2=0;

													MotorStop();
													
													InterimState=0;		// 返回就绪状态
													IsOpenSensorDetected=0;
												}
									}
									/* 情况 B: 尚未检测到开阀信号 (OpenSensor=1) */
									else
									{
											TempCount1=0;
											TempCount2++;
											
											if(TempCount2<SENSOR_ERROR_T)
											{
												MotorStartup();
												ErrorAlarm=Opening;
											}
											else
											{
												/* 开阀超时报错 */
												TempCount=0;
												TempCount1=0;
												TempCount2=0;

												MotorStop();
												StopFlag=1;

												ErrorAlarm=OpenError;
												InterimState=8;

												if(OverLoadFlag==1)
													uCoilsStatusLO.Bit4=OverLoadFlag;
												if(OverTemperatureFlag==1)
													uCoilsStatusLO.Bit5=OverTemperatureFlag;
												ucRegCoilsBuf[0]=uCoilsStatusLO.Byte;
											}
									}
					   	}
				}
			   break;

			   case 2:		/* 正常关阀 */
			   	{
					/* 重置标志位 */
					OverLoadFlag = 0;
					LowFreqFlag = 0;
					LowFreqCount = 0;
					
					/* 情况 A: 关阀传感器未触发 (CloseSensor=1) - 执行完整关阀动作 */
					if(CloseSensor == 1)
					{
						TempCount++;
						
						/* 阶段 1: 推杆电机正向运行 (关阀) - 0 ~ 4s */
						if(TempCount < CIRCLECOUNT)
						{
							ForwardRun();
							ErrorAlarm = Closing;
							TmpCloseStateXFlag = 1;
						}
						/* 阶段 2: 暂停/稳定 - 4s ~ 4.5s */
						else if(TempCount < (CIRCLECOUNT + 5))
						{
							KeepStop();
							ErrorAlarm = Closing;
							TmpCloseStateXFlag = 2;
						}
						/* 阶段 3: 推杆电机反向运行 (回退/释放应力) - 4.5s ~ 8s */
						else if(TempCount < (CIRCLECOUNT * 2))
						{
							ReverseRun();
							ErrorAlarm = Closing;
							TmpCloseStateXFlag = 3;
						}
						/* 阶段 4: 超时处理 - >8s */
						else
						{
							TempCount1 = 0;
							TmpCloseStateXFlag = 4;
							CloseErrorCount++;

							MotorStop();
							KeepStop();		// 推杆电机关阀后，还需要反向复位。

							if (CloseErrorCount >= 3)
							{
								/* 关阀失败，报 CloseError */
								TempCount = 0;
								StopFlag = 1;
								ErrorAlarm = CloseError;
								InterimState = 8;		// 跳转到故障状态
								
								/* 更新线圈状态 */
								if(OverLoadFlag == 1)
									uCoilsStatusLO.Bit4 = OverLoadFlag;
								if(OverTemperatureFlag == 1)
									uCoilsStatusLO.Bit5 = OverTemperatureFlag;
								ucRegCoilsBuf[0] = uCoilsStatusLO.Byte;
							}
							else
							{
								/* 重试推杆电机运动 */
								TempCount = 0;
								InterimState = 2;
							}
						}
					}
					/* 情况 B: 关阀传感器已触发 (CloseSensor=0) - 执行确认/复位动作 */
					else
					{
						TempCount1++;
						
						/* 阶段 1: 反向运行 (确保机械复位) - 0 ~ 4s */
						if(TempCount1 <= CIRCLECOUNT)
						{
							ReverseRun();
							ErrorAlarm = Closing;
							TmpCloseStateXFlag = 6;
						}
						/* 阶段 2: 关阀完成 */
						else
						{
							ErrorAlarm = Closed;
							TempCount = 0;
							TempCount1 = 0;

							MotorStop();
							KeepStop();		// 停止所有电机

							InterimState = 0;		// 返回就绪状态
							TmpCloseStateXFlag = 5;
						}
					}
				}
			   break;

			   case 3:		/* 复位开阀 */
			   	{					
					/* 1. 安全监测：检查紧急关阀条件 (超压、超温、外部信号) */
					if((OverpressureCheckFunc()==1)||(OverTemperatureCheckFunc()==1)||(ExCloseSwitch==1)||(ExCloseFlag==1)||(OverTempFlag==1)||(OverPressFlag==1))
						{ 
							 /* 如果不在关阀到位状态，则尝试紧急关阀 */
							 if(CloseSensor==1)
							 {
								 if((ErrorAlarm!=CloseError))
								 {
                                    UpdateEmergencyCloseActionCause();
                                    RemoteCloseCauseFlag = ((ExCloseSwitch == 1) || (ExCloseFlag == 1)) ? 1 : 0;
									InterimState=9;	// 跳转至紧急关阀状态
									ExCloseFlag=0;
									OverTempFlag=0;
									OverPressFlag=0;
									
									if(OverpressureCheckFunc()==1)
										OverLoadFlag=1;
									if(OverTemperatureCheckFunc()==1)
										OverTemperatureFlag=1;
								 }
							 }

							/* 重置计数器 */
							TempCount=0;
							TempCount1=0;
							TempCount2=0;
							TempCount3=0;
						}
					else /* 正常执行开阀流程 */
						{
								OverLoadFlag=0;
								OverTemperatureFlag=0;
								LowFreqFlag=0;
								LowFreqCount=0;
								TempCount3++;

								/* 阶段 A: 推杆电机复位序列 */
								
								/* A1: 推杆伸出 - 0 ~ 4s */
								if(TempCount3<CIRCLECOUNT)
									{
										ForwardRun();       // 伸出
										ErrorAlarm=Closing;
										TmpStateFlag=1;
									}
								/* A2: 暂停稳定 - 4s ~ 4.5s */
								else if(TempCount3<(CIRCLECOUNT+5))
									{
										KeepStop();         // 停止
										ErrorAlarm=Closing;
										TmpStateFlag=2;
									}
								/* A3: 推杆收回 (准备开阀) - 4.5s ~ 8s */
								else if(TempCount3<(CIRCLECOUNT*2))
									{
										ReverseRun();		// 收缩
										ErrorAlarm=Closing;
										TmpStateFlag=3;
									}
								/* 阶段 B: 无刷电机开阀序列 - >8s */
								else
									{
										/* 锁定在阶段B，保持推杆电机停止 */
										TempCount3=CIRCLECOUNT*3;
										KeepStop();
										TempCount++;
										
									 /* B1: 检测到已开阀 (或曾经检测到) */
									 if((OpenSensor==0)||(IsOpenSensorDetected==1))
										{
												TempCount1++;
												IsOpenSensorDetected=1;

												if(TempCount1<130) /* 第一次感应开位后继续运行时间 */  
													{
														MotorStartup();
														ErrorAlarm=Opening;
														TmpStateFlag=4;
													}
												else
													{
														/* 开阀完成 */
												   		TempCount=0;
														TempCount1=0;
														TempCount2=0;

														MotorStop();

														InterimState=0;		// 返回就绪状态
														ErrorAlarm=Opened;
														TmpStateFlag=5;
														IsOpenSensorDetected=0;
													}
										}
										/* B2: 尚未检测到开阀 */
										else	
										{
												TempCount1=0;
												TempCount2++;
												
												if(TempCount2<SENSOR_ERROR_T)
												{
													MotorStartup();
													ErrorAlarm=Opening;
													TmpStateFlag=9;
												}
												else
												{
													/* 开阀超时报错 */
													TempCount=0;
													TempCount1=0;
													TempCount2=0;
													TempCount3=0;
													TmpStateFlag=10;

													MotorStop();
													StopFlag=1;

													ErrorAlarm=OpenError;
													InterimState=8;
													
													if(OverLoadFlag==1)
														uCoilsStatusLO.Bit4=OverLoadFlag;
													if(OverTemperatureFlag==1)
														uCoilsStatusLO.Bit5=OverTemperatureFlag;
													ucRegCoilsBuf[0]=uCoilsStatusLO.Byte;
												}
										}
								}
					   	}
				}
			   break;

			   case 4:
			   	{
						
				}
			   break;

			   case 5:
			   	{
						
				}
			   break;

			   case 6:	
			   	{
						
			   }
			   break;
				 case 7:
			   	{

			   }
			   break;
			   case 8:		/* 超时/故障报警状态处理 */
			   	{
					/* 1. 检查是否存在紧急关阀条件 (超压、超温、外部关阀信号) */
					if((OverpressureCheckFunc()==1) || (OverTemperatureCheckFunc()==1) || 
					   (ExCloseSwitch==1) || (ExCloseFlag==1) || 
					   (OverTempFlag==1) || (OverPressFlag==1))
					{
						/* 如果阀门尚未关闭 (CloseSensor == 1 表示未关到位) */
						if(CloseSensor == 1)
						{
							/* 如果已经是关阀故障且重试次数超过3次，则不再尝试关闭，保持报警状态 */
							if((ErrorAlarm == CloseError) && (CloseErrorCount >= 3))
							{
								InterimState = 8;
								/* 清除部分临时标志，避免重复触发无效逻辑 */
								ExCloseFlag = 0;
								OverTempFlag = 0;
								OverPressFlag = 0;
							}
							/* 否则，尝试跳转到状态9进行紧急关阀 */
							else
							{
								InterimState = 9;		// 跳转至紧急关阀状态
                                UpdateEmergencyCloseActionCause();
                                RemoteCloseCauseFlag = ((ExCloseSwitch == 1) || (ExCloseFlag == 1)) ? 1 : 0;
								
								/* 复位标志 */
								ExCloseFlag = 0;
								OverTempFlag = 0;
								OverPressFlag = 0;
								
								/* 记录具体的故障类型 */
								if(OverpressureCheckFunc() == 1)
									OverLoadFlag = 1;
								if(OverTemperatureCheckFunc() == 1)
									OverTemperatureFlag = 1;

								TmpStateXFlag = 1;
							}
						}
						
						/* 重置计数器 */
						TempCount = 0;
						TempCount1 = 0; 
					}
					else
					{
						/* 无紧急故障，清除故障标志 */
						OverLoadFlag = 0;
						OverTemperatureFlag = 0;
						TmpStateXFlag = 2;
					}
					
					/* 2. 确保电机停止，维持停机状态 */
					MotorStop();
					KeepStop();
					
					TempCount = 0;
					TempCount1 = 0;
			
					StopFlag = 1;		// 标记系统停止

					/* 更新Modbus线圈状态 */
					if(OverLoadFlag == 1)
						uCoilsStatusLO.Bit4 = OverLoadFlag;
					if(OverTemperatureFlag == 1)
						uCoilsStatusLO.Bit5 = OverTemperatureFlag;
					ucRegCoilsBuf[0] = uCoilsStatusLO.Byte;

					/* 3. 处理按键输入 (人工干预) */
					if(IR_KEY_OPEN_FLAG)		// 按下开阀按键
					{
						SetManualActionCause();
						LowFreqFlag = 0;
						LowFreqCount = 0;
						IR_KEY_OPEN_FLAG = 0;
						
						/* 初始化开阀参数 */
						TempCount = 0;
						TempCount1 = 0;
						TempCount2 = 0;
						TempCount3 = 0;
						ErrorAlarm = Opening;
						CloseErrorCount = 0;
						IsOpenSensorDetected = 0;

						if(OpenSensor == 0)		// 传感器显示已开阀
						{
							// 确认为开阀状态 (无需再检查 CloseSensor)
							InterimState = 0;	// 直接进入就绪状态
							ErrorAlarm = Opened;
						}
						else // 传感器显示未开阀 (OpenSensor == 1)
						{
							if(CloseSensor == 1)
							{
								InterimState = 3;	// 从中间位置开阀 (通常对应无停止键的长开逻辑)
							}
							else // 处于关阀状态 (CloseSensor == 0)
							{
								InterimState = 1;	// 从关阀位置开阀
							}
						}
					}
					else if(IR_KEY_CLOSE_FLAG) // 按下关阀键
					{
						SetManualActionCause();
						IR_KEY_CLOSE_FLAG = 0;
						LowFreqFlag = 0;
						LowFreqCount = 0;
						
						if(CloseSensor == 0)	// 传感器显示已关阀
						{
							InterimState = 0;	// 保持就绪/关阀状态
							ErrorAlarm = Closed;
						}	
						else // 未关阀
						{
							InterimState = 2;	// 执行关阀流程
							TempCount = 0;
							TempCount1 = 0;
							TempCount2 = 0;
							TempCount3 = 0;
							ErrorAlarm = Closing;
							CloseErrorCount = 0;
						}
					}
			   }
			   break;
			   case 9:		/* 紧急关阀 */
			   	{
					/* 重置低功耗计数器，保持系统活跃 */
					LowFreqFlag=0;
					LowFreqCount=0;

					if(CloseSensor==1)						//不处于关阀状态，执行关阀序列
						{
					 		TempCount++;
							
							/* 阶段 1: 推杆电机正向运行 (关阀) - 0 ~ 4s */
							if(TempCount<CIRCLECOUNT)
								{
									ForwardRun();			//推杆电机正向---关阀
									ErrorAlarm=Closing;
									TmpStateXFlag=3;
								}
							/* 阶段 2: 暂停/稳定 - 4s ~ 4.5s */
							else if(TempCount<(CIRCLECOUNT+5))
								{
									KeepStop();
									ErrorAlarm=Closing;
									TmpStateXFlag=4;
								}
							/* 阶段 3: 推杆电机反向运行 (回退/释放应力) - 4.5s ~ 8s */
							else if(TempCount<(CIRCLECOUNT*2))
								{
									ReverseRun();
									ErrorAlarm=Closing;
									TmpStateXFlag=5;
								}
							/* 阶段 4: 超时/故障处理 - >8s */
							else
								{
									TempCount1=0;
									TmpStateXFlag=6;
									CloseErrorCount++;
									
									MotorStop();
									KeepStop();			
									
									if(CloseErrorCount >= 3)
										{
											TempCount=0;
											StopFlag=1;

											ErrorAlarm=CloseError;
											InterimState=8;			//达到最大重试次数后跳转到故障报警状态
											
											/* 更新线圈状态 */
											if(OverLoadFlag==1)
												uCoilsStatusLO.Bit4=OverLoadFlag;
											if(OverTemperatureFlag==1)
												uCoilsStatusLO.Bit5=OverTemperatureFlag;
											ucRegCoilsBuf[0]=uCoilsStatusLO.Byte;
										}
									else
										{
											TempCount=0;
											ErrorAlarm=Closing;
											InterimState=9;			//未达到最大重试次数，重新执行紧急关阀
										}
								}
						}
					else 		//处于关阀状态 (CloseSensor == 0)
						{
							TempCount1++;
							
							/* 阶段 1: 确保完全关闭/回退复位 - 0 ~ 4s */
							if(TempCount1<=CIRCLECOUNT)
								{
									ReverseRun();
									ErrorAlarm=Closing;
									TmpStateXFlag=8;
								}
							/* 阶段 2: 完成关阀流程 */
							else
								{
									TempCount=0;
									TempCount1=0;
									TmpStateXFlag=7;
									
									OverLoadFlag=1;			//标记操作完成或触发保护标志
									
									MotorStop();
									KeepStop();

									StopFlag=1;
                                    /* 根据关阀触发原因判断最终状态 */
                                    if(RemoteCloseCauseFlag == 1)
                                        ErrorAlarm=RemoteClose;                       //远程关阀
                                    else if(g_press_data.up_pressure>MaxPressure)            //超压关阀
                                        ErrorAlarm=OverPressClose;
                                    else if (g_press_data.up_pressure<MinPressure)
                                        ErrorAlarm=UnderPressClose;                      //欠压关阀
                                    else if (OverTemperatureCheckFunc()==1)
                                        ErrorAlarm=OverTempClose;                        //超温关阀
                                    else
                                        ErrorAlarm=NoError;                              //正常关阀
                                    RemoteCloseCauseFlag = 0;
									
									InterimState=0;			//返回初始状态									
								
									/* 更新线圈状态 */
									if(OverLoadFlag==1)
										uCoilsStatusLO.Bit4=OverLoadFlag;
									if(OverTemperatureFlag==1)
										uCoilsStatusLO.Bit5=OverTemperatureFlag;
									
									ucRegCoilsBuf[0]=uCoilsStatusLO.Byte;
								}

						}
			   }
			   break;
			   default:break;
			}
}














