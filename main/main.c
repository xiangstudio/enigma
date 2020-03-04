#include "stdlib.h"
#include "string.h"

#include "my_types.h"
#include "main.h"
#include "my_common.h"
#include "my_tools.h"
#include "my_command.h"
#include "my_rf.h"
#include "my_i2c.h"

#include "CMT2300A_EasyDriver_Function.h"
#include "CMT2300_GPIO_Define.h"
#include "CMT2300A_Register.h"
#include "HopeDuino_SPI3_Function.h"
#include "stm8l15x_itc.h"


unsigned char my_port_irq_event[MY_PORT_IRQ_MASK_LEN];
unsigned char MY_ADDR_ZERO[MY_COMMAND_FORMAT_CMD_ADDR_LEN];
unsigned char my_host_addr[MY_COMMAND_FORMAT_CMD_ADDR_LEN];
unsigned char my_self_addr[MY_COMMAND_FORMAT_CMD_ADDR_LEN];

char my_icu_power_down_notify = 0;
char my_icu_power_up_notify = 0;
char my_icu_is_pwr_up = 0;

unsigned char my_while_tick = 0;
unsigned char my_rssi = 0;

unsigned short my_rtc_counter = 0x450A; //normal 1s
//unsigned short my_rtc_counter = 0xaaa3; // slow to 3s 

unsigned int my_rtc_ticks_count = 0;
unsigned int my_rtc_press_start = 0;
unsigned char my_reset_press_count = 0;
unsigned int rx_timeout1 = 0;
unsigned int rx_timeout2 = 0;



#ifdef TESTMODE
unsigned char my_test_mode = 0;
#endif


/*
unsigned char my_port_irq_mask[MY_PORT_IRQ_MASK_LEN];
unsigned char my_port_irq_type = EXTI_Trigger_Falling;
void my_port_irq_mask_do(void) {
    int i = 0, j = 0;
  
    for (i=0; i<MY_PORT_IRQ_MASK_LEN; i++) {
        unsigned char mask = my_port_irq_mask[i];
        for (j=0; j<8; j++) {
            if ((mask>>j)&0x01) {
              switch (j) {
              case 0:
                GPIO_Init(GPIOB, GPIO_Pin_0, GPIO_Mode_In_PU_IT);
                EXTI_SetPinSensitivity(EXTI_Pin_0, (EXTI_Trigger_TypeDef)my_port_irq_type);  
                break;
              case 1:
                GPIO_Init(GPIOB, GPIO_Pin_1, GPIO_Mode_In_PU_IT);
                EXTI_SetPinSensitivity(EXTI_Pin_1, (EXTI_Trigger_TypeDef)my_port_irq_type);                  
                break;
              case 2:
                GPIO_Init(GPIOB, GPIO_Pin_2, GPIO_Mode_In_PU_IT);
                EXTI_SetPinSensitivity(EXTI_Pin_2, (EXTI_Trigger_TypeDef)my_port_irq_type);                  
                break;
              }
            } else {
              switch (j) {
              case 0:
                GPIO_Init(GPIOB, GPIO_Pin_0, GPIO_Mode_Out_PP_Low_Slow);
                break;
              case 1:
                GPIO_Init(GPIOB, GPIO_Pin_1, GPIO_Mode_Out_PP_Low_Slow);                  
                break;
              case 2:
                GPIO_Init(GPIOB, GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Slow);                
                break;
              }              
            }
        }
    }
}
*/

static void my_port_deinit(void) {
  
    //PB3: to cmt2300a gpio3 read 
    GPIO_Init(GPIOB, GPIO_Pin_3, GPIO_Mode_Out_PP_Low_Fast);
 
    //PC5: test mode
    GPIO_Init(GPIOC, GPIO_Pin_5, GPIO_Mode_Out_PP_Low_Fast);

    
}
                
static void my_port_reinit(void) {
  
    //PB3: to cmt2300a gpio3 read 
    GPIO_Init(GPIOB, GPIO_Pin_3, GPIO_Mode_In_PU_No_IT);
 
    //test mode detect only on boot
    //PC5: test mode 1
    //GPIO_Init(GPIOC, GPIO_Pin_5, GPIO_Mode_In_PU_No_IT);
    
    
}

static void my_port_init(void) {
    GPIO_DeInit(GPIOA);
    GPIO_DeInit(GPIOB);
    GPIO_DeInit(GPIOC);
    GPIO_DeInit(GPIOD);
    
    //------------------------PA ZONE 
    //PA0(SWIM)
    //PA1(RST)
    //PA2(TX)
    //PA3(RX)
    
    //------------------------PB ZONE 
        /* STM8L51X 有很多组GPIO，（GPIOA、GPIOB、GPIOC 等），
      每组GPIO由8个pin脚组成，每个pin脚我们都可以配置它为中断脚,
      分别为EXTI PIN0~7 */
    //memset(&my_port_irq_mask[0], 0, sizeof(my_port_irq_mask)); 
    //memset(&my_port_irq_event[0], 0, sizeof(my_port_irq_event));
    
    //PB0: use for key press
    //GPIO_Init(GPIOB, GPIO_Pin_0, GPIO_Mode_Out_PP_Low_Slow);
    GPIO_Init(GPIOB, GPIO_Pin_0, GPIO_Mode_In_PU_IT);
    EXTI_SetPinSensitivity(EXTI_Pin_0, (EXTI_Trigger_TypeDef)EXTI_Trigger_Falling);  
    
    //PB1: PIR EN 
    GPIO_Init(GPIOB, GPIO_Pin_1, GPIO_Mode_Out_PP_High_Fast);
    //PB2: PIR SIGN high trig
    GPIO_Init(GPIOB, GPIO_Pin_2, GPIO_Mode_In_FL_IT);
    EXTI_SetPinSensitivity(EXTI_Pin_2, (EXTI_Trigger_TypeDef)EXTI_Trigger_Rising);    
    
    //PB3: to cmt2300a gpio3 read 
    GPIO_Init(GPIOB, GPIO_Pin_3, GPIO_Mode_In_PU_No_IT);
    
    //PB4,5,6,7: to cmt2300a spi
    GPIO_Init(GPIOB, GPIO_Pin_4, GPIO_Mode_Out_PP_High_Slow);    //CSB   GPIO_Pin_4
    GPIO_Init(GPIOB, GPIO_Pin_5, GPIO_Mode_Out_PP_High_Slow);    //SDCK  GPIO_Pin_5
    GPIO_Init(GPIOB, GPIO_Pin_6, GPIO_Mode_Out_PP_High_Slow);    //FCSB  GPIO_Pin_6
    GPIO_Init(GPIOB, GPIO_Pin_7, GPIO_Mode_Out_PP_High_Slow);    //SDIO  GPIO_Pin_7    
    
    //------------------------PC ZONE 
    
    //PC0,1: sda, scl
    GPIO_Init(GPIOC, GPIO_Pin_0, GPIO_Mode_Out_OD_HiZ_Slow);
    GPIO_Init(GPIOC, GPIO_Pin_1, GPIO_Mode_Out_OD_HiZ_Slow);
    
    //PC2,3: no such pins
    
    //PC4: icu power up
    GPIO_Init(GPIOC, GPIO_Pin_4, GPIO_Mode_Out_PP_Low_Slow);
    
    //PC5: test mode
    GPIO_Init(GPIOC, GPIO_Pin_5, GPIO_Mode_In_PU_No_IT);
    
    //PC6: pair and reset
    GPIO_Init(GPIOC, GPIO_Pin_6, GPIO_Mode_In_PU_IT);
    EXTI_SetPinSensitivity(EXTI_Pin_6, (EXTI_Trigger_TypeDef)EXTI_Trigger_Falling);
    
    //------------------------PD ZONE 
    
    //PD0: no use
    GPIO_Init(GPIOD, GPIO_Pin_0, GPIO_Mode_Out_PP_Low_Slow);  
}

static void my_clk_cfg(void) {
    /* Select HSE as system clock source */
    
    CLK_SYSCLKSourceSwitchCmd(ENABLE);
    
    CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);
    
    /* system clock prescaler: 1*/
    
    CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);//16MHz
    
    while (CLK_GetSYSCLKSource() != CLK_SYSCLKSource_HSI){}
}

void my_rtc_config(unsigned short counter) {
    RTC_DeInit();
    CLK_LSICmd(ENABLE);
    
    /* Enable RTC clock */
    CLK_RTCClockConfig(CLK_RTCCLKSource_LSI, CLK_RTCCLKDiv_1);
    /* Wait for LSE clock to be ready */
    while (CLK_GetFlagStatus(CLK_FLAG_LSIRDY) == RESET);
    /* wait for 1 second for the LSE Stabilisation */
    
    //DelayMs(100);
    
    CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);

    /* Configures the RTC wakeup timer_step = RTCCLK/2 = LSE/2 = 61.03515625 us */
    RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div2);

    /* Enable wake up unit Interrupt */
    RTC_ITConfig(RTC_IT_WUT, ENABLE);
    
    /* 61.03515625 us * 512 = 31.25ms*/
    
    RTC_SetWakeUpCounter(counter); //0x2AAA-500ms 0x5554-1s 0xaaa3-3s
    
    RTC_WakeUpCmd(ENABLE);
}

/*
static void my_timer2_init(void) {
    //enable tim2 clock
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM2,ENABLE);
    //the timer2 is system timer 16M div 16
    TIM2_TimeBaseInit(TIM2_Prescaler_16,TIM2_CounterMode_Up,0xffff);

    TIM2_ARRPreloadConfig(ENABLE);
    //tim4 updata event Interrupt Enable
    // TIM2_ITConfig(TIM2_IT_Update, ENABLE);
 
    TIM2_Cmd(ENABLE);
}
*/

/*
static void my_timer4_init(void) {
    //enable tim4 clock
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM4,ENABLE);
    
    //the Timer4 is system timer 16M div 128 
    //and every 1ms base timer value is counter to 125
    TIM4_TimeBaseInit(TIM4_Prescaler_128,125);
    //enable tim4 preload register ARR
    TIM4_ARRPreloadConfig(ENABLE);
    //tim4 updata event Interrupt Enable
    TIM4_ITConfig(TIM4_IT_Update, ENABLE);
    //rnsnlr tim4
    TIM4_Cmd(ENABLE);
}
*/


void icu_pwr_up() {
    if (!my_icu_is_pwr_up) {
      
        //when power up, i2c work normal
        my_i2c_enable();
        
        my_icu_is_pwr_up = 1;
        my_port_irq_event[0] |= 0x10;
        GPIO_SetBits(GPIOC,GPIO_Pin_4);
    }
}

void icu_pwr_down() {
  
    //wlakaround bug, when mcu sleep, 
    //icu to i2c command cause mcu dead
    my_i2c_disable();
    
    GPIO_ResetBits(GPIOC,GPIO_Pin_4);
    my_icu_is_pwr_up = 0;
}

#ifdef WATCHDOG
static void iwdg_config(void)
{
  /* 使能LSI时钟 */
  CLK_LSICmd(ENABLE);
  
  /* 等待LSI时钟稳定 */
  while (CLK_GetFlagStatus(CLK_FLAG_LSIRDY) == RESET);
  
  /* 使能IWDG */
  IWDG_Enable();
  
  /* 使能写 IWDG_PR 和 IWDG_RLR 寄存器 */
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  
  /* 设IWDG的时钟预分频为64 */
  IWDG_SetPrescaler(IWDG_Prescaler_64);
  
  /* 设置重装载值为256，看门狗复位时间为：（256*64） / 38000 = 431.16ms */
  IWDG_SetReload(0xff);
  
  /* 重装载看门狗，开始计时 */
  IWDG_ReloadCounter();
}
#endif

#ifdef TESTMODE
unsigned char get_test_mode() {
    /*if ( GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5) == 0 && GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6) != 0 )
        my_test_mode = 1;
    
    if ( GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5) != 0 && GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6) == 0 )
        my_test_mode = 2;

    if ( GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5) == 0 && GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6) == 0 )
        my_test_mode = 3;*/
  
    if ( GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5) == 0)
      my_test_mode = 3;
    
    return 0;
}
#endif





//SLAVE mode
void main(void) {
  
    memset(&MY_ADDR_ZERO[0], 0, sizeof(MY_ADDR_ZERO));  
    memset(&my_port_irq_event[0], 0, sizeof(my_port_irq_event));
    memset(&my_host_addr[0], 0, sizeof(my_host_addr));
    memset(&my_self_addr[0], 0, sizeof(my_self_addr));     

    my_clk_cfg();
    my_rtc_config(my_rtc_counter);
    my_port_init();
    my_i2c_init();    
    
    my_command_init();
    
#ifdef TESTMODE    
    get_test_mode();
#endif    

    my_rf_init();
    my_rf_go_standby();
    my_rf_clear_int_flag();
    my_rf_clear_rx_tx_fifo();
    
#ifdef TESTMODE
    if  (my_test_mode==1 || my_test_mode==2) {  
        if (my_test_mode == 1) {
            GPIO_Init(GPIOB, GPIO_Pin_3, GPIO_Mode_Out_PP_Low_Fast);//direct test 
            GPIO_SetBits(GPIOB, GPIO_Pin_3);
            
            my_rf_go_tx();
        }

        if (my_test_mode == 2) {
            my_rf_go_rx();
        }
        
        while(1) {}        
    }
      
    if (my_test_mode == 3) {   
        memset(&my_host_addr[0], 0x00, sizeof(my_host_addr));
        memset(&my_self_addr[0], 0xff, sizeof(my_self_addr));
        my_command_update_addr();
    } 
#endif    
  
#ifdef WATCHDOG    
    iwdg_config();
#endif    
    
    icu_pwr_down();
      
    while(1) {      
        
        if (memcmp(my_self_addr, MY_ADDR_ZERO, 
                   MY_COMMAND_FORMAT_CMD_ADDR_LEN) == 0) { 
            my_port_irq_event[0] |= 0x80;
            icu_pwr_up();
        }
        
        if (my_reset_press_count > 0) {
            int timeout = 5000;
            while (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6) == 0) {              
                if (!timeout) {
                    my_icu_power_up_notify = 1;
                    my_port_irq_event[0] |= 0x02;
                    break;
                }
                delay_ms(1);
                timeout--;
            }
            my_reset_press_count = 0;
        }
            
        if (!my_icu_is_pwr_up) {
          
            my_rf_go_standby();//can save 2ms
            while(!my_rf_is_stby());
          
            my_rf_enable_tx_fifo();
            my_rf_go_tx();

            my_command_do_send();
            my_rf_tx(my_command_send_buf, MY_COMMAND_LEN);
            my_command_send_buf[MY_COMMAND_FORMAT_CMD_TYPE_POS] = 0;
            memset(&my_command_send_buf[MY_COMMAND_FORMAT_CMD_DATA_POS], 0, 
                MY_COMMAND_FORMAT_CMD_DATA_LEN);
    
            //after TX, rf auto into standby, just wait
            while(!my_rf_is_stby());
            my_rf_clear_int_flag();
            
            my_rf_enable_rx_fifo();
            my_rf_go_rx();
            my_rssi = my_rf_read_rssi();            
            
            rx_timeout1 = 0xff;
            while (rx_timeout1) {
                rx_timeout2 = 0x20; // 8f - 100ms, 20 - 28ms
                while (rx_timeout2) {
                    if (my_rf_int_h()) {
                        break;
                    }
                    rx_timeout2--;
                };
                if (rx_timeout2) {
                    break;
                }
                rx_timeout1--;
            };
                    
            if (rx_timeout1) {//receive do
                int int_flag = my_rf_read_int_flag();
                if (int_flag & my_rf_rx_ok_flag) {
                    my_rf_read_rx_fifo(my_command_recv_buf, MY_COMMAND_LEN);
                    if (my_command_do_recv() == 0) {
                    } else {
                    }
                }
            } else {
                delay_ms(my_while_tick%5);
            }

            my_rf_sleep();
            
        } //doing when icu power down
        
        //event process before sleep
        if (my_i2c_cmd_parse_notify) {
            my_i2c_cmd_parse();
            my_i2c_cmd_parse_notify = 0;
        }          
        
        if (!my_icu_power_down_notify && 
            my_icu_power_up_notify) {
            icu_pwr_up();
            my_icu_power_up_notify = 0;
        }          
        
        if (my_icu_power_down_notify) {
            icu_pwr_down();
            my_icu_power_down_notify = 0;
        }        
        
        //working time end
        clr_led_1();
        my_port_deinit();
        __enable_interrupt();
        

        if (!my_icu_is_pwr_up) {
#ifdef WATCHDOG              
            //stvp to write option byte IWDG_HALT to stop watchdog before halt
            IWDG_ReloadCounter();          
#endif            
          
            //wfi();// i2c 0k, halt i2c fail
            if (PWR->CSR2 & PWR_CR2_VREFINTF) {
                PWR->CSR2 |= PWR_CSR2_FWU|PWR_CSR2_ULP;
            }
            halt() ;
            nop();
        } else {
            int i;
            for (i=0; i<9; i++) {
#ifdef WATCHDOG                 
                IWDG_ReloadCounter(); 
#endif                
                delay_ms(100);
            }
        }
        
        //working time begin
        __disable_interrupt();
        my_port_reinit();
        set_led_1();  

        my_while_tick++;
    }//end while
  
}


