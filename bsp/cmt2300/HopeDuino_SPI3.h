#include "my_types.h"

#if 0  //limx debug
#define SPI3_SPEED 1
// /** Hardware brief **/    
// //PORTB      //DDRx  PORTx  PINx
#define _CSB  GPIO_Pin_0  // 1          1          0
#define _SDIO    GPIO_Pin_2       // 1          1          0
#define _FCSB    GPIO_Pin_3  // 1          1          0
#define _SDCK  GPIO_Pin_1  // 1          0          0
/*****************************************************************************************/

#define SetCSB()  GPIO_SetBits(GPIOB,GPIO_Pin_0)
#define ClrCSB()  GPIO_ResetBits(GPIOB,GPIO_Pin_0)

#define SetFCSB()  GPIO_SetBits(GPIOB,GPIO_Pin_3) 
#define ClrFCSB()  GPIO_ResetBits(GPIOB,GPIO_Pin_3)

#define SetSDCK()  GPIO_SetBits(GPIOB,GPIO_Pin_1) 
#define ClrSDCK()  GPIO_ResetBits(GPIOB,GPIO_Pin_1)

#define SetSDIO()  GPIO_SetBits(GPIOB,GPIO_Pin_2)  
#define ClrSDIO()  GPIO_ResetBits(GPIOB,GPIO_Pin_2)

#define InputSDIO()  GPIO_Init(GPIOB, GPIO_Pin_2, GPIO_Mode_In_PU_No_IT)
#define OutputSDIO()  GPIO_Init(GPIOB, GPIO_Pin_2, GPIO_Mode_Out_PP_High_Slow)

#define SDIO_H()  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_2)
#define SDIO_L()  (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_2)==0)
 
/*****************************************************************************************/
#endif

#define SPI3_SPEED CMT2300A_SPI_DELAY //len 16
// /** Hardware brief **/    
// //PORTB      //DDRx  PORTx  PINx
#define _CSB  GPIO_Pin_4 //0 // 1          1          0
#define _SDIO    GPIO_Pin_7     //2        // 1          1          0
#define _FCSB    GPIO_Pin_6 //3 // 1          1          0
#define _SDCK  GPIO_Pin_5 //1 // 1          0          0
/*****************************************************************************************/

#define SetCSB()  GPIO_SetBits(GPIOB,GPIO_Pin_4)
#define ClrCSB()  GPIO_ResetBits(GPIOB,GPIO_Pin_4)

#define SetFCSB()  GPIO_SetBits(GPIOB,GPIO_Pin_6) 
#define ClrFCSB()  GPIO_ResetBits(GPIOB,GPIO_Pin_6)

#define SetSDCK()  GPIO_SetBits(GPIOB,GPIO_Pin_5) 
#define ClrSDCK()  GPIO_ResetBits(GPIOB,GPIO_Pin_5)

#define SetSDIO()  GPIO_SetBits(GPIOB,GPIO_Pin_7)  
#define ClrSDIO()  GPIO_ResetBits(GPIOB,GPIO_Pin_7)

#define InputSDIO()  GPIO_Init(GPIOB, GPIO_Pin_7, GPIO_Mode_In_PU_No_IT)
#define OutputSDIO()  GPIO_Init(GPIOB, GPIO_Pin_7, GPIO_Mode_Out_PP_High_Slow)

#define SDIO_H()  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)
#define SDIO_L()  (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)==0)
 
/*****************************************************************************************/





 

