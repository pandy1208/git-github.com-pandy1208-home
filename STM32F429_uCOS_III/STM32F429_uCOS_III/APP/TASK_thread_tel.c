#include  <includes.h>
#include <task_variable.h>

void task_thread_tel(void *p_arg)
{
		OS_ERR err;
		OS_MSG_SIZE size;
		unsigned char *p_rev,*p_data;//p_rev指向动态内存头结点用于释放内存
																			//p_data指向数据用于查找
		p_rev = OSQPend(TEL_Q,0,OS_OPT_PEND_BLOCKING,&size,(CPU_TS*)0,&err);		//阻塞等待管道TEL_Q数据
		/*数据处理*/
		

    
	
		/*数据处理*/
		OSMemPut(buf_s485,p_rev,&err);					//释放内存，内存池为buf_s485
}
