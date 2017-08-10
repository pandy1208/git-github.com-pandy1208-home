#include "BSP_gyroscope.h"
#include "stm32f4xx.h"
#include "task_variable.h"

int BSP_GYR_Init(void);
int	BSP_GYR_read(const char *cmd,unsigned char lenth,unsigned char** rev);
int	BSP_GYR_write(const char *cmd,unsigned char lenth,unsigned char** rev);

static void i2c_send_chars(int n,const char *data);

/*
	函数名:BSP_GYR_Init
	功能:初始化电池管理设备
*/
int BSP_GYR_Init(void)
{
	/*初始化陀螺仪*/
	
	return 0;
}


/*
	函数名:BSP_GYR_read
	功能:读取陀螺仪设备信息
	参数const char *cmd指令指针，unsigned char lenth指令长度,unsigned char** rev接收到数据
	函数返回值:成功0 失败-1
*/
int	BSP_GYR_read(const char *cmd,unsigned char lenth,unsigned char** rev)
{
	/*读取GYR数据*/
		/*读取CAM数据*/
	OS_ERR err;
	OS_MSG_SIZE msg_size;
	
	i2c_send_chars( lenth, cmd);
	
	//I2C或者SIP操作     
	*rev = OSQPend(I2C_Q,500,OS_OPT_PEND_NON_BLOCKING,&msg_size,0,&err);       
	
	
	/*原始数据处理函数，如果需要处理则填充，无需则忽略*/
	
	return -1;
	
	return 0;
}


/*
	函数名:BSP_GYR_write
	功能:设置陀螺仪设备（如果需要设置的话请填写）
	参数const char *cmd指令指针，unsigned char lenth指令长度,unsigned char** rev回应结果
	函数返回值:成功0 失败-1
*/
int	BSP_GYR_write(const char *cmd,unsigned char lenth,unsigned char** rev)
{
	/*写GYR指令*/
	
	return -1;
	
	return 0;
}

/*
	函数名:i2c_send_chars
	功能:i2c发送数据函数
	参数unsigned char *cmd指令指针，unsigned char lenth指令长度,unsigned char** rev回应结果
	函数返回值:成功0 失败-1
*/
static void i2c_send_chars(int n,const char *data)
//static void spi_send_chars(int n,const char *data)
{

	
	
	
}
