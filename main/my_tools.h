#ifndef MY_TOOLS_H
#define MY_TOOLS_H

#include "my_types.h"

void delay_us(int us);
void delay_ms(int ms);

#define set_led_1() GPIO_SetBits(GPIOC,GPIO_Pin_5)
#define clr_led_1() GPIO_ResetBits(GPIOC,GPIO_Pin_5)

//#define set_led_2() GPIO_SetBits(GPIOB,GPIO_Pin_2)
//#define clr_led_2() GPIO_ResetBits(GPIOB,GPIO_Pin_2)

extern void bin2hex(unsigned char mychar, unsigned char* p1, unsigned char* p2);
extern unsigned char hex2bin(unsigned char p1, unsigned char p2);

#endif



