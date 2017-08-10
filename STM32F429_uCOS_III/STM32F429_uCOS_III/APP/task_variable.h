#ifndef TASK_VARIABLE_H
#define	TASK_VARIABLE_H

#include  <includes.h>
#include 	"app_cfg.h"
#include  <os.h>
#include <stm32f4xx.h>

typedef unsigned char INT8U;


//DEBUG��Ϣ����
#define DEBUG_PRINT 0

//��485�ڴ��
#define NBLKS_m485	5
#define BLKSIZE_m485	256
extern OS_MEM * buf_m485;
extern unsigned char	part_buf_m485[NBLKS_m485][BLKSIZE_m485];

//��485�ڴ��
#define NBLKS_s485	5
#define BLKSIZE_s485	128
extern OS_MEM * buf_s485;
extern unsigned char	part_buf_s485[NBLKS_s485][BLKSIZE_s485];

//uart�ڴ��
#define NBLKS_uart	5
#define BLKSIZE_uart	128
extern OS_MEM * buf_uart;
extern unsigned char	part_buf_uart[NBLKS_uart][BLKSIZE_uart];

//i2c�ڴ��
#define NBLKS_i2c	5
#define BLKSIZE_i2c	256
extern OS_MEM * buf_i2c;
extern unsigned char	part_buf_i2c[NBLKS_i2c][BLKSIZE_i2c];

//������Ϣ�ڴ��
#define NBLKS_act	5
#define BLKSIZE_act	128
extern OS_MEM * buf_act;
extern unsigned char	part_buf_act[NBLKS_act][BLKSIZE_act];


//�����߳���Ϣ����
#define ACT_QUEUE_N 5
extern 	OS_Q    * ACT_Q;
extern void * MsgGrp_ACT[ACT_QUEUE_N];

//ң���߳���Ϣ����
#define TEL_QUEUE_N 5
extern 	OS_Q    * TEL_Q;
extern void * MsgGrp_TEL[TEL_QUEUE_N];

//ͼ�ߴ����߳���Ϣ����
#define CAM_QUEUE_N 5
extern 	OS_Q    * CAM_Q;
extern void * MsgGrp_CAM[CAM_QUEUE_N];

//�����������߳���Ϣ����
#define ULT_QUEUE_N 5
extern 	OS_Q    * ULT_Q;
extern void * MsgGrp_ULT[ULT_QUEUE_N];

//��ʪ�ȴ����߳���Ϣ����
#define HUM_QUEUE_N 5
extern 	OS_Q    * HUM_Q;
extern void * MsgGrp_HUM[HUM_QUEUE_N];

//�����Ǵ����߳���Ϣ����
#define GYR_QUEUE_N 5
extern 	OS_Q    * GYR_Q;
extern void * MsgGrp_GYR[GYR_QUEUE_N];

//��ع����߳���Ϣ����
#define BMS_QUEUE_N 5
extern 	OS_Q    * BMS_Q;
extern void * MsgGrp_BMS[BMS_QUEUE_N];

 //��485�����жϵ��ɼ��̵߳Ķ���
#define M485_QUEUE_N 5
extern	OS_Q    * M485_Q;
extern void * MsgGrp_M485[M485_QUEUE_N];

 //��485�����жϵ��ɼ��̵߳Ķ���
#define S485_QUEUE_N 5
extern 	OS_Q    * S485_Q;
extern void * MsgGrp_S485[S485_QUEUE_N];
 
  //I2C�жϵ��ɼ��̵߳Ķ���
#define I2C_QUEUE_N 5
extern 	OS_Q    * I2C_Q;
extern void * MsgGrp_I2C[I2C_QUEUE_N];
 
  //UART�����жϵ��ɼ��̵߳Ķ���
#define UART_QUEUE_N 5
extern 	OS_Q    * UART_Q;
extern void * MsgGrp_UART[UART_QUEUE_N];


//��485���ƻ����ź���
extern OS_MUTEX * MASTER_485_CONTEROL_MUT;


//�߳�������
extern void task_thread_act(void *p_arg);
extern void task_thread_tel(void *p_arg);
extern void task_thread_cam(void *p_arg);
extern void task_thread_ult(void *p_arg);
extern void task_thread_hum(void *p_arg);
extern void task_thread_gyr(void *p_arg);
extern void task_thread_bms(void *p_arg);


extern int BSP_CAM_Init(void);
extern int	BSP_CAM_read(const char *cmd,unsigned char lenth,unsigned char** rev);
extern int	BSP_CAM_write(const char *cmd,unsigned char lenth,unsigned char** rev);
extern int BSP_BMS_Init(void);
extern int	BSP_BMS_read(const char *cmd,unsigned char lenth,unsigned char** rev);
extern int	BSP_BMS_write(const char *cmd,unsigned char lenth,unsigned char** rev);
extern int BSP_GYR_Init(void);
extern int	BSP_GYR_read(const char *cmd,unsigned char lenth,unsigned char** rev);
extern int	BSP_GYR_write(const char *cmd,unsigned char lenth,unsigned char** rev);
extern int BSP_HUM_Init(void);
extern int	BSP_HUM_read(const char *cmd,unsigned char lenth,unsigned char** rev);
extern int	BSP_HUM_write(const char *cmd,unsigned char lenth,unsigned char** rev);
extern int BSP_TEL_Init(void);
extern int	BSP_TEL_read(const char *cmd,unsigned char lenth,unsigned char** rev);
extern int	BSP_TEL_write(const char *cmd,unsigned char lenth,unsigned char** rev);
extern int BSP_IO_Init(void);
extern int	BSP_IO_read(const char *cmd,unsigned char lenth,unsigned char** rev);
extern int	BSP_IO_write(const char *cmd,unsigned char lenth,unsigned char** rev);
extern int BSP_ULT_Init(void);
extern int	BSP_ULT_read(const char *cmd,unsigned char lenth,unsigned char** rev);
extern int	BSP_ULT_write(const char *cmd,unsigned char lenth,unsigned char** rev);

#endif
