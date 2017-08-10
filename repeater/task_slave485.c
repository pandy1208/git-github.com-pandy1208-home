#include "485communication.h"
#include <stdio.h>
#include <string.h>
#include <sys/msg.h>
#include <pthread.h>
#include <sys/time.h>
#include <memory.h>
#include <malloc.h>

#define BUFFER_SIZE 1500
#define MSG_MASTER485 0x01
#define MSG_SLAVE485 0x02
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

struct handle_mode hm;
struct tcp_msgbuf tcp_msg_type;

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

struct communication_config_struct ccs = {
	0x55aa,
	//4007,
	8234,
//	"202.107.249.238",
	"121.196.244.172",
	"192.168.1.1",
	"192.168.1.33",
	"255.255.255.0",
	{0x12,0x08,0x35,0x39,0x1f,0x55},
	"32020000303"
};


//////////////////////////////////////////
//slave485
//////////////////////////////////////////

const unsigned char slave485_tcp_config_1[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?><root><common><id>";
const unsigned char slave485_tcp_config_2[] = "</id><type>canshu</type><mode>";
const unsigned char slave485_tcp_config_3[] = "</mode><time>";
const unsigned char slave485_tcp_config_4[] =	"</time><strategy>";
const unsigned char slave485_tcp_config_5[] =	"</strategy><canshu1>";
const unsigned char slave485_tcp_config_6[] =	"</canshu1><canshu2>";
const unsigned char slave485_tcp_config_7[] =	"</canshu2><canshu3>";
const unsigned char slave485_tcp_config_8[] =	"</canshu3><canshu4>";
const unsigned char slave485_tcp_config_9[] =	"</canshu4></common></root>";//用作双水箱参数数据尾
const unsigned char slave485_tcp_config_10[] ="</canshu3></common></root>";//用作单水箱参数数据尾

const unsigned char slave485_tcp_data_1[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?><root><common><id>";
const unsigned char slave485_tcp_data_2[] = "</id><type>shuju</type><mode>";
const unsigned char slave485_tcp_data_3[] = "</mode><time>";
const unsigned char slave485_tcp_data_4[] = "</time><data>";
const unsigned char slave485_tcp_data_5[] = "</data><onoff>";
const unsigned char slave485_tcp_data_6[] = "</onoff><force>";
const unsigned char slave485_tcp_data_7[] = "</force><alarm>";
const unsigned char slave485_tcp_data_8[] = "</alarm></common></root>";

//server_ctrl应答
const unsigned char server_Ctrl_ack_1[] ="<?xml version=\"1.0\" encoding=\"utf-8\"?><root><common><id>";
const unsigned char server_Ctrl_ack_2[] ="</id><type>server_ctrl_ack</type></common></root>"; 


//SystemCfg应答
const unsigned char system_cfg_ack_1[] ="<?xml version=\"1.0\" encoding=\"utf-8\"?><root><common><id>";
const unsigned char system_cfg_ack_2[] ="</id><type>SystemCfg</type></common></root>";


const unsigned char set_initval_ack_1[] ="<?xml version=\"1.0\" encoding=\"utf-8\"?><root><common><id>";
const unsigned char set_initval_ack_2[] ="</id><type>SetInitVal</type></common></root>"; 

//HeatPumpConfigation应答
const char heat_pump_cfg_ack1[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?><root><common><id>";
const char heat_pump_cfg_ack2[] = "</id><timestamp>";
const char heat_pump_cfg_ack3[] = "</timestamp><type>HeatPumpConfigation</type><data>";
const char heat_pump_cfg_ack4[] = "</data></common></root>"; 


unsigned char tcpDataCtrl[1024] = "1234567890aaxml version=\"1.0\" encoding=\"utf-8\"?><root><common><id>33010000101</id><type>shuju</type><mode>1</mode><time>20151224131511</time><data>1234567890123456789012345678</data><onoff>111000111</onoff><force>10101011</force><alarm>10101010101010101010</alarm></common></root>";
unsigned char tcpDataMetering[512] = "1234567890aaxml version=\"1.0\" encoding=\"utf-8\"?><root><common><id>33010000101</id><timestamp>14105060</timestamp><type>metering</type><data>1/2/3/4/5/6/7/8/9/0/1</data></common></root>";
unsigned char tcpDataHeatPump[512] = "1234567890aaxml version=\"1.0\" encoding=\"utf-8\"?><root><common><id>33010000101</id><timestamp>14105060</timestamp><type>HeatPumpStatus</type><data>123456789012345678901234567890123456789012345678901234567890</data></common></root>";

unsigned char tcp_data[1500] = {0x0};


void slave485_packet_sift_assembling(struct handle_mode * hm,struct msgbuf  *rev_s_485);
void slave485_data2tcp(struct handle_mode * hm);

void single_tank_data_send2tcp(struct handle_mode * hm);
void single_tank_config2tcp(struct handle_mode * hm);
void double_tank_data_send2tcp(struct handle_mode * hm);
void double_tank_config2tcp(struct handle_mode * hm);
void strcpy_n_end_with_0(unsigned char * d,struct handle_mode * hm,int start,int n);

unsigned int CRCValue;
char err,ctrl_com1_num=0,i=0;
char CRCValuechar[8];
pthread_mutex_t DATA_CAL_CTRL_LOCK;//计量变量程互斥锁
pthread_mutex_t TCP_DATA_LOCK;//tcp_data[]变量程互斥锁
extern char msg_box,CatCtrlDate;
struct msgbuf *s485_msg = NULL;
void task_slave485_pthread(void)
{
	
	tcp_msg_type.mtype = 3;

	hm.mode = reserve;
    hm.ready_flag = unready;
    hm.next_packet_no = 0x30;

	pthread_mutex_init(&DATA_CAL_CTRL_LOCK, NULL);
	pthread_mutex_init(&TCP_DATA_LOCK, NULL);
	//初始化计量变量程互斥锁

	while(1)
	{
		s485_msg = (struct msgbuf *)malloc(sizeof(struct msgbuf));/*为指针变量分配安全的地址*/
		//printf("s485_msg add:%x\n",s485_msg);
		memset(s485_msg->mtext,0,sizeof(char)*MAST485_BUF_SIZE);
		msgrcv(msg_box,s485_msg,MAST485_BUF_SIZE,MSG_SLAVE485,0);
		//printf("s485_msg:%s\n",s485_msg->mtext);
		if((s485_msg->mtext[0] != 0x03) || (s485_msg->mtext[1] != 0x10))//不是发给中继器的数据
		{
		//printf("not for repeater free s485_msg\n");
/*
printf("\r\n");
for(i = 0;i < ctrl_com1_num;i++)
	printf("%02x ",s485_msg->mtext[i]);
*/
		  free(s485_msg);
		  s485_msg = NULL;
          continue;
		}
/*
printf("\r\n");
for(ctrl_com1_num = 0;ctrl_com1_num < strlen(s485_msg->mtext);ctrl_com1_num ++)
printf("%02x ",s485_msg->mtext[ctrl_com1_num]);
*/
		CRCValue = CRC16_MODBUS(s485_msg->mtext,6);
		for(err=0;err<6;err++)
        {
            CRCValuechar[err] = s485_msg->mtext[err];	
        }
		CRCValuechar[6] = (char)(CRCValue&0xff);
        CRCValuechar[7]	= (char)((CRCValue>>8)&0xff);
		master485_send_chars(slave485_fd,8,(const char *)CRCValuechar);
		slave485_packet_sift_assembling(&hm,s485_msg);
		s485_msg = NULL;
		if(hm.ready_flag == ready)
        {
            //sysstat = OSFlagQuery(SYS_START_SEM_F,&err);
            //if((sysstat&(SYS_TCP_SET_UP_OK|SYS_TCP_IDENTIFY_PASST)) == (SYS_TCP_SET_UP_OK|SYS_TCP_IDENTIFY_PASST))
            if(1)
            {
                slave485_data2tcp(&hm);
            }
            else
            {
                ;
            }		  
        }
	}
}


void slave485_packet_sift_assembling(struct handle_mode * hm,struct msgbuf *rev_s_485)
{
	//在这里还可以加一些对hm->ready_flage的判断
    if(rev_s_485 == NULL )
    {
        return ;
    }

    if((rev_s_485->mtext[7] == '0') && (rev_s_485->mtext[8] == '0'))//如果是第一组参数
    {
        if((rev_s_485->mtext[9] >= 0x31)&&(rev_s_485->mtext[9]<= 0x34))
        {
            hm->mode = single_tank_config;	                 
        }
        else if((rev_s_485->mtext[9] >= 0x35)&&(rev_s_485->mtext[9]<= 0x38)) 
        {         
//printf("\r\ndoubule_tank_config\r\n");
            hm->mode = double_tank_config;	
        }
        else//单水箱双水箱位，不合法
        {
            //无法处理释放掉内存,结束
			//printf("not right data free s485_msg\n");
            free(rev_s_485);
            rev_s_485 = NULL;	
        }

        //如果数据合法，把数据加到链表中
        if(rev_s_485 != NULL)
        {
            for(hm->next_packet_no --;0x30 <= hm->next_packet_no;hm->next_packet_no --)
            {
                if(hm->packet_arry[hm->next_packet_no-0x30] != NULL)
                {
                    free(hm->packet_arry[hm->next_packet_no-0x30]);
                    hm->packet_arry[hm->next_packet_no - 0x30] = NULL;	
                }
            }
            hm->next_packet_no = 0x31;
            hm->packet_arry[0] = rev_s_485;
            rev_s_485 = NULL;
        }
        //拼接数据的第一帧结束			
     }
    
    else if((rev_s_485->mtext[7] == '2') && (rev_s_485->mtext[8] == '0'))             //如果是第一组数据
    {
        if((rev_s_485->mtext[9] >= 0x31)&&(rev_s_485->mtext[9]<= 0x34))
        {
             hm->mode = single_tank_data;
        }
        else if((rev_s_485->mtext[9] >= 0x35)&&(rev_s_485->mtext[9]<= 0x38))
        {
            hm->mode = double_tank_data;
        }
        else//单水箱双水箱位，不合法
        {
            //无法处理释放掉内存,结束
            //printf("not right data free s485_msg\n");
            free(rev_s_485);
            rev_s_485 = NULL;	
        }
        //如果数据合法，把数据加到链表中
        if(rev_s_485 != NULL)
        {
			//printf("first backet data available put in arry\n");
            for(hm->next_packet_no --;0x30 <= hm->next_packet_no;hm->next_packet_no --)
            {
                if(hm->packet_arry[hm->next_packet_no-0x30] != NULL)
                {
                    free(hm->packet_arry[hm->next_packet_no-0x30]);
                    hm->packet_arry[hm->next_packet_no - 0x30] = NULL;	
                }
            }
            hm->next_packet_no = 0x31;
            hm->packet_arry[0] = rev_s_485;
            rev_s_485 = NULL;
        }
        //拼接数据的第一帧结束	          
    }
    
    else
    { //如果数据不是第一组数据
        //printf("second backet data available put in arry\n");
        if(rev_s_485->mtext[7] == '1')
            rev_s_485->mtext[8] = rev_s_485->mtext[8] + 0x10;
        //查看数据包是否合法	
        if(rev_s_485->mtext[8] == hm->next_packet_no)
        {
            hm->packet_arry[hm->next_packet_no - 0x30] = rev_s_485;
            rev_s_485 = NULL;
            hm->next_packet_no ++;
        }
        else if(rev_s_485->mtext[8] == (hm->next_packet_no - 1))	//重发报
        {
            //printf("not right data free s485_msg\n");
            free(rev_s_485);
            rev_s_485 = NULL;
        }
        else
        {
            //数据已经不具有，可用性，抛弃所有数据
            //
            //printf("data is useless free s485_msg\n");
            free(rev_s_485);
            rev_s_485 = NULL;

resetpacket://初始化
            hm->mode = reserve;
            hm->ready_flag = unready;
            for(hm->next_packet_no = 0;hm->next_packet_no < DOUBLE_CONFIG_CNT;hm->next_packet_no ++)
            {
                if(hm->packet_arry[hm->next_packet_no] != NULL)
                {
					//printf("init packet_arry[] free next_packet_no\n");
                    free(hm->packet_arry[hm->next_packet_no]);
                    hm->packet_arry[hm->next_packet_no] = NULL;		
                }
            }
            hm->next_packet_no = 0x30;
            return ;	
        }
    }
    //检查数据包是否接受，完毕
    if(hm->mode == single_tank_config)
    {
        if(hm->next_packet_no == 0x3D)
        {
            hm->ready_flag = ready;	
        }
        else if(hm->next_packet_no < 0x3D)	
        {
            hm->ready_flag = unready;	
        }
        else
        {
            goto resetpacket;	
        }

    }
    else if(hm->mode == single_tank_data)
    {
        if(hm->next_packet_no == 0x32)
        {
            hm->ready_flag = ready;
        }
        else if(hm->next_packet_no < 0x32)
        {
            hm->ready_flag = unready;	
        }
        else
        {
            goto resetpacket;
        }
    }
    else if(hm->mode == double_tank_config)
    {
        if(hm->next_packet_no == 0x43)
        {
            hm->ready_flag = ready;
        }
        else if(hm->next_packet_no < 0x43)
        {
            hm->ready_flag = unready;
        }
        else
        {
            goto resetpacket;
        }
    }
    else if(hm->mode == double_tank_data)
    {
        if(hm->next_packet_no == 0x32)
        {
            hm->ready_flag = ready;
        }
        else if (hm->next_packet_no < 0x32)
        {
            hm->ready_flag = unready;	
        }
        else
        {
            goto resetpacket;	
        }
    }		
}

void slave485_data2tcp(struct handle_mode * hm)
{
    if(hm->ready_flag == unready)
    {
        return ;
    }
    switch (hm->mode)
    {
        case single_tank_data:
            single_tank_data_send2tcp(hm);
            break;
        case single_tank_config:
            single_tank_config2tcp(hm);
            break;
        case double_tank_data:
            double_tank_data_send2tcp(hm);
            break;
        case double_tank_config:
            double_tank_config2tcp(hm);
            break;
    }

    //释放hm
    hm->mode = reserve;
    hm->ready_flag = unready;
    for(hm->next_packet_no = 0;hm->next_packet_no < 18;hm->next_packet_no ++)
    {
        if(hm->packet_arry[hm->next_packet_no] != NULL)
        {
            free(hm->packet_arry[hm->next_packet_no]);
            hm->packet_arry[hm->next_packet_no] = NULL;		
        }
    }
    hm->next_packet_no = 0x30;
}

void single_tank_data_send2tcp(struct handle_mode * hm)
{
	pthread_mutex_lock(&DATA_CAL_CTRL_LOCK);
	//获取计量变量程互斥锁,保证数据整合的原子性 
	int cnt = 0;
        
	strcpy((char *)&tcpDataCtrl[10],(char *)slave485_tcp_data_1);
	cnt = strlen((const char *)slave485_tcp_data_1);
	
	//id
	strcat((char *)&tcpDataCtrl[10],(char *)ccs.use_this_id);
	cnt += strlen((const char *)ccs.use_this_id);
	
	strcat((char *)&tcpDataCtrl[10],(char *)slave485_tcp_data_2);
	cnt += strlen((const char *)slave485_tcp_data_2);
	
	//mode
	strcpy_n_end_with_0(&tcpDataCtrl[10+cnt],hm,0,1);
	cnt += 1;

	strcat((char *)&tcpDataCtrl[10],(char *)slave485_tcp_data_3);
	cnt += strlen((const char *)slave485_tcp_data_3);
	
	//time
	strcpy_n_end_with_0(&tcpDataCtrl[10+cnt],hm,1,14);
	cnt += 14;

	strcat((char *)&tcpDataCtrl[10],(char *)slave485_tcp_data_4);
	cnt += strlen((const char *)slave485_tcp_data_4);

	//data
	strcpy_n_end_with_0(&tcpDataCtrl[10+cnt],hm,15,20);
	cnt += 20;

	strcat((char *)&tcpDataCtrl[10],(char *)slave485_tcp_data_5);
	cnt += strlen((const char *)slave485_tcp_data_5);

	//onoff
	strcpy_n_end_with_0(&tcpDataCtrl[10+cnt],hm,35,10);
	cnt += 10;

	strcat((char *)&tcpDataCtrl[10],(char *)slave485_tcp_data_6);
	cnt += strlen((const char *)slave485_tcp_data_6);

	//force
	strcpy_n_end_with_0(&tcpDataCtrl[10+cnt],hm,45,6);
	cnt += 6;

	strcat((char *)&tcpDataCtrl[10],(char *)slave485_tcp_data_7);
	cnt += strlen((const char *)slave485_tcp_data_7);

	//alarm
	strcpy_n_end_with_0(&tcpDataCtrl[10+cnt],hm,51,17);
	cnt += 17;

	strcat((char *)&tcpDataCtrl[10],(char *)slave485_tcp_data_8);
	cnt += strlen((const char *)slave485_tcp_data_8);
        
   // pthread_mutex_unlock(&DATA_CAL_CTRL_LOCK);

	tcp_msg_type.packet_type = single_tank_data;
	if( -1 == msgsnd(msg_box,&tcp_msg_type,1,0) )
		printf("single_tank_data tcp msgsed fail!\n");

	//释放计量变量程互斥锁
	//发送请求
	//OSQPost(TCP_SEN_Q,tcp_data);
	//printf("single_tank_data:%s\n",tcpDataCtrl);
}

void single_tank_config2tcp(struct handle_mode * hm)
{
	pthread_mutex_lock(&TCP_DATA_LOCK);

	int cnt = 0;

	strcpy((char *)&tcp_data[10],(char *)slave485_tcp_config_1);
	cnt = strlen((const char *)slave485_tcp_config_1);
	
	//id
	strcat((char *)&tcp_data[10],(char *)ccs.use_this_id);
	cnt += strlen((const char *)ccs.use_this_id);
	
	strcat((char *)&tcp_data[10],(char *)slave485_tcp_config_2);
	cnt += strlen((const char *)slave485_tcp_config_2);
	
	//mode
	strcpy_n_end_with_0(&tcp_data[10+cnt],hm,0,1);
	cnt += 1;

	strcat((char *)&tcp_data[10],(char *)slave485_tcp_config_3);
	cnt += strlen((const char *)slave485_tcp_config_3);
	
	//time
	strcpy_n_end_with_0(&tcp_data[10+cnt],hm,1,14);
	cnt += 14;

	strcat((char *)&tcp_data[10],(char *)slave485_tcp_config_4);
	cnt += strlen((const char *)slave485_tcp_config_4);

	//strategy 
	strcpy_n_end_with_0(&tcp_data[10+cnt],hm,15,42);
	cnt += 42;

	strcat((char *)&tcp_data[10],(char *)slave485_tcp_config_5);
	cnt += strlen((const char *)slave485_tcp_config_5);

	//canshu1 
	strcpy_n_end_with_0(&tcp_data[10+cnt],hm,57,240);
	cnt += 240;

	strcat((char *)&tcp_data[10],(char *)slave485_tcp_config_6);
	cnt += strlen((const char *)slave485_tcp_config_6);

	//canshu2 
	strcpy_n_end_with_0(&tcp_data[10+cnt],hm,297,136);
	cnt += 136;

	strcat((char *)&tcp_data[10],(char *)slave485_tcp_config_7);
	cnt += strlen((const char *)slave485_tcp_config_7);

	//canshu3 
	strcpy_n_end_with_0(&tcp_data[10+cnt],hm,433,327);
	cnt += 327;

	strcat((char *)&tcp_data[10],(char *)slave485_tcp_config_10);
	cnt += strlen((const char *)slave485_tcp_config_10);
	
	tcp_msg_type.packet_type = single_tank_config;
	if( -1 == msgsnd(msg_box,&tcp_msg_type,1,0) )
		printf("single_tank_config tcp msgsed fail!\n");
	//发送请求
	//OSQPost(TCP_SEN_Q,tcp_data);
	//pthread_mutex_unlock(&TCP_DATA_LOCK);
	
}

void double_tank_data_send2tcp(struct handle_mode * hm)
{
	pthread_mutex_lock(&DATA_CAL_CTRL_LOCK);
	//获取计量变量程互斥锁,保证数据整合的原子性 
	int cnt = 0;

	strcpy((char *)&tcpDataCtrl[10],(char *)slave485_tcp_data_1);
	cnt = strlen((const char *)slave485_tcp_data_1);
	
	//id
	strcat((char *)&tcpDataCtrl[10],(char *)ccs.use_this_id);
	cnt += strlen((const char *)ccs.use_this_id);
	
	strcat((char *)&tcpDataCtrl[10],(char *)slave485_tcp_data_2);
	cnt += strlen((const char *)slave485_tcp_data_2);

	//mode
	strcpy_n_end_with_0(&tcpDataCtrl[10+cnt],hm,0,1);
	cnt += 1;

	strcat((char *)&tcpDataCtrl[10],(char *)slave485_tcp_data_3);
	cnt += strlen((const char *)slave485_tcp_data_3);
	
	//time
	strcpy_n_end_with_0(&tcpDataCtrl[10+cnt],hm,1,14);
	cnt += 14;

	strcat((char *)&tcpDataCtrl[10],(char *)slave485_tcp_data_4);
	cnt += strlen((const char *)slave485_tcp_data_4);

	//data
	strcpy_n_end_with_0(&tcpDataCtrl[10+cnt],hm,15,28);
	cnt += 28;

	strcat((char *)&tcpDataCtrl[10],(char *)slave485_tcp_data_5);
	cnt += strlen((const char *)slave485_tcp_data_5);

	//onoff
	strcpy_n_end_with_0(&tcpDataCtrl[10+cnt],hm,43,9);
	cnt += 9;

	strcat((char *)&tcpDataCtrl[10],(char *)slave485_tcp_data_6);
	cnt += strlen((const char *)slave485_tcp_data_6);

	//force
	strcpy_n_end_with_0(&tcpDataCtrl[10+cnt],hm,52,8);
	cnt += 8;

	strcat((char *)&tcpDataCtrl[10],(char *)slave485_tcp_data_7);
	cnt += strlen((const char *)slave485_tcp_data_7);

	//alarm
	strcpy_n_end_with_0(&tcpDataCtrl[10+cnt],hm,60,20);
	cnt += 20;

	strcat((char *)&tcpDataCtrl[10],(char *)slave485_tcp_data_8);
	cnt += strlen((const char *)slave485_tcp_data_8);
    
	//pthread_mutex_unlock(&DATA_CAL_CTRL_LOCK);

	tcp_msg_type.packet_type = double_tank_data;
	if( -1 == msgsnd(msg_box,&tcp_msg_type,1,0) )
		printf("double_tank_data tcp msgsed fail!\n");	

	//释放计量变量程互斥锁
	//发送请求
	//OSQPost(TCP_SEN_Q,tcp_data);
	//printf("double_tank_data:%s\n",tcpDataCtrl);
}

void double_tank_config2tcp(struct handle_mode * hm)
{
//printf("\r\n double_tank_config2tcp\r\n");
	pthread_mutex_lock(&TCP_DATA_LOCK);
	int cnt = 0;
	
	strcpy((char *)&tcp_data[10],(char *)slave485_tcp_config_1);
	cnt = strlen((const char *)slave485_tcp_config_1);
	//id
	strcat((char *)&tcp_data[10],(char *)ccs.use_this_id);
	cnt += strlen((const char *)ccs.use_this_id);
	
	strcat((char *)&tcp_data[10],(char *)slave485_tcp_config_2);
	cnt += strlen((const char *)slave485_tcp_config_2);
	
	//mode
	strcpy_n_end_with_0(&tcp_data[10+cnt],hm,0,1);
	cnt += 1;

	strcat((char *)&tcp_data[10],(char *)slave485_tcp_config_3);
	cnt += strlen((const char *)slave485_tcp_config_3);
	
	//time
	strcpy_n_end_with_0(&tcp_data[10+cnt],hm,1,14);
	cnt += 14;

	strcat((char *)&tcp_data[10],(char *)slave485_tcp_config_4);
	cnt += strlen((const char *)slave485_tcp_config_4);

	//strategy 20161017pandy
	strcpy_n_end_with_0(&tcp_data[10+cnt],hm,15,53);
	cnt += 53;

	strcat((char *)&tcp_data[10],(char *)slave485_tcp_config_5);
	cnt += strlen((const char *)slave485_tcp_config_5);

	//canshu1 
	strcpy_n_end_with_0(&tcp_data[10+cnt],hm,68,336);
	cnt += 336;

	strcat((char *)&tcp_data[10],(char *)slave485_tcp_config_6);
	cnt += strlen((const char *)slave485_tcp_config_6);

	//canshu2 
	strcpy_n_end_with_0(&tcp_data[10+cnt],hm,404,152);
	cnt += 152;

	strcat((char *)&tcp_data[10],(char *)slave485_tcp_config_7);
	cnt += strlen((const char *)slave485_tcp_config_7);

	//canshu3 20161017pandy
	strcpy_n_end_with_0(&tcp_data[10+cnt],hm,556,348);
	cnt += 348;

	strcat((char *)&tcp_data[10],(char *)slave485_tcp_config_8);
	cnt += strlen((const char *)slave485_tcp_config_8);
      
    //canshu4
    strcpy_n_end_with_0(&tcp_data[10+cnt],hm,904,257);
	cnt += 257;

	strcat((char *)&tcp_data[10],(char *)slave485_tcp_config_9);
	cnt += strlen((const char *)slave485_tcp_config_9);
	
	tcp_msg_type.packet_type = double_tank_config;
	if( -1 == msgsnd(msg_box,&tcp_msg_type,1,0) )
		printf("double_tank_config tcp msgsed fail!\n");        
	//pthread_mutex_unlock(&TCP_DATA_LOCK);
	//发送请求
	//OSQPost(TCP_SEN_Q,tcp_data);	
}

void strcpy_n_end_with_0(unsigned char * d,struct handle_mode * hm,int start,int n)
{
    int index,dt;
    unsigned char *s;

    index = start / 62;
    dt = start % 62;
    s = &(hm->packet_arry[index]->mtext[9 + dt]);
    while(n > 0)
    {
        *d = *s;
        n--;
        d++;
        s++;
        dt++;
        if(dt>61)
        {
            dt = 0;
            index++;
            s = &(hm->packet_arry[index]->mtext[9 + dt]);
        }		
    }
    *d = '\0';

}
