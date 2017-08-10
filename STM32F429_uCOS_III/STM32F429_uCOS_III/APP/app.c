/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  <includes.h>
#include "task_variable.h"
#include <os.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/
typedef unsigned char INT8U;

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/
//主485内存池
#define NBLKS_m485	5
#define BLKSIZE_m485	256
 OS_MEM * buf_m485;
 INT8U	part_buf_m485[NBLKS_m485][BLKSIZE_m485];

//从485内存池
#define NBLKS_s485	5
#define BLKSIZE_s485	128
 OS_MEM * buf_s485;
 INT8U	part_buf_s485[NBLKS_s485][BLKSIZE_s485];

//uart内存池
#define NBLKS_uart	5
#define BLKSIZE_uart	128
 OS_MEM * buf_uart;
 INT8U	part_buf_uart[NBLKS_uart][BLKSIZE_uart];

//i2c内存池
#define NBLKS_i2c	5
#define BLKSIZE_i2c	256
 OS_MEM * buf_i2c;
 INT8U	part_buf_i2c[NBLKS_i2c][BLKSIZE_i2c];

//动作消息内存池
#define NBLKS_act	5
#define BLKSIZE_act	128
 OS_MEM * buf_act;
 INT8U	part_buf_act[NBLKS_act][BLKSIZE_act];


//动作线程消息队列
#define ACT_QUEUE_N 5
 	OS_Q    * ACT_Q;
 void * MsgGrp_ACT[ACT_QUEUE_N];

//遥控线程消息队列
#define TEL_QUEUE_N 5
 	OS_Q    * TEL_Q;
 void * MsgGrp_TEL[TEL_QUEUE_N];

//图线处理线程消息队列
#define CAM_QUEUE_N 5
 	OS_Q    * CAM_Q;
 void * MsgGrp_CAM[CAM_QUEUE_N];

//超声波处理线程消息队列
#define ULT_QUEUE_N 5
 	OS_Q    * ULT_Q;
 void * MsgGrp_ULT[ULT_QUEUE_N];

//温湿度处理线程消息队列
#define HUM_QUEUE_N 5
 	OS_Q    * HUM_Q;
 void * MsgGrp_HUM[HUM_QUEUE_N];

//陀螺仪处理线程消息队列
#define GYR_QUEUE_N 5
 	OS_Q    * GYR_Q;
 void * MsgGrp_GYR[GYR_QUEUE_N];

//电池管理线程消息队列
#define BMS_QUEUE_N 5
 	OS_Q    * BMS_Q;
 void * MsgGrp_BMS[BMS_QUEUE_N];

 //主485串口中断到采集线程的队列
#define M485_QUEUE_N 5
 	OS_Q    * M485_Q;
 void * MsgGrp_M485[M485_QUEUE_N];

 //从485串口中断到采集线程的队列
#define S485_QUEUE_N 5
 	OS_Q    * S485_Q;
 void * MsgGrp_S485[S485_QUEUE_N];
 
  //I2C中断到采集线程的队列
#define I2C_QUEUE_N 5
 	OS_Q    * I2C_Q;
 void * MsgGrp_I2C[I2C_QUEUE_N];
 
  //UART串口中断到采集线程的队列
#define UART_QUEUE_N 5
 	OS_Q    * UART_Q;
 void * MsgGrp_UART[UART_QUEUE_N];

//主485控制互斥信号量
 OS_MUTEX * MASTER_485_CONTEROL_MUT;


                                                                /* ----------------- APPLICATION GLOBALS -------------- */
static  OS_TCB   AppTaskStartTCB;
static  CPU_STK  AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];

static  OS_TCB   AppTask1StartTCB;
static  CPU_STK  AppTask1StartStk[APP_CFG_TASK_START_STK_SIZE];

static  OS_TCB   AppTask2StartTCB;
static  CPU_STK  AppTask2StartStk[APP_CFG_TASK_START_STK_SIZE];

static  OS_TCB   AppTask3StartTCB;
static  CPU_STK  AppTask3StartStk[APP_CFG_TASK_START_STK_SIZE];

static  OS_TCB   AppTask4StartTCB;
static  CPU_STK  AppTask4StartStk[APP_CFG_TASK_START_STK_SIZE];

static  OS_TCB   AppTask5StartTCB;
static  CPU_STK  AppTask5StartStk[APP_CFG_TASK_START_STK_SIZE];

static  OS_TCB   AppTask6StartTCB;
static  CPU_STK  AppTask6StartStk[APP_CFG_TASK_START_STK_SIZE];

static  OS_TCB   AppTask7StartTCB;
static  CPU_STK  AppTask7StartStk[APP_CFG_TASK_START_STK_SIZE];

static  OS_TCB   AppTask8StartTCB;
static  CPU_STK  AppTask8StartStk[APP_CFG_TASK_START_STK_SIZE];

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart          (void     *p_arg);
/**/
static void Synchronization_init(void);
static void Bsp_Init(void);

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

int main(void)
{
    OS_ERR  err;

    OSInit(&err);                                               /* Init uC/OS-III.                                      */
		
		/*内存池、队列、信号量初始化*/
		Synchronization_init();
		/*外设初始化*/
		Bsp_Init();
	
    OSTaskCreate((OS_TCB       *)&AppTaskStartTCB,              /* Create the start task                                */
                 (CPU_CHAR     *)"App Task Start",
                 (OS_TASK_PTR   )AppTaskStart,
                 (void         *)0u,
                 (OS_PRIO       )APP_CFG_TASK_START_PRIO,
                 (CPU_STK      *)&AppTaskStartStk[0u],
                 (CPU_STK_SIZE  )AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE / 10u],
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE,
                 (OS_MSG_QTY    )0u,
                 (OS_TICK       )0u,
                 (void         *)0u,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);
								 
		OSTaskCreate((OS_TCB       *)&AppTask1StartTCB,              /* 创建动作线程                                */
                 (CPU_CHAR     *)"App Task1 Start",
                 (OS_TASK_PTR   )task_thread_act,
                 (void         *)0u,
                 (OS_PRIO       )TASK_ACT_SETUP_PRIO,
                 (CPU_STK      *)&AppTask1StartStk[0u],
                 (CPU_STK_SIZE  )AppTask1StartStk[APP_CFG_TASK_START_STK_SIZE / 10u],
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE,
                 (OS_MSG_QTY    )5u,															/*任务现在最大可接收管道消息数量为5*/
                 (OS_TICK       )0u,
                 (void         *)0u,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);
								 
			OSTaskCreate((OS_TCB       *)&AppTask2StartTCB,              /* 创建遥控线程                                 */
                 (CPU_CHAR     *)"App Task2 Start",
                 (OS_TASK_PTR   )task_thread_tel,
                 (void         *)0u,
                 (OS_PRIO       )TASK_TEL_SETUP_PRIO,
                 (CPU_STK      *)&AppTask2StartStk[0u],
                 (CPU_STK_SIZE  )AppTask2StartStk[APP_CFG_TASK_START_STK_SIZE / 10u],
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE,
                 (OS_MSG_QTY    )5u,															/*任务现在最大可接收管道消息数量为5*/
                 (OS_TICK       )0u,
                 (void         *)0u,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);
								 
			OSTaskCreate((OS_TCB       *)&AppTask3StartTCB,              /* 创建图线处理线程                                 */
                 (CPU_CHAR     *)"App Task3 Start",
                 (OS_TASK_PTR   )task_thread_cam,
                 (void         *)0u,
                 (OS_PRIO       )TASK_CAM_SETUP_PRIO,
                 (CPU_STK      *)&AppTask3StartStk[0u],
                 (CPU_STK_SIZE  )AppTask3StartStk[APP_CFG_TASK_START_STK_SIZE / 10u],
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE,
                 (OS_MSG_QTY    )5u,															/*任务现在最大可接收管道消息数量为5*/
                 (OS_TICK       )0u,
                 (void         *)0u,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);
								
			OSTaskCreate((OS_TCB       *)&AppTask4StartTCB,              /* 创建超声波线程                                 */
                 (CPU_CHAR     *)"App Task4 Start",
                 (OS_TASK_PTR   )task_thread_ult,
                 (void         *)0u,
                 (OS_PRIO       )TASK_ULT_SETUP_PRIO,
                 (CPU_STK      *)&AppTask4StartStk[0u],
                 (CPU_STK_SIZE  )AppTask4StartStk[APP_CFG_TASK_START_STK_SIZE / 10u],
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE,
                 (OS_MSG_QTY    )5u,																/*任务现在最大可接收管道消息数量为5*/
                 (OS_TICK       )0u,
                 (void         *)0u,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);
								 
			OSTaskCreate((OS_TCB       *)&AppTask5StartTCB,              /* 创建温湿度处理线程                                 */
                 (CPU_CHAR     *)"App Task5 Start",
                 (OS_TASK_PTR   )task_thread_hum,
                 (void         *)0u,
                 (OS_PRIO       )TASK_HUM_SETUP_PRIO,
                 (CPU_STK      *)&AppTask5StartStk[0u],
                 (CPU_STK_SIZE  )AppTask5StartStk[APP_CFG_TASK_START_STK_SIZE / 10u],
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE,
                 (OS_MSG_QTY    )5u,																/*任务现在最大可接收管道消息数量为5*/
                 (OS_TICK       )0u,
                 (void         *)0u,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);
								 
			OSTaskCreate((OS_TCB       *)&AppTask6StartTCB,              /* 创建陀螺仪线程                                 */
                 (CPU_CHAR     *)"App Task6 Start",
                 (OS_TASK_PTR   )task_thread_gyr,
                 (void         *)0u,
                 (OS_PRIO       )TASK_GYR_SETUP_PRIO,
                 (CPU_STK      *)&AppTask6StartStk[0u],
                 (CPU_STK_SIZE  )AppTask6StartStk[APP_CFG_TASK_START_STK_SIZE / 10u],
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE,
                 (OS_MSG_QTY    )5u,															/*任务现在最大可接收管道消息数量为5*/
                 (OS_TICK       )0u,
                 (void         *)0u,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);
								 
								 
			OSTaskCreate((OS_TCB       *)&AppTask7StartTCB,              /* 创建电池管理线程                                 */
                 (CPU_CHAR     *)"App Task7 Start",
                 (OS_TASK_PTR   )task_thread_bms,
                 (void         *)0u,
                 (OS_PRIO       )TASK_BMS_SETUP_PRIO,
                 (CPU_STK      *)&AppTask7StartStk[0u],
                 (CPU_STK_SIZE  )AppTask7StartStk[APP_CFG_TASK_START_STK_SIZE / 10u],
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE,
                 (OS_MSG_QTY    )5u,															/*任务现在最大可接收管道消息数量为5*/
                 (OS_TICK       )0u,
                 (void         *)0u,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);
								 
			OSTaskCreate((OS_TCB       *)&AppTask8StartTCB,              /* 创建采集处理线程                                 */
                 (CPU_CHAR     *)"App Task8 Start",
                 (OS_TASK_PTR   )task_thread_bms,
                 (void         *)0u,
                 (OS_PRIO       )TASK_BMS_SETUP_PRIO,
                 (CPU_STK      *)&AppTask8StartStk[0u],
                 (CPU_STK_SIZE  )AppTask8StartStk[APP_CFG_TASK_START_STK_SIZE / 10u],
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE,
                 (OS_MSG_QTY    )5u,															/*任务现在最大可接收管道消息数量为5*/
                 (OS_TICK       )0u,
                 (void         *)0u,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);

    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */

    (void)&err;

    return (0u);
}


/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;


   (void)p_arg;

    BSP_Init();                                                 /* Initialize BSP functions                             */
    CPU_Init();                                                 /* Initialize the uC/CPU services                       */

    cpu_clk_freq = BSP_CPU_ClkFreq();                           /* Determine SysTick reference freq.                    */
    cnts         = cpu_clk_freq                                /* Determine nbr SysTick increments                     */
                 / (CPU_INT32U)OSCfg_TickRate_Hz;

    OS_CPU_SysTickInit(cnts);                                   /* Init uC/OS periodic time src (SysTick).              */

    Mem_Init();                                                 /* Initialize memory managment module                   */
    Math_Init();                                                /* Initialize mathematical module                       */


#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif


#if (APP_CFG_SERIAL_EN == DEF_ENABLED)
    App_SerialInit();                                           /* Initialize Serial communication for application ...  */
#endif

                                                                /* ... tracing                                          */
    APP_TRACE_DBG(("\n\n\r"));
    APP_TRACE_DBG(("Creating Application kernel objects\n\r"));
   
    APP_TRACE_DBG(("Creating Application Tasks\n\r"));


    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */

        //BSP_LED_Toggle(1u);//可做心跳指示灯
        OSTimeDlyHMSM(0u, 0u, 2u, 0u,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);

    }
}






/*
	函数名:Synchronization_init
	功能:初始化内存池,队列,信号量
*/
static void Synchronization_init(void)
{
	OS_ERR err;
/*内存池创建*/
	//创建主485内存池
	OSMemCreate(buf_m485,"buf_m485",part_buf_m485,NBLKS_m485,BLKSIZE_m485,&err);
	
	//创建从485内存池
	OSMemCreate(buf_s485,"buf_s485",part_buf_s485,NBLKS_s485,BLKSIZE_s485,&err);

	//创建uart内存池
	OSMemCreate(buf_uart,"buf_uart",part_buf_uart,NBLKS_uart,BLKSIZE_uart,&err);

	//创建i2c内存池
	OSMemCreate(buf_i2c,"buf_i2c",part_buf_i2c,NBLKS_i2c,BLKSIZE_i2c,&err);

	//创建动作消息内存池
	OSMemCreate(buf_act,"buf_act",part_buf_act,NBLKS_act,BLKSIZE_act,&err);
	
/*消息队列创建*/
	//动作线程消息队列
	OSQCreate(ACT_Q,"ACT_Q",ACT_QUEUE_N,&err);

	//遥控线程队列
	OSQCreate(TEL_Q,"TEL_Q",TEL_QUEUE_N,&err);

	//图线处理线程队列
	OSQCreate(CAM_Q,"CAM_Q",CAM_QUEUE_N,&err);

	//超声波处理线程队列
	OSQCreate(ULT_Q,"ULT_Q",ULT_QUEUE_N,&err);
	
	//温湿度处理线程队列
	OSQCreate(HUM_Q,"HUM_Q",HUM_QUEUE_N,&err);
	
	//陀螺仪处理线程队列
	OSQCreate(GYR_Q,"GYR_Q",GYR_QUEUE_N,&err);
	
	//电池管理线程队列
	OSQCreate(BMS_Q,"BSM_Q",BMS_QUEUE_N,&err);
	
	//主485到采集线程队列
	OSQCreate(M485_Q,"M485_Q",M485_QUEUE_N,&err);
	
	//从485到采集线程队列
	OSQCreate(S485_Q,"S485_Q",S485_QUEUE_N,&err);
	
	//I2C到采集线程队列
	OSQCreate(I2C_Q,"I2C_Q",I2C_QUEUE_N,&err);
	
	//UART到采集线程队列
	OSQCreate(UART_Q,"UART_Q",UART_QUEUE_N,&err);
 
/*互斥锁创建*/ 
  //主master485互斥锁
	OSMutexCreate( MASTER_485_CONTEROL_MUT,"m485MUT",&err);
	


/*调试信号量创建*/
#if DEBUG_PRINT == 1
	DEBUG_P  =  OSSemCreate(1);
#endif
	
}


 /**
   函数名:systick_config
   功能:初始化定时器10ms中断
   使用系统滴答定时器，用于任务调度
SystemFrequency / 1000 1ms

SystemFrequency / 100000 10us

SystemFrequency / 1000000 1us
 */
void systick_config(void)
{
	//10ms
	SysTick_Config(SystemCoreClock / 100);
}


/*
	函数名:Bsp_Init
	功能:初始化外设
*/
static void Bsp_Init(void)
{
	//NVIC_PriorityGroupConfig (NVIC_PriorityGroup_4);
	//systick_config();
	BSP_BMS_Init();
	BSP_CAM_Init();
	BSP_GYR_Init();
	BSP_HUM_Init();
	BSP_TEL_Init();
	BSP_ULT_Init();
	BSP_IO_Init();
}
