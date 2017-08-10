#ifndef _485COMMUNICATION_H
#define _485COMMUNICATION_H

extern int set_opt(int fd,int nSpeed,char nEvent);
extern int master485_init(void);
extern int slave485_init(void);
extern void master485_send_chars(int fd,int n,const char *data);
extern void master485_test();
extern void master_input_pthread(void);
extern void slave_input_pthread(void);
extern void repeater_metering_pthread(void);
extern void task_slave485_pthread(void);
extern void task_socket_output_pthread(void);
extern void task_socket_input_pthread(void);
extern void task_tcp_identify_thread(void *p_arg);
extern void task_tcp_heart_beat_thread(void *p_arg);
extern void task_tcp_heart_beat_thread(void *p_arg);
extern void task_identify_timer_thread(void *p_arg);
extern void task_master485_timer_thread(void *p_arg);
extern void task_heart_timer_thread(void *p_arg);

extern char TimerState;
extern unsigned char MsgType;
extern int master485_fd;
extern int slave485_fd;


#define RECONNECTTIME 5
#define MAST485_BUF_SIZE 256
#define MSG_KEY 1234
#define	unready 0x00
#define	ready 0x01
//485数据报的定义
#define reserve				 0x00
#define single_tank_data  	 0x01
#define single_tank_config   0x02
#define	double_tank_data	 0x03
#define	double_tank_config 	 0x04

#define DOUBLE_CONFIG_CNT 19  //双水箱配置参数包数
#define DOUBLE_DATA_CNT  2
#define SINGLE_CONFIG_CNT 19
#define SINGLE_DATA_CNT  2
#define FREE_PROTOCAL  2
#define MOD_BUS  1
#define CJ_T188   0

struct msgbuf{
	long mtype;
	unsigned char packet_type;
	unsigned char mtext[MAST485_BUF_SIZE];
};

struct tcp_msgbuf{
	long mtype;
	unsigned char packet_type;
};

//触摸屏策略数据结构体
struct handle_mode
{
	unsigned char mode;
	unsigned char ready_flag;
	unsigned char next_packet_no;
	struct msgbuf *packet_arry[32];
	
};

extern char msg_box;
extern const char cmd[8];

struct m3_h{
    float val;  
    float samplingInterval; 
};

struct m3{
  float val;
  float pre_val;
  char flag;
};

/*
------累计值结构体------

AllQuse_mj:用户累计供热量
AllEsys_kwh：系统累计耗电量
initEsys_kwh：电表耗电量初始值
*/
struct meteringAccumulatedVal{	
    float AllQuse_mj;               //累计用户供热量
    float AllEsys_kwh;              //累计系统耗电量
    float AllMv_m3;
    float initEsys_kwh;             //电表初始电度值
    float initMv_m3;
    unsigned int timestamp_pre_metering;
    unsigned short flag;
};

/*
------采集值结构体------

XX_coulometer_KW:电表功率读数值
XX_coulometer_KWH：电表耗电量读数值
XX_coulometer_A：电表电流读数值
*/
struct meteringCollectionVal{	  
  
    //热量表数据采集
    float calorimeter_in_temp;
    float calorimeter_out_temp;
    struct m3_h calorimeterr_m3_h;
    float calorimeterr_m3;
    float calorimeterr_kw;
    float calorimeterr_kwh;

    //三相电能表数据采集
    float triphase_coulometer_KW;			
    float triphase_coulometer_KWH;
    float triphase_coulometer_A;	

    //单相电能表数据采集
    float uniphase_coulometer_KW;
    float uniphase_coulometer_KWH;
    float uniphase_coulometer_A;
    
    float init_tank1_yw;
    float init_tank2_yw;
    float tank1_yw;
    float tank2_yw;
};

/*
------计算值结构体------

Esys_kwh：系统耗电量（一个采集间隔内）
CurM3_val:用户实时流量
Quse_kj：用户供热量（一个采集间隔内）
calorimeterr_m3_h：流量
Qss_g：节煤量
mco2_g：CO2减排量
mso2_g：SO2减排量
mnox_g：氮氧化物减排量
mfc_g：粉尘减排量

AllQuse_mj：用户累计供热量
AllEsys_kwh：系统累计耗电量
AllM3_val:用户累计供水量
*/
struct meteringCalculatedVal{
    //实时值
    float calorimeter_in_temp;
    float calorimeter_out_temp;
    float Esys_kwh;
    float triphase_coulometer_KW;			
    float triphase_coulometer_A;
    float Quse_kj;
    struct m3 calorimeterr_m3;
    float calorimeterr_m3_h_cyc;//发送周期内的平均流量
    float Qss_g;
    float mco2_g;
    float mso2_g;
    float mnox_g;
    float mfc_g;
    
    //累计量
    float AllQuse_mj;
    float AllEsys_kwh;
    float AllMv_m3;
    float ALLMvuse_m3;//用户耗水量
};


#endif
