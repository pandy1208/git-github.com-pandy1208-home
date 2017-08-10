#ifndef __BSP_LED_H
#define __BSP_LED_H

extern int BSP_ULT_Init(void);
extern int	BSP_ULT_read(const char *cmd,unsigned char lenth,unsigned char** rev);
extern int	BSP_ULT_write(const char *cmd,unsigned char lenth,unsigned char** rev);


#endif

