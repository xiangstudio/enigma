#include "main.h"

#include "stdlib.h"
#include "string.h"
#include "my_types.h"

#include "CMT2300A_EasyDriver_Function.h"
#include "CMT2300A_Register.h"
#include "CMT2300_GPIO_Define.h"

#include "my_common.h"
#include "my_tools.h"
#include "my_command.h"
#include "my_i2c.h"
#include "my_uart.h"
#include "my_rf.h"

unsigned char my_command_send_buf[MY_COMMAND_LEN];
unsigned char my_command_recv_buf[MY_COMMAND_LEN];

unsigned char MY_CMD_DATA_ZERO[MY_COMMAND_FORMAT_CMD_DATA_LEN];

void my_command_update_addr() {
    memcpy(&my_command_send_buf[MY_COMMAND_FORMAT_CMD_DST_ADDR_POS], my_host_addr, MY_COMMAND_FORMAT_CMD_ADDR_LEN);
    memcpy(&my_command_send_buf[MY_COMMAND_FORMAT_CMD_SRC_ADDR_POS], my_self_addr, MY_COMMAND_FORMAT_CMD_ADDR_LEN);  
}

void my_command_init() {
    memset(&my_command_send_buf[0], 0, MY_COMMAND_LEN);
    memset(&my_command_recv_buf[0], 0, MY_COMMAND_LEN);
    
    my_command_update_addr();
    memset(&MY_CMD_DATA_ZERO[0], 0, MY_COMMAND_FORMAT_CMD_DATA_LEN);
}

int my_command_do_recv() {
    //unsigned char *cmd_src_addr = NULL;
    unsigned char *cmd_dst_addr = NULL;

    //cmd_src_addr = &my_command_recv_buf[MY_COMMAND_FROMAT_CMD_SRC_ADDR_POS];
    cmd_dst_addr = &my_command_recv_buf[MY_COMMAND_FORMAT_CMD_DST_ADDR_POS];
    
    if (memcmp(my_self_addr, cmd_dst_addr, MY_COMMAND_FORMAT_CMD_ADDR_LEN)) {
        return -1;
    }

    if ( my_command_recv_buf[MY_COMMAND_FORMAT_CMD_TYPE_POS] == MY_COMMAND_FORMAT_CMD_TYPE_SEND &&
         //memcmp(&my_command_recv_buf[MY_COMMAND_FORMAT_CMD_DATA_POS], &MY_CMD_DATA_ZERO[0], MY_COMMAND_FORMAT_CMD_DATA_LEN) != 0) {
         my_command_recv_buf[MY_COMMAND_FORMAT_CMD_DATA_POS] == MY_COMMAND_FORMAT_CMD_DATA_VALID0 &&
         my_command_recv_buf[MY_COMMAND_FORMAT_CMD_DATA_POS+1] == MY_COMMAND_FORMAT_CMD_DATA_VALID1 ) {
          
        memcpy(&my_i2c_remote_cmd_data[0], &my_command_recv_buf[MY_COMMAND_FORMAT_CMD_DATA_POS], 
               MY_COMMAND_FORMAT_CMD_DATA_LEN);
        
        my_command_send_buf[MY_COMMAND_FORMAT_CMD_TYPE_POS] = MY_COMMAND_FORMAT_CMD_TYPE_RESP;
        
        my_port_irq_event[0] |= 0x40;
        my_icu_power_up_notify = 1;
    }
    
    return 0;
}
  
int my_command_do_send() {

    my_command_send_buf[MY_COMMAND_FORMAT_CMD_EXTA_POS] = (~((my_rssi&0x7f)-1))&0x7f; 
    
    if (my_port_irq_event[0] != 0) {
        my_command_send_buf[MY_COMMAND_FORMAT_CMD_DATA_POS] = my_port_irq_event[0];
        my_command_send_buf[MY_COMMAND_FORMAT_CMD_TYPE_POS] = MY_COMMAND_FORMAT_CMD_TYPE_NOTIFY;
    }
    
    if (my_command_send_buf[MY_COMMAND_FORMAT_CMD_TYPE_POS] == 0) {
        my_command_send_buf[MY_COMMAND_FORMAT_CMD_TYPE_POS] = MY_COMMAND_FORMAT_CMD_TYPE_HEARTBEAT;
    }
    
#ifdef TESTMODE
    if (my_test_mode != 0) {
        my_command_send_buf[MY_COMMAND_FORMAT_CMD_DATA_POS] = rx_timeout1;
        my_command_send_buf[MY_COMMAND_FORMAT_CMD_DATA_POS+1] = rx_timeout2;
        my_command_send_buf[MY_COMMAND_FORMAT_CMD_TYPE_POS] = MY_COMMAND_FORMAT_CMD_TYPE_HEARTBEAT;        
    }
#endif       
    
    return 0;
}





