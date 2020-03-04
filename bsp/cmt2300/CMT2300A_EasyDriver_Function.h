extern bool  FixedPktLength;      //false: for contain packet length in Tx message, the same mean with variable lenth//
extern byte  PayloadLength;      //unit: byte  range: 1-255, effect FIFO buffer                                   
extern byte  PktRssi;             //with Sign
extern byte  PktRssiCode;
extern bool  PktRevError;

//State Ctrl
extern void  vSoftReset(void);
extern bool  bGoSleep(void); 
extern bool  bGoStandby(void); 
extern bool  bGoTx(void);
extern bool  bGoRx(void);
extern bool  bGoSwitch(void);
extern byte  bReadStatus(void);
extern byte  bReadRssi(bool unit_dbm);

//GPIO & Interrupt 
extern void  vGpioFuncCfg(byte io_cfg);
extern void  vIntSrcCfg(byte int_1, byte int_2);
extern void  vInt1SrcCfg(byte int_1);
extern void  vInt2SrcCfg(byte int_2);     
extern void  vIntSrcEnable(byte en_int);
extern byte  bIntSrcFlagClr(void);
extern void  vEnableAntSwitch(byte mode);   
   
//Fifo packet handle
extern void  vClearFifo(void);
extern byte  bReadFifoFlag(void);
extern word  wReadIntFlag(void);
extern void  vEnableRdFifo(void);
extern void  vEnableWrFifo(void);
extern void  vSetPayloadLength(bool mode, byte length);
extern void  vAckPacket(bool en);
extern void  vWriteFifo(byte dat);
extern byte  bReadFifo(void);
 
//Config
extern void  vInit(void);
extern void  vCfgBank(word cfg[], byte length);
extern void  vAfterCfg(void);
  
//appliciation
extern byte  bGetMessage(byte msg[]);
extern byte  bGetMessageByFlag(byte msg[]);
extern void  vSendMessage(byte msg[], byte length);
extern bool  bSendMessageByFlag(byte msg[], byte length);

//FHSS Part
extern byte  FhssChannelRange;
extern byte  FhssChannel;
extern signed int  FhssRssiAvg;
extern signed char  FhssRssiTH;
extern void  vSetChannelOffset(word interval);
extern void  vSetChannel(word channel);
extern void  vSetTxPreamble(word length);
extern byte  bFHSSDetect(void);
extern void AntSwitch_byhand(byte mode);

void vSetDirect();

