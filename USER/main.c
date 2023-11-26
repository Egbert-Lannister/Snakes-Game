///***************************************************
//̰����
//STM32F103ZET6
//����:̰������Ϸ,�Ե�ʳ���,�ٶ�����,���ȱ䳤,ҧ���Լ���Ϸ����,��λ���¿�ʼ

//ϵͳ�ļ�
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "beep.h"
//�ڲ�����
#include "rand.h"
//��Χ��·
#include "led.h"
#include "key.h"
#include "lcd_4_3.h"
//�û��㷨
#include "my_task.h"

#define GAME_WIDTH	240	
#define GAME_HEIGHT	320
#define GAME_XPART  24
#define GAME_YPART  32

//#define GAME_WIDTH	480	//������Ļ�ֱ���Ϊ800*480,Ϊ��ά��������,�ֳ�80*48�ȷ�
//#define GAME_HEIGHT	800
//#define GAME_XPART  48
//#define GAME_YPART  80
enum sta //ö��״̬
{ 
	OFF,//0
	ON	//1
} station;

enum dir //ö��̰���߷���
{
	UP,		//��
	DOWN,	//��
	LEFT,	//��
	RIGHT	//��
}direction;

typedef struct  //̰������Ҫ������
{										    
	u16 speed;		//�ٶ�
	u16 length;     //����
	u16 energybuf;  //��������
	u16 width;      //�߿�	
	u16 life;       //������	
	u16 firstx;     //��ͷ����x
	u16 firsty;     //��ͷ����y
	u16 lastx;      //��β����x
	u16 lasty;      //��β����y
	u16 color;      //����ɫ	
}snakes; 
snakes snake;//��ʱsnake����snakes�ṹ���е����в���

void Snake_Init(snakes * snake)//̰���߲�����ʼ��
{
	snake->speed=5;
	snake->length=10;
	snake->energybuf=0;
	snake->width=1;
	snake->life=3;
	snake->firstx=4;
	snake->firsty=5;
	snake->lastx=0;
	snake->lasty=0;
	snake->color=BLUE;
}

typedef struct  //ʳ����Ҫ������
{										    
	u16  energy ;//ʳ������
	u16  x ;	//ʳ������x
	u16  y ;	//ʳ������y
}apples; 
apples apple;

void Apple_Init(apples * apple)//ʳ�������ʼ��
{
	apple->energy=1;
//	apple->x=24;	//������������������
//	apple->y=40;
}

typedef struct  //�¼���Ҫ������
{										    
	u16  Process ;//�Ƿ���Ϸ��ͣ
	u16  GameSta ;//�Ƿ���Ϸ����
	u16  AppleSta;//ʳ��״̬ 
	u16  Direction;//��ǰ������
}events; 
events event;

void Event_Init(events * event)//�¼�������ʼ��
{
	event->Process=ON;//û����ͣ
	event->GameSta=ON;//û�н���
	event->AppleSta=ON;
	event->Direction=DOWN;//��ʼ��������
}
typedef struct  //̰�����������
{
	u16 x;
	u16 y;
}axiss;
axiss snake_axis[100];  //��������󳤶ȿ��Դﵽ100

void Display(u16 x,u16 y,u16 color)//����80*48������ӳ�䵽800*480����Ļ��
{
	LCD_Fill(GAME_WIDTH/GAME_XPART*x,GAME_HEIGHT/GAME_YPART*y,GAME_WIDTH/GAME_XPART*(x+1),GAME_HEIGHT/GAME_YPART*(y+1),color);
}

void DisplayInit(void) //�����ʼ��,Ĭ���ڸ�λ��,����ͷ����
{
	u16 i;
	for(i=0;i<5;i++)//����ʼ�����������������
	{
		snake_axis[i].x=i;
		snake_axis[i].y=0;
	}
		for(i=5;i<10;i++)
	{
		snake_axis[i].x=4;
		snake_axis[i].y=i-4;
	}
	LCD_Clear(WHITE);
	for(i=0;i<10;i++)
	{
		Display(snake_axis[i].x,snake_axis[i].y,BLUE);//���ճ�ʼ�����ӡ������
	}
}

void GameOver(void)//��Ϸ��������,��ӡ"GAME OVER",��������,����������ݴ�����
{
	
	LCD_ShowString(180,388,200,24,24,"GAME OVER !"); 
  BEEP=1;  
	delay_ms(500);
	BEEP=0;
	vTaskSuspend(EVENTTask_Handler);//�����¼�������,�������ݴ�����event_task
}

//ȫ�ֱ���//////////////////////////////////////

int main(void)
 {	 
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	Rand_Adc_Init();	//�������ʼ��
 	LED_Init();			     //LED�˿ڳ�ʼ��
	KEY_Init();	//������ʼ��,���ư�����/��/��/��
	BEEP_Init();
	LCD_Init();//Ĭ��Ϊ����ģʽ
	DisplayInit();//��ӡ��ʼ����
	Snake_Init(&snake);//̰���߲�����ʼ��
	Apple_Init(&apple);//ʳ�������ʼ��
	Event_Init(&event);//�¼�������ʼ��
	xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������              
    vTaskStartScheduler();          //�����������
}
 
//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ��� 
    //����ʳ������
    xTaskCreate((TaskFunction_t )apple_task,     
                (const char*    )"apple_task",   
                (uint16_t       )APPLE_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )APPLE_TASK_PRIO,
                (TaskHandle_t*  )&APPLETask_Handler);        
    //�����¼�����
    xTaskCreate((TaskFunction_t )event_task,     
                (const char*    )"event_task",   
                (uint16_t       )EVENT_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )EVENT_TASK_PRIO,
                (TaskHandle_t*  )&EVENTTask_Handler);  
	//������ʾ����
    xTaskCreate((TaskFunction_t )display_task,     
                (const char*    )"display_task",   
                (uint16_t       )DISPLAY_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )DISPLAY_TASK_PRIO,
                (TaskHandle_t*  )&DISPLAYTask_Handler);  
	//������˸����
    xTaskCreate((TaskFunction_t )led_task,     
                (const char*    )"led_task",   
                (uint16_t       )LED_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LED_TASK_PRIO,
                (TaskHandle_t*  )&LEDTask_Handler);  
	//������������
    xTaskCreate((TaskFunction_t )key_task,     
                (const char*    )"key_task",   
                (uint16_t       )KEY_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )KEY_TASK_PRIO,
                (TaskHandle_t*  )&KEYTask_Handler);  
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//�¼�������,�������ݴ�����
void event_task(void *pvParameters)
{
	while(1)
	{
		if(event.GameSta==ON)//�����Ϸ���������
		{
			if(event.Process==ON)//���û����ͣ�����
			{
				switch(event.Direction)//��ⰴ�����,���ݷ��������ͷ����
				{
					case UP:
					{
						snake.firsty-=1;
						if(snake.firsty>GAME_YPART-1)//�����ͷy���곬����Ļ����Ϸ����(Ҳ�������óɳ�����Ļ�ʹ�ͷ��ʾ)
						{
							GameOver();
							//snake.firsty=GAME_XPART-1;
						}
					}break;
					case DOWN:
					{
						snake.firsty+=1;
						if(snake.firsty>GAME_YPART-1)
						{
							GameOver();
							//snake.firsty=0;
						}
					}break;
					case LEFT:
					{
						snake.firstx-=1;
						if(snake.firstx>GAME_XPART-1)
						{
							GameOver();
							//snake.firstx=GAME_XPART-1;
						}
					}break;
					case RIGHT:
					{
						snake.firstx+=1;
						if(snake.firstx>GAME_XPART-1)
						{
							GameOver();
							//snake.firstx=0;
						}
					}break;
				}
				snake.lastx=snake_axis[0].x;//��������β���� ����������β
				snake.lasty=snake_axis[0].y;
				if(snake.firstx==apple.x&&snake.firsty==apple.y)//�����ʱ��������ʳ��������ͬ
				{
					event.AppleSta=OFF;	//ʳ�ﱻ�Ե�
					snake.energybuf+=apple.energy;//�ߵ�������һ
					vTaskResume(APPLETask_Handler);	//ʹ������ʳ�ﺯ��	�ָ�apple_task
				}
				vTaskResume(DISPLAYTask_Handler);	//ʹ����ʾ����			
			}
		}else GameOver();//�����ϷΪ����״̬����Ϸ����
		delay_ms(1000/snake.speed);	//�����ߵ��ٶȵ����˺������ݴ�������ʱ����	200ms
	}
}

//����������,���ݰ��������ߵķ���,���ݴ�����֮ǰ���һ�ΰ�����Ϊ��Ч
void key_task(void *pvParameters)
{
	u8 key;
    while(1)
    {
		key=KEY_Scan(0);//����ɨ�躯������/��/��/��
		switch(key)
		{
			case WKUP_PRES:
			{
				if(event.Direction!=DOWN)//�����ǰ�����밴�������෴,����Ӧ
				event.Direction=UP;
			}break;
			case KEY1_PRES:
			{
				if(event.Direction!=UP)
				event.Direction=DOWN;
			}break;
			case KEY2_PRES:
			{
				if(event.Direction!=RIGHT)
				event.Direction=LEFT;
			}break;
			case KEY0_PRES:
			{
				if(event.Direction!=LEFT)
				event.Direction=RIGHT;
			}break;		
		}
        delay_ms(20);//ÿ20ms��Ӧһ��
    }
}

//ʳ��������,ʳ�ﱻ�Ե�ʱ�����˺���
void apple_task(void *pvParameters)
{
	u16 flag,i;
    while(1)
    {
		flag=1;
		while(flag)//�����µ�ʳ������
		{
			flag=0;
			apple.x=Get_Rand()%(u16)(GAME_XPART);//�������
			apple.y=Get_Rand()%(u16)(GAME_YPART);
			for(i=0;i<snake.length;i++) //ʳ�����겻����������
			{
				if(snake_axis[i].x==apple.x&&snake_axis[i].y==apple.y)
				{
					flag++;
				}
			}
		}
		Display(apple.x,apple.y,RED);//�������꽫ʳ����ʾ����
		vTaskSuspend(APPLETask_Handler);//��������	apple_task
    }
}


//��ʾ������,��ʾ��ͷ,��ȥ��β,�������ߵ�����
void display_task(void *pvParameters)
{
	u16 i;
    while(1)
    {
		 Display(snake.firstx,snake.firsty,BLUE);//��ʾ��ͷ
        if(snake.energybuf==0) //���û�гԵ�ʳ��
		{
			Display(snake.lastx,snake.lasty,WHITE);//��ȥ��β
			for(i=0;i<snake.length-1;i++)
			{
				snake_axis[i].x=snake_axis[i+1].x;//������������
				snake_axis[i].y=snake_axis[i+1].y;
			}	
		}else //����Ե���ʳ��
		{
			snake.energybuf--;//������������һ
			snake.length++; //�����һ
			if(snake.length%2==0 & snake.speed<=20)//Ϊ�˽��ͺ�����Ϸ�Ѷȣ���������ÿ������ʳ�������һ���ٶ�����ֵΪ20
				snake.speed++;
		}	
		snake_axis[snake.length-1].x=snake.firstx;//���µ��������Ϊ��ͷ������
		snake_axis[snake.length-1].y=snake.firsty;
		for(i=0;i<snake.length-1;i++)//�����û��ҧ���Լ�
		{
			if(snake_axis[i].x==snake.firstx&&snake_axis[i].y==snake.firsty)
			{
				event.GameSta=OFF; //�������������Ƿ�����ͷ������ͬ
			}
		}
		vTaskSuspend(DISPLAYTask_Handler);
    }
}

//��˸������,֤������û������
void led_task(void *pvParameters)
{
    while(1)
    {
        LED0=!LED0;
        delay_ms(500);
    }
}

