
extern void vSpi3Init(void);    /** initialize software SPI-3 **/ 
extern void vSpi3Write(word dat);   /** SPI-3 send one word **/
extern void vSpi3WriteCmd(byte dat);  /*  SPI-3 send one byte to Cmd mode*/
extern void vSpi3WriteTreable(byte addr, word dat); /** SPI-3 send one address, two value, 3byte**/
extern byte bSpi3Read(byte addr);   /** SPI-3 read one byte **/
extern word wSpi3ReadTreable(byte addr); /** SPI-3 read word mode **/ 
extern void vSpi3WriteFIFO(byte dat);  /** SPI-3 send one byte to FIFO **/
extern byte bSpi3ReadFIFO(void);   /** SPI-3 read one byte from FIFO **/
extern void vSpi3BurstWriteFIFO(byte ptr[], byte length);   /** SPI-3 burst send N byte to FIFO**/
extern void vSpi3BurstReadFIFO(byte ptr[], byte length);   /** SPI-3 burst read N byte to FIFO**/
extern void vSpi3BurstWriteCmd(byte addr, byte ptr[], byte length);
extern void vSpi3BurstReadCmd(byte addr, byte ptr[], byte length);
extern void vSpi3WriteBytes(byte addr,byte* pInBuf,word len);
extern void vSpi3ReadBytes(byte addr,byte* pOutBuf,word len);

