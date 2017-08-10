#include "485communication.h"
#include <stdio.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <sys/msg.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1500
#define MSG_MASTER485 0x01
#define MSG_SOCKET	0x03
#define MSG_SEQUENCE 0X10
#define MSG_RESULT 0X11
#define MSG_HEART_RESULT 0X12
#define MSG_SERVER_CTRL 0X13
#define MSG_TIMESTAMP_CAL 0X14
#define MSG_SYSTEM_CFG 0X15
#define MSG_INIT_VAL 0X16
#define MSG_HEATPUMP 0X17
#define MSG_OUTPUT 0X20
#define TIMEOUT	0x30
#define PASS 0x01
#define NOPASS 0x00

const unsigned char header_packet_header[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?><root><common><id>";

const unsigned char header_packet_tail[] = "</id><type>notify</type></common><heart_beat operation=\"notify\"></heart_beat></root>"; 

struct communication_config_struct{
	unsigned int flag;
	unsigned int port;
	unsigned char d_ip[16];
	unsigned char w5100_gateway[16];
	unsigned char w5100_s_ip[16];
	unsigned char w5100_mask[16];
	unsigned char w5100_phy_addr[6];
	unsigned char use_this_id[12];
}; 
extern struct communication_config_struct ccs;
extern unsigned char tcp_output_buf[BUFFER_SIZE];
extern unsigned char SYS_TCP_IDENTIFY_PASST;
extern char HeartTimerState;

extern char msg_box;
struct tcp_msgbuf heart_beat_msg;

pthread_mutex_t TCP_OUTPUT_BUF_LOCK;//tcp_output_buf[]变量程互斥锁

void task_tcp_heart_beat_thread(void *p_arg)
{

	pthread_mutex_init(&TCP_OUTPUT_BUF_LOCK, NULL);

	while(1)
	{
		while(SYS_TCP_IDENTIFY_PASST)
		{
			pthread_mutex_lock(&TCP_OUTPUT_BUF_LOCK);
			//组建心跳包
			strcpy((char *)&tcp_output_buf[10],(const char *)header_packet_header);
			strcat((char *)&tcp_output_buf[10],(const char *)ccs.use_this_id);
			strcat((char *)&tcp_output_buf[10],(const char *)header_packet_tail);
		
			heart_beat_msg.mtype = MSG_SOCKET;
			heart_beat_msg.packet_type = MSG_OUTPUT;
			msgsnd(msg_box,&heart_beat_msg,1,0);
			HeartTimerState = 0x01;
			printf("heart_beat_msg:%s !\r\n",&tcp_output_buf[10]);
			
			msgrcv(msg_box,&heart_beat_msg,1,MSG_HEART_RESULT,0);
			if(heart_beat_msg.packet_type == TIMEOUT)
			{
				printf("\r\nheart_beat timeout !\r\n");
				continue;
			}
			HeartTimerState = 0x00;
			printf("heart_result received !\r\n");
			sleep(25);
		}
		sleep(1);
	}
}

