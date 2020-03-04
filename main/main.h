#ifndef MAIN_H
#define MAIN_H

#include "stm8l15x.h"
#include "stm8l15x_conf.h"
#include "stm8l15x_it.h"
#include "my_common.h"

#define MY_PORT_IRQ_MASK_LEN 1

extern unsigned char my_host_addr[MY_COMMAND_FORMAT_CMD_ADDR_LEN];
extern unsigned char my_self_addr[MY_COMMAND_FORMAT_CMD_ADDR_LEN];
extern unsigned char my_port_irq_mask[MY_PORT_IRQ_MASK_LEN];
extern unsigned char my_port_irq_type;
extern unsigned char my_port_irq_event[MY_PORT_IRQ_MASK_LEN];
extern unsigned short my_rtc_counter;
extern unsigned char my_rssi;
extern unsigned char my_test_mode;
extern char my_icu_is_pwr_up;
extern char my_icu_power_down_notify;
extern char my_icu_power_up_notify;
extern unsigned int rx_timeout1;
extern unsigned int rx_timeout2;
extern unsigned int my_rtc_ticks_count;
extern unsigned int my_rtc_press_start;
extern unsigned char my_reset_press_count;

extern void icu_pwr_up();
extern void icu_pwr_down();
extern void my_port_irq_mask_do(void);
extern void my_rtc_config(unsigned short counter);

//#define WATCHDOG //must flash image with stp tool to write option bytes
#define TESTMODE


#endif //MAIN_H

