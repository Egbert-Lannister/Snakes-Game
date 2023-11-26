///***************************************************
//贪吃蛇
//STM32F103ZET6
//功能:贪吃蛇游戏,吃到食物后,速度增加,长度变长,咬到自己游戏结束,复位重新开始

//系统文件
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "beep.h"
//内部驱动
#include "rand.h"
//外围电路
#include "led.h"
#include "key.h"
#include "lcd_4_3.h"
//用户算法
#include "my_task.h"

#define GAME_WIDTH	240	
#define GAME_HEIGHT	320
#define GAME_XPART  24
#define GAME_YPART  32

//#define GAME_WIDTH	480	//设置屏幕分辨率为800*480,为了维持正方形,分成80*48等分
//#define GAME_HEIGHT	800
//#define GAME_XPART  48
//#define GAME_YPART  80
enum sta //枚举状态
{ 
	OFF,//0
	ON	//1
} station;

enum dir //枚举贪吃蛇方向
{
	UP,		//上
	DOWN,	//下
	LEFT,	//左
	RIGHT	//右
}direction;

typedef struct  //贪吃蛇重要参数集
{										    
	u16 speed;		//速度
	u16 length;     //长度
	u16 energybuf;  //能量缓存
	u16 width;      //蛇宽	
	u16 life;       //生命数	
	u16 firstx;     //蛇头坐标x
	u16 firsty;     //蛇头坐标y
	u16 lastx;      //蛇尾坐标x
	u16 lasty;      //蛇尾坐标y
	u16 color;      //蛇颜色	
}snakes; 
snakes snake;//此时snake符合snakes结构体中得所有参数

void Snake_Init(snakes * snake)//贪吃蛇参数初始化
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

typedef struct  //食物重要参数集
{										    
	u16  energy ;//食物能量
	u16  x ;	//食物坐标x
	u16  y ;	//食物坐标y
}apples; 
apples apple;

void Apple_Init(apples * apple)//食物参数初始化
{
	apple->energy=1;
//	apple->x=24;	//坐标由随机数计算出来
//	apple->y=40;
}

typedef struct  //事件重要参数集
{										    
	u16  Process ;//是否游戏暂停
	u16  GameSta ;//是否游戏结束
	u16  AppleSta;//食物状态 
	u16  Direction;//蛇前进方向
}events; 
events event;

void Event_Init(events * event)//事件参数初始化
{
	event->Process=ON;//没有暂停
	event->GameSta=ON;//没有结束
	event->AppleSta=ON;
	event->Direction=DOWN;//初始方向向下
}
typedef struct  //贪吃蛇坐标参数
{
	u16 x;
	u16 y;
}axiss;
axiss snake_axis[100];  //设蛇身最大长度可以达到100

void Display(u16 x,u16 y,u16 color)//按照80*48的坐标映射到800*480的屏幕上
{
	LCD_Fill(GAME_WIDTH/GAME_XPART*x,GAME_HEIGHT/GAME_YPART*y,GAME_WIDTH/GAME_XPART*(x+1),GAME_HEIGHT/GAME_YPART*(y+1),color);
}

void DisplayInit(void) //蛇身初始化,默认在该位置,并蛇头向下
{
	u16 i;
	for(i=0;i<5;i++)//将初始坐标存入蛇身坐标内
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
		Display(snake_axis[i].x,snake_axis[i].y,BLUE);//按照初始坐标打印出蛇身
	}
}

void GameOver(void)//游戏结束函数,打印"GAME OVER",蜂鸣器响,挂起核心数据处理函数
{
	
	LCD_ShowString(180,388,200,24,24,"GAME OVER !"); 
  BEEP=1;  
	delay_ms(500);
	BEEP=0;
	vTaskSuspend(EVENTTask_Handler);//挂起事件任务函数,核心数据处理函数event_task
}

//全局变量//////////////////////////////////////

int main(void)
 {	 
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	Rand_Adc_Init();	//随机数初始化
 	LED_Init();			     //LED端口初始化
	KEY_Init();	//按键初始化,自制按键上/下/左/右
	BEEP_Init();
	LCD_Init();//默认为竖屏模式
	DisplayInit();//打印初始蛇身
	Snake_Init(&snake);//贪吃蛇参数初始化
	Apple_Init(&apple);//食物参数初始化
	Event_Init(&event);//事件参数初始化
	xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}
 
//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区 
    //创建食物任务
    xTaskCreate((TaskFunction_t )apple_task,     
                (const char*    )"apple_task",   
                (uint16_t       )APPLE_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )APPLE_TASK_PRIO,
                (TaskHandle_t*  )&APPLETask_Handler);        
    //创建事件任务
    xTaskCreate((TaskFunction_t )event_task,     
                (const char*    )"event_task",   
                (uint16_t       )EVENT_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )EVENT_TASK_PRIO,
                (TaskHandle_t*  )&EVENTTask_Handler);  
	//创建显示任务
    xTaskCreate((TaskFunction_t )display_task,     
                (const char*    )"display_task",   
                (uint16_t       )DISPLAY_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )DISPLAY_TASK_PRIO,
                (TaskHandle_t*  )&DISPLAYTask_Handler);  
	//创建闪烁任务
    xTaskCreate((TaskFunction_t )led_task,     
                (const char*    )"led_task",   
                (uint16_t       )LED_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LED_TASK_PRIO,
                (TaskHandle_t*  )&LEDTask_Handler);  
	//创建输入任务
    xTaskCreate((TaskFunction_t )key_task,     
                (const char*    )"key_task",   
                (uint16_t       )KEY_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )KEY_TASK_PRIO,
                (TaskHandle_t*  )&KEYTask_Handler);  
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//事件任务函数,核心数据处理函数
void event_task(void *pvParameters)
{
	while(1)
	{
		if(event.GameSta==ON)//如果游戏正常则继续
		{
			if(event.Process==ON)//如果没有暂停则继续
			{
				switch(event.Direction)//检测按键情况,根据方向调整蛇头坐标
				{
					case UP:
					{
						snake.firsty-=1;
						if(snake.firsty>GAME_YPART-1)//如果蛇头y坐标超出屏幕则游戏结束(也可以设置成超出屏幕就从头显示)
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
				snake.lastx=snake_axis[0].x;//保存下蛇尾坐标 用于消除蛇尾
				snake.lasty=snake_axis[0].y;
				if(snake.firstx==apple.x&&snake.firsty==apple.y)//如果此时的坐标与食物坐标相同
				{
					event.AppleSta=OFF;	//食物被吃掉
					snake.energybuf+=apple.energy;//蛇的能量加一
					vTaskResume(APPLETask_Handler);	//使能生成食物函数	恢复apple_task
				}
				vTaskResume(DISPLAYTask_Handler);	//使能显示函数			
			}
		}else GameOver();//如果游戏为结束状态则游戏结束
		delay_ms(1000/snake.speed);	//按照蛇的速度调整此核心数据处理函数的时间间隔	200ms
	}
}

//输入任务函数,根据按键调整蛇的方向,数据处理函数之前最后一次按键视为有效
void key_task(void *pvParameters)
{
	u8 key;
    while(1)
    {
		key=KEY_Scan(0);//按键扫描函数，上/下/左/右
		switch(key)
		{
			case WKUP_PRES:
			{
				if(event.Direction!=DOWN)//如果当前方向与按键方向相反,则不响应
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
        delay_ms(20);//每20ms响应一次
    }
}

//食物任务函数,食物被吃掉时触发此函数
void apple_task(void *pvParameters)
{
	u16 flag,i;
    while(1)
    {
		flag=1;
		while(flag)//生成新的食物坐标
		{
			flag=0;
			apple.x=Get_Rand()%(u16)(GAME_XPART);//随机函数
			apple.y=Get_Rand()%(u16)(GAME_YPART);
			for(i=0;i<snake.length;i++) //食物坐标不能在蛇身上
			{
				if(snake_axis[i].x==apple.x&&snake_axis[i].y==apple.y)
				{
					flag++;
				}
			}
		}
		Display(apple.x,apple.y,RED);//按照坐标将食物显示出来
		vTaskSuspend(APPLETask_Handler);//挂起任务	apple_task
    }
}


//显示任务函数,显示蛇头,消去蛇尾,并更新蛇的坐标
void display_task(void *pvParameters)
{
	u16 i;
    while(1)
    {
		 Display(snake.firstx,snake.firsty,BLUE);//显示蛇头
        if(snake.energybuf==0) //如果没有吃到食物
		{
			Display(snake.lastx,snake.lasty,WHITE);//消去蛇尾
			for(i=0;i<snake.length-1;i++)
			{
				snake_axis[i].x=snake_axis[i+1].x;//挨个更新坐标
				snake_axis[i].y=snake_axis[i+1].y;
			}	
		}else //如果吃到了食物
		{
			snake.energybuf--;//能量缓冲区减一
			snake.length++; //蛇身加一
			if(snake.length%2==0 & snake.speed<=20)//为了降低后期游戏难度，所以设置每吃两个食物蛇身加一，速度上限值为20
				snake.speed++;
		}	
		snake_axis[snake.length-1].x=snake.firstx;//最新的坐标更新为蛇头的坐标
		snake_axis[snake.length-1].y=snake.firsty;
		for(i=0;i<snake.length-1;i++)//检查有没有咬到自己
		{
			if(snake_axis[i].x==snake.firstx&&snake_axis[i].y==snake.firsty)
			{
				event.GameSta=OFF; //遍历蛇身坐标是否与蛇头坐标相同
			}
		}
		vTaskSuspend(DISPLAYTask_Handler);
    }
}

//闪烁任务函数,证明程序没有死机
void led_task(void *pvParameters)
{
    while(1)
    {
        LED0=!LED0;
        delay_ms(500);
    }
}

