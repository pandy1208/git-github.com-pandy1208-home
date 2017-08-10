#include "BSP_telecontrol.h"
#include "stm32f4xx.h"
#include "task_variable.h"

#define slave485_ctrl_send()  	/*设置从485为发送模式*/  
#define slave485_ctrl_revice()  	/*设置从485为接收模式*/ 

int BSP_TEL_Init(void);
int	BSP_TEL_read(const char *cmd,unsigned char lenth,unsigned char** rev);
int	BSP_TEL_write(const char *cmd,unsigned char lenth,unsigned char** rev);

static void slave485_send_chars(int n,const char *data);

/*
	函数名:BSP_BMS_Init
	功能:初始化电池管理设备
*/
int BSP_TEL_Init(void)
{
	/*初始化TEL*/
	
	return 0;
}


/*
	函数名:BSP_TEL_read
	功能:读取遥控设备信息
	参数const char *cmd指令指针，unsigned char lenth指令长度,unsigned char** rev接收到数据
	函数返回值:成功0 失败-1
*/
int	BSP_TEL_read(const char *cmd,unsigned char lenth,unsigned char** rev)
{
	/*读取BMS数据*/
	OS_ERR err;
	OS_MSG_SIZE msg_size;
	
	slave485_ctrl_revice();        
	*rev = OSQPend(S485_Q,500,OS_OPT_PEND_NON_BLOCKING,&msg_size,0,&err);       
	
	
	/*原始数据处理函数，如果需要处理则填充，无需则忽略*/
	
	
	
	
	return -1;
	
	return 0;
}


/*
	函数名:BSP_BMS_write
	功能:设置电池管理设备（如果需要设置的话请填写）
	参数const char *cmd指令指针，unsigned char lenth指令长度,unsigned char** rev接收到数据
	函数返回值:成功0 失败-1
*/
int	BSP_TEL_write(const char *cmd,unsigned char lenth,unsigned char** rev)
{
	
	return -1;
	
	return 0;
}

/*
	函数功能: 电池管理串口外设发送数据
	参数:int n 发送个数,const char * data指令指针
*/
static void slave485_send_chars(int n,const char *data)
{

}
