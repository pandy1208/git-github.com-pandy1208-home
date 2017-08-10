#include "BSP_LED.h"
#include "stm32f4xx.h"

int BSP_LED_Init(void)
{
	RCC->AHB1ENR |=	0x1<<6; //打开GPIOG时钟
	GPIOG->MODER |= 0x5<<26; //PG13,PG14
	GPIOG->BSRRH	= 0x3<<13;	//关闭LED
	return 0;
}

void        BSP_LED_On                        (unsigned char     led)
{
	GPIOG->BSRRL = 0x1<<((led&0x1) + 13);
}


void        BSP_LED_Off                       (unsigned char     led)
{
	GPIOG->BSRRH = 0x1<<((led&0x1) + 13);
}


void        BSP_LED_Toggle                    (unsigned char     led)
{
	if(GPIOG->ODR & (0x1<<((led&0x1) + 13)) )
		GPIOG->BSRRH = 0x1<<((led&0x1) + 13);
	else
		GPIOG->BSRRL = 0x1<<((led&0x1) + 13);
}

