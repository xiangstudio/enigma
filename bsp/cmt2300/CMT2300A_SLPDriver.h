/*
 * THE FOLLOWING FIRMWARE IS PROVIDED: 
 *  (1) "AS IS" WITH NO WARRANTY; 
 *  (2) TO ENABLE ACCESS TO CODING INFORMATION TO GUIDE AND FACILITATE CUSTOMER.
 * CONSEQUENTLY, HopeRF SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR
 * CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 * 
 * Copyright (C) HopeRF
 *
 * website: www.HopeRF.com
 *          www.HopeRF.cn     
 */

/*! 
 * file       CMT2300A_SLPDriver
 * brief      how to use CMT2300A
 * hardware   HopeDuino with RFM300
 *            
 *
 * version    1.0
 * date       Jan 09 2017
 * author     QY Ruan
 *
 * version    1.1
 * date       Feb 3 2017
 * modify:
 *            1. Change CSMA
 *
 * version    1.2
 * date       Feb 16 2017
 * modify:   
 *            1. vEnableRdFifo should be use in bGoRx, but not in bGetMessage, 
 *               because vEnableRdFifo is active when FIFO merge active;
 *            2. add clear fifo after vEnableWrFifo
 *            3. add depend PayloadLength value, select enable FIFO Merge, in vEnableRdFifo & vEnableWrFifo 
 */
 
 #ifndef CMT2300A_SLPDriver_h
 #define CMT2300A_SLPDriver_h

 #ifndef F_CPU
  #define F_CPU 16000000UL //HopeDuino use 16MHz Xo
 #endif 

 #ifndef byte
  typedef unsigned char byte;
 #endif
 
 #ifndef word
  typedef unsigned int  word;
 #endif 

 #include <avr/io.h>
 #include <util/delay.h>
 #include <HopeDuino_SPI3.h>
 #include <CMT2300A_Register.h>
 
 /** Hardware brief **/    
 //PORTB      //DDRx  PORTx  PINx
 #define _GPO1    0x01       // 0          0          0
 
 //PORTD
 #define _GPO2    0x80       // 0          0          0
 #define _GPO3    0x40       // 0          0          0
 #define _GPO4    0x20       // 0          0          0 
 
 #define GPO1In() (DDRB &= (~_GPO1))
 #define GPO1_H()  (PINB&_GPO1) 
 #define GPO1_L() ((PINB&_GPO1)==0)

 #define GPO2In() (DDRD &= (~_GPO2))
 #define GPO2_H()  (PIND&_GPO2) 
 #define GPO2_L() ((PIND&_GPO2)==0)

 #define GPO3In() (DDRD &= (~_GPO3))
 #define GPO3_H()  (PIND&_GPO3) 
 #define GPO3_L() ((PIND&_GPO3)==0)

 #define GPO4In() (DDRD &= (~_GPO4))
 #define GPO4_H()  (PIND&_GPO4) 
 #define GPO4_L() ((PIND&_GPO4)==0)
 
 enum dutycycelType {
                  DC_OFF_MODE,    //Duty-Cycle-Off
                  SLEEP_TO_STBY_MODE,  //SleepTimer wakeup mcu, and to standby
                  SLEEP_TO_RX_MODE,    //SleepTimer wakeup mcu, and to rx
                  RX_SEMIAUTO_MODE,       //Rx->STBY/Sleep->mcu, semi-auto mode by mcu
                  RX_FULLAUTO_MODE,   //full auto
                  TX_SEMIAUTO_MODE,  
                  TX_FULLAUTO_MODE
         };
 
 enum signaldetType {
                 ONLY_RSSI,
                 ONLY_PJD, 
                 RSSI_PJD
                 };
 
 enum rxextendType {
               EXT_DISABLE  = 0, // 
               EXIT_T1_RSSI   = 1,
               EXIT_T1_PREAM    = 2, 
               EXIT_T1_RSSInPREAM = 3,  
               EXT_RSSI_AWAYS  = 4, 
               EXT_T2_RSSI   = 5, 
               EXT_T2_PREAM  = 6,
               EXT_T2_RSSInPREAM = 7, 
               EXT_T2_PREAMorSYNC = 8, 
               EXT_T2_PREAMorNode = 9,
               EXT_T2_PoSoN  =10, 
               EXIT_T2_RSSI  =11, 
               EXIT_T2_PREAM   =12, 
               EXIT_T2_RSSInPREAM =13 
               }; 
 
 enum exitstateType {
                 EXIT_TO_SLEEP, 
                 EXIT_TO_STBY, 
                 EXIT_TO_TFS, 
                 EXIT_TO_RFS, 
                 };
                 
 enum pjdwinType {
               JUMP_4 = 0, 
               JUMP_6 = 1, 
               JUMP_8 = 2, 
               JUMP_10= 3
               };                
 
 class cmt2300aSLP
 {
  public: 
/**/ bool  FixedPktLength;      //false: for contain packet length in Tx message, the same mean with variable lenth
                                               //true : for doesn't include packet length in Tx message, the same mean with fixed length
/**/ byte  PayloadLength;      //unit: byte  range: 1-255                                     
  byte  PktRssi;        //with Sign
  byte  PktRssiCode;
  bool  PktRevError;

   //State Ctrl
   void  vSoftReset(void);
   bool  bGoSleep(void); 
   bool  bGoStandby(void); 
   bool  bGoTx(void);
   bool  bGoRx(void);
  bool  bGoSwitch(void);
   byte  bReadStatus(void);
   byte  bReadRssi(bool unit_dbm);

  //GPIO & Interrupt 
  void  vGpioFuncCfg(byte io_cfg);
  void  vIntSrcCfg(byte int_1, byte int_2);
  void  vInt1SrcCfg(byte int_1);
  void  vInt2SrcCfg(byte int_2);       
  void  vIntSrcEnable(byte en_int);
  byte  bIntSrcFlagClr(void);
  void  vEnableAntSwitch(byte mode);   
   
  //Fifo packet handle
  void  vClearFifo(void);
  byte  bReadFifoFlag(void);
  word  wReadIntFlag(void);
  void  vEnableRdFifo(void);
  void  vEnableWrFifo(void);
  void  vSetPayloadLength(bool mode, byte length);
     void  vAckPacket(bool en);
  void  vWriteFifo(byte dat);
  byte  bReadFifo(void);
 
  //Config
  void  vInit(void);  
  void  vCfgBank(word cfg[], byte length);
  void  vAfterCfg(void);
  
  //appliciation
  byte  bGetMessage(byte msg[]);
  byte  bGetMessageByFlag(byte msg[]);
  void  vSendMessage(byte msg[], byte length);
  bool  bSendMessageByFlag(byte msg[], byte length);

  //FHSS Part
  byte  FhssChannelRange;
  byte  FhssChannel;
  signed int  FhssRssiAvg;
 signed char  FhssRssiTH;
  void  vSetChannelOffset(word interval);
  void  vSetChannel(word channel);
  void  vSetTxPreamble(word length);
  byte  bFHSSDetect(void);  
 
       //***************Add in SLPDriver as below***************************     
  //********************Duty-Cycle*************************************
  word   SleepTimerM;
  byte   SleepTimerR;
  word   RxTimerT1M;
  byte   RxTimerT1R;
  word   RxTimerT2M;
  byte   RxTimerT2R;
  byte   TxPktNum;
  byte   TxPktGap;
  
  pjdwinType      PJDSel;
  dutycycelType  DutyCycleMode;
  signaldetType   DetectMode;
  rxextendType  RxExtendMode;
  exitstateType  RxExitState;    //only for RX_SEMIAUTO_MODE to use
  exitstateType  TxExitState;    //only for TX_SEMIAUTO_MODE to use
  byte   RssiTrigLevel;                       //Range: 1~127
   byte   PreambleRxDetect;           //unit: byte  range: 0-31

  void   vSuperLowPowerCfg(void);
  void   vRxDutyCycleOff(void);
  void   vRxDutyCycleOn(void);
  void   vTxDutyCycleOff(void);
  void   vTxDutyCycleOn(void);
  void   vDutyCycleGoOn(void);
  void   vDutyCyclePause(void);
  void   vSetFifoInStby(byte msg[], byte length);
   
       
  private:
   bool  RssiTrig;
   bool  FhssLockTrig;
   byte  FhssLockChannel;
 signed char  RssiPeakLock;
   byte  FhssPeakChannel;
   spi3Class Spi3;   
 };
#else
 #warning "CMT2300A_SLPDriver.h have been defined!"

#endif
