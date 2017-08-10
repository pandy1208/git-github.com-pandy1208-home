#include "BSP_ultrasonic.h"
#include "stm32f4xx.h"
#include "task_variable.h"


int BSP_ULT_Init(void);
int	BSP_ULT_read(const char *cmd,unsigned char lenth,unsigned char** rev);
int	BSP_ULT_write(const char *cmd,unsigned char lenth,unsigned char** rev);

static void uart_send_chars(int n,const char *data);

/*
	������:BSP_ULT_Init
	����:��ʼ����ع����豸
*/
int BSP_ULT_Init(void)
{
	/*��ʼ��ULT*/
	
	return 0;
}

/*
	������:BSP_ULT_read
	����:��ȡ�������豸��Ϣ
	����const char *cmdָ��ָ�룬unsigned char lenthָ���,unsigned char** rev���յ�����
	��������ֵ:�ɹ�0 ʧ��-1
*/
int	BSP_ULT_read(const char *cmd,unsigned char lenth,unsigned char** rev)
{
	/*��ȡULT����*/
	OS_ERR err;
	OS_MSG_SIZE msg_size;
	
	uart_send_chars(lenth,cmd);
       
	*rev = OSQPend(UART_Q,500,OS_OPT_PEND_NON_BLOCKING,&msg_size,0,&err);       
	
	
	/*ԭʼ���ݴ������������Ҫ��������䣬���������*/
	
	
	return -1;
	
	return 0;
}

/*
	������:BSP_BMS_write
	����:���ó������豸�������Ҫ���õĻ�����д��
	����const char *cmdָ��ָ�룬unsigned char lenthָ���,unsigned char** rev���յ�����
	��������ֵ:�ɹ�0 ʧ��-1
*/
int	BSP_ULT_write(const char *cmd,unsigned char lenth,unsigned char** rev)
{
	/*дBMSָ��*/
	OS_ERR err;
	OS_MSG_SIZE msg_size;
	   
	uart_send_chars(lenth,cmd);
	
	*rev = OSQPend(UART_Q,500,OS_OPT_PEND_NON_BLOCKING,&msg_size,0,&err);       
	
	
	/*ԭʼ���ݴ������������Ҫ��������䣬���������*/
	
	
	return -1;
	
	return 0;
}

/*
	��������: �������������跢������
	����:int n ���͸���,const char * dataָ��ָ��
*/
static void uart_send_chars(int n,const char *data)
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

