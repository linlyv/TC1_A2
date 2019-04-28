
#ifndef __USER_RTC_H_
#define __USER_RTC_H_





void USER_FUNC rtctime_init(void);
void USER_FUNC key_timeout_handler( void* arg );
	
USER_FUNC void rtc_thread_func(void * arg);
int printtime();

int update_rtctime();
extern struct tm * user_time_t;  


#endif
