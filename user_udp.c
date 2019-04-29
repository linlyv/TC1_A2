
#include "appmain.h"
#include "user_function.h"
#include <hsf.h>
#include <hfnet.h>
#include <string.h>

#define LOCAL_UDP_PORT 10182
#define REMOTE_UDP_PORT 10181

#define MAX_UDP_DATA_SIZE          (1024)
#define MAX_UDP_SEND_QUEUE_SIZE    (5)
//mico_queue_t udp_msg_send_queue = NULL;
static HFMSGQ_HANDLE udp_msgq=NULL;


typedef struct
{
    uint32_t datalen;
    uint8_t data[MAX_UDP_DATA_SIZE];
} udp_send_msg_t, *p_udp_send_msg_t;

static int  udp_msg_send( int socket, const unsigned char* msg, uint32_t msg_len );
void udp_thread( void *arg );

USER_FUNC void user_udp_init( void )
{
   	hfthread_create(udp_thread,"udpcontrol",1024,(void*)1,1,NULL,NULL);  //∆Ù∂Øudpœﬂ≥Ã
}

/*create udp socket*/
void USER_FUNC udp_thread( void *arg )
{
		struct timeval timeout;
    struct sockaddr_in addr;
    fd_set readfds;   //rset
		uint8_t err,ret;
    socklen_t addrLen = sizeof(addr);
    int udp_fd=-1 , len,maxfd,tmp;
    p_udp_send_msg_t p_send_msg = NULL;
    int msg_send_event_fd=-1 ;
    char ip_address[16];
    char buf[512];

    /*Establish a UDP port to receive any data sent to this port*/
		memset((char*)&addr,0,sizeof(addr));
    udp_fd = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    if(udp_fd<0)
		{
			u_printf("create udp socket fail\n");
		}
		
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons( LOCAL_UDP_PORT );
    err = bind( udp_fd, (struct sockaddr *) &addr, sizeof(addr) );
		tmp=60;
    setsockopt(udp_fd, SOL_SOCKET,SO_BROADCAST,&tmp,sizeof(tmp));
    //require_noerr( err, exit );
     maxfd=udp_fd;
		 
    u_printf("[udp_thread]Open local UDP port %d\n", addr.sin_port);
		void *msg;
    while ( 1 )
    {
			  /*maxfd = udp_fd;
				if(maxfd<msg_send_event_fd)
					maxfd=msg_send_event_fd;*/
				timeout.tv_sec= 0.2;
				timeout.tv_usec= 0;

				
        FD_ZERO( &readfds );
				if(msg_send_event_fd>=0)
					FD_SET( msg_send_event_fd, &readfds );
        FD_SET( udp_fd, &readfds );
        ret=select( udp_fd + 1, &readfds, NULL, NULL, NULL );
				
				
        //u_printf("check port %d", LOCAL_UDP_PORT);
        /*Read data from udp and send data back */
        if ( FD_ISSET( udp_fd, &readfds ) )
        {
            len = recvfrom( udp_fd, buf, 1024, 0, (struct sockaddr *) &addr, &addrLen );

            strcpy( ip_address, inet_ntoa( addr.sin_addr ) );
            if(len<1024) buf[len]=0;
            u_printf( "udp recv from %s:%d, len:%d \n", ip_address,addr.sin_port, len );
            user_function_cmd_received(1,buf);
           sendto( udp_fd, buf, len, 0, (struct sockaddr *) &addr, sizeof(struct sockaddr_in) );
        }

        /* recv msg from user worker thread to be sent to server */
        //if ( FD_ISSET( msg_send_event_fd, &readfds ) ){
				
						if(hfmsgq_recv(udp_msgq,&msg,10,0)==HF_SUCCESS)
						{
							u_printf("recv msg %p\n",msg);
							p_send_msg=msg;//get data from udp_msgq 
							
							udp_msg_send( udp_fd, p_send_msg->data, p_send_msg->datalen );
							//u_printf("udp_fd=%d\n",udp_fd);
							 hfmem_free( p_send_msg );
							p_send_msg = NULL;
							
						}else{
							//u_printf("recv msg fail.. %p\n",msg);
							continue;
						}
				//}
				if(ret<=0){
					//u_printf("ret=%d,udp_fd=%d\n",ret,udp_fd);
					continue;
				}
          
        
    }

    //exit:
}



// send msg to udp
static int  udp_msg_send( int socket, const unsigned char* msg, uint32_t msg_len )
{

    int ret = 0;

    struct sockaddr_in addr;
		//memset((char*)&addr,0,sizeof(addr));
	  //sockfd=socket(AF_INET,SOCK_DGRAM,0);
	  
		
     addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons( LOCAL_UDP_PORT );
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_BROADCAST;//
	
    addr.sin_port = htons( REMOTE_UDP_PORT );
    /*the receiver should bind at port=20000*/
    ret=sendto( socket, msg, msg_len, 0, (struct sockaddr *) &addr, sizeof(addr) );
		u_printf("[udp_msg_send]  len=%d\n",ret);
	
	  //u_printf("remote port:%d\n",REMOTE_UDP_PORT);
		//u_printf("sent:%s , socket fd:%d,addr:%s:%d\n",msg,socket,inet_ntoa( addr.sin_addr ),addr.sin_port);
    return 1;
}

/* Application collect data and seng them to udp send queue */
void user_udp_send( char *arg )
{
    
    p_udp_send_msg_t p_send_msg = NULL;

    /* Push the latest data into send queue*/
    p_send_msg = calloc( 1, sizeof(udp_send_msg_t) );
   

    p_send_msg->datalen = strlen( arg );
    memcpy( p_send_msg->data, arg, p_send_msg->datalen );
    u_printf( "[user_udp_send]p_send_msg->data=%s\n", p_send_msg->data );
	  udp_msgq=hfmsgq_create(2,256);
		if(udp_msgq==NULL)
		{
			u_printf("create msg_queue fail\n");
		}
		if(hfmsgq_send(udp_msgq,&p_send_msg,10,0)==HF_SUCCESS)
		{
			u_printf("send msg to queue%p\n",p_send_msg);
		}else{
			u_printf("send msg to queue  fail!  address :%p\n",p_send_msg);
		}
    


}
