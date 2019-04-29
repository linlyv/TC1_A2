
#include "appmain.h"
#include "user_gpio.h"
#include "user_rtc.h"
#include <time.h>
#include <hsf.h>

#include "cJSON/cJSON.h"
//#include "user_mqtt_client.h"
#include "user_function.h"

#define TIMER2_ID		(2)  //timer2 for rtctime
static hftimer_handle_t user_rtc_timer=NULL;


struct tm * user_time_t;
int rtctimes;


USER_FUNC void rtc_thread_func(void * arg){
	
	int i, j;
  char task_flag[PLUG_NUM] = { -1, -1, -1 ,-1,-1};   //记录要哪个插座要返回数据
	
	bool ntptime_succeed;
	ntptime_succeed=0;
	struct tm * t_t;
	time_t t;  
  t=1546272000;  //2019-01-1 0:0:0
  t_t=gmtime(&t); //时间结构体
	//u_printf("now time:hour:%d,min:%d,sec:%d\n",t_t->tm_hour,t_t->tm_min,t_t->tm_sec);
	u_printf("rtc_thread started.");
	while(1){
     //msleep(50);
			char update_user_config_flag = 0;
			if(t_t->tm_min%10==0){   //
					if(!ntptime_succeed){
						if(update_rtctime()){
							ntptime_succeed=1;
							//t_t=user_time_t;
							u_printf("now time is:%d : %d\n",t_t->tm_hour,t_t->tm_min);
						}else{
							ntptime_succeed=0;
						}
					}
					else{
						//u_printf("update time succeed\n");
					}
					
			}else{
					ntptime_succeed=0;
			}

			for ( i = 0; i < PLUG_NUM; i++ )
			{
					for ( j = 0; j < PLUG_TIME_TASK_NUM; j++ )
					{
							if ( u_config.plug[i].task[j].on != 0 )
							{
									
									uint8_t repeat = u_config.plug[i].task[j].repeat;
									if (    
									t_t->tm_sec == 0 && t_t->tm_min == u_config.plug[i].task[j].minute && t_t->tm_hour == u_config.plug[i].task[j].hour
									&& ((repeat == 0x00) || repeat & (1 << (t_t->tm_wday - 1)) )
									)
									{
											if ( u_config.plug[i].on != u_config.plug[i].task[j].action )
											{
												//u_printf("u_config.plug[i].task[j].action:%d\n",u_config.plug[i].task[j].action);
													user_relay_set( i, u_config.plug[i].task[j].action );
													update_user_config_flag = 1;
												//u_printf("u_config.plug[i].task[j].action=%d\n",u_config.plug[i].task[j].action);
													//user_mqtt_send_plug_state( i );
											}
											if ( repeat == 0x00 )
											{
													task_flag[i] = j;
													u_config.plug[i].task[j].on = 0;
													update_user_config_flag = 1;
											}
										//	u_printf("repeat:%d\n",repeat);
									}
							}
					}
			}

			if ( update_user_config_flag == 1 )
			{
					update_user_config_flag = 0;

					cJSON *json_send = cJSON_CreateObject( );
					cJSON_AddStringToObject( json_send, "mac", strMac );
				
					for ( i = 0; i < PLUG_NUM; i++ )
					{
							char strTemp1[] = "plug_X";
							strTemp1[5] = i + '0';
							cJSON *json_send_plug = cJSON_CreateObject( );
							cJSON_AddNumberToObject( json_send_plug, "on", u_config.plug[i].on );

							if ( task_flag[i] >= 0 )
							{
									cJSON *json_send_plug_setting = cJSON_CreateObject( );

									j = task_flag[i];
									char strTemp2[] = "task_X";
									strTemp2[5] = j + '0';
									//u_printf("[strTemp2] task_X=>%s\n",strTemp2);
									cJSON *json_send_plug_task = cJSON_CreateObject( );
									cJSON_AddNumberToObject( json_send_plug_task, "hour", u_config.plug[i].task[j].hour );
									cJSON_AddNumberToObject( json_send_plug_task, "minute", u_config.plug[i].task[j].minute );
									cJSON_AddNumberToObject( json_send_plug_task, "repeat", u_config.plug[i].task[j].repeat );
									cJSON_AddNumberToObject( json_send_plug_task, "action", u_config.plug[i].task[j].action );
									cJSON_AddNumberToObject( json_send_plug_task, "on", u_config.plug[i].task[j].on );
									cJSON_AddItemToObject( json_send_plug_setting, strTemp2, json_send_plug_task );

									cJSON_AddItemToObject( json_send_plug, "setting", json_send_plug_setting );

									task_flag[i] = -1;
							}
							cJSON_AddItemToObject( json_send, strTemp1, json_send_plug );
					}

					char *json_str = cJSON_Print( json_send );
					u_printf("****rtc thread ********>>>  json_send:\n %d bytes\n",sizeof(json_str));
					user_send( false, json_str );    //发送数据, 

					hfmem_free( json_str );
					json_str=NULL;
					cJSON_Delete( json_send );
			}
		
	}
	u_printf("rtc_thread_func exit..");
}

 void USER_FUNC rtctime_timeout_handler( void* arg ){
	 
	time_t t;  
  rtctimes=rtctimes+1;
  t=rtctimes+8*3600;  
  user_time_t=gmtime(&t); //时间结构体
}


 void USER_FUNC rtctime_init(void ){
	if((user_rtc_timer = hftimer_create("TIMER2",1000,1,TIMER2_ID,rtctime_timeout_handler,0))==NULL)//每一秒加一,模拟时钟在跑
  {
    u_printf("create timer 2 fail\n");
  }
	u_printf("rtc_init done.\n");
	hfthread_create(rtc_thread_func,"rtccontrol",1024,(void*)1,1,NULL,NULL);
	hftimer_start(user_rtc_timer);
}


int update_rtctime(void){
	int time_mm;
	time_mm=hfntp_get_time(ntpserver,123,200);
	if(time_mm<0){
		return 0;
	}
	rtctimes=time_mm;
	u_printf("*****ntp time got.*****\n ");
	return 1;
}


int printtime()  
{  
  time_t t;  

  t=rtctimes+8*3600;  //东八区
  user_time_t=gmtime(&t); //时间结构体
	u_printf("hour:%d,min:%d\n",user_time_t->tm_hour,user_time_t->tm_min);
  char s[100];  
  strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", user_time_t);  
  u_printf("%d: %s\n", (int)t, s);  

	
  return 1;  
}  

