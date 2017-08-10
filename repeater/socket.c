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
extern unsigned char SYS_TCP_IDENTIFY_PASST;

char send_buffer[BUFFER_SIZE] = {"hzd repeater connected !"};

int recbyte,client_socket;
char tcp_connect_state = 0x0,CatTcpState = 0;

char *TcpFailed = "Can Not Connect To Service!Please check destination IP & Port!";

unsigned char tcp_header[] = {0x68,0x68,0x16,0x16,0x00};
char tcp_tail[] = {0x55,0xaa,0x55,0xaa,0x00};
unsigned short tcp_len = 0x0,rev_crc_v,cal_crc_v;
unsigned char *type = NULL;

unsigned char tcpinput_buffer[BUFFER_SIZE] = {0x0};

extern  pthread_mutex_t TCP_OUTPUT_BUF_LOCK;
extern  pthread_mutex_t DATA_CAL_CTRL_LOCK;
extern  pthread_mutex_t TCP_DATA_LOCK;

extern unsigned char tcpDataCtrl[1024];
extern unsigned char tcpDataMetering[512];
extern unsigned char tcpDataHeatPump[512];
extern unsigned char tcp_data[BUFFER_SIZE];
extern unsigned char tcp_output_buf[BUFFER_SIZE];
extern int rtc_fd; 
extern char msg_box;
struct tcp_msgbuf tcp_msg;
struct tcp_msgbuf distribute_msg;

/*
	函数名：  tcp_data_aes_crc_header_tail
	功能：数据报的加密校验，添加头尾。
	参数：传入参数为数据报指针，函数会在次指针前添加4字节的头，4字节的有效数据长，在有效数据后面，
	      添加2字节的CRC校验，4字节的尾。
*/
void tcp_data_aes_crc_header_tail(unsigned char* tcp_data)
{
		unsigned short CRCValue;
		int i = 0;
//printf("going to AES_Padding_PKCS7 !\n");
//printf("CatTcpState = %d\r\n",CatTcpState);
if(CatTcpState){
printf("\r\n发送数据:0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ",tcp_data[0],tcp_data[1],tcp_data[2],tcp_data[3],tcp_data[4],tcp_data[5]);
printf("%s\n",&tcp_data[10]);

write(rtc_fd, &tcp_data[10], strlen(&tcp_data[10]));

}
//for(i=0;i<174;i++){
//printf("%02x ",tcp_data[i]);
//}printf("\r\n");
		//把数据报文以16位对齐，准备校验
		AES_Padding_PKCS7(&tcp_data[10]);

//for(i=0;i<174;i++){
//printf("%02x ",tcp_data[i]);
//}printf("\r\n");
		//计算补齐后的有效数据

		*(unsigned short *)tcp_data = strlen((char *)&tcp_data[10]);
//printf("going to AES_CipherBuffer !\n");
		//加密字符串，加密后在任何位置都可能出现0x00
		AES_CipherBuffer(&tcp_data[10]);

//for(i=0;i<174;i++){
//printf("%02x ",tcp_data[i]);
//}printf("\r\n");
		//填写密文报头
		tcp_data[2] = 0x68;
		tcp_data[3] = 0x68;
		tcp_data[4] = 0x16;
		tcp_data[5] = 0x16;

		//有效数据长度
		tcp_data[6] = ((*(unsigned short *)tcp_data)/1000)+0x30;
		tcp_data[7] = (((*(unsigned short *)tcp_data)/100)%10)+0x30;
		tcp_data[8] = (((*(unsigned short *)tcp_data)%100)/10)+0x30;
		tcp_data[9] = ((*(unsigned short *)tcp_data)%10)+0x30;
//printf("going to CRC16_CCITT !\n");
		//进行CRC校验
		//(*(uint16_t *)tcp_data) 为有效数据长度，校验时加上包头部分+8
		CRCValue = CRC16_CCITT(&tcp_data[2],(*(unsigned short *)tcp_data)+8);
//printf("CRCValue = %x\n",CRCValue);
//printf("tcp_data = ",*(unsigned short *)tcp_data);
		//向数据报文写入CRC效验码
		tcp_data[(*(unsigned short *)tcp_data)+10]   = CRCValue>>8;
		tcp_data[(*(unsigned short *)tcp_data)+11]   = CRCValue;
//printf("tcp_data[10] = %02x\n",tcp_data[(*(unsigned short *)tcp_data)+10]);
//printf("tcp_data[11] = %02x\n",tcp_data[(*(unsigned short *)tcp_data)+11]);

		//填写数据报尾
		tcp_data[(*(unsigned short *)tcp_data)+12]	= 0x55; 
		tcp_data[(*(unsigned short *)tcp_data)+13] 	= 0xaa;
		tcp_data[(*(unsigned short *)tcp_data)+14] 	= 0x55;
		tcp_data[(*(unsigned short *)tcp_data)+15] 	= 0xaa;
		
		 //计算要发送数据的总长度
		 //包头(4)+有效数据长度记录（4）+有效数据长度+校验和（2）+报尾（4）
		 (*(unsigned short *)tcp_data) += 14;	
//for(i=0;i<176;i++){
//printf("tcp data AES after:0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ",tcp_data[0],tcp_data[1],tcp_data[2],tcp_data[3],tcp_data[4],tcp_data[5]);
//if(CatTcpState)
//printf("%s\n",&tcp_data[6]);
//printf("%02x ",tcp_data[i]);
//}
}

void task_socket_output_pthread(void)
{
	unsigned char AES_Key[17] = "0123456789abcdef";
	AES_KeyExpansion(AES_Key);	
	while(1)
	{
		tcp_msg.packet_type = 0x0;
		msgrcv(msg_box,&tcp_msg,1,MSG_SOCKET,0);
		//printf("going to 1 !\n");
if(CatTcpState)
	printf("\r\nsocket_output_pthread tcp_connect_state = %d\r\n",tcp_connect_state);
		if(tcp_connect_state)
		{//printf("going to 2 !\n");
			switch(tcp_msg.packet_type)
			{
				case MSG_OUTPUT:tcp_data_aes_crc_header_tail(tcp_output_buf);if(tcp_connect_state)send(client_socket,&tcp_output_buf[2],*(unsigned short *)tcp_output_buf,0);pthread_mutex_unlock(&TCP_OUTPUT_BUF_LOCK);/*printf("tcpmsg send out !\n");*/break;

				case single_tank_data:printf("\r\nsingle_tank_data %s\r\n",tcpDataCtrl);tcp_data_aes_crc_header_tail(tcpDataCtrl);if(tcp_connect_state)send(client_socket,&tcpDataCtrl[2],*(unsigned short *)tcpDataCtrl,0);pthread_mutex_unlock(&DATA_CAL_CTRL_LOCK);break;

				case single_tank_config:printf("\r\ndouble_tank_config %s\r\n",tcp_data);tcp_data_aes_crc_header_tail(tcp_data);if(tcp_connect_state)send(client_socket,&tcp_data[2],*(unsigned short *)tcp_data,0);pthread_mutex_unlock(&TCP_DATA_LOCK);break;

				case double_tank_data:printf("\r\ndouble_tank_data %s\r\n",tcpDataCtrl);tcp_data_aes_crc_header_tail(tcpDataCtrl);if(tcp_connect_state)send(client_socket,&tcpDataCtrl[2],*(unsigned short *)tcpDataCtrl,0);pthread_mutex_unlock(&DATA_CAL_CTRL_LOCK);break;

				case double_tank_config:printf("\r\ndouble_tank_config %s\r\n",tcp_data);tcp_data_aes_crc_header_tail(tcp_data);if(tcp_connect_state)send(client_socket,&tcp_data[2],*(unsigned short *)tcp_data,0);pthread_mutex_unlock(&TCP_DATA_LOCK);break;

				default :printf("tcpmsg error !\n");
			}
		}
		sleep(1);
	}
}

void task_socket_input_pthread(void)
{
	/* 接收服务器发过来的数据*/
	while(!tcp_connect_state)
	{
		//设置一个socket地址结构client_addr,代表客户机internet地址, 端口
	   	struct sockaddr_in client_addr;
		bzero(&client_addr,sizeof(client_addr)); //把一段内存区的内容全部设置为0
		client_addr.sin_family = AF_INET;    //internet协议族
		client_addr.sin_addr.s_addr = htons(INADDR_ANY);//INADDR_ANY表示自动获取本机地址
		client_addr.sin_port = htons(0);    //0表示让系统自动分配一个空闲端口
		//创建用于internet的流协议(TCP)socket,用client_socket代表客户机socket
		client_socket = socket(AF_INET,SOCK_STREAM,0);
		if( client_socket < 0)
		    printf("Create Socket Failed!\n");
		//把客户机的socket和客户机的socket地址结构联系起来
		if( bind(client_socket,(struct sockaddr*)&client_addr,sizeof(client_addr)))
		    printf("Client Bind Port Failed!\n"); 
	 
		//设置一个socket地址结构server_addr,代表服务器的internet地址, 端口
		struct sockaddr_in server_addr;
		bzero(&server_addr,sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		if(inet_aton((const char *)ccs.d_ip,&server_addr.sin_addr) == 0) //服务器的IP地址
		    printf("Server IP Address Error!\n");
		server_addr.sin_port = htons(ccs.port);
		socklen_t server_addr_length = sizeof(server_addr);
		//向服务器发起连接,连接成功后client_socket代表了客户机和服务器的一个socket连接
		if(connect(client_socket,(struct sockaddr*)&server_addr, server_addr_length) < 0)
		{
			if(CatTcpState)
				write(rtc_fd, TcpFailed, strlen(TcpFailed));
		    printf("Can Not Connect To Service!\n");
			tcp_connect_state = 0x0;
			close(client_socket);
			sleep(1);
		}
		else
			tcp_connect_state = 0x1;

		//printf("s_addr = %#x ,port : %#x\r\n",server_addr.sin_addr.s_addr,server_addr.sin_port);	
printf("d_ip:%s, s_port:%d\n",ccs.d_ip,ccs.port);
		sleep(RECONNECTTIME);
		while(tcp_connect_state)
		{
			if( (recbyte = read(client_socket, tcpinput_buffer, BUFFER_SIZE)) <= 0)
			{
				printf("Service is closed !\r\n");
				tcp_connect_state = 0x0;
				SYS_TCP_IDENTIFY_PASST = 0;
				//关闭socket
				close(client_socket);
				sleep(1);
			}
			else
			{
				tcpinput_buffer[recbyte]='\0';
//printf("tcp recbyte:%d \r\n",recbyte);

//printf("tcp data rec:0x%02x 0x%02x 0x%02x 0x%02x ",tcpinput_buffer[0],tcpinput_buffer[1],tcpinput_buffer[2],tcpinput_buffer[3]);
//printf("%s\n",&tcpinput_buffer[4]);

				if(strncmp((const char *)&tcpinput_buffer[0],(const char *)tcp_header,4) != 0)
				continue;

				tcp_len = (unsigned short)((tcpinput_buffer[4]-0x30)*1000 + (tcpinput_buffer[5]-0x30)*100 + (tcpinput_buffer[6]-0x30)*10 + (tcpinput_buffer[7]-0x30));

//printf("tcp_len:%d\r\n",tcp_len);
//printf("tcp_data[8] = %02x\n",tcpinput_buffer[tcp_len+8]);
//printf("tcp_data[9] = %02x\n",tcpinput_buffer[tcp_len+9]);

				if(recbyte<tcp_len)
				{
					printf("recbyte<tcp_len failed !");
					continue;
				}

				//提取CRC校验值
				rev_crc_v = (unsigned short)((tcpinput_buffer[tcp_len+8]<<8) +tcpinput_buffer[tcp_len+9]);
//printf("提取CRC校验值 success !\r\n");
				//计算crc（计算的是TCP报头[0x68,0x68,0x16,0x16]+有效数据存储为[4字节]+有效数据的CRC）
				cal_crc_v = CRC16_CCITT(&tcpinput_buffer[0],tcp_len+8);
//printf("CRCValue = %x\n",cal_crc_v);
				//如果CRC校验失败，放弃本报问
				if(rev_crc_v != cal_crc_v)
				{
					printf("tcp crc check failed !\r\n");
					continue;
				}
//printf("tcp crc check success !\r\n");
				//判断尾部是否正确
				if(strncmp((const char *)&tcpinput_buffer[tcp_len + 10],(const char *)tcp_tail,4) != 0)
					continue;
//printf("tcp tcp_tail check success !\r\n");
				//解密数据
				AES_InvCipherBuffer(&tcpinput_buffer[12],tcp_len);

				        //取出尾部的多余字节
				AES_InvPadding_PKCS7(&tcpinput_buffer[12]);

//printf("\r\n解密数据:%s\n",&tcpinput_buffer[12]);

				//分析报的类型
				type = (unsigned char *)strstr((char *)&tcpinput_buffer[12],(char *)"<type>");
				if(type == NULL){
					printf("type == NULL !\r\n");
					continue;
				}
				
				//准备比较类型，把认证报和心跳包分别发送给响应的任务，其他他送给回调行数处理
				distribute_msg.mtype = 	3;
				if(strncmp((const char *)(type+6),"sequence",8) == 0)
				{
					distribute_msg.mtype = 	MSG_SEQUENCE;
					tcp_msg.packet_type = MSG_SEQUENCE;		
					if( -1 == msgsnd(msg_box,&distribute_msg,1,0) )
						printf("sequence msgsed fail!\n");
					continue;
				}

				if(strncmp((const char *)(type+6),"result",6) == 0)
				{
					distribute_msg.mtype = MSG_RESULT;
					tcp_msg.packet_type = MSG_RESULT;
					if( -1 == msgsnd(msg_box,&distribute_msg,1,0) )
						printf("result msgsed fail!\n");
					continue;
				}

				if(strncmp((const char *)(type+6),"heart_result",10) == 0)
				{
					distribute_msg.mtype = MSG_HEART_RESULT;
					if( -1 == msgsnd(msg_box,&distribute_msg,1,0) )
						printf("heart_result msgsed fail!\n");
					continue;
				}
/*
				if(strncmp((const char *)(type+6),"server_ctrl",11) == 0)
				{
					distribute_msg.mtype = MSG_SERVER_CTRL;
					if( -1 == msgsnd(msg_box,&distribute_msg,1,0) )
						printf("server_ctrl msgsed fail!\n");
					continue;
				}
				
				if(strncmp((const char *)(type+6),"TimestampCal",12) == 0)
				{
					distribute_msg.mtype = MSG_TIMESTAMP_CAL;
					if( -1 == msgsnd(msg_box,&distribute_msg,1,0) )
						printf("TimestampCal msgsed fail!\n");
					continue;
				}
				
				if(strncmp((const char *)(type+6),"SystemCfg",9) == 0)
				{
					distribute_msg.mtype = MSG_SYSTEM_CFG;
					if( -1 == msgsnd(msg_box,&distribute_msg,1,0) )
						printf("SystemCfg msgsed fail!\n");
					continue;
				}
				
				if(strncmp((const char *)(type+6),"InitVal",7) == 0)
				{
					distribute_msg.mtype = MSG_INIT_VAL;
					if( -1 == msgsnd(msg_box,&distribute_msg,1,0) )
						printf("InitVal msgsed fail!\n"); 
					continue;
				}
				
				if(strncmp((const char *)(type+6),"HeatPump",8) == 0)
				{
					distribute_msg.mtype = MSG_HEATPUMP;
					if( -1 == msgsnd(msg_box,&distribute_msg,1,0) )
						printf("HeatPump msgsed fail!\n");
					continue;
				}
*/
			}
		}
		sleep(1);
	}
	sleep(1);
}
