#include "BSP_ultrasonic.h"
#include "stm32f4xx.h"
#include "task_variable.h"


int BSP_ULT_Init(void);
int	BSP_ULT_read(const char *cmd,unsigned char lenth,unsigned char** rev);
int	BSP_ULT_write(const char *cmd,unsigned char lenth,unsigned char** rev);

static void uart_send_chars(int n,const char *data);

/*
	函数名:BSP_ULT_Init
	功能:初始化电池管理设备
*/
int BSP_ULT_Init(void)
{
	/*初始化ULT*/
	
	return 0;
}

/*
	函数名:BSP_ULT_read
	功能:读取超声波设备信息
	参数const char *cmd指令指针，unsigned char lenth指令长度,unsigned char** rev接收到数据
	函数返回值:成功0 失败-1
*/
int	BSP_ULT_read(const char *cmd,unsigned char lenth,unsigned char** rev)
{
	/*读取ULT数据*/
	OS_ERR err;
	OS_MSG_SIZE msg_size;
	
	uart_send_chars(lenth,cmd);
       
	*rev = OSQPend(UART_Q,500,OS_OPT_PEND_NON_BLOCKING,&msg_size,0,&err);       
	
	
	/*原始数据处理函数，如果需要处理则填充，无需则忽略*/
	
	
	return -1;
	
	return 0;
}

/*
	函数名:BSP_BMS_write
	功能:设置超声波设备（如果需要设置的话请填写）
	参数const char *cmd指令指针，unsigned char lenth指令长度,unsigned char** rev接收到数据
	函数返回值:成功0 失败-1
*/
int	BSP_ULT_write(const char *cmd,unsigned char lenth,unsigned char** rev)
{
	/*写BMS指令*/
	OS_ERR err;
	OS_MSG_SIZE msg_size;
	   
	uart_send_chars(lenth,cmd);
	
	*rev = OSQPend(UART_Q,500,OS_OPT_PEND_NON_BLOCKING,&msg_size,0,&err);       
	
	
	/*原始数据处理函数，如果需要处理则填充，无需则忽略*/
	
	
	return -1;
	
	return 0;
}

/*
	函数功能: 超声波串口外设发送数据
	参数:int n 发送个数,const char * data指令指针
*/
static void uart_send_chars(int n,const char *data)
{
/*具体串口操作，以下只是一个对USART1操作的例子*/
	while( n > 0)
	{
		while((USART1->SR&USART_SR_TXE) == 0);
		USART1->DR =  *data;
		data++;
		n--;
	}
	while((USART1->SR&USART_SR_TC) == 0);	
}

