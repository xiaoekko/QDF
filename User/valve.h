#include "./SYSTEM/sys/sys.h"

#define SENSOR_ERROR_T          560		//	40s   	开阀故障检测时间

#define CIRCLECOUNT            	40    	//  4s	关阀故障检测时间



#define STALL_CHECK_PERIOD     500    // 增加堵转检测周期到500ms
#define STALL_PULSE_THRESHOLD   5     // 降低堵转判断阈值
#define MOTOR_START_DELAY      2000   // 增加电机启动延时到2秒
#define PUSHMOTOR_STABLE_TIME   50    // 推杆电机状态稳定时间(5秒)


// 外部变量声明
extern volatile uint32_t pulseCount;    // 脉冲计数器
extern volatile uint32_t lastPulseCount; // 上次的脉冲计数
extern volatile uint32_t stallCheckTimer; // 堵转检测计时器
extern volatile uint32_t motorStartDelay; // 电机启动延时计数器
extern volatile uint8_t isMotorRunning;   // 电机运行状态标志

void HandModelFunc(void);

uint8_t OverpressureCheckFunc(void);
uint8_t OverTemperatureCheckFunc(void);


