#include "BSP_iocontrol.h"
#include "stm32f4xx.h"
 

int BSP_IO_Init(void);
int	BSP_IO_read(const char *cmd,unsigned char lenth,unsigned char** rev);
int	BSP_IO_write(const char *cmd,unsigned char lenth,unsigned char** rev);

/*
	������:BSP_IO_Init
	����:��ʼ�������IO��
*/
int BSP_IO_Init(void)
{
	/*��ʼ��IO*/
	
	return 0;
}


/*
	������:BSP_IO_read
	����:����Ϣ
	����unsigned char *cmdָ��ָ�룬unsigned char lenthָ���,unsigned char** rev���յ�����
	��������ֵ:�ɹ�0 ʧ��-1
*/
int	BSP_IO_read(const char *cmd,unsigned char lenth,unsigned char** rev)
{
	/*��ȡCAM����*/
	
	return -1;
	
	return 0;
}


/*
	������:BSP_CAM_write
	����:��������ͷ�豸�������Ҫ���õĻ�����д��
	����unsigned char *cmdָ��ָ�룬unsigned char lenthָ���,unsigned char** rev��Ӧ���
	��������ֵ:�ɹ�0 ʧ��-1
*/
int	BSP_IO_write(const char *cmd,unsigned char lenth,unsigned char** rev)
{
	/*дCAMָ��*/
	
	return -1;
	
	return 0;
}
