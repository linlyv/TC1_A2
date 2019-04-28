#include "user_flash.h"

#include "cJSON/cJSON.h"
#include <hfnet.h>
#include <string.h>
#include "user_gpio.h"

static int USER_FUNC upadateConfigData(){
    u_printf("[upadateConfigData]....\n");
		cJSON *json_config_root = cJSON_CreateObject();
		if(json_config_root == NULL)
		{
			u_printf("create json faile\r\n");
			return 0;
		}
		
		char mac[13];
		hfnet_get_mac_address(mac);
		cJSON_AddNumberToObject(json_config_root, "CID", 10000);
		cJSON_AddStringToObject(json_config_root, "MAC", mac);

		cJSON *jsonPL = cJSON_CreateObject();
		cJSON_AddStringToObject(jsonPL, "msg", "login in");

		cJSON_AddItemToObject(json_config_root, "PL", jsonPL);
		char *out = cJSON_Print(json_config_root);

		//hfnet_socketa_send(out, strlen(out), 0);

		hfmem_free(out);
		cJSON_Delete(json_config_root);
		return 0;
	}

	
	
USER_FUNC void test_uflash_start(void)
{
    uint32_t i;
    uint32_t value;
    int pages;
    
    hfuflash_erase_page(0,1);
    for(i=0;i<HFFLASH_PAGE_SIZE;)
    {
        if(hfuflash_write(i,(char*)&i,sizeof(i))<sizeof(i))
        {
            u_printf("uflash eof\n");
            break;
        }
        i+=sizeof(i);
    }
    
    for(i=0;i<HFFLASH_PAGE_SIZE;)
    {
        if(hfuflash_read(i,(char*)&value,4)<4)
        {
            u_printf("uflash eof\n");
            break;
        }
        u_printf("%d\n",value);
        i+=4;
    }
    pages = (HFUFLASH_SIZE+HFUFLASH1_SIZE)/HFFLASH_PAGE_SIZE;
    for(i=0;i<pages;i++)
    {
        u_printf("erase test %d\n",i);
        msleep(1000);
        if(hfuflash_erase_page(i*HFFLASH_PAGE_SIZE,pages-i)!=HF_SUCCESS)
        {
            u_printf("test erase fail!\n");
            return ;
        }
    }
    
    for(i=0;i<HFUFLASH_SIZE+HFUFLASH1_SIZE;)
    {
        if(hfuflash_write(i,(char*)&i,sizeof(i))<sizeof(i))
        {
            u_printf("uflash eof\n");
            break;
        }
        if(hfuflash_read(i,(char*)&value,4)<4)
        {
            u_printf("uflash eof\n");
            break;
        }
        if(value!=i)
        {
            u_printf("test fail %d %d\n",i,value);
        }
        i+=sizeof(i);
    }
    
}

USER_FUNC int test_flash_one_cycle(uint32_t addr)
{
	static uint8_t test_data[4096];
	int i;
	
	flash_erase_page(addr,1);
	flash_read(addr,test_data,4096,0);
	for(i=0;i<4096;i++)
	{
		if(test_data[i]!=0xFF)
			return 1;
	}

	memset(test_data,0x55,4096);
	flash_write(addr,test_data,4096,0);
	flash_read(addr,test_data,4096,0);
	for(i=0;i<4096;i++)
	{
		if(test_data[i]!=0x55)
			return 2;
	}

	memset(test_data,0xAA,4096);
	flash_erase_page(addr,1);
	flash_write(addr,test_data,4096,0);
	flash_read(addr,test_data,4096,0);
	for(i=0;i<4096;i++)
	{
		if(test_data[i]!=0xAA)
			return 3;
	}

	return 0;
}    

USER_FUNC void test_flash_start(void)
{
    int rc=0;
    int rc1,rc2=0;
    int i,fail_times;

    
    //hfsys_disable_all_soft_watchdogs();
    hfthread_disable_softwatchdog(NULL);// LLJ CHANGE 
    hfthread_suspend_all();
    fail_times=0;

    u_printf("\r\n****************************************************\r\n");
    u_printf("  TIMES       OK          FAIL  \r\n\r\n");

    for(i=0;i<10000;i++)
    {
        hfgpio_fset_out_high(LED);
        rc1=test_flash_one_cycle(USERPAGE);
        rc2=test_flash_one_cycle(USERPAGE+4096);
        rc = rc1|(rc2<<4);
        if(rc!=0)
        {
            fail_times++;
        }
        u_printf("\r    %05d        %05d        %05d",i,i-fail_times,fail_times);   
        hfgpio_fset_out_low(HFGPIO_F_NLINK);
    }
    hfthread_resume_all();
    return;
}
