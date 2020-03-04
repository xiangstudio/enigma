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
 * file       CMT2300A_AdvDriver
 * brief      how to use CMT2300A
 * hardware   HopeDuino with RFM300
 *            
 *
 * version    1.0
 * date       Dec 29 2016
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
 *            2. add clear fifo after call vEnableWrFifo
 
 */
 
 #ifndef CMT2300A_AdvDriver_h
 #define CMT2300A_AdvDriver_h

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
 //PORTB                     //DDRx        //PORTx    PINx
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
 
 
 enum modulationType {OOK, FSK, GFSK};
 enum crcType        {CRC_CCITT, CRC_IBM, CRC_ITU};
 enum whiteType      {PN9_CCITT, PN9_IBM, PN7_AMI};
 enum prefixType     {TX_ZERO, TX_HIGH, TX_PREAMB}; 
 
 enum cdr_avgType    {CDR_6_8, CDR_1_2, CDR_6_16, CDR_1_4, CDR_11_64, CDR_1_8, CDR_3_32, CDR_1_16};
 enum cdr_rangeType  {BIT_ERR_6, BIT_ERR_9, BIT_ERR_12, BIT_ERR_15};
 enum cdr_modeType   {CDR_TRACING, CDR_COUNTING, CDR_MANCHESTER, CDR_RAWDATA};
 
  class cmt2300aAdvance
 {
  public: 
  bool  FixedPktLength;  //false: for contain packet length in Tx message, the same mean with variable lenth
                          //true : for doesn't include packet length in Tx message, the same mean with fixed length
  byte  PayloadLength;  //unit: byte  range: 1-255                                     
   
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

  //***************Add in AdvanceDriver as below***************************
       
  //*******************PartStructure***************************************
  bool  DirectMode;    //false: PacketMode; true: DirectMode
  
  //PayloadPart
  bool  PayloadPriority;   //false: 先对payload+CRC每个byte MSB进行编解码
                           //true:  先对payload+CRC每个byte LSB进行编解码
  bool  FifoMerge;    //false: Tx 32, Rx 32; true: total 64Byte
  byte  FifoTH;

  //PreamblePart
  word  PreambleTxLength;   //unit: byte  range: 0-65536
  byte  PreambleRxDetect;   //unit: byte  range: 0-31
  bool  PreambleNibble;   //false: unit = byte;  
                                  //true:  unit = nibble;
  byte  PreambleValue;   //default: 0x55
  prefixType TxPrefix;   //
  //SyncPart
  bool  SyncEnMan;    //true: SyncManchester;  false: SyncDcFree(NRZ)
  byte  SyncSize;     //unit: none, range: 1-8[Byte], value '0' is not allowed!
  byte  SyncTol;
  byte  SyncWord[8];
  //NodePart
  byte  NodeEnable;    //[0]: Disable; [1] Only NodeAddress; [2] NodeAddress&Zero; [3] NodeAddress&Zero&AllOne
  bool  NodePosition;    //false: after length;
  //true:  before length;
  byte  NodeSize;     //range: 1-4
  byte  NodeValue[4];
  byte  NodeRev[4];
  //CrcPart 
  bool  CrcEnable;    //true:  CRC enable， 
  //false: CRC disable
  crcType  CrcMode;    //default: CCITT-16 
  //         IBM-16
  //         ITU-16
  word  CrcSeed;     //
  bool  CrcDatOnly;    //false: with length/node
                                  //true:  data only
  bool  CrcInverse;    //false: not inverse crc result
                                  //true:  inverse crc result 
  bool  CrcSwap;     //false: [15:8]+[7:0]
                                  //true:  [7:0]+[15:8]
  bool  CrcBitOrder;    //false: [15:0]
                                  //true:  [0:15]
  //WhitenPart 
  bool  WhitenEnable;    //false: Disable;  true: Enable
  word  WhitenSeed;    //
  bool  WhitenSeedActive;
  whiteType WhitenType;
  //ManchesterPart
  bool  ManEnable;                //false: Disable;  true: Enable
  bool  ManDataInverse;   //false: 01='1'；10='0'
                         //true : 10='1'；01='0' 
  //FECPart
  bool  FecEnable;    //
  bool  FecType;     //                                           
  //*********************************************************************
  
  //******************CdrPart********************************************
  cdr_avgType   CdrAvgSel;  //range: 0~7, 越大滤波越厉害；
  cdr_modeType  CdrModeSel;  //range: 0~3, 0-tracing, 1-count, 2-manchester, 3-no cdr(raw data)
                                  //       tracing 适合各种应用情况，也便于速率偏差；
                                  //       count   适合速率很准，且对准速率情况； 
  cdr_rangeType CdrRangeSel;  //range: 0-6.3%, 1-9.4%, 2-12.5%, 3-15.6%
  bool          Cdr3rdEn;
  bool          Cdr4thEn;
  //*********************************************************************

  //******************Function*******************************************
  void  vSetMultipeSend(byte pkt_cnt, byte pkt_interval);     
  void  vBaseBankCfg(void);
  void  vGaussianSetting(bool en, byte bt);
  byte  bGetVoltage(void);
  void  vCdrCfg(void);
     
  private:   
   bool  RssiTrig;
   bool  FhssLockTrig;
   byte  FhssLockChannel;
   signed char  RssiPeakLock;
   byte  FhssPeakChannel;   
   spi3Class Spi3;
   
 };
#else
 #warning "CMT2300A_AdvDriver.h have been defined!"

#endif
