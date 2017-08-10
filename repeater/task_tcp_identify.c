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
#include <signal.h>

#define BUFFER_SIZE 1500
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

unsigned char SYS_TCP_IDENTIFY_PASST = 0x0;

unsigned char MD5_Key[4]="123";

const unsigned char md5_packet_header[] ="<?xml version=\"1.0\" encoding=\"utf-8\"?><root><common><id>";

const unsigned char md5_med[] ="</id><type>md5</type></common><id_validata operation=\"md5\"><md5>";

const unsigned char md5_packet_tail[] ="</md5></id_validata></root>"; 

const unsigned char identify_packet_header[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?><root><common><id>";

const unsigned char identify_packet_tail[] = "</id><type>request</type></common><id_validata operation=\"request\"></id_validata></root>";
//const unsigned char identify_packet_tail[] = "</id><type>sequence</type></common><id_validata operation=\"sequence\"></id_validata></root>";
unsigned char tcp_output_buf[BUFFER_SIZE] = {0x0};

extern pthread_mutex_t TCP_OUTPUT_BUF_LOCK;

extern int client_socket;
extern char IdentifyTimerState;
extern unsigned char tcpinput_buffer[BUFFER_SIZE];
extern char tcp_connect_state;
extern char msg_box;
struct tcp_msgbuf identify_msg;
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

void task_tcp_identify_thread(void *p_arg)
{
	unsigned char *sequeue = NULL;
	unsigned char md5r[33];
	while(1)
	{
//printf("\r\nidentify_thread tcp_connect_state =%d\r\n",tcp_connect_state);
		if(tcp_connect_state == 1)
		{
			sleep(5);

			pthread_mutex_lock(&TCP_OUTPUT_BUF_LOCK);		

			identify_msg.mtype = MSG_SOCKET;
			identify_msg.packet_type = MSG_OUTPUT;
			strcpy((char *)&tcp_output_buf[10],(char *)identify_packet_header);
			strcat((char *)&tcp_output_buf[10],(char *)ccs.use_this_id);
			strcat((char *)&tcp_output_buf[10],(char *)identify_packet_tail);
	//printf("\r\nidentify ready !\r\n");
			msgsnd(msg_box,&identify_msg,1,0);
			IdentifyTimerState = 1;
			//printf("identify_msg:%s !\r\n",&tcp_output_buf[10]);
//			signal(SIGALRM,handler);
			msgrcv(msg_box,&identify_msg,1,MSG_SEQUENCE,0);
			IdentifyTimerState = 0;
			if(identify_msg.packet_type == TIMEOUT)
			{
				printf("\r\nidentify timeout !\r\n");
				close(client_socket);
				continue;
			}		
			
			//获取sequence的起始地址
			sequeue = (unsigned char *)strstr((char *)&tcpinput_buffer[12],(char *)"</sequence>");
			if(sequeue == NULL)
			{
				printf("认证信息应答出错(1)\r\n");
				continue;
			}
printf("sequence received !\r\n");
			*sequeue     = '\0'; 
        	*(sequeue+1) = '\0';
			sequeue = (unsigned char *)strstr((char *)&tcpinput_buffer[12],(char *)"<sequence>");
			if(sequeue == NULL)
			{
				printf("认证信息应答出错(2)。\r\n");
				continue;	
			}

			sequeue +=10;
			//在序列号中追加密钥        
		    strcat((char *)sequeue,(char *)MD5_Key);
		            
		    //计算MD5
			MD5_CalculateMD5Value(sequeue,md5r);
//printf("MD5_CalculateMD5Value !\r\n");		            
			//组建立MD5应答包
			strcpy((char *)&tcp_output_buf[10],(char *)md5_packet_header);
			strcat((char *)&tcp_output_buf[10],(char *)ccs.use_this_id);
			strcat((char *)&tcp_output_buf[10],(char *)md5_med);
			strcat((char *)&tcp_output_buf[10],(char *)md5r);
			strcat((char *)&tcp_output_buf[10],(char *)md5_packet_tail);

			identify_msg.mtype = MSG_SOCKET;
			identify_msg.packet_type = MSG_OUTPUT;
			msgsnd(msg_box,&identify_msg,1,0);
			IdentifyTimerState = 1;
			//signal(SIGALRM,handler);
			//alarm(25);
			msgrcv(msg_box,&identify_msg,1,MSG_RESULT,0);
			IdentifyTimerState = 0;
			//alarm(0);
			if(identify_msg.packet_type == MSG_RESULT)
			{
				printf("\r\nmd5 timeout !\r\n");
				continue;
			}
			//查找结果
printf("\r\n解密数据:%s\n",&tcpinput_buffer[12]);
			sequeue = (unsigned char *)strstr((char *)&tcpinput_buffer[12],(char *)"<result>");
			if(sequeue == NULL)
			{
				printf("认证信息应答出错(3)。\r\n");
				continue;	
			}
			//调整结果指针
			sequeue += 8;

		   	//查看是否成功登陆
			if(strncmp((const char *)(sequeue),"pass",4) == 0){
				SYS_TCP_IDENTIFY_PASST = PASS;
				printf("\r\nidentify pass\r\n");
			}
			while(SYS_TCP_IDENTIFY_PASST){
			sleep(5);
			}
			//msgrcv(msg_box,&identify_msg,1,MSG_RESULT,0);
			//printf("result received !\r\n");
		}
		sleep(1);
	}
}

