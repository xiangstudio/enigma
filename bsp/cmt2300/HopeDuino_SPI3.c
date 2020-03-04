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
 * file       HopeDuino_3SPI.cpp
 * brief      for HopeRF's EVB to use softwave 3-SPI (for CMOSTEK RF-IC)
 * hardware   HopeRF's EVB
 *            
 *
 * version    1.1
 * date       Jan 15 2015
 * author     QY Ruan
 */

#include "my_tools.h"
#include "main.h"
#include "HopeDuino_SPI3.h"

/**********************************************************
**Name:  vSpi3Init
**Func:  Init Spi-3 Config
**Note:  
**********************************************************/
void vSpi3Init(void)
{
 //GPIO_Init(GPIOB, _CSB, GPIO_Mode_Out_PP_High_Slow);
 //GPIO_Init(GPIOB, _SDIO, GPIO_Mode_Out_PP_High_Slow);
 //GPIO_Init(GPIOB, _FCSB, GPIO_Mode_Out_PP_High_Slow);
 //GPIO_Init(GPIOB, _SDCK, GPIO_Mode_Out_PP_High_Slow); 
 
 GPIO_SetBits(GPIOB,_CSB);
 GPIO_SetBits(GPIOB,_SDIO);
 GPIO_SetBits(GPIOB,_FCSB); 
 ClrSDCK();
}

/**********************************************************
**Name:  vSpi3WriteByte
**Func:  SPI-3 send one byte
**Input:
**Output:  
**********************************************************/
void vSpi3WriteByte(byte dat, bool sda_in)
{

  byte bitcnt; 
 
 SetFCSB();    //FCSB = 1;
 
  OutputSDIO();   //SDA output mode
  SetSDIO();    //    output 1
 
  ClrSDCK();    
  ClrCSB();

  for(bitcnt=8; bitcnt!=0; bitcnt--)
   {
  ClrSDCK(); 
  if(dat&0x80)
    SetSDIO();
   else
    ClrSDIO();
  
  #if (SPI3_SPEED!=0)
      delay_us(SPI3_SPEED);
  #endif
  
  SetSDCK();
   dat <<= 1;   

  #if (SPI3_SPEED!=0)
      delay_us(SPI3_SPEED);
  #endif
   }
  if(sda_in)
   {
   InputSDIO();  //shift to input for read
   ClrSDCK();
   } 
  else
   {
   ClrSDCK(); 
   SetSDIO();
   }
}

/**********************************************************
**Name:  bSpi3ReadByte
**Func:  SPI-3 read one byte
**Input:
**Output:  
**********************************************************/
byte bSpi3ReadByte(void)
{
 byte RdPara = 0;
  byte bitcnt;
 
   ClrSDCK();
  InputSDIO();   
    ClrCSB(); 
    
  delay_us(SPI3_SPEED);  //Time-Critical
  delay_us(SPI3_SPEED);  //Time-Critical    
    
  for(bitcnt=8; bitcnt!=0; bitcnt--)
   {
   SetSDCK();
   RdPara <<= 1;
   
  #if (SPI3_SPEED!=0)
      delay_us(SPI3_SPEED);
  #endif

   if(SDIO_H())
    RdPara |= 0x01;
   else
    RdPara |= 0x00;
   ClrSDCK();

  #if (SPI3_SPEED!=0)
      delay_us(SPI3_SPEED);
  #endif
   } 
  ClrSDCK();
  //OutputSDIO();
  //SetSDIO();
  //SetCSB();   
  return(RdPara); 
}

/**********************************************************
**Name:   vSpi3Write
**Func:  SPI Write One word
**Input:  Write word
**Output: none
**********************************************************/
void vSpi3Write(word dat)
{
  vSpi3WriteByte(((byte)(dat>>8)&0x7F), FALSE);
  vSpi3WriteByte(((byte)dat), FALSE);
  SetCSB();
}

void vSpi3WriteCmd(byte dat)
{
  vSpi3WriteByte((dat&0x7F), FALSE);
  SetCSB();
}

void vSpi3WriteTreable(byte addr, word dat)
{

  vSpi3WriteByte((addr&0x7F), FALSE);
  vSpi3WriteByte(((byte)(dat>>8)), FALSE);
  vSpi3WriteByte(((byte)dat), FALSE);
  SetCSB();
}

void vSpi3BurstWriteCmd(byte addr, byte ptr[], byte length)
{
    byte i;
  vSpi3WriteByte((addr&0x7F), FALSE);
  for(i=0; i<length; i++)
   vSpi3WriteByte(ptr[i], FALSE);
 SetCSB();
}



/**********************************************************
**Name:   bSpi3Read
**Func:  SPI-3 Read One byte
**Input:  readout addresss
**Output: readout byte
**********************************************************/
byte bSpi3Read(byte addr)
{
 byte dat;
   vSpi3WriteByte((addr|0x80), TRUE);
   dat = bSpi3ReadByte();
 OutputSDIO();
  SetSDIO();
  SetCSB();   
   return(dat);
}

/*
word wSpi3ReadTreable(byte addr)
{
 word dat;
   vSpi3WriteByte((addr|0x80), TRUE);
  dat   = bSpi3ReadByte();
  dat <<= 8;
  dat  |= bSpi3ReadByte();
 OutputSDIO();
  SetSDIO();
  SetCSB();   
  return(dat);
}
*/

void vSpi3BurstReadCmd(byte addr, byte ptr[], byte length)
{
 byte i;

   vSpi3WriteByte((addr|0x80), TRUE);
  for(i=0; i<length; i++)
   ptr[i] = bSpi3ReadByte();
 OutputSDIO();
  SetSDIO();
  SetCSB();   
}


/**********************************************************
**Name:   vSpi3WriteFIFO
**Func:  SPI-3 send one byte to FIFO
**Input:  one byte buffer
**Output: none
**********************************************************/
void vSpi3WriteFIFO(byte dat)
{
  byte bitcnt; 
 
  SetCSB(); 
 OutputSDIO(); 
 ClrSDCK();
  ClrFCSB();   //FCSB = 0
 for(bitcnt=8; bitcnt!=0; bitcnt--)
   {
   ClrSDCK();
   
   if(dat&0x80)
   SetSDIO();  
  else
   ClrSDIO();

  #if (SPI3_SPEED!=0)
      delay_us(SPI3_SPEED);
  #endif
   
  SetSDCK();

  #if (SPI3_SPEED!=0)
      delay_us(SPI3_SPEED);
  #endif

   dat <<= 1;
   }
  ClrSDCK(); 
  delay_us(SPI3_SPEED);  //Time-Critical
  delay_us(SPI3_SPEED);  //Time-Critical
  SetFCSB();
 SetSDIO();
  delay_us(SPI3_SPEED);  //Time-Critical
  delay_us(SPI3_SPEED);  //Time-Critical
}

/**********************************************************
**Name:   bSpi3ReadFIFO
**Func:  SPI-3 read one byte to FIFO
**Input:  none
**Output: one byte buffer
**********************************************************/
byte bSpi3ReadFIFO(void)
{
 byte RdPara;
  byte bitcnt; 

  SetCSB();
 InputSDIO();
  ClrSDCK();
 ClrFCSB();
   
  delay_us(SPI3_SPEED);  //Time-Critical
  delay_us(SPI3_SPEED);  //Time-Critical
  
  for(bitcnt=8; bitcnt!=0; bitcnt--)
   {

  SetSDCK();

  #if (SPI3_SPEED!=0)
      delay_us(SPI3_SPEED);
  #endif

   RdPara <<= 1;
   if(SDIO_H())
    RdPara |= 0x01;  //NRZ MSB
   else
     RdPara |= 0x00;  //NRZ MSB
   ClrSDCK();

  #if (SPI3_SPEED!=0)
      delay_us(SPI3_SPEED);
  #endif

   }
  
  ClrSDCK();
  SetFCSB();
 OutputSDIO();
 SetSDIO();
  return(RdPara);
}

/**********************************************************
**Name:   vSpi3BurstWriteFIFO
**Func:  burst wirte N byte to FIFO
**Input:  array length & head pointer
**Output: none
**********************************************************/
void vSpi3BurstWriteFIFO(byte ptr[], byte length)
{
  byte i;
  if(length!=0x00)
   {
   for(i=0;i<length;i++)
    vSpi3WriteFIFO(ptr[i]);
   }
  return;
}

/**********************************************************
**Name:   vSpiBurstRead
**Func:  burst wirte N byte to FIFO
**Input:  array length  & head pointer
**Output: none
**********************************************************/
void vSpi3BurstReadFIFO(byte ptr[], byte length)
{
 byte i;
  if(length!=0)
   {
   for(i=0;i<length;i++)
    ptr[i] = bSpi3ReadFIFO();
   } 
  return;
}

void vSpi3SendByte(byte dat)
{
 byte bitcnt; 

  for(bitcnt=0; bitcnt<8; bitcnt++)
 {
  ClrSDCK(); 
  if(dat&0x80)
   SetSDIO();
  else
   ClrSDIO();
  
  delay_us(SPI3_SPEED);
  dat <<= 1;  
  SetSDCK();    
  delay_us(SPI3_SPEED);
 }
}

byte bSpi3RecvByte(void)
{
 byte RdPara = 0;
  byte bitcnt;
  
    
  for(bitcnt=0; bitcnt<8; bitcnt++)
  {
   
   RdPara <<= 1;
  ClrSDCK();  
   delay_us(SPI3_SPEED);
  SetSDCK();
   if(SDIO_H())
    RdPara |= 0x01;
   else
    RdPara |= 0x00;
   
   delay_us(SPI3_SPEED);
  } 
   
  return(RdPara); 
}

void vSpi3WriteBytes(byte addr,byte* pInBuf,word len)
{
 word i;

 OutputSDIO();
 SetSDIO();    //    output 1
 
  ClrSDCK();    
  ClrCSB();
 delay_us(SPI3_SPEED);
 delay_us(SPI3_SPEED);

 vSpi3SendByte(addr|0x00);

 for(i=0;i<len;i++)
 {
  vSpi3SendByte(pInBuf[i]);
 }

 ClrSDCK();
 delay_us(SPI3_SPEED);
 delay_us(SPI3_SPEED);
 SetCSB();
 SetSDIO();
 InputSDIO();
}

void vSpi3ReadBytes(byte addr,byte* pOutBuf,word len)
{
 word i;

 OutputSDIO();
 SetSDIO();    //    output 1
 
  ClrSDCK();    
  ClrCSB();
 delay_us(SPI3_SPEED);
 delay_us(SPI3_SPEED);

 vSpi3SendByte(addr|0x80);

 SetSDIO();
 InputSDIO();

 for(i=0;i<len;i++)
 {
  pOutBuf[i]=bSpi3RecvByte();
 }

 ClrSDCK();
 delay_us(SPI3_SPEED);
 delay_us(SPI3_SPEED);
 SetCSB();
 SetSDIO();
 InputSDIO();
}


