
#include "appmain.h"
#include "user_gpio.h"
#include "user_rtc.h"
//#include "user_mqtt_client.h"
#include "user_udp.h"
#include <string.h>
//#include "cJSON/cJSON.h"



#define TIMER1_ID		(1)   //定时器id


char ntpserver[]="pool.ntp.org";

uint8_t key_time = 0;
#define BUTTON_LONG_PRESS_TIME    10     //
int hour, min, secnod; //时间

int relay[]={SW1,SW2,SW3,SW4,SW5,SW6}; //对应每路继电器GPIO

static hftimer_handle_t user_key_timer=NULL;
hfthread_mutex_t keythread_lock=NULL_MUTEX;

int time_mm;
unsigned char config_data[1024];
u_config_t  user_defaultconfig;
u_config_t   u_config;




void USER_FUNC user_led_set( char x )
{
    if ( x == -1 )
		{
        if(hfgpio_fpin_is_high( LED )){
					hfgpio_fset_out_low( LED );
				}else
					hfgpio_fset_out_high( LED );
		}
    else if ( x )
        hfgpio_fset_out_high( LED );
    else
        hfgpio_fset_out_low( LED );
}

bool USER_FUNC relay_out( void )
{
    uint8_t i;
    for ( i = 0; i < PLUG_NUM; i++ )
    {
        if ( u_config.plug[i].on != 0 || user_defaultconfig.plug[i].on!=0 )
        {
            return true;
        }
    }
    return false;
}

/*user_relay_set
 * 设置继电器开关
 * x:编号 0-5
 * y:开关 0:关 1:开
 */
void USER_FUNC user_relay_set(uint8_t x,uint8_t y )
{
	 
    if (x >= PLUG_NUM ) return;

    if((y == 1) ? Relay_ON : Relay_OFF) 
			hfgpio_fset_out_high( relay[x] );
		else hfgpio_fset_out_low( relay[x] );

    user_defaultconfig.plug[x].on = y;
    u_config.plug[x].on = y;
    if ( relay_out( ) )
        user_led_set( 1 );
    else
        user_led_set( 0 );
}

/*
 * 设置所有继电器开关
 * y:0:全部关   1:根据记录状态开关所有
 *
 */
void USER_FUNC user_relay_set_all( uint8_t y )
{
    uint8_t i;
    for ( i = 0; i < PLUG_NUM; i++ )
        user_relay_set( i, y );
}

void USER_FUNC key_long_press( void )
{
//    os_log("key_long_press");
//    user_led_set( 1 );
//    user_mqtt_send( "mqtt test" );
	uint8_t i;
	for ( i = 0; i < PLUG_NUM; i++ )
	 u_printf("plug=%d---",u_config.plug[i].on );
}

void USER_FUNC key_long_10s_press( void )
{
    //OSStatus err;
   // char i = 0;
    //os_log( "WARNGIN: user params restored!" );
//    for ( i = 0; i < 3; i++ )
//    {
//        user_led_set( 1 );
//        mico_rtos_thread_msleep( 100 );
//        user_led_set( 0 );
//    }
//
    //appRestoreDefault_callback( user_config, sizeof(user_config_t) );
    ////sys_config->micoSystemConfig.ssid[0] = 0;
    //mico_system_context_update( mico_system_context_get( ) );
}
void USER_FUNC key_short_press( void )
{
    int i;
    if ( relay_out() )
    {		
        user_relay_set_all( 0 );
    }
    else
    {
        user_relay_set_all( 1 );
    }

    for ( i = 0; i < PLUG_NUM; i++ )
    {
        //user_mqtt_send_plug_state(i);
    }

}


 void USER_FUNC key_timeout_handler( void* arg )
{

    static uint8_t key_trigger, key_continue;
    static uint8_t key_last;
	
		hour = (hfsys_get_time()/1000)/3600;
		min = (hfsys_get_time()/1000 - hour*3600)/60;
    //按键扫描程序
	 
    uint8_t tmp = (0xfe | hfgpio_fpin_is_high( KEY ));
    //key_trigger = tmp & (tmp ^ key_continue);
    key_continue = tmp;
	 //HF_Debug(DEBUG_WARN,"key timeout,tmp=%d,,key_continue=%d,,key_time=%d\n",tmp,key_continue,key_time);	

	  if(!hfgpio_fpin_is_high( KEY )){
    
        //any button pressed
        key_time++;
			  //if(key_time%5==0)u_printf("button long pressed:%d",key_time);
        if ( key_time < 10 ){
          key_last = key_continue;
					//u_printf("button short pressed:%d,key_last=%d",key_time,key_last);
				}
        else
        {
            if ( key_time == 50 )
            {
              key_long_press( );
							printtime();//打印时间
							u_printf("hfuflash_size:%d\n",hfuflash_size());
            }
            else if ( key_time == 100 )
            {
                //key_long_10s_press( );
								
								u_printf("key time is 100");

            }
            else if ( key_time ==180 )
            {
							
								user_function_cmd_received(1,"{\"cmd\":\"device report\"}");
                user_led_set( 1 );
							  u_printf("key time is 180");
            }
            else if ( key_time == 203 )
            {
                user_led_set( 0 );	
							//u_printf("hfsmtlk_start > >>");
							//user_function_cmd_received(1,"{\"cmd\":\"device report\"}");
							  //hfsmtlk_start();
                //key_time = 201;
            }
        }
				if(key_time>204){
					key_time=0;
				}

    
		
	}
}




void USER_FUNC key_rising_irq_handler ( uint32_t arg1,uint32_t arg2 ){
    //u_printf("user_key_timer start success,key_time=%d\n",key_time);	
	  if(key_time<2){
		}
    else if(key_time<20){
			if(hfgpio_fpin_is_high( KEY )){
				HF_Debug(DEBUG_WARN,"short press\n");	
				key_short_press();
				//hfconfig_file_data_read(20,config_data,100);

			}
		}	
		else if(key_time<80){
			HF_Debug(DEBUG_WARN,"4s press\n");	
			//u_printf("time=%s", hfsys_get_time() );
		}
		else if(key_time<150){
			HF_Debug(DEBUG_WARN,"5s press\n");	
		}
		else if (key_time<204){
			HF_Debug(DEBUG_WARN,"10s press\n");	
		}

		key_time=0;
}

USER_FUNC void keyscan_thread_func(void * arg){
	while(1){
		
		if(hfsys_get_time()%30000==0){
			time_mm=hfntp_get_time(ntpserver,123,500);
			u_printf("get ntp time:%d",time_mm);
			printtime(time_mm);
		}

	}
	u_printf("keyscan_thread_func exit..");
}
 
void USER_FUNC key_init( )
{

	if((user_key_timer = hftimer_create("TIMER1",50,1,TIMER1_ID,key_timeout_handler,0))==NULL)
  {
    u_printf("create timer 1 fail\n");
  }
	if(hfthread_mutext_new(&keythread_lock)!=0)
	{
		HF_Debug(DEBUG_LEVEL,"create mutex fail\n");
		return;
	}
	///hfthread_create(keyscan_thread_func,"keycontrol",256,(void*)1,1,NULL,NULL);    //线程初始化


	if(hfgpio_configure_fpin_interrupt(KEY,HFPIO_IT_EDGE,key_rising_irq_handler,1)!=HF_SUCCESS)
	{
		u_printf("configure HFGPIO_F_USER_RELOAD fail\n");
    HF_Debug(DEBUG_WARN,"configure KEY fail\n");		
		return;
	}else
	 HF_Debug(DEBUG_WARN,"configure KEY success\n");		
	
	hftimer_start(user_key_timer);
	appRestoreDefault();
	
}

void USER_FUNC appRestoreDefault(  void  )
{
    int i, j;

    //u_printf("mqtt-port=%d",u_config.mqtt_port);
    u_config.mqtt_ip[0] = 0;
    u_config.mqtt_port = 0;
    u_config.mqtt_user[0] = 0;
    u_config.mqtt_password[0] = 0;
    u_config.version = USER_CONFIG_VERSION;
    for ( i = 0; i < PLUG_NUM; i++ )
    {
        u_config.plug[i].on = 0;

        //???? ??1-6
			  u_config.plug[i].name[0] = 0xB2;
        u_config.plug[i].name[1] = 0xE5;
        u_config.plug[i].name[2] = 0xBF;
        u_config.plug[i].name[3] = 0xDA;
       // u_config.plug[i].name[4] = 0x8f;
       // u_config.plug[i].name[5] = 0xa3;
        u_config.plug[i].name[4] = i + '1';
        u_config.plug[i].name[5] = 0;

       

      
        for ( j = 0; j < PLUG_TIME_TASK_NUM; j++ )
        {
            u_config.plug[i].task[j].hour = 0;
            u_config.plug[i].task[j].minute = 0;
            u_config.plug[i].task[j].repeat = 0x00;
            u_config.plug[i].task[j].on = 0;
            u_config.plug[i].task[j].action = 1;
        }
    }
//    mico_system_context_update( sys_config );
    user_defaultconfig=u_config;
		HF_Debug(DEBUG_WARN,"app config success...\n");	
}

void printTask(){
	 int i, j;
	 for ( i = 0; i < 1; i++ )
    {

			u_printf("u_config.plug[i].name:%s\n",u_config.plug[i].name);
        for ( j = 0; j < 3; j++ )
        {
					u_printf("u_config.plug[%d].task[%d].hour: %d\n",i,j,u_config.plug[i].task[j].hour);
					u_printf("u_config.plug[%d].task[%d].minute: %d\n",i,j,u_config.plug[i].task[j].minute);
					u_printf("u_config.plug[%d].task[%d].repeat: %d\n",i,j,u_config.plug[i].task[j].repeat);
					u_printf("u_config.plug[%d].task[%d].on: %d\n",i,j,u_config.plug[i].task[j].on);
					u_printf("u_config.plug[%d].task[%d].action: %d\n",i,j,u_config.plug[i].task[j].action);

        }
    }
	
	
}