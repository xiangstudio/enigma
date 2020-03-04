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


#include <CMT2300A_AdvDriver.h>

/**********************************************************
State Ctrl
**********************************************************/
/**********************************************************
**Name:     vSoftReset
**Function: Software reset Chipset
**Input:    none
**Output:   none
**********************************************************/
void cmt2300aAdvance::vSoftReset(void)
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
bool cmt2300aAdvance::bGoSleep(void)
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
bool cmt2300aAdvance::bGoStandby(void)
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
bool cmt2300aAdvance::bGoTx(void)
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
bool cmt2300aAdvance::bGoRx(void)
{
 byte tmp, i;

 RssiTrig = false;
 PktRevError = false;
 vSetPayloadLength(false, PayloadLength);		//Set traget reveive length

 vEnableRdFifo();

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
bool cmt2300aAdvance::bGoSwitch(void)
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
byte cmt2300aAdvance::bReadStatus(void)
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
byte cmt2300aAdvance::bReadRssi(bool unit_dbm)
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
void cmt2300aAdvance::vGpioFuncCfg(byte io_cfg)
{
 Spi3.vSpi3Write(((word)CUS_IO_SEL<<8)+io_cfg);
}

/**********************************************************
**Name:     vIntSrcCfg
**Function: config interrupt source  
**Input:    int_1, int_2
**Output:   none
**********************************************************/
void cmt2300aAdvance::vIntSrcCfg(byte int_1, byte int_2)
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
void cmt2300aAdvance::vInt1SrcCfg(byte int_1)
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
void cmt2300aAdvance::vInt2SrcCfg(byte int_2)
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
void cmt2300aAdvance::vIntSrcEnable(byte en_int)
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
byte cmt2300aAdvance::bIntSrcFlagClr(void)
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
void cmt2300aAdvance::vEnableAntSwitch(byte mode)
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
void cmt2300aAdvance::vClearFifo(void)
{
 Spi3.vSpi3Write(((word)CUS_FIFO_CLR<<8)+FIFO_CLR_RX+FIFO_CLR_TX);
}

/**********************************************************
**Name:     bReadFifoFlag
**Function: read fifo state flag
**Input:    none
**Output:   FIFO state
**********************************************************/
byte cmt2300aAdvance::bReadFifoFlag(void)
{
 return(Spi3.bSpi3Read(CUS_FIFO_FLAG));
}

/**********************************************************
**Name:     bReadIntFlag
**Function: read interrupt flag
**Input:    none
**Output:   interrupt flag
**********************************************************/
word cmt2300aAdvance::wReadIntFlag(void)
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
void cmt2300aAdvance::vEnableRdFifo(void)
{
 byte tmp;
 tmp = Spi3.bSpi3Read(CUS_FIFO_CTL);
 tmp &= (~(SPI_FIFO_RD_WR_SEL|FIFO_RX_TX_SEL));
 Spi3.vSpi3Write(((word)CUS_FIFO_CTL<<8)+tmp);
}

/**********************************************************
**Name:     vEnableWrFifo
**Function: set Fifo for wirte
**Input:    none
**Output:   none
**********************************************************/
void cmt2300aAdvance::vEnableWrFifo(void)
{
 byte tmp;
 tmp = Spi3.bSpi3Read(CUS_FIFO_CTL);
 tmp |= (SPI_FIFO_RD_WR_SEL|FIFO_RX_TX_SEL);
 Spi3.vSpi3Write(((word)CUS_FIFO_CTL<<8)+tmp);
}

/**********************************************************
**Name:     vSetPayloadLength
**Function: set Fifo length to used
**Input:    none
**Output:   none
**********************************************************/
void cmt2300aAdvance::vSetPayloadLength(bool mode, byte length)
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
 			{
 			if(NodeEnable)
 				len = length+NodeSize-1;
 			else
 				len = length-1;
 			}	
 		else
 			{
			if(NodeEnable)
				{
				if(!NodePosition)
					len = length;
				else
					len = length+NodeSize;
				}		 		
 			else
 				len = length;
 			}	
 		}
 	else					//Rx
 		{
 		if(NodeEnable)
  			len = length+NodeSize-1;
  		else
 			len = length - 1;
 		}
 		

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
void cmt2300aAdvance::vAckPacket(bool en)
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
byte cmt2300aAdvance::bReadFifo(void)
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
void cmt2300aAdvance::vWriteFifo(byte dat)
{
 Spi3.vSpi3WriteFIFO(dat);
}	

/**********************************************************
**函数名称：vSetMultipleSend
**函数功能：设置连续发包
**输入参数：pkt_cnt: 报文个数
            pkt_interval: 报文间隔
**输出参数：none
**Note：    set 0 should be normal
**********************************************************/
void cmt2300aAdvance::vSetMultipeSend(byte pkt_cnt, byte pkt_interval)
{
 Spi3.vSpi3Write(((word)CUS_PKT27<<8)+pkt_cnt);
 Spi3.vSpi3Write(((word)CUS_PKT28<<8)+pkt_interval);
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
void cmt2300aAdvance::vInit(void)
{
 Spi3.vSpi3Init();
 GPO1In();
 GPO2In();
 GPO3In();
 GPO4In();
 
 vSoftReset();
 bGoStandby();
}

void cmt2300aAdvance::vCfgBank(word cfg[], byte length)
{
 byte i;
 
 if(length!=0)
 	{	
 	for(i=0; i<length; i++)	
 		Spi3.vSpi3Write(cfg[i]);
 	}	
}

void cmt2300aAdvance::vAfterCfg(void)            //call after vCfgBank
{
 byte tmp;	
 tmp = Spi3.bSpi3Read(CUS_MODE_STA);
 tmp|= CFG_RETAIN;
 tmp&= (~RSTN_IN_EN);			//Disable RstPin	
 Spi3.vSpi3Write(((word)CUS_MODE_STA<<8)+tmp);

 tmp = 0x00;                    //Disable All Calibration, when no need to use DutyCycleMode
 Spi3.vSpi3Write(((word)CUS_SYS2<<8)+tmp);

 bIntSrcFlagClr();
 
 FhssChannelRange = 0;
 FhssChannel      = 0;
 FhssRssiAvg      = 0;
 FhssRssiTH       = 0;
}


void cmt2300aAdvance::vGaussianSetting(bool en, byte bt)        //Only for FSK use
{
 byte tmp;
 tmp = Spi3.bSpi3Read(CUS_TX1);
 tmp&= 0x1F;
 if(en)
 	{
    tmp |= GAUS_EN;
    switch(bt) 	
    	{
    	case 1:  tmp |= BT_SEL_5; break;
    	case 2:  tmp |= BT_SEL_8; break;
    	case 3:  tmp |= BT_SEL_10;break;
    	default: tmp |= BT_SEL_3; break;
    	}
 	}
 Spi3.vSpi3Write(((word)CUS_TX1<<8)+tmp);
}

/**********************************************************
**Name:     vBaseBankCfg
**Function: 更新BaseBank配置
**Input:    none
**Output:   none
**********************************************************/
void cmt2300aAdvance::vBaseBankCfg(void)
{
 byte tmp;
 byte i;
 
 tmp = 0;
 if(DirectMode)
 	tmp = DIRECT_MODE;
 else
 	tmp = PACKET_MODE;
 if(PreambleNibble) 	
 	tmp |= PREAM_UNIT_NIBBLE;
 else	
 	tmp |= PREAM_UNIT_BYTE;
 tmp |= (PreambleRxDetect<<RX_PREAM_SIZE_SHIFT);
 Spi3.vSpi3Write(((word)CUS_PKT1<<8)+tmp);
 
 Spi3.vSpi3Write(((word)CUS_PKT2<<8)+(byte)PreambleTxLength);
 Spi3.vSpi3Write(((word)CUS_PKT3<<8)+(byte)(PreambleTxLength>>8));
 Spi3.vSpi3Write(((word)CUS_PKT4<<8)+PreambleValue);
 
 tmp = Spi3.bSpi3Read(CUS_PKT5);
 tmp &= 0x80;
 if(SyncEnMan)
 	tmp |= SYNC_MAN_EN;
 else 
 	tmp &= (~SYNC_MAN_EN);
 switch(SyncSize)
 	{

	case 2: tmp |= SYNC_SIZE_2; break;
	case 3: tmp |= SYNC_SIZE_3; break;
	case 4: tmp |= SYNC_SIZE_4; break;
	case 5: tmp |= SYNC_SIZE_5; break;
	case 6: tmp |= SYNC_SIZE_6; break;
	case 7: tmp |= SYNC_SIZE_7; break;
	case 8: tmp |= SYNC_SIZE_8; break;
	default:tmp |= SYNC_SIZE_1; break;
	}
 switch(SyncTol)
 	{
 	case 1: tmp |= SYNC_TOL_1; break;
 	case 2: tmp |= SYNC_TOL_2; break;
 	case 3: tmp |= SYNC_TOL_3; break;
 	case 4: tmp |= SYNC_TOL_4; break;
 	case 5: tmp |= SYNC_TOL_5; break;
 	case 6: tmp |= SYNC_TOL_6; break;
 	case 7: tmp |= SYNC_TOL_7; break;
 	default:tmp |= SYNC_TOL_0; break;
 	}
 Spi3.vSpi3Write(((word)CUS_PKT5<<8)+tmp);
 for(i=0;i<8;i++)								
 	Spi3.vSpi3Write(((word)(CUS_PKT13-i)<<8)+SyncWord[i]);	 	
 
 tmp = Spi3.bSpi3Read(CUS_PKT14);
 tmp &= 0x80;			//note: limit payload in 255 byte
 if(!FixedPktLength)
 	tmp |= PKT_TYPE_VARIABLE;
 if(PayloadPriority)
 	tmp |= PAYLOAD_BIT_ORDER_LSB;
 if(NodePosition)
 	tmp |= NODE_LENG_POS_NODE;
 Spi3.vSpi3Write(((word)CUS_PKT14<<8)+tmp);
 Spi3.vSpi3Write(((word)CUS_PKT15<<8)+PayloadLength);
 
 tmp = Spi3.bSpi3Read(CUS_PKT16);
 tmp &= 0xC0;
 switch(NodeEnable)
 	{
 	case 1:  tmp |= NODE_DET_NODE; break;
 	case 2:  tmp |= NODE_DET_NODE_ZERO; break;
 	case 3:  tmp |= NODE_DET_ALL; break;
 	default: tmp |= NODE_DET_DISABLE; break;
 	}
 switch(NodeSize)
 	{
 	case 1:  tmp |= NODE_SIZE_1; break;
 	case 2:  tmp |= NODE_SIZE_2; break;
	case 3:  tmp |= NODE_SIZE_3; break; 		
 	case 4:  tmp |= NODE_SIZE_4; break;
 	default: tmp &= 0xC0;	
 	}
 Spi3.vSpi3Write(((word)CUS_PKT16<<8)+tmp);
 for(i=0;i<4;i++)								
 	Spi3.vSpi3Write(((word)(CUS_PKT20-i)<<8)+NodeValue[i]);	 
 
 tmp = 0;
 if(CrcEnable)
 	tmp |= CRC_EN;
 else 
 	tmp |= CRC_DIS;
 switch(CrcMode)
 	{
 	case CRC_ITU:   tmp |= CRC_TYPE_ITU; break;
 	case CRC_IBM:   tmp |= CRC_TYPE_IBM; break;
 	case CRC_CCITT: 
 	default:        tmp |= CRC_TYPE_CCITT; break;
 	}
 if(CrcDatOnly)
 	tmp |= CRC_RANGE_ONLY_DATA;
 if(CrcInverse)
 	tmp |= CRC_BIT_INV;
 if(CrcSwap)
 	tmp |= CRC_BYTE_SWAP;
 if(FecEnable)
 	tmp |= FEC_EN;
 if(FecType)
 	tmp |= FEC_TYPE_B;
 Spi3.vSpi3Write(((word)CUS_PKT21<<8)+tmp);
 Spi3.vSpi3Write(((word)CUS_PKT22<<8)+(byte)CrcSeed);
 Spi3.vSpi3Write(((word)CUS_PKT23<<8)+(byte)(CrcSeed>>8));

 tmp = 0;
 if(ManEnable)
 	{
 	tmp |= MANCH_EN;
	i = Spi3.bSpi3Read(CUS_CDR1);
	i &= 0xFC;
	i |= CDR_MANCHESTER_MODE;
	Spi3.vSpi3Write(((word)CUS_CDR1<<8)|i);
	}
 if(ManDataInverse)
 	tmp |= MANCH_TYPE_B;
 else
 	tmp |= MANCH_TYPE_A;
 if(WhitenEnable)
 	tmp |= WHITEN_EN;
 switch(WhitenType)
 	{
 	case PN7_AMI:   tmp |= WHITEN_TYPE_PN7; break;
 	case PN9_IBM:   tmp |= WHITEN_TYPE_IBM; break;
 	case PN9_CCITT:
 	default:        tmp |= WHITEN_TYPE_CCITT; break;
 	}
 if(WhitenSeedActive)
 	tmp |= WHITEN_SEED_TYPE;
 if(CrcBitOrder)
 	tmp |= CRC_BIT_ORDER;
 tmp |= (((WhitenSeed>>8)&0x01)<<WHITEN_SEED_SHIFT);
 Spi3.vSpi3Write(((word)CUS_PKT24<<8)+tmp);
 Spi3.vSpi3Write(((word)CUS_PKT25<<8)+(byte)WhitenSeed);
 
 tmp = Spi3.bSpi3Read(CUS_PKT26);
 tmp &= 0xFC;
 switch(TxPrefix)
	{
	case TX_ZERO:   tmp |= TX_PREFIX_ZERO; break;
	case TX_HIGH:   tmp |= TX_PREFIX_HIGH; break;
	case TX_PREAMB:
	default:        tmp |= TX_PREFIX_PREAM;break;
	} 
 Spi3.vSpi3Write(((word)CUS_PKT26<<8)+tmp);
 
 tmp = Spi3.bSpi3Read(CUS_PKT29);
 tmp &= 0x80;
 tmp |= FifoTH;
 Spi3.vSpi3Write(((word)CUS_PKT29<<8)+tmp);
}

/**********************************************************
**Name:     vCdrCfg
**Function: 更新Cdr配置
**Input:    none
**Output:   none
**********************************************************/
void cmt2300aAdvance::vCdrCfg(void)
{
 byte tmp;
 byte rdtmp;
 
 rdtmp = Spi3.bSpi3Read(CUS_CDR1);
 tmp   = rdtmp&0x80;
 switch(CdrAvgSel)
 	{
 	case CDR_6_8:  tmp |= (0<<CDR_AVG_SEL_SHIFT); break;
 	case CDR_1_2:  tmp |= (1<<CDR_AVG_SEL_SHIFT); break;
 	case CDR_6_16: tmp |= (2<<CDR_AVG_SEL_SHIFT); break;
 	case CDR_1_4:  tmp |= (3<<CDR_AVG_SEL_SHIFT); break;
 	case CDR_11_64:tmp |= (4<<CDR_AVG_SEL_SHIFT); break;
 	case CDR_1_8:  tmp |= (5<<CDR_AVG_SEL_SHIFT); break;
 	case CDR_3_32: tmp |= (6<<CDR_AVG_SEL_SHIFT); break;
 	case CDR_1_16: tmp |= (7<<CDR_AVG_SEL_SHIFT); break;
 	default:       tmp |= (rdtmp&0x70); break;		//no change
 	}
 switch(CdrRangeSel)
 	{
	case BIT_ERR_6:  tmp |= (0<<CDR_RANGE_SEL_SHIFT); break;
	case BIT_ERR_9:  tmp |= (1<<CDR_RANGE_SEL_SHIFT); break;
	case BIT_ERR_12: tmp |= (2<<CDR_RANGE_SEL_SHIFT); break;
	case BIT_ERR_15: tmp |= (3<<CDR_RANGE_SEL_SHIFT); break;
 	default:         tmp |= (rdtmp&0x0C); break;	//no change
 	}
 switch(CdrModeSel)
 	{
 	case CDR_TRACING:    tmp |= (CDR_TRACING_MODE<<CDR_MODE_SHIFT); break;
  	case CDR_COUNTING:   tmp |= (CDR_COUNTING_MODE<<CDR_MODE_SHIFT); break;
 	case CDR_MANCHESTER: tmp |= (CDR_MANCHESTER_MODE<<CDR_MODE_SHIFT); break;
 	case CDR_RAWDATA:    tmp |= (CDR_RAWDATA_MODE<<CDR_MODE_SHIFT); break;
 	default:             tmp |= (rdtmp&0x03); break;	//no change
 	}
 Spi3.vSpi3Write(((word)CUS_CDR1<<8)+tmp);
 
 
 rdtmp = Spi3.bSpi3Read(CUS_CDR2);
 if(Cdr3rdEn)
 	rdtmp |= CDR_3RD_EN;
 else
 	rdtmp &= (~CDR_3RD_EN);
 
 if(Cdr4thEn)
 	rdtmp |= CDR_4TH_EN;
 else
 	rdtmp &= (~CDR_4TH_EN);
 Spi3.vSpi3Write(((word)CUS_CDR2<<8)+rdtmp);
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
byte cmt2300aAdvance::bGetMessage(byte msg[])
{
 byte i;	
 
 if(FixedPktLength)
 	{
 	if(NodeEnable!=0)
		Spi3.vSpi3BurstReadFIFO(NodeRev, NodeSize);
 	Spi3.vSpi3BurstReadFIFO(msg, PayloadLength);
	i = PayloadLength;
	}
 else
 	{
 	if(NodeEnable!=0)	
 		{
 		if(!NodePosition)
 			{
 			Spi3.vSpi3BurstReadFIFO(NodeRev, NodeSize);
 			i = Spi3.bSpi3ReadFIFO();		
 			}
 		else
 			{
 			i = Spi3.bSpi3ReadFIFO();
			Spi3.vSpi3BurstReadFIFO(NodeRev, NodeSize);
		 	i-= NodeSize;
 			}
 		}
 	else
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
byte cmt2300aAdvance::bGetMessageByFlag(byte msg[])
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
 			if(NodeEnable!=0)
				Spi3.vSpi3BurstReadFIFO(NodeRev, NodeSize);
 			Spi3.vSpi3BurstReadFIFO(msg, PayloadLength);
			rev = PayloadLength;
			}
 		else
 			{
 			if(NodeEnable!=0)	
 				{
 				if(!NodePosition)
 					{
 					Spi3.vSpi3BurstReadFIFO(NodeRev, NodeSize);
 					rev = Spi3.bSpi3ReadFIFO();		
 					}
 				else
 					{
 					rev = Spi3.bSpi3ReadFIFO();
					Spi3.vSpi3BurstReadFIFO(NodeRev, NodeSize);
				 	rev-= NodeSize;
 					}
 				}
 			else
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
 			if(NodeEnable!=0)
				Spi3.vSpi3BurstReadFIFO(NodeRev, NodeSize);
 			Spi3.vSpi3BurstReadFIFO(msg, PayloadLength);
			rev = PayloadLength;
			}
 		else
 			{
 			if(NodeEnable!=0)	
 				{
 				if(!NodePosition)
 					{
 					Spi3.vSpi3BurstReadFIFO(NodeRev, NodeSize);
 					rev = Spi3.bSpi3ReadFIFO();		
 					}
 				else
 					{
 					rev = Spi3.bSpi3ReadFIFO();
					Spi3.vSpi3BurstReadFIFO(NodeRev, NodeSize);
				 	rev-= NodeSize;
 					}
 				}
 			else
 				rev = Spi3.bSpi3ReadFIFO();	
 			Spi3.vSpi3BurstReadFIFO(msg, rev);
 			}	
			
 		RssiTrig = false;	
 		PktRevError = false;	
		}
 	}
 
 if((tmp&COL_ERR_FLG)||(tmp&PKT_ERR_FLG))	//错误处理
	{
	rev = 0;
	PktRevError = true;
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
void cmt2300aAdvance::vSendMessage(byte msg[], byte length)
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
bool cmt2300aAdvance::bSendMessageByFlag(byte msg[], byte length)
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
void cmt2300aAdvance::vSetChannelOffset(word interval)
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
void cmt2300aAdvance::vSetChannel(word channel)
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
void cmt2300aAdvance::vSetTxPreamble(word length)
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
byte cmt2300aAdvance::bFHSSDetect(void)
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


/**********************************************************
**函数名称：bGetVoltage
**函数功能：voltage detect
**输入参数：none
**输出参数：voltage value
**备注    ：
**********************************************************/
byte cmt2300aAdvance::bGetVoltage(void)
{
 return(Spi3.bSpi3Read(CUS_LBD_RESULT));
}

