#ifndef __MY_TASK_H
#define __MY_TASK_H	     
#include "FreeRTOS.h"	 
#include "task.h"

//用户任务
	//任务优先级
	#define EVENT_TASK_PRIO		7
	//任务堆栈大小	
	#define EVENT_STK_SIZE 		128
	//任务句柄
	TaskHandle_t EVENTTask_Handler;
	//任务函数
	void event_task(void *pvParameters);

	//任务优先级
	#define KEY_TASK_PRIO		6
	//任务堆栈大小	
	#define KEY_STK_SIZE 		128
	//任务句柄
	TaskHandle_t KEYTask_Handler;
	//任务函数
	void key_task(void *pvParameters);	

	//任务优先级
	#define APPLE_TASK_PRIO		5
	//任务堆栈大小	
	#define APPLE_STK_SIZE 		128  
	//任务句柄
	TaskHandle_t APPLETask_Handler;
	//任务函数
	void apple_task(void *pvParameters);
	
	//任务优先级
	#define SNAKE_TASK_PRIO		4
	//任务堆栈大小	
	#define SNAKE_STK_SIZE 		128  
	//任务句柄
	TaskHandle_t SNAKETask_Handler;
	//任务函数
	void snake_task(void *pvParameters);
	
	//任务优先级
	#define DISPLAY_TASK_PRIO		3
	//任务堆栈大小	
	#define DISPLAY_STK_SIZE 		128
	//任务句柄
	TaskHandle_t DISPLAYTask_Handler;
	//任务函数
	void display_task(void *pvParameters);	
	
	//任务优先级
	#define LED_TASK_PRIO		2
	//任务堆栈大小	
	#define LED_STK_SIZE 		128
	//任务句柄
	TaskHandle_t LEDTask_Handler;
	//任务函数
	void led_task(void *pvParameters);	

	//任务优先级
	#define START_TASK_PRIO		1
	//任务堆栈大小	
	#define START_STK_SIZE 		128  
	//任务句柄
	TaskHandle_t StartTask_Handler;
	//任务函数
	void start_task(void *pvParameters);

#endif  


