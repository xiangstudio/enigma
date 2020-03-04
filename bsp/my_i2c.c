#include "main.h"
#include <string.h>
#include "my_tools.h"
#include "my_i2c.h"
#include "my_rf.h"

#define I2C_OWN_ADDRESS7 0x48

#define I2C_RECV_BUF_LEN 16
#define I2C_SEND_BUF_LEN 16

unsigned char i2c_recv_buf[I2C_RECV_BUF_LEN];
int i2c_recv_ptr = 0;

unsigned char i2c_send_buf[I2C_SEND_BUF_LEN];
int i2c_send_ptr = 0, i2c_send_tail = 0;

unsigned char my_i2c_cmd_parse_notify = 0;

unsigned char my_i2c_remote_cmd_data[MY_COMMAND_FORMAT_CMD_DATA_LEN];

void my_i2c_enable() {
    /* I2C  clock Enable*/
    CLK_PeripheralClockConfig(CLK_Peripheral_I2C1, ENABLE);  
    
    I2C_DeInit(I2C1);
    
    /* Initialize I2C peripheral */
    I2C_Init(I2C1,400000, I2C_OWN_ADDRESS7<<1, I2C_Mode_I2C, I2C_DutyCycle_2,
             I2C_Ack_Enable, I2C_AcknowledgedAddress_7bit);
    
    I2C_ITConfig(I2C1, (I2C_IT_TypeDef)(I2C_IT_ERR | I2C_IT_EVT | I2C_IT_BUF), ENABLE);     
    I2C_Cmd(I2C1, ENABLE);
}

void my_i2c_disable() {
    I2C_Cmd(I2C1, DISABLE);
}

void my_i2c_init(void) {
    my_i2c_enable();

    memset(i2c_recv_buf, 0, sizeof(i2c_recv_buf));
    memset(i2c_send_buf, 0, sizeof(i2c_send_buf));
    
    memset(my_i2c_remote_cmd_data, 0, sizeof(my_i2c_remote_cmd_data));
   
}

void i2c_send_handler(void) {
    if (i2c_send_tail == 0) {
        I2C_SendData(I2C1, '\r');
        return;
    }
    
    I2C_SendData(I2C1, i2c_send_buf[i2c_send_ptr]);
    i2c_send_ptr++;
    if (i2c_send_ptr >= i2c_send_tail) {
        i2c_send_ptr = 0;
        i2c_send_tail = 0;
    }
}

void i2c_recv_handler(void) {
    unsigned char data = 0;
   
    data = I2C1->DR;
    if (data == '\r' || data == '\n') {
        i2c_recv_buf[i2c_recv_ptr] = 0;
        
#if 0  //echo test       
        memcpy(i2c_send_buf, i2c_recv_buf, i2c_recv_ptr);
        i2c_send_tail = i2c_recv_ptr;
        i2c_send_ptr = 0;
#endif          
        if (strncmp((char *)i2c_recv_buf, "ATD?", strlen("ATD?")) == 0) {
            int i = 0;    
      
            //echo test, too many code will affect the i2c bus
            i2c_send_tail = 0;
            
            if ((i2c_send_tail+1) < I2C_SEND_BUF_LEN) {
                i2c_send_buf[i2c_send_tail] = '+';
                i2c_send_tail += 1;
            }
            
            if ((i2c_send_tail + MY_COMMAND_FORMAT_CMD_DATA_LEN*2) < I2C_SEND_BUF_LEN) {
                for (i=0; i<MY_COMMAND_FORMAT_CMD_DATA_LEN; i++) {
                    bin2hex(my_i2c_remote_cmd_data[i], &i2c_send_buf[i2c_send_tail],  &i2c_send_buf[i2c_send_tail+1]);
                    i2c_send_tail += 2;
                }
            }
            
            if ((i2c_send_tail+1) < I2C_SEND_BUF_LEN) {
                i2c_send_buf[i2c_send_tail] = '\r';
                i2c_send_tail += 1;
            }
            
            i2c_send_ptr = 0;       

        } else if (strncmp((char *)i2c_recv_buf, "ATE?", strlen("ATE?")) == 0) {
            int i = 0;    
      
            //echo test, too many code will affect the i2c bus
            i2c_send_tail = 0;
            
            if ((i2c_send_tail+1) < I2C_SEND_BUF_LEN) {
                i2c_send_buf[i2c_send_tail] = '+';
                i2c_send_tail += 1;
            }
            
            if ((i2c_send_tail + MY_PORT_IRQ_MASK_LEN*2) < I2C_SEND_BUF_LEN) {
                for (i=0; i<MY_PORT_IRQ_MASK_LEN; i++) {
                    bin2hex(my_port_irq_event[i], &i2c_send_buf[i2c_send_tail],  &i2c_send_buf[i2c_send_tail+1]);
                    i2c_send_tail += 2;
                }
            }
            
            if ((i2c_send_tail+1) < I2C_SEND_BUF_LEN) {
                i2c_send_buf[i2c_send_tail] = '\r';
                i2c_send_tail += 1;
            }
            
            i2c_send_ptr = 0;       

        } else {
            const char* CMD_END_TAG = "+OK\r";
            i2c_send_tail = 0;
            
            if ((i2c_send_tail+strlen(CMD_END_TAG)) < I2C_SEND_BUF_LEN) {
                memcpy(&i2c_send_buf[i2c_send_tail], (unsigned char *)CMD_END_TAG, strlen(CMD_END_TAG));
                i2c_send_tail += strlen(CMD_END_TAG);
            }
            
            i2c_send_ptr = 0;       

        }        

        my_i2c_cmd_parse_notify = 1;
        
    } else {
      
        i2c_recv_buf[i2c_recv_ptr] = data;
        i2c_recv_ptr++;
        if (i2c_recv_ptr > I2C_RECV_BUF_LEN) {
            i2c_recv_ptr = 0;
        }
        
    }
}

void my_i2c_cmd_parse(void) {
    if (strcmp((char *)i2c_recv_buf, "AT=SLEEP") == 0) {

        memset(my_i2c_remote_cmd_data, 0, sizeof(my_i2c_remote_cmd_data)); //clear data
        my_port_irq_event[0] = 0;//clear event
        my_icu_power_down_notify = 1;
        
    } else if (strncmp((char *)i2c_recv_buf, "ATS=", strlen("ATS=")) == 0) {
        int pos = 0;
        unsigned char counter;

        pos = strlen("ATS=");

        counter = hex2bin(i2c_recv_buf[pos+0], i2c_recv_buf[pos+1]);
        my_rtc_counter = counter<<8;
        
        counter = hex2bin(i2c_recv_buf[pos+2], i2c_recv_buf[pos+3]);
        my_rtc_counter = my_rtc_counter|counter;
         
        my_rtc_config(my_rtc_counter);
        
    }else if (strncmp((char *)i2c_recv_buf, "ADR=", strlen("ADR=")) == 0) {
        int i = 0, j = 0, pos = 0;    

        pos = strlen("ADR=");
        
        j = 0;
        for (i=0; i<MY_COMMAND_FORMAT_CMD_ADDR_LEN; i++) {
            my_self_addr[i] = hex2bin(i2c_recv_buf[pos+j], i2c_recv_buf[pos+j+1]);
            j += 2;
        }
        
        for (i=0; i<MY_COMMAND_FORMAT_CMD_ADDR_LEN; i++) {
            my_host_addr[i] = hex2bin(i2c_recv_buf[pos+j], i2c_recv_buf[pos+j+1]);
            j += 2;
        }
        my_command_update_addr();
    } else if (strncmp((char *)i2c_recv_buf, "ATP=", strlen("ATP=")) == 0) {
        //dynamic setting rtc cause some problem, so comment it
        /*int i = 0, j = 0, pos = 0;    

        pos = strlen("ATP=");
        
        j = 0;
        for (i=0; i<1; i++) {
            my_port_irq_mask[i] = hex2bin(i2c_recv_buf[pos+j], i2c_recv_buf[pos+j+1]);
            j += 2;
        }
        
        my_port_irq_type = hex2bin(i2c_recv_buf[pos+j], i2c_recv_buf[pos+j+1]);
        j += 2;
        
        my_port_irq_mask_do();*/
    }

    i2c_recv_ptr = 0;  
}

void I2C_IRQHandler(void) {
    unsigned char status = 0;

    if (I2C1->SR1 & 0x02) { //addr match
        status = I2C1->SR3;
        if (status & 0x04) {
          i2c_send_handler();
        }
    }
    else if ((I2C1->SR1 & 0x40)&&(0 == (I2C1->SR3 & 0x04))) {
        i2c_recv_handler();
    }
    
    I2C1->CR2 = I2C1->CR2; //without it, read gpio errors, ???

    I2C_ClearITPendingBit(I2C1, (I2C_IT_TypeDef)(I2C_IT_ERR | I2C_IT_EVT | I2C_IT_BUF));
}

#if 0    
#define I2C_SR1_TXE         ((unsigned char)0x80) /*!< Data Register Empty (transmitters) */
#define I2C_SR1_RXNE        ((unsigned char)0x40) /*!< Data Register not Empty (receivers) */
#define I2C_SR1_STOPF       ((unsigned char)0x10) /*!< Stop detection (Slave mode) */
#define I2C_SR1_ADD10       ((unsigned char)0x08) /*!< 10-bit header sent (Master mode) */
#define I2C_SR1_BTF         ((unsigned char)0x04) /*!< Byte Transfer Finished */
#define I2C_SR1_ADDR        ((unsigned char)0x02) /*!< Address sent (master mode)/matched (slave mode) */
#define I2C_SR1_SB          ((unsigned char)0x01) /*!< Start Bit (Master mode) */
    
#define I2C_SR2_SMBALERT    ((unsigned char)0x80) /*!< SMBus Alert */
#define I2C_SR2_TIMEOUT     ((unsigned char)0x40) /*!< Time out or TLow error */
#define I2C_SR2_WUFH        ((unsigned char)0x20) /*!< Wake-up from Halt */
#define I2C_SR2_PECERR      ((unsigned char)0x10) /*!< PEC error in reception */
#define I2C_SR2_OVR         ((unsigned char)0x08) /*!< Overrun/Underrun */
#define I2C_SR2_AF          ((unsigned char)0x04) /*!< Acknowledge Failure */
#define I2C_SR2_ARLO        ((unsigned char)0x02) /*!< Arbitration Lost (master mode) */
#define I2C_SR2_BERR        ((unsigned char)0x01) /*!< Bus Error */    
    
#define I2C_SR3_DUALF       ((unsigned char)0x80) /*!< Dual flag (Slave mode) */
#define I2C_SR3_SMBHOST     ((unsigned char)0x40) /*!< SMBus Host Header (Slave mode) */
#define I2C_SR3_SMBDEFAULT  ((unsigned char)0x20) /*!< SMBus Default Header (Slave mode) */
#define I2C_SR3_GENCALL     ((unsigned char)0x10) /*!< General Call Header (Slave mode) */
#define I2C_SR3_TRA         ((unsigned char)0x04) /*!< Transmitter/Receiver */
#define I2C_SR3_BUSY        ((unsigned char)0x02) /*!< Bus Busy */
#define I2C_SR3_MSL         ((unsigned char)0x01) /*!< Master/Slave */
#endif

