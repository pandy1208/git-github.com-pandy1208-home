#ifndef __BSP_LED_H
#define __BSP_LED_H


#define BSP_LIGHT_on()  	/*设置主灯光控制管脚*/  
#define BSP_LIGHT_off()  	/*设置主灯光控制管脚*/  

#define BSP_WATER_on()  	/*设置喷淋控制管脚*/  
#define BSP_WATER_off()  	/*设置喷淋控制管脚*/  

#define BSP_WATER_on()  	/*设置喷淋控制管脚*/  
#define BSP_WATER_off()  	/*设置喷淋控制管脚*/  

#define BSP_ALARM_on()  	/*设置报警控制管脚*/  
#define BSP_ALARM_off()  	/*设置报警控制管脚*/  

#define BSP_POWER_state()  	/*获取开关管脚状态*/ 

extern int BSP_IO_Init(void);
extern int	BSP_IO_read(const char *cmd,unsigned char lenth,unsigned char** rev);
extern int	BSP_IO_write(const char *cmd,unsigned char lenth,unsigned char** rev);


#endif

