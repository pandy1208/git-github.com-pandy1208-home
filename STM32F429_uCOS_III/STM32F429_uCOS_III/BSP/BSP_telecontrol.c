#include "BSP_telecontrol.h"
#include "stm32f4xx.h"
#include "task_variable.h"

#define slave485_ctrl_send()  	/*���ô�485Ϊ����ģʽ*/  
#define slave485_ctrl_revice()  	/*���ô�485Ϊ����ģʽ*/ 

int BSP_TEL_Init(void);
int	BSP_TEL_read(const char *cmd,unsigned char lenth,unsigned char** rev);
int	BSP_TEL_write(const char *cmd,unsigned char lenth,unsigned char** rev);

static void slave485_send_chars(int n,const char *data);

/*
	������:BSP_BMS_Init
	����:��ʼ����ع����豸
*/
int BSP_TEL_Init(void)
{
	/*��ʼ��TEL*/
	
	return 0;
}


/*
	������:BSP_TEL_read
	����:��ȡң���豸��Ϣ
	����const char *cmdָ��ָ�룬unsigned char lenthָ���,unsigned char** rev���յ�����
	��������ֵ:�ɹ�0 ʧ��-1
*/
int	BSP_TEL_read(const char *cmd,unsigned char lenth,unsigned char** rev)
{
	/*��ȡBMS����*/
	OS_ERR err;
	OS_MSG_SIZE msg_size;
	
	slave485_ctrl_revice();        
	*rev = OSQPend(S485_Q,500,OS_OPT_PEND_NON_BLOCKING,&msg_size,0,&err);       
	
	
	/*ԭʼ���ݴ������������Ҫ��������䣬���������*/
	
	
	
	
	return -1;
	
	return 0;
}


/*
	������:BSP_BMS_write
	����:���õ�ع����豸�������Ҫ���õĻ�����д��
	����const char *cmdָ��ָ�룬unsigned char lenthָ���,unsigned char** rev���յ�����
	��������ֵ:�ɹ�0 ʧ��-1
*/
int	BSP_TEL_write(const char *cmd,unsigned char lenth,unsigned char** rev)
{
	
	return -1;
	
	return 0;
}

/*
	��������: ��ع��������跢������
	����:int n ���͸���,const char * dataָ��ָ��
*/
static void slave485_send_chars(int n,const char *data)
{

}
