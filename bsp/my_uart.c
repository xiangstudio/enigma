#include "main.h"
#ifdef MY_UART

void my_uart_open(void) {
    //pA2 to uart_tx
    GPIO_Init(GPIOA, GPIO_Pin_2, GPIO_Mode_Out_PP_High_Slow);
    //pA3 to uart_Rx
    GPIO_Init(GPIOA, GPIO_Pin_3, GPIO_Mode_In_PU_No_IT);

    SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortA,ENABLE);
    USART_DeInit(USART1);
    CLK_PeripheralClockConfig(CLK_Peripheral_USART1,ENABLE);

    USART_Init(USART1, 9600, USART_WordLength_8b, USART_StopBits_1,
    USART_Parity_No, (USART_Mode_TypeDef)(USART_Mode_Tx | USART_Mode_Rx));
                   
    /* Enable general interrupts */
    //enableInterrupts();

    /* Enable the USART Receive interrupt: this interrupt is generated when the USART
    receive data register is not empty */
    //USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    /* Enable the USART Transmit complete interrupt: this interrupt is generated when the USART
    transmit Shift Register is empty */
    //USART_ITConfig(USART1, USART_IT_TC, ENABLE);

    /* Enable USART */
    USART_Cmd(USART1, ENABLE);
}

void my_uart_close(void) {
    CLK_PeripheralClockConfig(CLK_Peripheral_USART1,DISABLE);

    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);

    USART_Cmd(USART1, DISABLE);
}

void my_uart_sendbyte(byte c) { 
    word  timeout = 0;

    while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET)
    {
            if(timeout++>2000)break;
    }

    USART_SendData8(USART1,c);
    while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);  
}

void my_uart_sendbuf(unsigned char buf[],int len) { 
    byte i;
    byte j=0;
    
    while(USART1->SR&USART_SR_RXNE) {
       i= USART1->DR;
       delay_ms(1000);
       j++;
       if(j==0) break;    
    }
        
    GPIO_SetBits(GPIOD,GPIO_Pin_0);//DE=1,TX
    delay_ms(100);
    
    for(i=0;i<len;i++)
       my_uart_sendbyte(buf[i]);

    GPIO_ResetBits(GPIOD,GPIO_Pin_0);//DE=1,RX   
}

byte my_uart_receive_byte(byte *p) {  
    /* //limx debug
      for(SysTime=0;SysTime<5;)  //timeout 150ms
      {
    TimerQuery();
    if(USART1->SR&USART_SR_RXNE)
    {
       *p= USART1->DR;
       return 1;
    }
    
      }
    */

    return 0;
}

static char bin2char(unsigned char numb) {
    char ret = '0';
    
    if (numb <= 9)
        ret = '0' + numb;
    else if (numb >= 0x0a && numb <= 0x0f)
        ret = 'a' + (numb-0x0a);
    
    return ret;
}

void my_printf_str(unsigned char *str) {
    my_uart_sendbuf(str, strlen((char *)str));
}

void my_printf_byte(unsigned char numb) {
    unsigned char buf[3] = {0,0};
    unsigned char lt = numb&0x0F;
    unsigned char ht = (numb&0xF0)>>4;
    buf[0] = bin2char(ht);
    buf[1] = bin2char(lt);
    my_uart_sendbuf(buf, 2);
}

void my_printf_char(unsigned char mychar) {
    my_uart_sendbuf(&mychar, 1);
}

void my_printf_hex(unsigned char *buf, unsigned char len) {
    unsigned char *hexbuf = (unsigned char *)malloc(len*2);
    int i = 0, j = 0;
    
    my_uart_sendbuf("\r\n", 2);
    for (i=0; i<len; i++) {
      unsigned char lt = buf[i]&0x0F;
      unsigned char ht = (buf[i]&0xF0)>>4;
      hexbuf[j+0] = bin2char(ht);
      hexbuf[j+1] = bin2char(lt);
      j+=2;
    }
    my_uart_sendbuf(hexbuf, len*2);
    free(hexbuf);
}

#endif //#ifdef MY_UART
