#include  <includes.h>
#include 	"task.h"

void task_thread_bms(void *p_arg)
{
		OS_ERR err;
		OS_MSG_SIZE size;
		unsigned char *p_rev,*p_data;//p_rev指向动态内存头结点用于释放内存
																			//p_data指向数据用于查找
		p_rev = OSQPend(BMS_Q,0,OS_OPT_PEND_BLOCKING,&size,(CPU_TS*)0,&err);		//阻塞等待管道BMS_Q数据
		/*数据处理*/
		

    
	
		/*数据处理*/
		OSMemPut(buf_m485,p_rev,&err);					//释放内存，内存池为buf_m485
}
