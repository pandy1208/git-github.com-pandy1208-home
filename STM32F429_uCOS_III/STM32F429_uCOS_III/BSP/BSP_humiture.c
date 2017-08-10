#include "BSP_humiture.h"
#include "stm32f4xx.h"
#include "task_variable.h"


#define master485_ctrl_send()  	/*������485Ϊ����ģʽ*/  
#define master485_ctrl_revice()  	/*������485Ϊ����ģʽ*/ 



int BSP_HUM_Init(void);
int	BSP_HUM_read(const char *cmd,unsigned char lenth,unsigned char** rev);
int	BSP_HUM_write(const char *cmd,unsigned char lenth,unsigned char** rev);

static void master485_send_chars(int n,const char *data);

/*
	������:BSP_HUM_Init
	����:��ʼ����ع����豸
*/
int BSP_HUM_Init(void)
{
	/*��ʼ��CAM*/
	
	return 0;
}


/*
	������:BSP_HUM_read
	����:��ȡ��ʪ���豸��Ϣ
	����const char *cmdָ��ָ�룬unsigned char lenthָ���,unsigned char** rev���յ�����
	��������ֵ:�ɹ�0 ʧ��-1
*/
int	BSP_HUM_read(const char *cmd,unsigned char lenth,unsigned char** rev)
{
	/*��ȡHUM����*/
	//��ȡMASTER_485������
	OS_ERR err;
	OS_MSG_SIZE msg_size;
	OSMutexPend(MASTER_485_CONTEROL_MUT,0,OS_OPT_PEND_BLOCKING,0,&err);
	
	master485_ctrl_send(); 
	master485_send_chars(lenth, cmd);
	master485_ctrl_revice();        
	*rev = OSQPend(M485_Q,500,OS_OPT_PEND_NON_BLOCKING,&msg_size,0,&err);       
	OSMutexPost(MASTER_485_CONTEROL_MUT,OS_OPT_POST_NONE,&err);
	
	
	/*ԭʼ���ݴ������������Ҫ��������䣬���������*/
	
	
	return -1;
	
	return 0;
}


/*
	������:BSP_HUM_write
	����:������ʪ���豸�������Ҫ���õĻ�����д��
	����const char *cmdָ��ָ�룬unsigned char lenthָ���,unsigned char** rev��Ӧ���
	��������ֵ:�ɹ�0 ʧ��-1
*/
int	BSP_HUM_write(const char *cmd,unsigned char lenth,unsigned char** rev)
{
	/*дHUMָ��*/
	//��ȡMASTER_485������
	OS_ERR err;
	OS_MSG_SIZE msg_size;
	OSMutexPend(MASTER_485_CONTEROL_MUT,0,OS_OPT_PEND_BLOCKING,0,&err);
	
	master485_ctrl_send(); 
	master485_send_chars(lenth, cmd);
	master485_ctrl_revice();        
	*rev = OSQPend(M485_Q,500,OS_OPT_PEND_NON_BLOCKING,&msg_size,0,&err);       
	OSMutexPost(MASTER_485_CONTEROL_MUT,OS_OPT_POST_NONE,&err);
	
	
	/*ԭʼ���ݴ������������Ҫ��������䣬���������*/
	
	
	
	return -1;
	
	return 0;
}

static void master485_send_chars(int n,const char *data)
{
/*���崮�ڲ���������ֻ��һ����USART1����������*/
	while( n > 0)
	{
		while((USART1->SR&USART_SR_TXE) == 0);
		USART1->DR =  *data;
		data++;
		n--;
	}
	while((USART1->SR&USART_SR_TC) == 0);	
}
