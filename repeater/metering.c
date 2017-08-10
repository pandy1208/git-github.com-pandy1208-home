#include "485communication.h"
#include <stdio.h>
#include <sys/msg.h>
#include <pthread.h>
#include <sys/time.h>
#include <memory.h>
#include <malloc.h>

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

void get_calorimeter_add(void);
void get_init_yw(void);
unsigned short string2int(char *p, unsigned char n);

static const char calorimeter_broadcast_cmd[20] = {
0xFE, 0xFE, 0xFE, 0xFE, 0x68, 0x20, 0xAA, 0xAA, 0xAA, 0xAA, 
0xAA, 0x11, 0x11, 0x01, 0x03, 0x1F, 0x90, 0x00, 0xAF, 0x16
};

char calorimeter_cmd[20] = {
0xFE, 0xFE, 0xFE, 0xFE, 0x68, 0x20, 0x05, 0x00, 0x04, 0x63, 
0x00, 0x11, 0x11, 0x01, 0x03, 0x1F, 0x90, 0x12, 0xDB, 0x16
};

struct meteringAccumulatedVal accumulatedVal = {0};
struct meteringCollectionVal collectionVal = {0};
struct meteringCalculatedVal calculatedVal = {0};

static const char yw_cmd1[6] = "00061";
static const char yw_cmd2[6] = "00061";
extern char calorimeter_cmd[20];

extern char Master485TimerState;
extern char msg_box;
struct msgbuf m485_msg;
void repeater_metering_pthread(void)
{
//while(1)sleep(10);	
	//f_mount(0,&fatfs_fs);//读取历史文件
	get_calorimeter_add(); 
printf("\r\ncalorimeter_cmd:");
char num_ptf_cmd = 0;
for(num_ptf_cmd = 0;num_ptf_cmd < 20;num_ptf_cmd ++)
	printf("%02x ",calorimeter_cmd[num_ptf_cmd]);
   	//while(meteringInit(&accumulatedVal) != 0);

	while(1)
	{
		master485_send_chars(master485_fd,20,(const char *)calorimeter_cmd);
		Master485TimerState = 1;
		memset(m485_msg.mtext,0,sizeof(char)*MAST485_BUF_SIZE);
		msgrcv(msg_box,&m485_msg,MAST485_BUF_SIZE,MSG_MASTER485,0);
		Master485TimerState = 0;
		if(m485_msg.packet_type == TIMEOUT)
				printf("\r\nread calorimeter timeout !\r\n");
		else		
			printf("calorimeter data:%s\n",m485_msg.mtext);
		sleep(5);
	}
}

extern int rtc_fd;
extern char CatMasterDate;
void get_calorimeter_add(void)
{
	unsigned char err,*rev,i=1,j=0,temp=0;
	//while(1)
	{
get_add:
		i=1;j=0;temp=0;

		master485_send_chars(master485_fd,20,(const char *)calorimeter_broadcast_cmd);

if(CatMasterDate)
write(rtc_fd, calorimeter_broadcast_cmd, 20);

		Master485TimerState = 1;
		memset(m485_msg.mtext,0,sizeof(char)*MAST485_BUF_SIZE);
		msgrcv(msg_box,&m485_msg,MAST485_BUF_SIZE,MSG_MASTER485,0);
		Master485TimerState = 0;
		if(m485_msg.packet_type == TIMEOUT)
				printf("\r\nget_calorimeter_add timeout !\r\n");

printf("get_calorimeter_add:%s\n",m485_msg.mtext);
	  while(i<68)
	  {
		if((m485_msg.mtext[i-1] == 0x68)&&(m485_msg.mtext[i] == 0x25))
		  {
		    calorimeter_cmd[6] = (char)m485_msg.mtext[i+1];
		    calorimeter_cmd[7] = (char)m485_msg.mtext[i+2];
		    calorimeter_cmd[8] = (char)m485_msg.mtext[i+3];
		    calorimeter_cmd[9] = (char)m485_msg.mtext[i+4];
		    calorimeter_cmd[10] = (char)m485_msg.mtext[i+5];
		    for(j=4;j<18;j++)
		    {
		      temp+=calorimeter_cmd[j];
		    }
		    calorimeter_cmd[18] = temp;
		    break;
		  }
		i++;
	  }
	  if(i>=68)
		goto get_add;     
	}                  
}

void get_init_yw(void)
{
    unsigned char *rev;
    float tmp1_yw = 999.9, tmp2_yw=999.9;
//读取恒温水箱的液位
    do
    {
        while( tmp1_yw > 2 )
        {
			master485_send_chars(master485_fd,5,(const char *)yw_cmd2);
			Master485TimerState = 1;
			memset(m485_msg.mtext,0,sizeof(char)*MAST485_BUF_SIZE);
			msgrcv(msg_box,&m485_msg,MAST485_BUF_SIZE,MSG_MASTER485,0);
			Master485TimerState = 0;
			if(m485_msg.packet_type == TIMEOUT)
				{
					printf("\r\nget_calorimeter_add timeout !\r\n");
					continue;
				}
printf("get_init_yw1:%s\n",m485_msg.mtext);

            tmp1_yw = ((float)string2int(m485_msg.mtext, 4))/100;
        }
        while( tmp2_yw > 2 )
        {

            master485_send_chars(master485_fd,5,(const char *)yw_cmd2);
			Master485TimerState = 1;
			memset(m485_msg.mtext,0,sizeof(char)*MAST485_BUF_SIZE);
			msgrcv(msg_box,&m485_msg,MAST485_BUF_SIZE,MSG_MASTER485,0);
			Master485TimerState = 0;
			if(m485_msg.packet_type == TIMEOUT)
				{
					printf("\r\nget_calorimeter_add timeout !\r\n");
					continue;
				}
printf("get_init_yw2:%s\n",m485_msg.mtext);

            tmp2_yw = ((float)string2int(m485_msg.mtext, 4))/100;
        }
    }while((!(tmp1_yw - tmp2_yw)<=0.01)&&(!(tmp2_yw - tmp1_yw)<=0.01));

/*
    collectionVal.init_tank1_yw = (tmp1_yw+tmp2_yw)/2;

    //读取集热水箱的液位
    if(sys_cfg.sys_type != 1)
    {
        tmp1_yw = 999.9, tmp2_yw=999.9;
        do
        {
            while( tmp1_yw > 2 )
            {

                toReadInstrument(5, yw_cmd1, &rev, FREE_PROTOCAL);
                tmp1_yw = ((float)string2int(rev+2, 4))/100;
                OSMemPut(buf_485,rev);
            }
            while( tmp2_yw > 2 )
            {

                toReadInstrument(5, yw_cmd1, &rev, FREE_PROTOCAL);
                tmp2_yw = ((float)string2int(rev+2, 4))/100;
                OSMemPut(buf_485,rev);
            }
        }while((!(tmp1_yw - tmp2_yw)<=0.01)&&(!(tmp2_yw - tmp1_yw)<=0.01));
        collectionVal.init_tank2_yw = (tmp1_yw+tmp2_yw)/2;
    }
*/
}

unsigned short string2int(char *p, unsigned char n)
{
    char i; 
    unsigned short j = 1;
    unsigned short result = 0;
    if(n == 1)
        return (p[0] - 0x30);
    
    for(i = n; i > 0; i--)
    {
        if((p[i-1] < 0x30) || (p[i-1] > 0x39))
          break;
        result += (p[i - 1] - 0x30) * j;
        j *= 10;
    }
    return result;
}

