
#ifndef __USER_KEY_H_
#define __USER_KEY_H_

#include <hsf.h>

#define SW1	(HFGPIO_F_USER_DEFINE)
#define SW2	(HFGPIO_F_USER_DEFINE+1)
#define SW3	(HFGPIO_F_USER_DEFINE+2)
#define SW4	(HFGPIO_F_USER_DEFINE+3)
#define SW5	(HFGPIO_F_USER_DEFINE+4)
#define SW6	(HFGPIO_F_USER_DEFINE+5)
#define LED	(HFGPIO_F_USER_DEFINE+6)
#define KEY	(HFGPIO_F_USER_DEFINE+7)
#define POWER	(HFGPIO_F_USER_DEFINE+8)


void USER_FUNC user_led_set(char x);
void USER_FUNC key_init(void);
void USER_FUNC user_relay_set(uint8_t x,uint8_t y );
void USER_FUNC user_relay_set_all( uint8_t y );
bool USER_FUNC relay_out( void );
void USER_FUNC key_long_press( void );
void USER_FUNC key_long_10s_press( void );
//void USER_FUNC key_short_press( void );
 void USER_FUNC key_timeout_handler( void* arg );
 void USER_FUNC key_falling_irq_handler( uint32_t arg1,uint32_t arg2 );
 void USER_FUNC key_Rising_irq_handler ( uint32_t arg1,uint32_t arg2 );
 void appRestoreDefault(  void  );
 
 void printTask();
#endif
