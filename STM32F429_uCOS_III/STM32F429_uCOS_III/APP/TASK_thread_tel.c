#include  <includes.h>
#include <task_variable.h>

void task_thread_tel(void *p_arg)
{
		OS_ERR err;
		OS_MSG_SIZE size;
		unsigned char *p_rev,*p_data;//p_revָ��̬�ڴ�ͷ��������ͷ��ڴ�
																			//p_dataָ���������ڲ���
		p_rev = OSQPend(TEL_Q,0,OS_OPT_PEND_BLOCKING,&size,(CPU_TS*)0,&err);		//�����ȴ��ܵ�TEL_Q����
		/*���ݴ���*/
		

    
	
		/*���ݴ���*/
		OSMemPut(buf_s485,p_rev,&err);					//�ͷ��ڴ棬�ڴ��Ϊbuf_s485
}
