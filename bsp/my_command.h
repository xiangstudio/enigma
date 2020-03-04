#ifndef MY_COMMAND_H
#define MY_COMMAND_H

extern void my_command_init();
extern void my_command_update_addr();
extern int my_command_do_recv();
extern int my_command_do_send();

extern unsigned char my_command_send_buf[MY_COMMAND_LEN];
extern unsigned char my_command_recv_buf[MY_COMMAND_LEN];

#endif
