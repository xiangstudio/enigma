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


#include <CMT2300A_SLPDriver.h>

/**********************************************************
State Ctrl
**********************************************************/
/**********************************************************
**Name:     vSoftReset
**Function: Software reset Chipset
**Input:    none
**Output:   none
**********************************************************/
void cmt2300aSLP::vSoftReset(void)
{
 Spi3.vSpi3Write(((word)CUS_SOFTRST<<8)+0xFF); 
 _delay_ms(15);				
}

/**********************************************************
**Name:     bGoSleep
**Function: Entry Sleep Mode
**Input:    none
**Output:   none
**********************************************************/
bool cmt2300aSLP::bGoSleep(void)
{
 byte tmp;
 
 Spi3.vSpi3Write(((word)CUS_MODE_CTL<<8)+MODE_GO_SLEEP);	
 _delay_us(100);		
 tmp = (MODE_MASK_STA & Spi3.bSpi3Read(CUS_MODE_STA));	
 if(tmp==MODE_STA_SLEEP)
 	return(true);
 else
 	return(false);
}

/**********************************************************
**Name:     bGoStandby
**Function: Entry Standby Mode
**Input:    none
**Output:   none
**********************************************************/
bool cmt2300aSLP::bGoStandby(void)
{
 byte tmp, i;	
 
 RssiTrig = false;
 PktRevError = false;
 
 Spi3.vSpi3Write(((word)CUS_MODE_CTL<<8)+MODE_GO_STBY);	
 for(i=0; i<50; i++)
 	{
 	_delay_us(50);	
	tmp = (MODE_MASK_STA & Spi3.bSpi3Read(CUS_MODE_STA));	
	if(tmp==MODE_STA_STBY)
		break;
	}
 if(i>=50)
 	return(false);
 else
 	return(true);
}


/**********************************************************
**Name:     bGoTx
**Function: Entry Tx Mode
**Input:    none
**Output:   none
**********************************************************/
bool cmt2300aSLP::bGoTx(void)
{
 byte tmp, i;
 
 Spi3.vSpi3Write(((word)CUS_MODE_CTL<<8)+MODE_GO_TFS);	
 for(i=0; i<50; i++)
 	{
 	_delay_us(50);	
	tmp = (MODE_MASK_STA & Spi3.bSpi3Read(CUS_MODE_STA));	
	if(tmp==MODE_STA_TFS)
		break;
	}
 if(i>=50)
 	return(false);
  
 Spi3.vSpi3Write(((word)CUS_MODE_CTL<<8)+MODE_GO_TX);		
 for(i=0; i<50; i++)
 	{
 	_delay_us(50);	
	tmp = (MODE_MASK_STA & Spi3.bSpi3Read(CUS_MODE_STA));	
	if(tmp==MODE_STA_TX)
		break;
	}
 if(i>=50)
 	return(false);
 else
 	return(true);
}

/**********************************************************
**Name:     bGoRx
**Function: Entry Rx Mode
**Input:    none
**Output:   none
**********************************************************/
bool cmt2300aSLP::bGoRx(void)
{
 byte tmp, i;

 RssiTrig = false;
 PktRevError = false;
 
 vSetPayloadLength(false, PayloadLength);		//Set traget reveive length
 
 vEnableRdFifo();								//when FIFO Merge Active, set fifo to Rd 

 Spi3.vSpi3Write(((word)CUS_MODE_CTL<<8)+MODE_GO_RFS);	
 for(i=0; i<50; i++)
 	{
 	_delay_us(50);	
	tmp = (MODE_MASK_STA & Spi3.bSpi3Read(CUS_MODE_STA));	
	if(tmp==MODE_STA_RFS)
		break;
	}
 if(i>=50)
 	return(false);
  
 Spi3.vSpi3Write(((word)CUS_MODE_CTL<<8)+MODE_GO_RX);		
 for(i=0; i<50; i++)
 	{
 	_delay_us(50);	
	tmp = (MODE_MASK_STA & Spi3.bSpi3Read(CUS_MODE_STA));	
	if(tmp==MODE_STA_RX)
		break;
	}
 if(i>=50)
 	return(false);
 else
 	return(true);
}

/**********************************************************
**Name:     bGoSwitch
**Function: Tx to Rx  or  Rx to Tx, use for quick switch 
**Input:    none
**Output:   none
**********************************************************/
bool cmt2300aSLP::bGoSwitch(void)
{
 byte tmp, i, z;

 tmp = (MODE_MASK_STA & Spi3.bSpi3Read(CUS_MODE_STA));	
 if((tmp!=MODE_STA_RX)&&(tmp!=MODE_STA_TX))
 	return(false);
 
 Spi3.vSpi3Write(((word)CUS_MODE_CTL<<8)+MODE_GO_SWITCH);	
 for(i=0; i<50; i++)
 	{
 	_delay_us(50);		
 	z = (MODE_MASK_STA & Spi3.bSpi3Read(CUS_MODE_STA));
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
 	return(false);
 else
 	return(true);
}

/**********************************************************
**Name:     bReadStatus
**Function: read chipset status
**Input:    none
**Output:   none
**********************************************************/
byte cmt2300aSLP::bReadStatus(void)
{
 return(MODE_MASK_STA & Spi3.bSpi3Read(CUS_MODE_STA));		
}

/**********************************************************
**Name:     bReadRssi
**Function: Read Rssi
**Input:    true------dBm;
            false-----Code;
**Output:   none
**********************************************************/
byte cmt2300aSLP::bReadRssi(bool unit_dbm)
{
 if(unit_dbm)
 	return(Spi3.bSpi3Read(CUS_RSSI_DBM)-128);		//with sign
 else		
 	return(Spi3.bSpi3Read(CUS_RSSI_CODE));
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
void cmt2300aSLP::vGpioFuncCfg(byte io_cfg)
{
 Spi3.vSpi3Write(((word)CUS_IO_SEL<<8)+io_cfg);
}

/**********************************************************
**Name:     vIntSrcCfg
**Function: config interrupt source  
**Input:    int_1, int_2
**Output:   none
**********************************************************/
void cmt2300aSLP::vIntSrcCfg(byte int_1, byte int_2)
{
 byte tmp;
 tmp = INT_MASK & Spi3.bSpi3Read(CUS_INT1_CTL);
 Spi3.vSpi3Write(((word)CUS_INT1_CTL<<8)+(tmp|int_1));
 
 tmp = INT_MASK & Spi3.bSpi3Read(CUS_INT2_CTL);
 Spi3.vSpi3Write(((word)CUS_INT2_CTL<<8)+(tmp|int_2));
}

/**********************************************************
**Name:     vInt1SrcCfg
**Function: config interrupt source 1
**Input:    int_1
**Output:   none
**********************************************************/
void cmt2300aSLP::vInt1SrcCfg(byte int_1)
{
 byte tmp;
 tmp = INT_MASK & Spi3.bSpi3Read(CUS_INT1_CTL);
 Spi3.vSpi3Write(((word)CUS_INT1_CTL<<8)+(tmp|int_1));
}

/**********************************************************
**Name:     vInt2SrcCfg
**Function: config interrupt source 2 
**Input:    int_2
**Output:   none
**********************************************************/
void cmt2300aSLP::vInt2SrcCfg(byte int_2)
{
 byte tmp;
 tmp = INT_MASK & Spi3.bSpi3Read(CUS_INT2_CTL);
 Spi3.vSpi3Write(((word)CUS_INT2_CTL<<8)+(tmp|int_2));
}

/**********************************************************
**Name:     vIntSrcEnable
**Function: enable interrupt source 
**Input:    en_int
**Output:   none
**********************************************************/
void cmt2300aSLP::vIntSrcEnable(byte en_int)
{
 Spi3.vSpi3Write(((word)CUS_INT_EN<<8)+en_int);				
}

/**********************************************************
**Name:     vIntSrcFlagClr
**Function: clear flag
**Input:    none
**Output:   equ CMT23_INT_EN
**     .7      .6     .5        .4       .3       .2       .1       .0
**   SL_TMO  RX_TMO  TX_TMO  PREAM_OK  SYNC_OK  NODE_OK  CRC_OK  PKT_DONE
**********************************************************/
byte cmt2300aSLP::bIntSrcFlagClr(void)
{
 byte tmp;
 byte int_clr2 = 0;
 byte int_clr1 = 0;
 byte flg = 0;
 
 tmp = Spi3.bSpi3Read(CUS_INT_FLAG);
 if(tmp&LBD_FLG)				//LBD_FLG_Active
 	int_clr2 |= LBD_CLR;
 
 if(tmp&PREAM_OK_FLG)			//Preamble Active
 	{
 	int_clr2 |= PREAM_OK_CLR;
 	flg |= PREAM_OK_EN;
	}
 if(tmp&SYNC_OK_FLG)			//Sync Active
 	{
 	int_clr2 |= SYNC_OK_CLR;		
 	flg |= SYNC_OK_EN;		
 	}
 if(tmp&NODE_OK_FLG)			//Node Addr Active
 	{
 	int_clr2 |= NODE_OK_CLR;	
 	flg |= NODE_OK_EN;
 	}
 if(tmp&CRC_OK_FLG)				//Crc Pass Active
 	{
 	int_clr2 |= CRC_OK_CLR;
 	flg |= CRC_OK_EN;
 	}
 if(tmp&PKT_OK_FLG)				//Rx Done Active
 	{
 	int_clr2 |= PKT_DONE_CLR;
 	flg |= PKT_DONE_EN;
 	}
 	
 if(tmp&COL_ERR_FLG)			//这两个都必须通过RX_DONE清除
 	int_clr2 |= PKT_DONE_CLR;
 if(tmp&PKT_ERR_FLG)
 	int_clr2 |= PKT_DONE_CLR;
 	
 Spi3.vSpi3Write(((word)CUS_INT_CLR2<<8)+int_clr2);	//Clear flag
 
 
 tmp = Spi3.bSpi3Read(CUS_INT_CLR1);
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
 Spi3.vSpi3Write(((word)CUS_INT_CLR1<<8)+int_clr1);	//Clear flag 
 
 return(flg);
}

/**********************************************************
**Name:     vEnableAntSwitch
**Function:  
**Input:    
**Output:   none
**********************************************************/
void cmt2300aSLP::vEnableAntSwitch(byte mode)
{
 byte tmp;
 tmp = Spi3.bSpi3Read(CUS_INT1_CTL);
 tmp&= 0x3F;
 switch(mode)
 	{
 	case 1:
 		tmp |= RF_SWT1_EN; break;		//GPO1=RxActive; GPO2=TxActive	
 	case 2:
 		tmp |= RF_SWT2_EN; break;		//GPO1=RxActive; GPO2=!RxActive
 	case 0:
 	default:
 		break;							//Disable	
 	}
 Spi3.vSpi3Write(((word)CUS_INT1_CTL<<8)+tmp);
}

/**********************************************************
Fifo Packet Handle
**********************************************************/
/**********************************************************
**Name:     vClearFIFO
**Function: clear FIFO buffer
**Input:    none
**Output:   none
**********************************************************/
void cmt2300aSLP::vClearFifo(void)
{
 Spi3.vSpi3Write(((word)CUS_FIFO_CLR<<8)+FIFO_CLR_RX+FIFO_CLR_TX);
}

/**********************************************************
**Name:     bReadFifoFlag
**Function: read fifo state flag
**Input:    none
**Output:   FIFO state
**********************************************************/
byte cmt2300aSLP::bReadFifoFlag(void)
{
 return(Spi3.bSpi3Read(CUS_FIFO_FLAG));
}

/**********************************************************
**Name:     bReadIntFlag
**Function: read interrupt flag
**Input:    none
**Output:   interrupt flag
**********************************************************/
word cmt2300aSLP::wReadIntFlag(void)
{
 word tmp;
 tmp = Spi3.bSpi3Read(CUS_INT_CLR1);
 tmp<<=8;
 tmp|= Spi3.bSpi3Read(CUS_INT_FLAG);
 return(tmp);	
}

/**********************************************************
**Name:     vEnableRdFifo
**Function: set Fifo for read
**Input:    none
**Output:   none
**********************************************************/
void cmt2300aSLP::vEnableRdFifo(void)
{
 byte tmp;
 tmp = Spi3.bSpi3Read(CUS_FIFO_CTL);
 tmp &= (~(SPI_FIFO_RD_WR_SEL|FIFO_RX_TX_SEL));
 if(PayloadLength>31)
 	tmp |= FIFO_MERGE_EN;
 else
 	tmp &= (~FIFO_MERGE_EN); 
 Spi3.vSpi3Write(((word)CUS_FIFO_CTL<<8)+tmp);
}

/**********************************************************
**Name:     vEnableWrFifo
**Function: set Fifo for wirte
**Input:    none
**Output:   none
**********************************************************/
void cmt2300aSLP::vEnableWrFifo(void)
{
 byte tmp;
 tmp = Spi3.bSpi3Read(CUS_FIFO_CTL);
 tmp |= (SPI_FIFO_RD_WR_SEL|FIFO_RX_TX_SEL);
 
 if(PayloadLength>31)
 	tmp |= FIFO_MERGE_EN;
 else
 	tmp &= (~FIFO_MERGE_EN); 

 Spi3.vSpi3Write(((word)CUS_FIFO_CTL<<8)+tmp);
}

/**********************************************************
**Name:     vSetPayloadLength
**Function: set Fifo length to used
**Input:    none
**Output:   none
**********************************************************/
void cmt2300aSLP::vSetPayloadLength(bool mode, byte length)
{
 byte tmp;	
 byte len;
 bGoStandby();
 
 tmp = Spi3.bSpi3Read(CUS_PKT14);
 tmp&= 0x8F;				//length<256
 tmp&= (~0x01);				//Packet Mode
 if(length!=0)
 	{
 	if(mode)                //Tx
 		{
 		if(FixedPktLength)
			len = length-1;
 		else
			len = length;
 		}
 	else					//Rx
		len = length - 1;
 		
 	if(FixedPktLength)
		tmp |= PKT_TYPE_FIXED;
 	else
		tmp |= PKT_TYPE_VARIABLE;
	}
 else
 	return;

 Spi3.vSpi3Write(((word)CUS_PKT14<<8)+tmp);
 Spi3.vSpi3Write(((word)CUS_PKT15<<8)+(byte)len);	//Payload length
}

/**********************************************************
**Name:     vAckPacket
**Function: Ack Packet
**Input:    [1] Enable; [0] Disable
**Output:   none
**********************************************************/
void cmt2300aSLP::vAckPacket(bool en)
{
 byte tmp;		
 tmp = Spi3.bSpi3Read(CUS_PKT14);
 if(en)
   	tmp|= AUTO_ACK_EN;
 else
 	tmp&= (~AUTO_ACK_EN);
 Spi3.vSpi3Write(((word)CUS_PKT14<<8)+tmp);
}

/**********************************************************
**函数名称：bReadFifo
**函数功能：接收长包用的读取Fifo
**输入参数：none
**输出参数：none
**Note：    should be call vEnableRdFifo() at first
**********************************************************/
byte cmt2300aSLP::bReadFifo(void)
{
 return(Spi3.bSpi3ReadFIFO());
}

/**********************************************************
**函数名称：vWriteFifo
**函数功能：接收长包用的读取Fifo
**输入参数：none
**输出参数：none
**Note：    should be call  vEnableWrFifo() at first
**********************************************************/
void cmt2300aSLP::vWriteFifo(byte dat)
{
 Spi3.vSpi3WriteFIFO(dat);
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
void cmt2300aSLP::vInit(void)
{
 Spi3.vSpi3Init();
 GPO1In();
 GPO2In();
 GPO3In();
 GPO4In();
 
 vSoftReset();
 bGoStandby();
}

void cmt2300aSLP::vCfgBank(word cfg[], byte length)
{
 byte i;
 
 if(length!=0)
 	{	
 	for(i=0; i<length; i++)	
 		Spi3.vSpi3Write(cfg[i]);
 	}	
}

void cmt2300aSLP::vAfterCfg(void)            //call after vCfgBank
{
 byte tmp;	
 tmp = Spi3.bSpi3Read(CUS_MODE_STA);
 tmp|= CFG_RETAIN;
 tmp&= (~RSTN_IN_EN);						//Disable RstPin	
 Spi3.vSpi3Write(((word)CUS_MODE_STA<<8)+tmp);

 tmp = Spi3.bSpi3Read(CUS_SYS2);			//Enable Calibration
 tmp|= (LFOSC_RECAL_EN|LFOSC_CAL1_EN|LFOSC_CAL2_EN);
 Spi3.vSpi3Write(((word)CUS_SYS2<<8)+tmp);

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
byte cmt2300aSLP::bGetMessage(byte msg[])
{
 byte i;	
 
 if(FixedPktLength)
 	{
  	Spi3.vSpi3BurstReadFIFO(msg, PayloadLength);
	i = PayloadLength;
	}
 else
 	{
	i = Spi3.bSpi3ReadFIFO();	
 	Spi3.vSpi3BurstReadFIFO(msg, i);
 	}
 return(i);
}

/**********************************************************
**函数名称：bGetMessageByFlag
**函数功能：接收一包数据
**输入参数：无
**输出参数：非0——接收成功
**            0——没接收到数据
**           FF——接收到错误
**备注:     查询标识机制，节省GPO中断
**********************************************************/
byte cmt2300aSLP::bGetMessageByFlag(byte msg[])
{
 byte tmp;
 byte tmp1;
 byte rev = 0;
 tmp = Spi3.bSpi3Read(CUS_INT_FLAG);
 if((tmp&SYNC_OK_FLG)&&(!RssiTrig))
 	{
 	PktRssi = bReadRssi(true);
 	PktRssiCode = bReadRssi(false);
 	RssiTrig = true;
 	}
 
 tmp1 = Spi3.bSpi3Read(CUS_PKT21);

 if(tmp1&CRC_EN)			//Enable CrcCheck
 	{
 	if(tmp&CRC_OK_FLG)
 		{
 		if(FixedPktLength)
 			{
  			Spi3.vSpi3BurstReadFIFO(msg, PayloadLength);
			rev = PayloadLength;
			}
 		else
 			{	
			rev = Spi3.bSpi3ReadFIFO();	
 			Spi3.vSpi3BurstReadFIFO(msg, rev);
 			}
 		RssiTrig = false;
 		PktRevError = false;
 		}
 	}
 else
 	{
	if(tmp&PKT_OK_FLG) 		
		{
 		if(FixedPktLength)
 			{
  			Spi3.vSpi3BurstReadFIFO(msg, PayloadLength);
			rev = PayloadLength;
			}
 		else
 			{	
			rev = Spi3.bSpi3ReadFIFO();	
 			Spi3.vSpi3BurstReadFIFO(msg, rev);
 			}	
 		RssiTrig = false;		
		PktRevError = false;
		}
 	}
 
 if((tmp&COL_ERR_FLG)||(tmp&PKT_ERR_FLG))	//错误处理
	{
	PktRevError = true;	
	rev = 0;
	}
 return(rev);
}

/**********************************************************
**函数名称：vSendMessage
**函数功能：发射一包数据
**输入参数：none
**输出参数：none
**备注    ：需要与GPO的中断搭配使用，适用于MCU中断场合          
**********************************************************/
void cmt2300aSLP::vSendMessage(byte msg[], byte length)
{
 //mode1
 //vSetPayloadLength(true, length);
 //bGoStandby();
 //vEnableWrFifo();	
 //vClearFifo();
 //Spi3.vSpi3BurstWriteFIFO(msg, length);
 //bGoTx();
 
 //mode2
 bIntSrcFlagClr();
 vSetPayloadLength(true, length);
 vEnableWrFifo();	
 vClearFifo();
 bGoTx();
 Spi3.vSpi3BurstWriteFIFO(msg, length);
}

/**********************************************************
**函数名称：bSendMessageByFlag
**函数功能：发射一包数据
**输入参数：none
**输出参数：none
**备注    ：查询标识机制，节省GPO中断         
**********************************************************/
bool cmt2300aSLP::bSendMessageByFlag(byte msg[], byte length)
{
 byte tmp;
 word overtime;
 	
 //mode1
 //vSetPayloadLength(true, length);
 //bGoStandby();
 //vEnableWrFifo();	
 //vClearFifo();
 //Spi3.vSpi3BurstWriteFIFO(msg, length);
 //bGoTx();
 
 //mode2
 bIntSrcFlagClr();
 vSetPayloadLength(true, length);
 vEnableWrFifo();	
 vClearFifo();
 bGoTx();
 Spi3.vSpi3BurstWriteFIFO(msg, length);
 
 overtime = 0;
 do 
 	{
 	_delay_us(100);
	overtime++;
 	tmp = Spi3.bSpi3Read(CUS_INT_CLR1);
 	}while(((tmp&TX_DONE_FLG)==0)||(overtime>10000));
 if(overtime>=10000)
 	return(false);
 else
 	return(true);
}

/**********************************************************
**函数名称：vSetChannelOffset
**函数功能：设置跳频间隔
**输入参数：interval  unit:KHz
**输出参数：none
**备注    ：
**********************************************************/
void cmt2300aSLP::vSetChannelOffset(word interval)
{
 byte offset;
 offset = (interval<<1)/5;			//unit:2.5KHz
 Spi3.vSpi3Write(((word)CUS_FREQ_OFS<<8)+offset);
}

/**********************************************************
**函数名称：vSetChannel
**函数功能：设置频道
**输入参数：channel
**输出参数：none
**备注    ：
**********************************************************/
void cmt2300aSLP::vSetChannel(word channel)
{
 Spi3.vSpi3Write(((word)CUS_FREQ_CHNL<<8)+channel);
}

/**********************************************************
**函数名称：vSetTxPreamble
**函数功能：设置发射的Preamble长度
**输入参数：length
**输出参数：none
**备注    ：
**********************************************************/
void cmt2300aSLP::vSetTxPreamble(word length)
{
 Spi3.vSpi3Write(((word)CUS_PKT3<<8)+(byte)(length>>8));
 Spi3.vSpi3Write(((word)CUS_PKT2<<8)+(byte)length);
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
byte cmt2300aSLP::bFHSSDetect(void)
{
 signed char rssi;
 signed int  rssi_cal;
 byte i;
 
 bGoStandby();
 vSetChannel(FhssChannel);
 bGoRx();
 _delay_us(500);		
 rssi_cal = bReadRssi(true);
 for(i=0; i<3; i++)
	{
 	_delay_us(100);		
 	rssi_cal+= bReadRssi(true);
	}
 rssi = rssi_cal>>2;
 bGoStandby();

 
 if(!FhssLockTrig)
 	{
 	if((rssi>FhssRssiAvg)&&((rssi-FhssRssiAvg)>=FhssRssiTH))
 		{
 		FhssLockTrig = true;
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
 		FhssLockTrig = false;
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
}

/******************************************************************************
Duty-Cycle & Super Low Power
******************************************************************************/
void cmt2300aSLP::vSuperLowPowerCfg(void)
{
 byte tmp;
 byte tmp2;
 
 bGoStandby();

 Spi3.vSpi3Write(((word)CUS_SYS4<<8)+((byte)SleepTimerM));
 Spi3.vSpi3Write(((word)CUS_SYS5<<8)+((byte)(SleepTimerM>>4)&0x70)+(SleepTimerR&0x0F));
 
 Spi3.vSpi3Write(((word)CUS_SYS6<<8)+((byte)RxTimerT1M));
 Spi3.vSpi3Write(((word)CUS_SYS7<<8)+((byte)(RxTimerT1M>>4)&0x70)+(RxTimerT1R&0x0F));

 Spi3.vSpi3Write(((word)CUS_SYS8<<8)+((byte)RxTimerT2M));
 Spi3.vSpi3Write(((word)CUS_SYS9<<8)+((byte)(RxTimerT2M>>4)&0x70)+(RxTimerT2R&0x0F)); 
 
 tmp  = Spi3.bSpi3Read(CUS_SYS2);
 tmp &= 0x70;
 tmp2 = Spi3.bSpi3Read(CUS_SYS3);
 switch(DutyCycleMode)
 	{

 	case SLEEP_TO_STBY_MODE: 		//GoSleep->Sleep->Standby->McuCtrl
 		tmp |= SLEEP_TIMER_EN;	
 		break;		
 	case SLEEP_TO_RX_MODE:			//GoSleep->Sleep->Rx->McuCtrl
 		tmp |= (SLEEP_TIMER_EN+RX_DC_EN);
 		break;
 	case RX_SEMIAUTO_MODE:
 		tmp |= (SLEEP_TIMER_EN+RX_TIMER_EN);
 		tmp2&= 0xFC; 		
 		switch(RxExitState)
 			{
 		 	case EXIT_TO_STBY:  tmp2 |= RX_EXIT_STBY;  break;
 			case EXIT_TO_RFS:   tmp2 |= RX_EXIT_FS;    break;
 			case EXIT_TO_SLEEP: 
 			default:            tmp2 |= RX_EXIT_SLEEP; break;
 			}
 		break;
 	case RX_FULLAUTO_MODE:
 		tmp |= (SLEEP_TIMER_EN+RX_TIMER_EN+RX_DC_EN);
 		tmp2&= 0xFC; 		
 		tmp2|= RX_EXIT_SLEEP;
 		break;
 	case TX_SEMIAUTO_MODE:
 		tmp |= SLEEP_TIMER_EN;
 		tmp2&= 0xF3; 		
 		switch(TxExitState)
 			{
 			case EXIT_TO_STBY:  tmp2 |= TX_EXIT_STBY;  break;
 			case EXIT_TO_TFS:   tmp2 |= TX_EXIT_FS;    break;
 			case EXIT_TO_SLEEP: 
 			default:            tmp2 |= TX_EXIT_SLEEP; break;
 			}
 		break;	
 	case TX_FULLAUTO_MODE:
 		tmp |= (SLEEP_TIMER_EN+TX_DC_EN);
 		tmp2&= 0xF3; 		
 		tmp2|= TX_EXIT_SLEEP;
 		break;			
 	case DC_OFF_MODE:
 	default:                  
 		break; 						//all disable
 	}
 Spi3.vSpi3Write(((word)CUS_SYS2<<8)+tmp);
 Spi3.vSpi3Write(((word)CUS_SYS3<<8)+tmp2);

 if(TX_FULLAUTO_MODE==DutyCycleMode)
    {
    tmp  = Spi3.bSpi3Read(CUS_PKT29);
 	tmp |= FIFO_AUTO_RES_EN;
 	Spi3.vSpi3Write(((word)CUS_PKT29<<8)+tmp);
    }

 tmp = Spi3.bSpi3Read(CUS_SYS10);
 tmp&= 0xF0;
 tmp|= (byte)RxExtendMode;
 Spi3.vSpi3Write(((word)CUS_SYS10<<8)+tmp);


 tmp = Spi3.bSpi3Read(CUS_SYS11);
 tmp&= 0x9F;
 switch(DetectMode)
 	{
 	case ONLY_RSSI: tmp |= CCA_INT_ONLY_RSSI; break;
 	case ONLY_PJD:  tmp |= CCA_INT_ONLY_PJD;  break;
 	case RSSI_PJD:
 	default:        tmp |= CCA_INT_PJD_RSSI;  break;
 	}
 Spi3.vSpi3Write(((word)CUS_SYS11<<8)+tmp); 	
 
 tmp = Spi3.bSpi3Read(CUS_SYS12);
 tmp&= 0x3F;
 tmp|= (byte)PJDSel;
 Spi3.vSpi3Write(((word)CUS_SYS12<<8)+tmp);
 
 tmp  = Spi3.bSpi3Read(CUS_PKT1);
 tmp &= 0x07;
 tmp |= (PreambleRxDetect<<RX_PREAM_SIZE_SHIFT);
 Spi3.vSpi3Write(((word)CUS_PKT1<<8)+tmp);
 
 if((RssiTrigLevel!=0)&&(RssiTrigLevel<128))
 	{
    tmp = RssiTrigLevel;
    Spi3.vSpi3Write(((word)CUS_OOK3<<8)+tmp);
    }
 
 if(TxPktNum!=0)
 	{
 	tmp  = Spi3.bSpi3Read(CUS_PKT29);
 	tmp |= FIFO_AUTO_RES_EN;
 	Spi3.vSpi3Write(((word)CUS_PKT29<<8)+tmp);
 	
 	Spi3.vSpi3Write(((word)CUS_PKT27<<8)+TxPktNum);
 	Spi3.vSpi3Write(((word)CUS_PKT28<<8)+TxPktGap);
 	}
}

void cmt2300aSLP::vDutyCyclePause(void)
{
 byte tmp;
 tmp = Spi3.bSpi3Read(CUS_SYS2);
 tmp|= DC_PAUSE;
 Spi3.vSpi3Write(((word)CUS_SYS2<<8)+tmp);
}

void cmt2300aSLP::vDutyCycleGoOn(void)
{
 byte tmp;
 tmp = Spi3.bSpi3Read(CUS_SYS2);
 tmp&= (~DC_PAUSE);
 Spi3.vSpi3Write(((word)CUS_SYS2<<8)+tmp);
}

void cmt2300aSLP::vRxDutyCycleOff(void)
{
 byte tmp;
 
 tmp = Spi3.bSpi3Read(CUS_SYS2);
 tmp&= (~RX_DC_EN);
 Spi3.vSpi3Write(((word)CUS_SYS2<<8)+tmp);
}

void cmt2300aSLP::vRxDutyCycleOn(void)
{
 byte tmp;
 
 tmp = Spi3.bSpi3Read(CUS_SYS2);
 tmp|= RX_DC_EN;
 Spi3.vSpi3Write(((word)CUS_SYS2<<8)+tmp);
}

void cmt2300aSLP::vTxDutyCycleOff(void)
{
 byte tmp;
 
 tmp = Spi3.bSpi3Read(CUS_SYS2);
 tmp&= (~TX_DC_EN);
 Spi3.vSpi3Write(((word)CUS_SYS2<<8)+tmp);
}

void cmt2300aSLP::vTxDutyCycleOn(void)
{
 byte tmp;
 
 tmp = Spi3.bSpi3Read(CUS_SYS2);
 tmp|= TX_DC_EN;
 Spi3.vSpi3Write(((word)CUS_SYS2<<8)+tmp);
}

void cmt2300aSLP::vSetFifoInStby(byte msg[], byte length)
{
 bGoStandby();
 vSetPayloadLength(true, length);
 vEnableWrFifo();	
 Spi3.vSpi3BurstWriteFIFO(msg, length);
}
 

