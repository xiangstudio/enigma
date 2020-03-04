#define _GPO1 GPIO_Pin_4 
#define _GPO2 GPIO_Pin_5 
#define _GPO3 GPIO_Pin_7 
//#define _GPO4 0x20 
#define _RX_ANT GPIO_Pin_5 
#define _TX_ANT GPIO_Pin_4 

#define _GPO6 GPIO_Pin_6 //limx debug
 
#define GPO1In() GPIO_Init(GPIOC, _GPO1, GPIO_Mode_In_PU_No_IT)
#define GPO1_H() GPIO_ReadInputDataBit(GPIOC,_GPO1) 
#define GPO1_L() (GPIO_ReadInputDataBit(GPIOC,_GPO1)==0)

#define GPO2In() GPIO_Init(GPIOC, _GPO2, GPIO_Mode_In_PU_No_IT)
#define GPO2_H() GPIO_ReadInputDataBit(GPIOC,_GPO2)           
#define GPO2_L() (GPIO_ReadInputDataBit(GPIOC,_GPO2)==0)

#define GPO3In() GPIO_Init(GPIOB, _GPO3, GPIO_Mode_In_PU_No_IT)
#define GPO3_H() GPIO_ReadInputDataBit(GPIOB,_GPO3)           
#define GPO3_L() (GPIO_ReadInputDataBit(GPIOB,_GPO3)==0) 

//limx debug
#define GPO6In() GPIO_Init(GPIOB, _GPO6, GPIO_Mode_In_PU_No_IT)
#define GPO6_H() GPIO_ReadInputDataBit(GPIOB,_GPO6)           
#define GPO6_L() (GPIO_ReadInputDataBit(GPIOB,_GPO6)==0) 

#define TX_ANT_OUT() GPIO_Init(GPIOB, _TX_ANT, GPIO_Mode_Out_PP_High_Slow)
#define RX_ANT_OUT() GPIO_Init(GPIOB, _RX_ANT, GPIO_Mode_Out_PP_High_Slow) 
 
#define Set_RX_ANT() GPIO_SetBits(GPIOB,_RX_ANT)
#define Clr_RX_ANT() GPIO_ResetBits(GPIOB,_RX_ANT)
 
#define Set_TX_ANT() GPIO_SetBits(GPIOB,_TX_ANT)  
#define Clr_TX_ANT() GPIO_ResetBits(GPIOB,_TX_ANT)
