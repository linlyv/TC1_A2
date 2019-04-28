
#include "hsf.h"
#include "appmain.h"
#include "user_udp.h"
//#include "user_mqtt_client.h"
#include "user_function.h"
#include "user_gpio.h"
#include "user_power.h"
//mico_timer_t power_timer;
static hftimer_handle_t power_timer=NULL;

#define TIMER3_ID		(3)   //¶¨Ê±Æ÷id

static uint32_t clock_count_last = 0;
static uint32_t clock_count = 0;
static uint32_t timer_count = 0;
static uint32_t timer_irq_count = 0;

static void power_timer_handler( void* arg )
{

//    uint8_t pin_input = MicoGpioInputGet( POWER );
    uint32_t timer = 0;

    if ( timer_irq_count > 1 )
    {
        timer = (clock_count - clock_count_last);

				u_printf("power_irq_handler:%09u %u %u",timer,timer_irq_count,timer_count);
        if ( timer_count > 3 )
        {
            timer /= 1000;
            timer += 4294967; //0xffffffff/1000;
        } else if ( clock_count < clock_count_last )
        {
            timer += 0xffffffff;
            timer /= 1000;
        }else timer/=1000;
        power = 17100000 * (timer_irq_count - 1) / timer;
        timer_count = 0;
        timer_irq_count = 0;
    } else
    {
        timer_count++;
    }

//    if ( clock_count_last != timer_count )
//    {
////        os_log("power_irq_handler:%u-%u=%u",timer_count,clock_count_last,timer);
//        timer = (timer_count - clock_count_last);
//        if ( timer_count < clock_count_last ) timer += 0xffffffff;
//
//        timer = timer / 1000;
//        power = 15200000 / timer;
//        os_log("power_irq_handler:%u,%07u",power,timer);
//        clock_count_last = timer_count;
//    }
//    if(timer_count==0)  os_log("power_timer_handler Hight:%d",clock_count_last);
//    timer_count++;
//    clock_count_last=timer_count;
}

static void power_irq_handler( uint32_t arg1,uint32_t arg2 )
{
    //clock_count = mico_nanosecond_clock_value( );
		clock_count = hfsys_get_time( )*1000;
    if ( timer_irq_count == 0 ) clock_count_last = clock_count;
    timer_irq_count++;
}

void USER_FUNC user_power_init(  )
{
    u_printf("user_power_init\n");
		if((power_timer = hftimer_create("TIMER3",1000,1,TIMER3_ID,power_timer_handler,0))==NULL)
		{
			u_printf("create power timer 3 fail\n");
		}

    //mico_rtos_init_timer( &power_timer, 1000, power_timer_handler, NULL );

			if(hfgpio_configure_fpin_interrupt(POWER,HFPIO_IT_FALL_EDGE,power_irq_handler,1)!=HF_SUCCESS)
	{
		u_printf("configure POWER fail\n");
    HF_Debug(DEBUG_WARN,"configure powwer fail\n");		
		return;
	}else
	 HF_Debug(DEBUG_WARN,"configure power success\n");		
    //MicoGpioEnableIRQ( POWER, IRQ_TRIGGER_FALLING_EDGE, power_irq_handler, NULL );
}

