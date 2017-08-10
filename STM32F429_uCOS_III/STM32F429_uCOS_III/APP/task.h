#ifndef TASK_H
#define TASK_H

#include  <includes.h>
#include 	"app_cfg.h"
#include  <os.h>
#include <stm32f4xx.h>


//DEBUG信息开关
#define DEBUG_PRINT 0

//主485内存池
#define NBLKS_m485	5
#define BLKSIZE_m485	256
extern OS_MEM * buf_m485;
extern unsigned char	part_buf_m485[NBLKS_m485][BLKSIZE_m485];

//从485内存池
#define NBLKS_s485	5
#define BLKSIZE_s485	128
extern OS_MEM * buf_s485;
extern unsigned char	part_buf_s485[NBLKS_s485][BLKSIZE_s485];

//uart内存池
#define NBLKS_uart	5
#define BLKSIZE_uart	128
extern OS_MEM * buf_uart;
extern unsigned char	part_buf_uart[NBLKS_uart][BLKSIZE_uart];

//i2c内存池
#define NBLKS_i2c	5
#define BLKSIZE_i2c	256
extern OS_MEM * buf_i2c;
extern unsigned char	part_buf_i2c[NBLKS_i2c][BLKSIZE_i2c];

//动作消息内存池
#define NBLKS_act	5
#define BLKSIZE_act	128
extern OS_MEM * buf_act;
extern unsigned char	part_buf_act[NBLKS_act][BLKSIZE_act];


//动作线程消息队列
#define ACT_QUEUE_N 5
extern 	OS_Q    * ACT_Q;
extern void * MsgGrp_ACT[ACT_QUEUE_N];

//遥控线程消息队列
#define TEL_QUEUE_N 5
extern 	OS_Q    * TEL_Q;
extern void * MsgGrp_TEL[TEL_QUEUE_N];

//图线处理线程消息队列
#define CAM_QUEUE_N 5
extern 	OS_Q    * CAM_Q;
extern void * MsgGrp_CAM[CAM_QUEUE_N];

//超声波处理线程消息队列
#define ULT_QUEUE_N 5
extern 	OS_Q    * ULT_Q;
extern void * MsgGrp_ULT[ULT_QUEUE_N];

//温湿度处理线程消息队列
#define HUM_QUEUE_N 5
extern 	OS_Q    * HUM_Q;
extern void * MsgGrp_HUM[HUM_QUEUE_N];

//陀螺仪处理线程消息队列
#define GYR_QUEUE_N 5
extern 	OS_Q    * GYR_Q;
extern void * MsgGrp_GYR[GYR_QUEUE_N];

//电池管理线程消息队列
#define BMS_QUEUE_N 5
extern 	OS_Q    * BMS_Q;
extern void * MsgGrp_BMS[BMS_QUEUE_N];

//主485控制互斥信号量
extern OS_MUTEX * MASTER_485_CONTEROL_MUT;


//线程任务名
extern void task_thread_act(void *p_arg);
extern void task_thread_tel(void *p_arg);
extern void task_thread_cam(void *p_arg);
extern void task_thread_ult(void *p_arg);
extern void task_thread_hum(void *p_arg);
extern void task_thread_gyr(void *p_arg);
extern void task_thread_bms(void *p_arg);


#endif
