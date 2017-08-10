/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include  <includes.h>
//#include "stm32f10x_it.h"
#include 	<stdio.h>
#include  <task_variable.h>


/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
    NVIC_SystemReset();
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)//在向量表中直接指向ucos-iii，所以此中断不可能进入PANDY20170803
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)//在向量表中直接指向ucos-iii，所以此中断不可能进入PANDY20170803
{
}

/**
  * @brief  This function handles RTC Handler.
  * @param  None
  * @retval None
  */

void RTC_IRQHandler(void)
{
	OSIntEnter();
//        RTC_ClearITPendingBit(RTC_IT_SEC);
//        RTC_ClearITPendingBit(RTC_IT_ALR|RTC_IT_OW);
	OSIntExit();	
}
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles USART1_IRQHandler interrupt request.
  * @param  None
  * @retval None
  *	
  */
//uint8_t master485_char[30] = {0,END_REV};

uint8_t data1 = 0,*m485_data_h = NULL,*m485_data_p = NULL;
void USART1_IRQHandler (void)
{
    OS_ERR perr;

    OSIntEnter();

    if((USART1->SR&USART_SR_RXNE) != 0)
    {
        data1 = USART1->DR;
        USART1->SR &= ~(USART_SR_RXNE|USART_SR_ORE);
       // TIM_Cmd(TIM3,DISABLE);

        while(m485_data_h == NULL)
        {
            m485_data_h = NULL;			
            m485_data_h = OSMemGet(buf_m485,&perr);
            if(m485_data_h != NULL)
            {
                m485_data_p = &m485_data_h[2];
                break;
            }			
        }

    if(m485_data_p != NULL){
        *m485_data_p = data1;
        m485_data_p ++;
  //      if((m485_data_p - m485_data_h) >= (BLKSIZE_485-10))
        {
     //     if(OSQQuery(M485_USART_REC_Q,&qdata) == OS_ERR_NONE)
         /*   {
                *(uint16_t *)m485_data_h = m485_data_p - m485_data_h - 2;
								if((qdata.OSEventGrp != 0)&&\
                (OSQPost(M485_USART_REC_Q,m485_data_h) == OS_ERR_NONE))
                {
                    m485_data_p = m485_data_h = NULL;
                }
                else
                {
                    m485_data_p = &m485_data_h[2];
                }
            }
            else
            {
                m485_data_p = &m485_data_h[2];
            }*/

        }

        //TIM3->CNT = 0;
        //TIM_Cmd(TIM3,ENABLE);			
    }


    }else{
    USART1->SR &= (uint16_t)(~(USART_SR_RXNE|USART_SR_ORE));
    }

    OSIntExit();		
}

/**
  * @brief  This function handles USART2_IRQHandler interrupt request.
  * @param  None
  * @retval None
  * 
  */
static unsigned char data[2] = "\0\0";
void USART2_IRQHandler (void)
{
	OSIntEnter();
	if((USART2->SR&USART_SR_RXNE) != 0)
	{
		//返回0时，说明队列已满。
		//这里不作处理，也处理不了，只能预防
		//增强容错能里

		data[0] = USART2->DR;
		//queue_write_byte(0,data[0]);
		if((data[1] == '\r')&(data[0] == '\n'))
		{
			//OSSemPost(GPRS_DATA_RECEIVE_SEM);
		}
		data[1] = data[0]; 
	}else
	{
		USART2->SR &= (uint16_t)~(USART_SR_RXNE|USART_SR_ORE);
	}
	
	OSIntExit();
}


/**
  * @brief  This function handles USART1_IRQHandler interrupt request.
  * @param  None
  * @retval None
  * slave485接收触摸屏信息
  */

unsigned char * data_485_piont = NULL, *data_485_head = NULL;
static INT8U   databuf;
/*
void USART3_IRQHandler (void)
{
	OSIntEnter();
	if((USART3->SR&USART_SR_RXNE) != 0)
	{	
		databuf = USART3->DR;

		TIM2->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));

		if(data_485_piont == NULL)
		{
			data_485_head = OSMemGet(buf_485,&perr);
			data_485_piont = data_485_head;	
		}

		if(data_485_piont != NULL)
		{
			*data_485_piont = databuf;
			data_485_piont ++;
	
			TIM2->CNT = 0;
			TIM2->CR1 |= TIM_CR1_CEN; 
		}
	
	}else
	{
		USART3->SR &= ~(USART_SR_RXNE|USART_SR_ORE);
	}
	
	OSIntExit();
}
*/
/**
  * @brief  This function handles TIM2_IRQHandler interrupt request.
  * @param  None
  * @retval None
  * 定时：100ms
  */
/*
void TIM2_IRQHandler(void)
{
	OSIntEnter();
	
	if((TIM2->SR&TIM_SR_UIF) == TIM_SR_UIF)
	{
		if(data_485_head != data_485_piont)
		{
			if((data_485_head[0] == 0x03)&&(data_485_head[1] == 0x10))
			{	

				OSQPost(SLAVE485_DATA_Q,data_485_head);
				data_485_head = OSMemGet(buf_485,&perr);
				data_485_piont = data_485_head;	

			}else
			{
				data_485_piont	= data_485_head;
			}

		}

		TIM2->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));		
	}
	TIM2->SR &= (uint16_t)~TIM_SR_UIF;
	OSIntExit();	
}	


void TIM3_IRQHandler(void)
{
    OS_Q_DATA qdata;
    //uint8_t perr;

    OSIntEnter();

    if((TIM3->SR&TIM_SR_UIF) == TIM_SR_UIF)
    {
        if((m485_data_p - m485_data_h ) > 2)
        {			
            if(OSQQuery(M485_USART_REC_Q,&qdata) == OS_ERR_NONE)
            {
                *(uint16_t *)m485_data_h = m485_data_p - m485_data_h -2;
                if((qdata.OSEventGrp != 0)&&\
                (OSQPost(M485_USART_REC_Q,m485_data_h) == OS_ERR_NONE))
                {
                    m485_data_p = m485_data_h = NULL;
                    //m485_data_h = OSMemGet(buf_485,&perr);
                    //if(m485_data_h != NULL)
                    //    m485_data_p = &m485_data_h[2];                 
                }
                else
                    m485_data_p = &m485_data_h[2];           
            }
            else
                m485_data_p = &m485_data_h[2];
        }else
            m485_data_p = &m485_data_h[2];
        
    }	
    TIM_ClearITPendingBit(TIM3,TIM_SR_UIF); 
    TIM_Cmd(TIM3,DISABLE);
    OSIntExit();	
}

float TIM4_Count = 0;
void TIM4_IRQHandler(void)
{
    OSIntEnter();
    TIM4_Count +=0.01;
    TIM_ClearITPendingBit(TIM4,TIM_SR_UIF); 
    OSIntExit();	
}
*/
/******************************************
  喂狗定时器TIM5

******************************************/
/*
uint8_t TIM5_Count = 0,err;
uint16_t sec_10s = 0;//sec_10s 为10s加一次， 如果720，即2个小时
OS_FLAGS sysstat;
void TIM5_IRQHandler(void)
{
    OSIntEnter();
    TIM5_Count +=1;
    if(TIM5_Count >= 10)
    {
      TIM5_Count = 0;
      //查询连接是否存在
      sysstat = OSFlagQuery(SYS_START_SEM_F,&err);	
      if((sysstat & (SYS_TCP_SET_UP_OK | SYS_TCP_IDENTIFY_PASST)) != (SYS_TCP_SET_UP_OK | SYS_TCP_IDENTIFY_PASST))
      {
          sec_10s++;
          if(sec_10s >= 180)
          {
            //重启
            NVIC_SystemReset();
          }
      }
      else
        sec_10s = 0;
      watchdog_clear();
    }
    TIM_ClearITPendingBit(TIM5,TIM_SR_UIF); 
    OSIntExit();	
}
*/

/**
  * @brief  This function handles EXTI9_5_IRQHandler interrupt request.
  * @param  None
  * @retval None
  */

uint8_t ext_err;
void EXTI9_5_IRQHandler(void)
{
	//if(EXTI_GetFlagStatus (EXTI_Line9) == SET)
	{
		
		//EXTI_ClearFlag(EXTI_Line9);
	}
}
/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
