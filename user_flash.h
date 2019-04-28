#ifndef __USER_FLASH_H_
#define __USER_FLASH_H_


#include <hsf.h>


USER_FUNC void test_flash_start(void);
USER_FUNC void test_uflash_start(void);
USER_FUNC int test_flash_one_cycle(uint32_t addr);

static int USER_FUNC upadateConfigData();

#endif
