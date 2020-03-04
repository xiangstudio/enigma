#ifndef MY_RF_H
#define MY_RF_H

#include "my_command.h"

void my_rf_init(void);

#define my_rf_is_rx() (bReadStatus() == MODE_STA_RX)

#define my_rf_is_tx() (bReadStatus() == MODE_STA_TX)

#define my_rf_is_stby() (bReadStatus() == MODE_STA_STBY)

#define my_rf_go_standby() do { bGoStandby(); }while(bReadStatus() != MODE_STA_STBY)

#define my_rf_go_rx() do { bGoRx(); }while(bReadStatus() != MODE_STA_RX)

#define my_rf_go_tx() do { bGoTx(); }while(bReadStatus() != MODE_STA_TX)

#define my_rf_sleep() do { bGoSleep(); }while(bReadStatus() != MODE_STA_SLEEP)

#define my_rf_clear_rx_tx_fifo() do { vClearFifo(); } while(0)

#define my_rf_read_int_flag() bSpi3Read(CUS_INT_FLAG)

#define my_rf_clear_int_flag() bIntSrcFlagClr()

#define my_rf_set_payload_len() vSetPayloadLength(TRUE, MY_COMMAND_SEND_LEN)

#define my_rf_enable_tx_fifo() vEnableWrFifo()

#define my_rf_enable_rx_fifo() vEnableRdFifo()

#define my_rf_rx_ok_flag PKT_OK_FLG

#define my_rf_read_rx_fifo(buf, len) do { memset(buf, 0, len); \
    vSpi3BurstReadFIFO(buf, len); } while (0);

#define my_rf_read_rssi() bReadRssi(TRUE)

#define my_rf_tx(buf, len) vSpi3BurstWriteFIFO((byte *)buf, (byte)len)

void my_rf_while_before();

void my_rf_while_do(int while_ticks);

#define my_rf_int_h()  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3)
#define my_rf_int_l()  (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3)==0)

#endif
