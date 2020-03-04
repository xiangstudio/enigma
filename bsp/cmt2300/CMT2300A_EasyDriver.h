#include "my_types.h" 
//#include "HopeDuino_SPI3.h"
#include "CMT2300A_Register.h"
#include "HopeDuino_SPI3_Function.h"
#include "CMT2300_GPIO_Define.h"

bool FixedPktLength;
bool RssiTrig;
bool PktRevError;
byte PayloadLength;
byte PktRssi;
byte PktRssiCode;
bool PktRevError;
bool RssiTrig;
bool FhssLockTrig;
byte FhssLockChannel;
signed char RssiPeakLock;
byte FhssPeakChannel;


