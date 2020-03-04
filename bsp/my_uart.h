#ifndef MY_UART_H
#define MY_UART_H

void my_uart_open(void);
void my_uart_close(void);
void my_uart_sendbuf(unsigned char buf[],int len);

#endif


