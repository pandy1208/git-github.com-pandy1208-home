#include <unistd.h>  
#include <stdlib.h>  
#include <fcntl.h>  
#include <limits.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <stdio.h>  
#include <string.h>  
#include <pthread.h>
  
int ctr_fd = -1;
int rtc_fd = -1; 
int i = 0;
int read_num = 0;
char buffer[1500]; 
char CatCtrlDate = 0,CatTcpState = 0,CatMasterDate = 0;
pthread_t console_thread;
void task_console_thread(void *p_arg)
{
	char *rev = NULL;
	while(1)
	{
		read_num = read(rtc_fd, buffer, 1500); 

		rev = NULL;
		rev = strstr(buffer,"tcpstate");
	 	if(rev != NULL){
			CatTcpState = 1;
			CatCtrlDate = 0;
			CatMasterDate = 0;
			printf("捕捉tcp连接状态\r\n");
	 	} 

		rev = NULL;
		rev = strstr(buffer,"ctrldate");
	 	if(rev != NULL){
			printf("开始捕捉触摸屏数据\r\n");
			CatCtrlDate = 1;
			CatTcpState = 0;
			CatMasterDate = 0;
	 	}

		rev = NULL;
		rev = strstr(buffer,"master485date");
	 	if(rev != NULL){
			printf("开始捕捉电表、热量表等数据\r\n");
			CatCtrlDate = 0;
			CatTcpState = 0;
			CatMasterDate = 1;
	 	}

		rev = NULL;
		rev = strstr(buffer,"RepeaterShutdown");
	 	if(rev != NULL){
			CatTcpState = 0;
			CatCtrlDate = 0;
			CatMasterDate = 0;
			printf("中继器已关机！\r\n");
	 	} 

		rev = NULL;
		rev = strstr(buffer,"RepeaterRestart");
	 	if(rev != NULL){
			CatTcpState = 0;
			CatCtrlDate = 0;
			CatMasterDate = 0;
			printf("中继器正在重启!\r\n请重新选择捕捉内容！\r\n");
	 	} 


		if(CatTcpState){
			printf("cat date\r\n");
			for(i = 0;i < read_num;i ++)
				printf("%c",buffer[i]);
			printf("\r\n");
		}

		if(CatCtrlDate){
			for(i = 0;i < read_num;i ++)
				printf("%02x ",buffer[i]);
			printf("\r\n");
		}

		if(CatMasterDate){
			for(i = 0;i < read_num;i ++)
				printf("%02x ",buffer[i]);
			printf("\r\n");
		}

	}
}

int main()  
{  
    const char *r_to_c = "repeater_to_console"; 
	const char *c_to_r = "console_to_repeater";
    int res = 0;  
    const int open_mode = O_RDWR;  
    int bytes_sent = 0;   
  
	int err = 0;
	err = pthread_create(&console_thread,NULL,(void*)task_console_thread,NULL);

    if(access(r_to_c, F_OK) == -1)  
    {  
        //管道文件不存在  
        //创建命名管道  
        res = mkfifo(r_to_c, 0777);  
        if(res != 0)  
        {  
            fprintf(stderr, "Could not create fifo %s\n", r_to_c);  
            exit(EXIT_FAILURE);  
        }  
    }  

	 if(access(c_to_r, F_OK) == -1)  
    {  
        //管道文件不存在  
        //创建命名管道  
        res = mkfifo(c_to_r, 0777);  
        if(res != 0)  
        {  
            fprintf(stderr, "Could not create fifo %s\n", c_to_r);  
            exit(EXIT_FAILURE);  
        }  
    }  
   
    //打开FIFO文件  
    ctr_fd = open(c_to_r, open_mode); 
	//打开FIFO文件  
    rtc_fd = open(r_to_c, open_mode); 
 
  	while(1)
	{
		if(ctr_fd != -1)  
		{  
		    //向FIFO文件写数据  
			gets(buffer);

			printf("cmd is :%s\r\n",buffer);

			if( !strcmp(buffer,"help") ){
			printf("id		----	打印本进程ID\r\n");
			printf("exit	----	关闭中继器\r\n");
			printf("ctrldate---- 捕捉触摸屏数据\r\n");
			printf("master485date---- 捕捉电表、热量表数据\r\n");
			printf("tcpstate---- 捕捉tcp连接状态\r\n");
			printf("回车退出当前命令\r\n");
			}

			if( !strcmp(buffer,"exit") ){

				printf("关闭中继器\r\n");
	
			}

			if( !strcmp(buffer,"tcpstate") ){
				CatTcpState = 1;
				printf("捕捉tcp连接状态\r\n");
			}
			else{
				//printf("停止tcp连接状态\r\n");
				CatTcpState = 0;			
			}

			if( !strcmp(buffer,"ctrldate") ){
				printf("开始捕捉触摸屏数据\r\n");
				CatCtrlDate = 1;
			}
			else{
				CatCtrlDate = 0;
			}

			if( !strcmp(buffer,"master485date") ){
				printf("捕捉电表、热量表数据\r\n");
				CatMasterDate = 1;
			}
			else{
				CatMasterDate = 0;
				//printf("停止捕捉触摸屏数据\r\n");
			}

		    res = write(ctr_fd, buffer, sizeof(buffer));  
		    if(res == -1)  
		    {  
		        fprintf(stderr, "Write error on pipe\n");  
		        exit(EXIT_FAILURE);  
		    }  
	   
		}  
		else  
		    exit(EXIT_FAILURE);  
	}
  	close(ctr_fd);
	close(rtc_fd);
    exit(EXIT_SUCCESS);  
} 
