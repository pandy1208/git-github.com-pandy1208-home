#include "BSP_humiture.h"
#include "stm32f4xx.h"
#include "task_variable.h"


#define master485_ctrl_send()  	/*设置主485为发送模式*/  
#define master485_ctrl_revice()  	/*设置主485为接收模式*/ 



int BSP_HUM_Init(void);
int	BSP_HUM_read(const char *cmd,unsigned char lenth,unsigned char** rev);
int	BSP_HUM_write(const char *cmd,unsigned char lenth,unsigned char** rev);

static void master485_send_chars(int n,const char *data);

/*
	函数名:BSP_HUM_Init
	功能:初始化电池管理设备
*/
int BSP_HUM_Init(void)
{
	/*初始化CAM*/
	
	return 0;
}


/*
	函数名:BSP_HUM_read
	功能:读取温湿度设备信息
	参数const char *cmd指令指针，unsigned char lenth指令长度,unsigned char** rev接收到数据
	函数返回值:成功0 失败-1
*/
int	BSP_HUM_read(const char *cmd,unsigned char lenth,unsigned char** rev)
{
	/*读取HUM数据*/
	//获取MASTER_485互斥锁
	OS_ERR err;
	OS_MSG_SIZE msg_size;
	OSMutexPend(MASTER_485_CONTEROL_MUT,0,OS_OPT_PEND_BLOCKING,0,&err);
	
	master485_ctrl_send(); 
	master485_send_chars(lenth, cmd);
	master485_ctrl_revice();        
	*rev = OSQPend(M485_Q,500,OS_OPT_PEND_NON_BLOCKING,&msg_size,0,&err);       
	OSMutexPost(MASTER_485_CONTEROL_MUT,OS_OPT_POST_NONE,&err);
	
	
	/*原始数据处理函数，如果需要处理则填充，无需则忽略*/
	
	
	return -1;
	
	return 0;
}


/*
	函数名:BSP_HUM_write
	功能:设置温湿度设备（如果需要设置的话请填写）
	参数const char *cmd指令指针，unsigned char lenth指令长度,unsigned char** rev回应结果
	函数返回值:成功0 失败-1
*/
int	BSP_HUM_write(const char *cmd,unsigned char lenth,unsigned char** rev)
{
	/*写HUM指令*/
	//获取MASTER_485互斥锁
	OS_ERR err;
	OS_MSG_SIZE msg_size;
	OSMutexPend(MASTER_485_CONTEROL_MUT,0,OS_OPT_PEND_BLOCKING,0,&err);
	
	master485_ctrl_send(); 
	master485_send_chars(lenth, cmd);
	master485_ctrl_revice();        
	*rev = OSQPend(M485_Q,500,OS_OPT_PEND_NON_BLOCKING,&msg_size,0,&err);       
	OSMutexPost(MASTER_485_CONTEROL_MUT,OS_OPT_POST_NONE,&err);
	
	
	/*原始数据处理函数，如果需要处理则填充，无需则忽略*/
	
	
	
	return -1;
	
	return 0;
}

static void master485_send_chars(int n,const char *data)
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
