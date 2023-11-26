#ifndef __MY_TASK_H
#define __MY_TASK_H	     
#include "FreeRTOS.h"	 
#include "task.h"

//�û�����
	//�������ȼ�
	#define EVENT_TASK_PRIO		7
	//�����ջ��С	
	#define EVENT_STK_SIZE 		128
	//������
	TaskHandle_t EVENTTask_Handler;
	//������
	void event_task(void *pvParameters);

	//�������ȼ�
	#define KEY_TASK_PRIO		6
	//�����ջ��С	
	#define KEY_STK_SIZE 		128
	//������
	TaskHandle_t KEYTask_Handler;
	//������
	void key_task(void *pvParameters);	

	//�������ȼ�
	#define APPLE_TASK_PRIO		5
	//�����ջ��С	
	#define APPLE_STK_SIZE 		128  
	//������
	TaskHandle_t APPLETask_Handler;
	//������
	void apple_task(void *pvParameters);
	
	//�������ȼ�
	#define SNAKE_TASK_PRIO		4
	//�����ջ��С	
	#define SNAKE_STK_SIZE 		128  
	//������
	TaskHandle_t SNAKETask_Handler;
	//������
	void snake_task(void *pvParameters);
	
	//�������ȼ�
	#define DISPLAY_TASK_PRIO		3
	//�����ջ��С	
	#define DISPLAY_STK_SIZE 		128
	//������
	TaskHandle_t DISPLAYTask_Handler;
	//������
	void display_task(void *pvParameters);	
	
	//�������ȼ�
	#define LED_TASK_PRIO		2
	//�����ջ��С	
	#define LED_STK_SIZE 		128
	//������
	TaskHandle_t LEDTask_Handler;
	//������
	void led_task(void *pvParameters);	

	//�������ȼ�
	#define START_TASK_PRIO		1
	//�����ջ��С	
	#define START_STK_SIZE 		128  
	//������
	TaskHandle_t StartTask_Handler;
	//������
	void start_task(void *pvParameters);

#endif  


