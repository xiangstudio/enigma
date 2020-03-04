#ifndef MY_I2C_H
#define MY_I2C_H

#include "my_common.h"

#define SEND_START "ATD="                                                         
#define SEND_SPLIT ","
#define SEND_END   "\r"

extern unsigned char my_i2c_remote_cmd_data[MY_COMMAND_FORMAT_CMD_DATA_LEN];
extern unsigned char my_i2c_cmd_parse_notify;

extern void my_i2c_init(void);
extern void my_i2c_enable(void);
extern void my_i2c_disable(void);
extern void my_i2c_cmd_parse(void);
extern void I2C_IRQHandler(void);
extern void my_i2c_set_data_to_send(unsigned char *data, int len);
extern void my_i2c_set_event_to_send(unsigned char *data, int len);

#endif


