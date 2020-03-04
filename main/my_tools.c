#include "stdlib.h"
#include "string.h"

#include "my_types.h"
#include "main.h"
#include "my_uart.h"
#include "my_tools.h"

/*
static unsigned char led_switch_tag = 0;
void led_switchi_2() { 
    led_switch_tag = !led_switch_tag; 
    if (led_switch_tag) {
        set_led_2();
    } else {
        clr_led_2();
    }
}
*/

void delay_us(int us) {
    unsigned short i;
    for(i=0; i<us; i++) {
        nop();nop();
        nop();nop();
        nop();nop();
        nop();nop();
        nop();nop();
        nop();nop();
        nop();nop();
        nop();nop();
    }
}

void delay_ms(int ms) {
    unsigned short us;
    for(us=0;us<ms;us++) {
        delay_us(666);
    }
}

void bin2hex(unsigned char mychar, unsigned char* p1, unsigned char* p2) {
    unsigned char h_char = (mychar>>4)&0x0F;
    if (h_char <= 9) 
        h_char += 0x30;
    else if (h_char >= 10 && h_char <= 15)
        h_char = (h_char - 10) + 0x41;
    *p1 = h_char;
          
    unsigned char l_char = (mychar>>0)&0x0F;
    if (l_char <= 9) 
        l_char += 0x30;
    else if (l_char >= 10 && l_char <= 15)
        l_char = (l_char - 10) + 0x41;
    
    *p2 = l_char;
}

static unsigned char HexCharToBinBinChar(unsigned char c)
{
    if (c >= '0' && c <= '9')
            return c - '0';
    else if (c >= 'a' && c <= 'z')
            return c - 'a' + 10;
    else if (c >= 'A' && c <= 'Z')
            return c - 'A' + 10;
    
    return 0xff;
}

unsigned char hex2bin(unsigned char p1, unsigned char p2)
{
    unsigned char tmp = 0;
    tmp = HexCharToBinBinChar(p1);
    tmp <<= 4;
    tmp |= HexCharToBinBinChar(p2);
    
    return tmp;
}




