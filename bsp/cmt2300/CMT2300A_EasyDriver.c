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

/*****************************************************************************************/
//#include "project.h"
#include "my_tools.h"
#include "main.h"
#include "CMT2300A_EasyDriver.h"
#include "CMT2300_GPIO_Define.h"
/*****************************************************************************************/

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
void AntSwitch_byhand(byte mode);                   
/*****************************************************************************************/
byte StatusTemp=0;
/**********************************************************
State Ctrl
**********************************************************/
/**********************************************************
**Name:     vSoftReset
**Function: Software reset Chipset
**Input:    none
**Output:   none
**********************************************************/
void vSoftReset(void)
{
 vSpi3Write(((word)CUS_SOFTRST<<8)+0xFF); 
 delay_ms(15);    
}

/**********************************************************
**Name:     bGoSleep
**Function: Entry Sleep Mode
**Input:    none
**Output:   none
**********************************************************/
bool bGoSleep(void)
{
 byte tmp;
 
 vSpi3Write(((word)CUS_MODE_CTL<<8)+MODE_GO_SLEEP); 
 delay_us(100);  
 tmp = (MODE_MASK_STA & bSpi3Read(CUS_MODE_STA)); 
 if(tmp==MODE_STA_SLEEP)
  return(TRUE);
 else
  return(FALSE);
  
}

/**********************************************************
**Name:     bGoStandby
**Function: Entry Standby Mode
**Input:    none
**Output:   none
**********************************************************/
bool bGoStandby(void)
{
 byte tmp, i; 
 
 RssiTrig = FALSE;
 PktRevError = FALSE;
 
 vSpi3Write(((word)CUS_MODE_CTL<<8)+MODE_GO_STBY); 
 for(i=0; i<50; i++)
  {
  delay_us(50); 
 tmp = (MODE_MASK_STA & bSpi3Read(CUS_MODE_STA));
        
//myprintf_int("\r\nstat=", (int)tmp);
 if(tmp==MODE_STA_STBY)
  break;
 }
 if(i>=50)
  return(FALSE);
 else
  return(TRUE);
}


/**********************************************************
**Name:     bGoTx
**Function: Entry Tx Mode
**Input:    none
**Output:   none
**********************************************************/
bool bGoTx(void)
{
 byte tmp, i;
 
 vSpi3Write(((word)CUS_MODE_CTL<<8)+MODE_GO_TFS); 
 for(i=0; i<50; i++)
  {
  delay_us(50); 
 tmp = (MODE_MASK_STA & bSpi3Read(CUS_MODE_STA)); 
 if(tmp==MODE_STA_TFS)
  break;
 }
 if(i>=50)
  return(FALSE);
  
 vSpi3Write(((word)CUS_MODE_CTL<<8)+MODE_GO_TX);  
 for(i=0; i<50; i++)
  {
  delay_us(50);
  tmp=0; 
 tmp = (MODE_MASK_STA & bSpi3Read(CUS_MODE_STA));
 StatusTemp= tmp;
 if(tmp==MODE_STA_TX)
  break;
 }
 if(i>=50)
  return(FALSE);
 else
  return(TRUE);
}

/**********************************************************
**Name:     bGoRx
**Function: Entry Rx Mode
**Input:    none
**Output:   none
**********************************************************/
bool bGoRx(void)
{
 byte tmp, i;

 RssiTrig = FALSE;
 PktRevError = FALSE;
 
 vSetPayloadLength(FALSE, PayloadLength);  //Set traget reveive length

 vEnableRdFifo();        //when FIFO Merge Active, set fifo to Rd

 vSpi3Write(((word)CUS_MODE_CTL<<8)+MODE_GO_RFS); 
 for(i=0; i<50; i++)
  {
  delay_us(50); 
 tmp = (MODE_MASK_STA & bSpi3Read(CUS_MODE_STA)); 
 if(tmp==MODE_STA_RFS)
  break;
 }
 if(i>=50)
  return(FALSE);
  
 vSpi3Write(((word)CUS_MODE_CTL<<8)+MODE_GO_RX);  
 for(i=0; i<50; i++)
  {
  delay_us(50); 
 tmp = (MODE_MASK_STA & bSpi3Read(CUS_MODE_STA)); 
 if(tmp==MODE_STA_RX)
  break;
 }
 if(i>=50)
  return(FALSE);
 else
  return(TRUE);
}

/**********************************************************
**Name:     bGoSwitch
**Function: Tx to Rx  or  Rx to Tx, use for quick switch 
**Input:    none
**Output:   none
**********************************************************/
bool bGoSwitch(void)
{
 byte tmp, i, z;

 tmp = (MODE_MASK_STA & bSpi3Read(CUS_MODE_STA)); 
 if((tmp!=MODE_STA_RX)&&(tmp!=MODE_STA_TX))
  return(FALSE);
 
 vSpi3Write(((word)CUS_MODE_CTL<<8)+MODE_GO_SWITCH); 
 for(i=0; i<50; i++)
  {
  delay_us(50);  
  z = (MODE_MASK_STA & bSpi3Read(CUS_MODE_STA));
  if(tmp==MODE_STA_RX)
   {
   if(z==MODE_STA_TX)
    break;
   }
  else
   {
   if(z==MODE_STA_RX) 
    break;
   }
  }
 if(i>=50)
  return(FALSE);
 else
  return(TRUE);
}

/**********************************************************
**Name:     bReadStatus
**Function: read chipset status
**Input:    none
**Output:   none
**********************************************************/
byte bReadStatus(void)
{
 return(MODE_MASK_STA & bSpi3Read(CUS_MODE_STA));  
}

/**********************************************************
**Name:     bReadRssi
**Function: Read Rssi
**Input:    TRUE------dBm;
            FALSE-----Code;
**Output:   none
**********************************************************/
byte bReadRssi(bool unit_dbm)
{
  if(unit_dbm) {
    return(bSpi3Read(CUS_RSSI_DBM)-128);//with sign
  } else {  
    return(bSpi3Read(CUS_RSSI_CODE));
  }
}

/**********************************************************
GPIO & Interrupt CFG
**********************************************************/
/**********************************************************
**Name:     vGpioFuncCfg
**Function: GPIO Function config
**Input:    none
**Output:   none
**********************************************************/
void vGpioFuncCfg(byte io_cfg)
{
 vSpi3Write(((word)CUS_IO_SEL<<8)+io_cfg);
}

/**********************************************************
**Name:     vIntSrcCfg
**Function: config interrupt source  
**Input:    int_1, int_2
**Output:   none
**********************************************************/
void vIntSrcCfg(byte int_1, byte int_2)
{
 byte tmp;
 tmp = INT_MASK & bSpi3Read(CUS_INT1_CTL);
 vSpi3Write(((word)CUS_INT1_CTL<<8)+(tmp|int_1));
 
 tmp = INT_MASK & bSpi3Read(CUS_INT2_CTL);
 vSpi3Write(((word)CUS_INT2_CTL<<8)+(tmp|int_2));
}

/**********************************************************
**Name:     vInt1SrcCfg
**Function: config interrupt source 1
**Input:    int_1
**Output:   none
**********************************************************/
void vInt1SrcCfg(byte int_1)
{
 byte tmp;
 tmp = INT_MASK & bSpi3Read(CUS_INT1_CTL);
 vSpi3Write(((word)CUS_INT1_CTL<<8)+(tmp|int_1));
}

/**********************************************************
**Name:     vInt2SrcCfg
**Function: config interrupt source 2 
**Input:    int_2
**Output:   none
**********************************************************/
void vInt2SrcCfg(byte int_2)
{
 byte tmp;
 tmp = INT_MASK & bSpi3Read(CUS_INT2_CTL);
 vSpi3Write(((word)CUS_INT2_CTL<<8)+(tmp|int_2));
}

/**********************************************************
**Name:     vIntSrcEnable
**Function: enable interrupt source 
**Input:    en_int
**Output:   none
**********************************************************/
void vIntSrcEnable(byte en_int)
{
 vSpi3Write(((word)CUS_INT_EN<<8)+en_int);    
}

/**********************************************************
**Name:     vIntSrcFlagClr
**Function: clear flag
**Input:    none
**Output:   equ CMT23_INT_EN
**     .7      .6     .5        .4       .3       .2       .1       .0
**   SL_TMO  RX_TMO  TX_TMO  PREAM_OK  SYNC_OK  NODE_OK  CRC_OK  PKT_DONE
**********************************************************/
byte bIntSrcFlagClr(void)
{
 byte tmp;
 byte int_clr2 = 0;
 byte int_clr1 = 0;
 byte flg = 0;
 
 tmp = bSpi3Read(CUS_INT_FLAG);
 if(tmp&LBD_FLG)    //LBD_FLG_Active
  int_clr2 |= LBD_CLR;
 
 if(tmp&PREAM_OK_FLG)   //Preamble Active
  {
  int_clr2 |= PREAM_OK_CLR;
  flg |= PREAM_OK_EN;
 }
 if(tmp&SYNC_OK_FLG)   //Sync Active
  {
  int_clr2 |= SYNC_OK_CLR;  
  flg |= SYNC_OK_EN;  
  }
 if(tmp&NODE_OK_FLG)   //Node Addr Active
  {
  int_clr2 |= NODE_OK_CLR; 
  flg |= NODE_OK_EN;
  }
 if(tmp&CRC_OK_FLG)    //Crc Pass Active
  {
  int_clr2 |= CRC_OK_CLR;
  flg |= CRC_OK_EN;
  }
 if(tmp&PKT_OK_FLG)    //Rx Done Active
  {
  int_clr2 |= PKT_DONE_CLR;
  flg |= PKT_DONE_EN;
  }
  
 if(tmp&COL_ERR_FLG)   //这两个都必须通过RX_DONE清除
  int_clr2 |= PKT_DONE_CLR;
 if(tmp&PKT_ERR_FLG)
  int_clr2 |= PKT_DONE_CLR;
  
 vSpi3Write(((word)CUS_INT_CLR2<<8)+int_clr2); //Clear flag
 
 
 tmp = bSpi3Read(CUS_INT_CLR1);
 if(tmp&TX_DONE_FLG)
  {
  int_clr1 |= TX_DONE_CLR;
  flg |= TX_DONE_EN;
  } 
 if(tmp&SL_TMO_FLG)
  {
  int_clr1 |= SL_TMO_CLR;
  flg |= SL_TMO_EN;
  } 
 if(tmp&RX_TMO_FLG)
  {
  int_clr1 |= RX_TMO_CLR;
  flg |= RX_TMO_EN;
  } 
 vSpi3Write(((word)CUS_INT_CLR1<<8)+int_clr1); //Clear flag 
 
 return(flg);
}

/**********************************************************
**Name:     vEnableAntSwitch
**Function:  
**Input:    
**Output:   none
**********************************************************/
void vEnableAntSwitch(byte mode)
{
 byte tmp;
 tmp = bSpi3Read(CUS_INT1_CTL);
 tmp&= 0x3F;
 switch(mode)
  {
  case 1:
   tmp |= RF_SWT1_EN; break;  //GPO1=RxActive; GPO2=TxActive 
  case 2:
   tmp |= RF_SWT2_EN; break;  //GPO1=RxActive; GPO2=!RxActive
  case 0:
  default:
   break;       //Disable 
  }
 vSpi3Write(((word)CUS_INT1_CTL<<8)+tmp);
}

/**********************************************************************************/
void AntSwitch_byhand(byte mode)////[1]:tx  [0]:rx
{
    TX_ANT_OUT();
    RX_ANT_OUT();
   switch(mode)
   {
       case 0:
       Clr_RX_ANT();
       Set_TX_ANT();
       break;
       case 1:
       Set_RX_ANT();
       Clr_TX_ANT();
       break;
       default:
       break;
   }
 }     
/**********************************************************************************/

/**********************************************************
Fifo Packet Handle
**********************************************************/
/**********************************************************
**Name:     vClearFIFO
**Function: clear FIFO buffer
**Input:    none
**Output:   none
**********************************************************/
void vClearFifo(void)
{
 vSpi3Write(((word)CUS_FIFO_CLR<<8)+FIFO_CLR_RX+FIFO_CLR_TX);
}

/**********************************************************
**Name:     bReadFifoFlag
**Function: read fifo state flag
**Input:    none
**Output:   FIFO state
**********************************************************/
byte bReadFifoFlag(void)
{
 return(bSpi3Read(CUS_FIFO_FLAG));
}

/**********************************************************
**Name:     bReadIntFlag
**Function: read interrupt flag
**Input:    none
**Output:   interrupt flag
**********************************************************/
word wReadIntFlag(void)
{
 word tmp;
 tmp = bSpi3Read(CUS_INT_CLR1);
 tmp<<=8;
 tmp|= bSpi3Read(CUS_INT_FLAG);
 return(tmp); 
}

/**********************************************************
**Name:     vEnableRdFifo
**Function: set Fifo for read
**Input:    none
**Output:   none
**********************************************************/
void vEnableRdFifo(void)
{
 byte tmp;
 tmp = bSpi3Read(CUS_FIFO_CTL);
 tmp &= (~(SPI_FIFO_RD_WR_SEL|FIFO_RX_TX_SEL));
 if(PayloadLength>31)
  tmp |= FIFO_MERGE_EN;
 else
  tmp &= (~FIFO_MERGE_EN);
 vSpi3Write(((word)CUS_FIFO_CTL<<8)+tmp);
}

/**********************************************************
**Name:     vEnableWrFifo
**Function: set Fifo for wirte
**Input:    none
**Output:   none
**********************************************************/
void vEnableWrFifo(void)
{
 byte tmp;
 tmp = bSpi3Read(CUS_FIFO_CTL);
 tmp |= (SPI_FIFO_RD_WR_SEL|FIFO_RX_TX_SEL);
 if(PayloadLength>31)
  tmp |= FIFO_MERGE_EN;
 else
  tmp &= (~FIFO_MERGE_EN);
 vSpi3Write(((word)CUS_FIFO_CTL<<8)+tmp);
}

/**********************************************************
**Name:     vSetPayloadLength
**Function: set Fifo length to used
**Input:    none
**Output:   none
**********************************************************/
void vSetPayloadLength(bool mode, byte length)
{
 byte tmp; 
 byte len;
 bGoStandby();
 
 tmp = bSpi3Read(CUS_PKT14);
 tmp&= 0x8F;    //length<256
 tmp&= (~0x01);    //Packet Mode
 if(length!=0)
  {
  if(mode)                //Tx
   {
   if(FixedPktLength)
   len = length-1;
   else
   len = length;
   }
  else     //Rx
  len = length - 1;
   
  if(FixedPktLength)
  tmp |= PKT_TYPE_FIXED;
  else
  tmp |= PKT_TYPE_VARIABLE;
 }
 else
  return;

 vSpi3Write(((word)CUS_PKT14<<8)+tmp);
 vSpi3Write(((word)CUS_PKT15<<8)+(byte)len); //Payload length
}

/**********************************************************
**Name:     vAckPacket
**Function: Ack Packet
**Input:    [1] Enable; [0] Disable
**Output:   none
**********************************************************/
void vAckPacket(bool en)
{
 byte tmp;  
 tmp = bSpi3Read(CUS_PKT14);
 if(en)
    tmp|= AUTO_ACK_EN;
 else
  tmp&= (~AUTO_ACK_EN);
 vSpi3Write(((word)CUS_PKT14<<8)+tmp);
}

/**********************************************************
**函数名称：bReadFifo
**函数功能：接收长包用的读取Fifo
**输入参数：none
**输出参数：none
**Note：    should be call vEnableRdFifo() at GoRx
**********************************************************/
byte bReadFifo(void)
{
 return(bSpi3ReadFIFO());
}

/**********************************************************
**函数名称：vWriteFifo
**函数功能：接收长包用的读取Fifo
**输入参数：none
**输出参数：none
**Note：    should be call  vEnableWrFifo() at first
**********************************************************/
void vWriteFifo(byte dat)
{
 vSpi3WriteFIFO(dat);
} 


/**********************************************************
CFG
**********************************************************/
/**********************************************************
**Name:     vInit
**Function: Init. CMT2300A
**Input:    none
**Output:   none
**********************************************************/
void vInit(void)
{
 vSpi3Init();
 /*//limx debug
 GPO1In();
 GPO2In();
 GPO3In();
 */
 
 
 vSoftReset();
 bGoStandby();
}

void vCfgBank(word cfg[], byte length)
{
 byte i;
 
 if(length!=0)
  { 
  for(i=0; i<length; i++) 
   vSpi3Write(cfg[i]);
  } 
}

void vAfterCfg(void)            //call after vCfgBank
{
 byte tmp; 
 tmp = bSpi3Read(CUS_MODE_STA);
 tmp|= CFG_RETAIN;
 tmp&= (~RSTN_IN_EN);   //Disable RstPin 
 vSpi3Write(((word)CUS_MODE_STA<<8)+tmp);

 //limx debug, comment from hoperf
 //tmp = 0x00;                    //Disable All Calibration, when no need to use DutyCycleMode
 //vSpi3Write(((word)CUS_SYS2<<8)+tmp);

 //limx debug
 tmp = bSpi3Read(CUS_EN_CTL);
 tmp |= UNLOCK_STOP_EN;
 vSpi3Write(((word)CUS_EN_CTL<<8)+tmp);
 
 bIntSrcFlagClr();
 
 FhssChannelRange = 0;
 FhssChannel      = 0;
 FhssRssiAvg      = 0;
 FhssRssiTH       = 0;
}

/**********************************************************
appliciation
**********************************************************/
/**********************************************************
**函数名称：bGetMessage
**函数功能：接收一包数据
**输入参数：无
**输出参数：非0——接收成功
**            0——接收失败
**备注:     需要与GPO的中断搭配使用，适用于MCU中断场合
**********************************************************/
/*byte bGetMessage(byte msg[])
{
 byte i; 
 
 if(FixedPktLength)
  {
   vSpi3BurstReadFIFO(msg, PayloadLength);
 i = PayloadLength;
 }
 else
  {
 i = bSpi3ReadFIFO(); 
  vSpi3BurstReadFIFO(msg, i);
  }
 return(i);
}*/

/**********************************************************
**函数名称：bGetMessageByFlag
**函数功能：接收一包数据
**输入参数：无
**输出参数：非0——接收成功
**            0——没接收到数据
**           FF——接收到错误
**备注:     查询标识机制，节省GPO中断
**********************************************************/
/*byte bGetMessageByFlag(byte msg[])
{
 byte tmp;
 byte tmp1;
 byte rev = 0;
 tmp = bSpi3Read(CUS_INT_FLAG);
 if((tmp&SYNC_OK_FLG)&&(!RssiTrig))
  {
  PktRssi = bReadRssi(TRUE);
  PktRssiCode = bReadRssi(FALSE);
  RssiTrig = TRUE;
  }
 
 tmp1 = bSpi3Read(CUS_PKT21);
 if(tmp1&CRC_EN)   //Enable CrcCheck
  {
  if(tmp&CRC_OK_FLG)
   {
   if(FixedPktLength)
    {
     vSpi3BurstReadFIFO(msg, PayloadLength);
   rev = PayloadLength;
   }
   else
    { 
   rev = bSpi3ReadFIFO(); 
    vSpi3BurstReadFIFO(msg, rev);
    }
   RssiTrig = FALSE;
   PktRevError = FALSE;
   }
  }
 else
  {
 if(tmp&PKT_OK_FLG)   
  {
   if(FixedPktLength)
    {
     vSpi3BurstReadFIFO(msg, PayloadLength);
   rev = PayloadLength;
   }
   else
    { 
   rev = bSpi3ReadFIFO(); 
    vSpi3BurstReadFIFO(msg, rev);
    } 
   RssiTrig = FALSE;  
  PktRevError = FALSE;
  }
  }
 
 if((tmp&COL_ERR_FLG)||(tmp&PKT_ERR_FLG)) //错误处理
 {
 PktRevError = TRUE; 
 rev = 0;
 }
 return(rev);
}*/

/**********************************************************
**函数名称：vSendMessage
**函数功能：发射一包数据
**输入参数：none
**输出参数：none
**备注    ：需要与GPO的中断搭配使用，适用于MCU中断场合          
**********************************************************/
void vSendMessage(byte msg[], byte length)
{
  //char ret = 0;
 //mode1
  
#if 0
 vSetPayloadLength(TRUE, length);
 bGoStandby();
 vEnableWrFifo(); 
 vClearFifo();
 vSpi3BurstWriteFIFO(msg, length);
 ret = bGoTx();
 myprintf_char(ret);
#endif 
 
 //mode2
#if 1
 //bIntSrcFlagClr();
 
 /*
  vSetPayloadLength(TRUE, length);
  vEnableWrFifo(); 
  vClearFifo();
 
  bGoTx();
  // myprintf_str(msg);
  while(bReadStatus() != MODE_STA_TX);//limx debug
  */
 
 //limx debug, from hoperf chen
  /* 
  while(!bGoTx()()){
    bgostandby();
  }//RX also
  */
 
// DelayMs(10);
  vSpi3BurstWriteFIFO(msg, length);
#endif
 
     

}

/**********************************************************
**函数名称：bSendMessageByFlag
**函数功能：发射一包数据
**输入参数：none
**输出参数：none
**备注    ：查询标识机制，节省GPO中断         
**********************************************************/
bool bSendMessageByFlag(byte msg[], byte length)
{
 byte tmp;
 word overtime;
  
 //mode1
 //vSetPayloadLength(TRUE, length);
 //bGoStandby();
 //vEnableWrFifo(); 
 //vClearFifo();
 //vSpi3BurstWriteFIFO(msg, length);
 //bGoTx();
 
 //mode2
 bIntSrcFlagClr();
 vSetPayloadLength(TRUE, length);
 vEnableWrFifo(); 
 vClearFifo();
 bGoTx();
 vSpi3BurstWriteFIFO(msg, length);
 
 overtime = 0;
 do 
  {
  delay_us(100);
 overtime++;
  tmp = bSpi3Read(CUS_INT_CLR1);
  }while(((tmp&TX_DONE_FLG)==0)||(overtime>10000));
 if(overtime>=10000)
  return(FALSE);
 else
  return(TRUE);
}

/**********************************************************
**函数名称：vSetChannelOffset
**函数功能：设置跳频间隔
**输入参数：interval  unit:KHz
**输出参数：none
**备注    ：
**********************************************************/
/*void vSetChannelOffset(word interval)
{
 byte offset;
 offset = (interval<<1)/5;   //unit:2.5KHz
 vSpi3Write(((word)CUS_FREQ_OFS<<8)+offset);
}*/

/**********************************************************
**函数名称：vSetChannel
**函数功能：设置频道
**输入参数：channel
**输出参数：none
**备注    ：
**********************************************************/
void vSetChannel(word channel)
{
 vSpi3Write(((word)CUS_FREQ_CHNL<<8)+channel);
}

/**********************************************************
**函数名称：vSetTxPreamble
**函数功能：设置发射的Preamble长度
**输入参数：length
**输出参数：none
**备注    ：
**********************************************************/
void vSetTxPreamble(word length)
{
 vSpi3Write(((word)CUS_PKT3<<8)+(byte)(length>>8));
 vSpi3Write(((word)CUS_PKT2<<8)+(byte)length);
}

/**********************************************************
**函数名称：bFHSSDetect
**函数功能：Channel detect
**输入参数：range: channel range
            thres: threshold value
**输出参数：0xFF--no detect
            other-channel lock(in channel range)
**备注    ：
**********************************************************/
/*byte bFHSSDetect(void)
{
 signed char rssi;
 signed int  rssi_cal;
 byte i;
 
 bGoStandby();
 vSetChannel(FhssChannel);
 bGoRx();
 delay_us(500);  
 rssi_cal = bReadRssi(TRUE);
 for(i=0; i<3; i++)
 {
  delay_us(100);  
  rssi_cal+= bReadRssi(TRUE);
 }
 rssi = rssi_cal>>2;
 bGoStandby();

 
 if(!FhssLockTrig)
  {
  if((rssi>FhssRssiAvg)&&((rssi-FhssRssiAvg)>=FhssRssiTH))
   {
   FhssLockTrig = TRUE;
  RssiPeakLock    = rssi;
  FhssLockChannel = FhssChannel;
  FhssPeakChannel = FhssChannel;
  }
    else
     {
     rssi_cal = FhssRssiAvg+rssi;
        FhssRssiAvg = rssi_cal>>1;   
     }
  FhssChannel++;
  if(FhssChannel>=FhssChannelRange) 
   FhssChannel = 0;
  return(0xFF);
  }
 else
  {
 if(FhssChannel==FhssLockChannel)
   {
   FhssLockTrig = FALSE;
   return(FhssPeakChannel);
   }
  if(RssiPeakLock<=rssi)
   {
   RssiPeakLock    = rssi;
  FhssPeakChannel = FhssChannel;
   }
  FhssChannel++; 
  if(FhssChannel>=FhssChannelRange) 
   FhssChannel = 0;  
  return(0xFF); 
 }
}*/

void vSetDirect()
{
  byte tmp = bSpi3Read(CUS_TX1);
  tmp |= TX_DIN_GPIO;
  vSpi3Write(((word)CUS_TX1<<8) + tmp);
  
  tmp = bSpi3Read(CUS_FIFO_CTL);
  tmp |= TX_DIN_EN;
  tmp |= TX_DIN_GPIO3;//TX_DIN_HIGH;
  vSpi3Write(((word)CUS_FIFO_CTL<<8) + tmp);
}


