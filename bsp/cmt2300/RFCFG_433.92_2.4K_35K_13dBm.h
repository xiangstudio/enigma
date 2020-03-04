//;---------------------------------------
//;  CMT2300A Configuration File
//;  Generated by CMOSTEK RFPDK 1.45
//;  2017.11.02 14:25
//;---------------------------------------
//; Mode                      = Advanced
//; Part Number               = CMT2300A
//; Frequency                 = 433.920 MHz
//; Xtal Frequency            = 26.0000 MHz
//; Demodulation              = FSK
//; AGC                       = On
//; Data Rate                 = 2.4 kbps
//; Deviation                 = 35.0 kHz
//; Tx Xtal Tol.              = 20 ppm
//; Rx Xtal Tol.              = 20 ppm
//; TRx Matching Network Type = 13 dBm
//; Tx Power                  = +13 dBm
//; Gaussian BT               = NA
//; Bandwidth                 = Auto-Select kHz
//; CDR Type                  = Counting
//; CDR DR Range              = NA
//; AFC                       = On
//; AFC Method                = Auto-Select
//; Data Representation       = 0:F-low 1:F-high
//; Rx Duty-Cycle             = Off
//; Tx Duty-Cycle             = Off
//; Sleep Timer               = Off
//; Sleep Time                = NA
//; Rx Timer                  = Off
//; Rx Time T1                = NA
//; Rx Time T2                = NA
//; Rx Exit State             = STBY
//; Tx Exit State             = STBY
//; SLP Mode                  = Disable
//; RSSI Valid Source         = PJD
//; PJD Window                = 8 Jumps
//; LFOSC Calibration         = On
//; Xtal Stable Time          = 155 us
//; RSSI Compare TH           = NA
//; Data Mode                 = Packet
//; Whitening                 = Disable
//; Whiten Type               = NA
//; Whiten Seed Type          = NA
//; Whiten Seed               = NA
//; Manchester                = Disable
//; Manchester Type           = NA
//; FEC                       = Disable
//; FEC Type                  = NA
//; Tx Prefix Type            = 0
//; Tx Packet Number          = 1
//; Tx Packet Gap             = 32
//; Packet Type               = Variable Length
//; Node-Length Position      = First Node, then Length
//; Payload Bit Order         = Start from msb
//; Preamble Rx Size          = 2
//; Preamble Tx Size          = 8
//; Preamble Value            = 170
//; Preamble Unit             = 8-bit
//; Sync Size                 = 3-byte
//; Sync Value                = 11152852
//; Sync Tolerance            = None
//; Sync Manchester           = Disable
//; Node ID Size              = NA
//; Node ID Value             = NA
//; Node ID Mode              = None
//; Node ID Err Mask          = Disable
//; Node ID Free              = Disable
//; Payload Length            = 32
//; CRC Options               = None
//; CRC Seed                  = NA
//; CRC Range                 = NA
//; CRC Swap                  = NA
//; CRC Bit Invert            = NA
//; CRC Bit Order             = NA
//; Dout Mute                 = Off
//; Dout Adjust Mode          = Disable
//; Dout Adjust Percentage    = NA
//; Collision Detect          = Off
//; Collision Detect Offset   = NA
//; RSSI Detect Mode          = Always
//; RSSI Filter Setting       = No Filtering
//; RF Performance            = High
//; LBD Threshold             = 2.4 V
//; RSSI Offset               = 26
//; RSSI Offset Sign          = 1
//
//;---------------------------------------
//;  The following are the Register contents
//;---------------------------------------
//[CMT Bank]
unsigned int CMTBank[12] = {
0x0000,
0x0166,
0x02EC,
0x031C,
0x04F0,
0x0580,
0x0614,
0x0708,
0x0891,
0x0902,
0x0A02,
0x0BD0,
};
//[System Bank]
unsigned int SystemBank[12] = {
0x0CAE,
0x0DE0,
0x0E35,
0x0F00,
0x1000,
0x11F4,
0x1210,
0x13E2,
0x1442,
0x1520,
0x1600,
0x1781,
};
//[Frequency Bank]
unsigned int FrequencyBank[8] = {
0x1842,
0x1971,
0x1ACE,
0x1B1C,
0x1C42,
0x1D5B,
0x1E1C,
0x1F1C,
};
//[Data Rate Bank]
unsigned int DataRateBank[24] = {
0x2032,
0x2118,
0x2210,
0x2399,
0x24F1,
0x259B,
0x262C,
0x270A,
0x289F,
0x2939,
0x2A29,
0x2B29,
0x2CC0,
0x2D51,
0x2E2A,
0x2F53,
0x3000,
0x3100,
0x32B4,
0x3300,
0x3400,
0x3501,
0x3600,
0x3700,
};
//[Baseband Bank]
unsigned int BasebandBank[29] = {
0x3812,
0x3908,
0x3A00,
0x3BAA,
0x3C04,
0x3D00,
0x3E00,
0x3F00,
0x4000,
0x4100,
0x42D4,
0x432D,
0x44AA,
0x4501,
0x461F,
0x4700,
0x4800,
0x4900,
0x4A00,
0x4B00,
0x4C00,
0x4D00,
0x4E00,
0x4F60,
0x50FF,
0x5100,
0x5200,
0x531F,
0x5410,
};
//[TX Bank]
unsigned int TXBank[11] = {
0x5550,
0x560E,
0x5716,
0x5800,
0x5942,
0x5AB0,
0x5B00,
0x5C37,
0x5D0A,
0x5E3F,
0x5F7F,
};
//;---------------------------------------
//;  The following is the CRC result for
//;  the above Register contents
//;---------------------------------------
//0x53C2
