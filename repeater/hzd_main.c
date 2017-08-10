#include "485communication.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/msg.h>
#include <pthread.h>
#include <sys/time.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <signal.h>
#include <stdlib.h>  
#include <fcntl.h>  
#include <limits.h>  
#include <sys/stat.h>   
#include <string.h>  


#define BUFFER_SIZE 1500
#define MSG_MASTER485 0x01
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

void sys_communication_init(void);
void sys_msg_init(void);
void sys_start(void);
void sigroutine(int dunno);
void network_input(int read_num,char *data);

int main(void)				    
{
	//系统配置，初始化
 	sys_communication_init();

	//消息队列初始化
	sys_msg_init();
	
	//进入系统
	sys_start();
	//master485_test();
	signal(SIGHUP,sigroutine);
	signal(SIGINT,sigroutine);
	signal(SIGQUIT,sigroutine);
	while(1)sleep(100);
	return 0;
}


void sigroutine(int dunno)
{
	switch(dunno){
		case 1:printf("终端挂起或者控制进程终止\r\n");break;
		case 2:printf("键盘中断\r\n");break;
		case 3:printf("键盘的退出键被按下\r\n");break;
		default :;
	}
	return;
}

extern char CatCtrlDate,CatTcpState,CatMasterDate;
int ctr_fd = -1;
int rtc_fd = -1; 
int read_num = 0;
char buffer[1500]; 
void repeater_console_pthread(void)
{
	const char *r_to_c = "/root/pan/repeater/repeater_to_console"; 
	const char *c_to_r = "/root/pan/repeater/console_to_repeater";
    int res = 0;  
    const int open_mode = O_RDWR;  
    int bytes_sent = 0;  
	char *rev = NULL; 
	pid_t pid;
	pid = getpid();	

    if(access(r_to_c, F_OK) == -1)  
    {  
        //管道文件不存在  
        //创建命名管道  
        res = mkfifo(r_to_c, 0777);  
        if(res != 0)  
        {  
            fprintf(stderr, "Could not create fifo %s\n", r_to_c);  
            exit(EXIT_FAILURE);  
        }  
    }  

	 if(access(c_to_r, F_OK) == -1)  
    {  
        //管道文件不存在  
        //创建命名管道  
        res = mkfifo(c_to_r, 0777);  
        if(res != 0)  
        {  
            fprintf(stderr, "Could not create fifo %s\n", c_to_r);  
            exit(EXIT_FAILURE);  
        }  
    }  
   
    //打开FIFO文件  
    ctr_fd = open(c_to_r, open_mode); 
	//打开FIFO文件  
    rtc_fd = open(r_to_c, open_mode); 
 
  	while(1)
	{
		if(ctr_fd != -1)  
		{  
			memset(buffer,0,sizeof(char)*1500);
		    read_num = read(ctr_fd, buffer, 1500);  
		    if(res == -1){  
		        fprintf(stderr, "Write error on pipe\n");  
		        exit(EXIT_FAILURE);  
		    }  

			printf("cmd is :%s\r\n",buffer);

			if( !strcmp(buffer,"help") ){
			printf("id		----	打印本进程ID\r\n");
			printf("exit	----	关闭中继器\r\n");
			printf("ctrldate---- 捕捉触摸屏数据\r\n");
			printf("tcpstate---- 捕捉tcp连接状态\r\n");
			printf("回车退出当前命令\r\n");
			}
		
			if( !strcmp(buffer,"id") )
				printf("pid = %d\r\n",pid);

			if( !strcmp(buffer,"exit") ){

				printf("关闭中继器\r\n");
	
			}

			rev = NULL;
			rev = strstr(buffer,"ctrldate");
		 	if(rev != NULL){
				printf("开始捕捉触摸屏数据\r\n");
				CatCtrlDate = 1;
		 	}
		 	else{
				CatCtrlDate = 0;
				//printf("停止捕捉触摸屏数据\r\n");
			}

			rev = NULL;
			rev = strstr(buffer,"master485date");
		 	if(rev != NULL){
				printf("开始捕捉电表、热量表等数据\r\n");
				CatMasterDate = 1;
		 	}
		 	else{
				CatMasterDate = 0;
			}

			rev = NULL;
			rev = strstr(buffer,"tcpstate");
		 	if(rev != NULL){
				CatTcpState = 1;
				printf("捕捉tcp连接状态\r\n");
		 	}
			else{
				//printf("停止tcp连接状态\r\n");
				CatTcpState = 0;			
			}
	 
			network_input(read_num,buffer);

		}
		sleep(1);    
	}
	close(ctr_fd);
	close(rtc_fd);
}


pthread_t identify_timer_thread;
pthread_t master485_timer_thread;
pthread_t heart_timer_thread;

pthread_t console_thread;
pthread_t master_thread;
pthread_t slave_thread;
pthread_t metering_thread;
pthread_t task_slave_thread;
pthread_t socket_output_thread;
pthread_t socket_input_thread;
pthread_t tcp_identify_thread;
pthread_t tcp_heart_beat_thread;
void sys_start(void)
{
	int err = 0;
	err = pthread_create(&master_thread,NULL,(void*)master_input_pthread,NULL);

	err = pthread_create(&master485_timer_thread,NULL,(void*)task_master485_timer_thread,NULL);

	err = pthread_create(&identify_timer_thread,NULL,(void*)task_identify_timer_thread,NULL);

	err = pthread_create(&tcp_identify_thread,NULL,(void*)task_tcp_identify_thread,NULL);

	err = pthread_create(&heart_timer_thread,NULL,(void*)task_heart_timer_thread,NULL);

	err = pthread_create(&tcp_heart_beat_thread,NULL,(void*)task_tcp_heart_beat_thread,NULL);

	err = pthread_create(&console_thread,NULL,(void*)repeater_console_pthread,NULL);

	err = pthread_create(&metering_thread,NULL,(void*)repeater_metering_pthread,NULL);

	err = pthread_create(&task_slave_thread,NULL,(void*)task_slave485_pthread,NULL);

	err = pthread_create(&slave_thread,NULL,(void*)slave_input_pthread,NULL);

	err = pthread_create(&socket_output_thread,NULL,(void*)task_socket_output_pthread,NULL);

	err = pthread_create(&socket_input_thread,NULL,(void*)task_socket_input_pthread,NULL);

}


int master485_fd;
int slave485_fd;
void sys_communication_init(void)
{
	master485_fd = master485_init();

	slave485_fd = slave485_init();
	//sociket初始化，可能用到
	//sociket_init();	
}

//消息队列初始化
char msg_box = -1;
void sys_msg_init(void)
{
	msg_box = msgget((key_t)MSG_KEY,0666|IPC_CREAT);
	if( -1 == msg_box )
		printf("msg_box create fail!\n");
	else
		printf("msg_box create success!\n");

}

char IdentifyTimerState = 0x0;
extern struct tcp_msgbuf identify_msg;
void task_identify_timer_thread(void *p_arg)
{
	while(1)
	{
		sleep(1);
		while(IdentifyTimerState)
		{
			sleep(25);
			if(IdentifyTimerState)
			{
				identify_msg.mtype = MSG_SEQUENCE;
				identify_msg.packet_type = TIMEOUT;
				msgsnd(msg_box,&identify_msg,1,0);
			}	
			else
				continue;
		}
			
	}
}

char Master485TimerState = 0x0;
extern struct msgbuf m485_msg;
void task_master485_timer_thread(void *p_arg)
{
	while(1)
	{
		sleep(1);
		while(Master485TimerState)
		{
			sleep(1);
			if(Master485TimerState)
			{
				m485_msg.mtype = MSG_MASTER485;
				m485_msg.packet_type = TIMEOUT;
				msgsnd(msg_box,&m485_msg,1,0);
			}	
			else
				continue;
		}
			
	}
}

char HeartTimerState = 0x0;
extern struct tcp_msgbuf heart_beat_msg;
void task_heart_timer_thread(void *p_arg)
{
	while(1)
	{
		sleep(1);
		while(HeartTimerState)
		{
			sleep(5);
			if(HeartTimerState)
			{
				heart_beat_msg.mtype = MSG_HEART_RESULT;
				heart_beat_msg.packet_type = TIMEOUT;
				msgsnd(msg_box,&m485_msg,1,0);
			}	
			else
				continue;
		}
			
	}
}

void network_input(int read_num,char *data)
{
	int res = 0,write_num = 0;
	pid_t idp;
	
	res = 0;
	write_num = 0;
	char *rev = NULL;
	rev = strstr(data,"RepeaterIP");
 	if(rev != NULL){
		memcpy(ccs.w5100_s_ip,rev+11,read_num - 10);
		ccs.w5100_s_ip[15] = 0x0;
 	}
	res = 0;
	write_num = 0;
	rev = NULL;
	rev = strstr(data,"DestinationIP");
 	if(rev != NULL){
		memcpy(ccs.d_ip,rev+14,read_num - 13);
		ccs.d_ip[15] = 0x0;
 	}
	res = 0;
	write_num = 0;
	rev = NULL;
	rev = strstr(data,"Port");
 	if(rev != NULL){
		ccs.port = (*(rev+5)-0x30)*1000 + (*(rev+6)-0x30)*100 + (*(rev+7)-0x30)*10 + (*(rev+8)-0x30); 
 	}
	res = 0;
	write_num = 0;
	rev = NULL;
	rev = strstr(data,"GatewayIP");
 	if(rev != NULL){
		memcpy(ccs.w5100_gateway,rev+10,read_num - 9);
		ccs.w5100_gateway[15] = 0x0;
 	}
	res = 0;
	write_num = 0;
	rev = NULL;
	rev = strstr(data,"NetMask");
 	if(rev != NULL){
		memcpy(ccs.w5100_mask,rev+8,read_num - 7);
		ccs.w5100_mask[15] = 0x0;
 	}
	res = 0;
	write_num = 0;
	rev = NULL;
	rev = strstr(data,"RepeaterID");
 	if(rev != NULL){
		memcpy(ccs.use_this_id,rev+11,read_num - 10);
		ccs.use_this_id[11] = 0x0;
 	}

	res = 0;
	write_num = 0;
	rev = NULL;
	rev = strstr(data,"RepeaterShutdown");
 	if(rev != NULL){
		idp = getpid();
		exit(0);
 	}

}

