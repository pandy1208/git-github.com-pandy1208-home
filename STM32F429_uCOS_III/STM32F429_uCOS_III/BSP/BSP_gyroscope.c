#include "BSP_gyroscope.h"
#include "stm32f4xx.h"
#include "task_variable.h"

int BSP_GYR_Init(void);
int	BSP_GYR_read(const char *cmd,unsigned char lenth,unsigned char** rev);
int	BSP_GYR_write(const char *cmd,unsigned char lenth,unsigned char** rev);

static void i2c_send_chars(int n,const char *data);

/*
	������:BSP_GYR_Init
	����:��ʼ����ع����豸
*/
int BSP_GYR_Init(void)
{
	/*��ʼ��������*/
	
	return 0;
}


/*
	������:BSP_GYR_read
	����:��ȡ�������豸��Ϣ
	����const char *cmdָ��ָ�룬unsigned char lenthָ���,unsigned char** rev���յ�����
	��������ֵ:�ɹ�0 ʧ��-1
*/
int	BSP_GYR_read(const char *cmd,unsigned char lenth,unsigned char** rev)
{
	/*��ȡGYR����*/
		/*��ȡCAM����*/
	OS_ERR err;
	OS_MSG_SIZE msg_size;
	
	i2c_send_chars( lenth, cmd);
	
	//I2C����SIP����     
	*rev = OSQPend(I2C_Q,500,OS_OPT_PEND_NON_BLOCKING,&msg_size,0,&err);       
	
	
	/*ԭʼ���ݴ������������Ҫ��������䣬���������*/
	
	return -1;
	
	return 0;
}


/*
	������:BSP_GYR_write
	����:�����������豸�������Ҫ���õĻ�����д��
	����const char *cmdָ��ָ�룬unsigned char lenthָ���,unsigned char** rev��Ӧ���
	��������ֵ:�ɹ�0 ʧ��-1
*/
int	BSP_GYR_write(const char *cmd,unsigned char lenth,unsigned char** rev)
{
	/*дGYRָ��*/
	
	return -1;
	
	return 0;
}

/*
	������:i2c_send_chars
	����:i2c�������ݺ���
	����unsigned char *cmdָ��ָ�룬unsigned char lenthָ���,unsigned char** rev��Ӧ���
	��������ֵ:�ɹ�0 ʧ��-1
*/
static void i2c_send_chars(int n,const char *data)
//static void spi_send_chars(int n,const char *data)
{

	
	
	
}
