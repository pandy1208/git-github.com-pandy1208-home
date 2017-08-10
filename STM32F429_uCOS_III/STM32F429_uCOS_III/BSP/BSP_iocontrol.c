#include "BSP_iocontrol.h"
#include "stm32f4xx.h"
 

int BSP_IO_Init(void);
int	BSP_IO_read(const char *cmd,unsigned char lenth,unsigned char** rev);
int	BSP_IO_write(const char *cmd,unsigned char lenth,unsigned char** rev);

/*
	函数名:BSP_IO_Init
	功能:初始化所需的IO口
*/
int BSP_IO_Init(void)
{
	/*初始化IO*/
	
	return 0;
}


/*
	函数名:BSP_IO_read
	功能:读信息
	参数unsigned char *cmd指令指针，unsigned char lenth指令长度,unsigned char** rev接收到数据
	函数返回值:成功0 失败-1
*/
int	BSP_IO_read(const char *cmd,unsigned char lenth,unsigned char** rev)
{
	/*读取CAM数据*/
	
	return -1;
	
	return 0;
}


/*
	函数名:BSP_CAM_write
	功能:设置摄像头设备（如果需要设置的话请填写）
	参数unsigned char *cmd指令指针，unsigned char lenth指令长度,unsigned char** rev回应结果
	函数返回值:成功0 失败-1
*/
int	BSP_IO_write(const char *cmd,unsigned char lenth,unsigned char** rev)
{
	/*写CAM指令*/
	
	return -1;
	
	return 0;
}
