#ifndef __BSP_LED_H
#define __BSP_LED_H


#define BSP_LIGHT_on()  	/*�������ƹ���ƹܽ�*/  
#define BSP_LIGHT_off()  	/*�������ƹ���ƹܽ�*/  

#define BSP_WATER_on()  	/*�������ܿ��ƹܽ�*/  
#define BSP_WATER_off()  	/*�������ܿ��ƹܽ�*/  

#define BSP_WATER_on()  	/*�������ܿ��ƹܽ�*/  
#define BSP_WATER_off()  	/*�������ܿ��ƹܽ�*/  

#define BSP_ALARM_on()  	/*���ñ������ƹܽ�*/  
#define BSP_ALARM_off()  	/*���ñ������ƹܽ�*/  

#define BSP_POWER_state()  	/*��ȡ���عܽ�״̬*/ 

extern int BSP_IO_Init(void);
extern int	BSP_IO_read(const char *cmd,unsigned char lenth,unsigned char** rev);
extern int	BSP_IO_write(const char *cmd,unsigned char lenth,unsigned char** rev);


#endif

