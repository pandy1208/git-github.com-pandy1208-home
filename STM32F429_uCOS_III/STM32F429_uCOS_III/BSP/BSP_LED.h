#ifndef __BSP_LED_H
#define __BSP_LED_H


void        BSP_LED_On                        (unsigned char     led);

void        BSP_LED_Off                       (unsigned char     led);

void        BSP_LED_Toggle                    (unsigned char     led);

int 				BSP_LED_Init											(void);


#endif

