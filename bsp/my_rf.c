#include "main.h"

#include "stdlib.h"
#include "string.h"

#include "my_common.h"
#include "my_tools.h"
#include "my_types.h"
#include "my_uart.h"
#include "my_command.h"
#include "my_rf.h"

#include "CMT2300A_EasyDriver_Function.h"
#include "HopeDuino_SPI3_Function.h"
#include "CMT2300A_Register.h"
#include "CMT2300_GPIO_Define.h"

//#include "RfCongfig433.92_2.4k_35k.h"
//#include "RfCongfig868.35_2.4k_35k.h" 
//#include "RadioCfg868350K2K5KHz20dBm.h"
//#include "RadioCfg868350K2K5KHz13dBm.h"

//#include "RFCFG_433.92_40K_35K_13dBm_limx.h"
//#include "RFCFG_433.92_2.4K_35K_13dBm_limx_20k.h"
//#include "30k.exp.h"
//#include "40k.exp.h"

//#include "40K_30K_20DB_3SYNC_16DATA.exp.h"
//#include "40K_30K_20DB_3SYNC_8DATA.exp.h"
//#include "50K_40K_20DB_3SYNC_8DATA.exp.h"
//#include "80K_70K_20DB_3SYNC_8DATA.exp.h"

#include "80K_40K_13DB_3SYNC_8DATA.exp.h"

#ifdef TESTMODE      
#include "80K_40K_13DB_3SYNC_8DATA.exp.testmode1.exp.h"
#include "80K_40K_13DB_3SYNC_8DATA.exp.testmode2.exp.h"
#endif


#ifdef TESTMODE      
void my_rf_init_testmode1(void) {
    vCfgBank(CMTBank_testmode1, 12);   //Config 6 banks register 
    vCfgBank(SystemBank_testmode1, 12);
    vCfgBank(FrequencyBank_testmode1, 8);
    vCfgBank(DataRateBank_testmode1, 24);     
    vCfgBank(BasebandBank_testmode1, 29);
    vCfgBank(TXBank_testmode1, 11);
    vAfterCfg();
    
    bGoSleep();        
    bGoStandby();
      
    AntSwitch_byhand(1);
    vIntSrcEnable(TX_DONE_EN);
    vSetDirect();  //?????????
    bGoStandby();
}
#endif

#ifdef TESTMODE      
void my_rf_init_testmode2(void) {
    vCfgBank(CMTBank_testmode2, 12);   //Config 6 banks register 
    vCfgBank(SystemBank_testmode2, 12);
    vCfgBank(FrequencyBank_testmode2, 8);
    vCfgBank(DataRateBank_testmode2, 24);          
    vCfgBank(BasebandBank_testmode2, 29);
    vCfgBank(TXBank_testmode2, 11);
    vAfterCfg();
    
    vClearFifo();
    bGoSleep();        
    bGoStandby();
    
    vSetPayloadLength(TRUE, MY_COMMAND_LEN);//limx debug
    
    vGpioFuncCfg(GPIO3_DATA);

    vClearFifo();
    bGoStandby();
}
#endif

void my_rf_init(void) {
    FixedPktLength = TRUE; //limx debug FALSE;  //Fixed Packet Mode
    PayloadLength = MY_COMMAND_LEN; 

    vInit();
  
#ifdef TESTMODE      
    if (my_test_mode == 1) {
        my_rf_init_testmode1();
        return;
    }
    
    if (my_test_mode == 2) {
        my_rf_init_testmode2();
        return;
    }
#endif
    
    vCfgBank(CMTBank, 12);   //Config 6 banks register 
    vCfgBank(SystemBank, 12);
    vCfgBank(FrequencyBank, 8);
    vCfgBank(DataRateBank, 24);          
    
    vCfgBank(BasebandBank, 29);
    vCfgBank(TXBank, 11);
    vAfterCfg();
    
    vGpioFuncCfg(GPIO1_INT1+GPIO2_DATA+GPIO3_INT2+GPIO4_DOUT);
    vIntSrcCfg(INT_TX_FIFO_NMTY, INT_TX_DONE);   
    vIntSrcEnable(TX_DONE_EN);                   
    
    vClearFifo();
    bGoSleep();        

    bGoStandby();
    
    vSetPayloadLength(TRUE, MY_COMMAND_LEN);//limx debug
    //vEnableAntSwitch(0);    //[0]:No switch antenna; [1or2]:Switch Antenna
    //limx debug AntSwitch_byhand(0);//[1]:tx  [0]:rx

    vGpioFuncCfg(GPIO1_INT1+GPIO2_DATA+GPIO3_INT2+GPIO4_DOUT);
    //vIntSrcCfg(INT_TX_FIFO_NMTY, INT_TX_DONE);
    vIntSrcCfg(INT_TX_FIFO_NMTY, INT_TX_DONE);
    //vIntSrcEnable(TX_DONE_EN);
    //my_rf_go_tx();
    //bGoStandby();
    
    vGpioFuncCfg(GPIO1_INT1+GPIO2_DATA+GPIO3_INT2+GPIO4_DOUT);
    vIntSrcCfg(INT_RX_FIFO_WBYTE, INT_PKT_OK);  //when CRC disable, by RFPDK setting
    vIntSrcEnable(PKT_DONE_EN+CRC_OK_EN+PREAM_OK_EN+SYNC_OK_EN+TX_DONE_EN); 

    vClearFifo();
    bGoSleep();
}

