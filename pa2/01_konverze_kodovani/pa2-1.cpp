#ifndef __PROGTEST__
#include <iostream>
#include <fstream>
#include <stdlib.h>
using namespace std;
#endif /* __PROGTEST__ */

typedef struct {
    unsigned char bytes[4];
    char length;
    bool ok;
} BYTES;

/******************************************************************************/
/* DEC2BIN & BIN2DEC **********************************************************/

string dec2bin(int v) {
    string bin;
    int oneorzero;
    for(int i = 8; i > 0; i--) {
        oneorzero = v % 2;
        if(oneorzero == 1) {
            bin = "1" + bin;
        } else {
            bin = "0" + bin;
        }
        v /= 2;
    }
    return bin;
}

int bin2dec(string v) {
    if(v.length() > 8) {
        return 0;
    } else {
        int dec = 0, multiple = 1;
        char temp;
        for(int i = 7; i >= 0; i--) {
            temp = v[i];
            dec += atoi(&temp) * multiple;
            multiple *= 2;
        }
        return dec;
    }
}

void printBytes(BYTES b)
{
    for(int i = 0; i < b.length; i++) {
        //cout << dec2bin(b.bytes[i]) << " ";
        printf("%x ", b.bytes[i]);
    }
    cout << endl;
}

/******************************************************************************/
/* CONVERT CHAR FROM UTF16 TO UTF8 ********************************************/

BYTES utf8to16(BYTES utf8)
{
    BYTES utf16;
    int c;
    
    utf16.bytes[0] = 0;
    utf16.bytes[1] = 0;
    utf16.bytes[2] = 0;
    utf16.bytes[3] = 0;
    utf16.length = 2;
    utf16.ok = true;
    
    switch(utf8.length) {
        case 1:
            utf16.bytes[0] = utf8.bytes[0]; // 0[:]
            break;
        case 2:
            utf16.bytes[0] = ((utf8.bytes[0] & 3) << 6) | (utf8.bytes[1] & 63); // 0[6:] + 1[2:]
            utf16.bytes[1] = (utf8.bytes[0] & 28) >> 2; // 0[3:6]
            break;
        case 3:
            utf16.bytes[0] = ((utf8.bytes[1] & 3) << 6) | (utf8.bytes[2] & 63); // 1[6:] + 2[2:]
            utf16.bytes[1] = ((utf8.bytes[0] & 15) << 4) | ((utf8.bytes[1] & 60) >> 2); // 0[4:] + 1[2:8]
            break;
        case 4:
            // 0[5:] + 1[2:] + 2[2:] + 3[2:]
            c = ((utf8.bytes[0] & 7) << 18) | ((utf8.bytes[1] & 63) << 12) | ((utf8.bytes[2] & 63) << 6) | ((utf8.bytes[3] & 63));
            c -= 65536; // - 0x10000
            
            if(c > 1114112) utf16.ok = false;
            
            utf16.bytes[0] = (c & 261120) >> 10; // [2:10]
            utf16.bytes[1] = 216 | (c & 786432) >> 18; // 110110 + [0:2]
            utf16.bytes[2] = (c & 255); // [12:20]
            utf16.bytes[3] = 220 | (c & 768) >> 8; // 110111 + [10:12]
            utf16.length = 4;
            break;
    }
    
    return utf16;
}

/* CONVERT CHAR FROM UTF8 TO UTF16 ********************************************/

BYTES utf16to8(BYTES utf16)
{
    BYTES utf8;
    int c;
    
    utf8.bytes[0] = 0;
    utf8.bytes[1] = 0;
    utf8.bytes[2] = 0;
    utf8.bytes[3] = 0;
    utf8.length = 1;
    utf8.ok = true;
    
    if(utf16.length == 2) {
        if(utf16.bytes[1] == 0 && utf16.bytes[0] < 128) utf8.length = 1;
        else if(utf16.bytes[1] < 8) utf8.length = 2;
        else utf8.length = 3;
    } else if(utf16.length == 4) {
        utf8.length = 4;
    }
    
    switch(utf8.length) {
        case 1:
            utf8.bytes[0] = utf16.bytes[0]; // 0[:]
            break;
        case 2:
            utf8.bytes[0] = 192 | ((utf16.bytes[1] & 7) << 2) | ((utf16.bytes[0] & 192) >> 6); // 110 + 1[5:] + 0[:2]
            utf8.bytes[1] = 128 | (utf16.bytes[0] & 63); // 10 + 0[2:]
            break;
        case 3:
            utf8.bytes[0] = 224 | ((utf16.bytes[1] & 240) >> 4); // 1110 + 1[:4]
            utf8.bytes[1] = 128 | ((utf16.bytes[1] & 15) << 2) | ((utf16.bytes[0] & 192) >> 6); // 10 + 1[4:] + 0[:2]
            utf8.bytes[2] = 128 | (utf16.bytes[0] & 63); // 10 + 0[2:]
            break;
        case 4:
            // 1[6:] + 0[:] + 3[6:] + 2[:]
            c = ((utf16.bytes[1] & 3) << 18) | (utf16.bytes[0] << 10) | ((utf16.bytes[3] & 3) << 8) | utf16.bytes[2];
            c += 65536; // + 0x10000
            
            if(c > 1114112) utf8.ok = false;
            
            utf8.bytes[0] = 240 | ((c & 1835008) >> 18); // 11110 + c[:3]
            utf8.bytes[1] = 128 | ((c & 258048) >> 12); // 10 + c[3:9]
            utf8.bytes[2] = 128 | ((c & 4032) >> 6); // 10 + c[9:15]
            utf8.bytes[3] = 128 | (c & 63); // 10 + c[15:21]
            break;
    }
    
    return utf8;
}

/******************************************************************************/
/* CONVERT FILE FROM UTF8 TO UTF16 ********************************************/

bool UTF8toUTF16(const char* src, const char* dst)
{
    fstream fsrc, fdst;
    BYTES utf8, utf16;
    
    char position = 0;
    char getter;
    unsigned char c;
    
    fsrc.open(src, ios::in|ios::binary);
    fdst.open(dst, ios::out|ios_base::binary);
    
    if(!fsrc.is_open() || !fdst.is_open()) {
        return false;
    }
    
    utf8.length = 0;
    do {
        fsrc.get(getter);
        c = (unsigned char) getter;
        if(fsrc.eof()) break;
        
        if(((c & 192) >> 6) != 2) { // c[:2] != 10
            if(utf8.length > position) return false;
            if(((c & 128) >> 7) == 0) utf8.length = 1; // c[0] == 0
            else if(((c & 32) >> 5) == 0) utf8.length = 2; // c[2] == 0
            else if(((c & 16) >> 4) == 0) utf8.length = 3; // c[3] == 0
            else if(((c & 8) >> 3) == 0) utf8.length = 4; // c[4] == 0
            else return false;
            position = 0;
        }
        *(utf8.bytes+position++) = c;
        
        // check
        if(position == 1 && c > 128 && (c < 192 || c > 253)) return false;
        if(c > 253) return false;
        
        if(position == utf8.length) {
            utf16 = utf8to16(utf8);
            if(!utf16.ok) return false;
            for(int i = 0; i < utf16.length; i++) {
                fdst << utf16.bytes[i];
                if(fdst.bad()) {
                    return false;
                }
            }
        }
    } while(true);
    if(utf8.length != position) return false;
    
    fsrc.close();
    fdst.close();
    
    return true;
}

/* CONVERT FILE FROM UTF16 TO UTF8 ********************************************/

bool UTF16toUTF8(const char* src, const char* dst)
{
    fstream fsrc, fdst;
    BYTES utf8, utf16;
    
    char position = 0;
    char getter;
    unsigned char c, help;
    
    fsrc.open(src, ios::in|ios::binary);
    fdst.open(dst, ios::out|ios_base::binary);
    
    if(!fsrc.is_open() || !fdst.is_open()) {
        return false;
    }
    
    utf16.length = 2;
    do {
        fsrc.get(getter);
        c = (unsigned char) getter;
        if(fsrc.eof()) break;
        
        *(utf16.bytes+position++) = c;
        
        help = (c & 252) >> 2; // c[:6]
        if(position == 2 && help == 54) utf16.length = 4; // c[:6] == 110110
        
        // surrogate pars
        if(position == 4 && help != 55) return false;
        
        if(position == utf16.length) {
            utf8 = utf16to8(utf16);
            if(!utf8.ok) return false;
            for(int i = 0; i < utf8.length; i++) {
                fdst << utf8.bytes[i];
                if(fdst.bad()) {
                    return false;
                }
            }
            utf16.length = 2;
            position = 0;
        }
    
    } while(true);
    if(position != 0) return false;

    fsrc.close();
    fdst.close();
    
    return true;
}

#ifndef __PROGTEST__
int main(void)   
{
    UTF8toUTF16("utf8", "utf16-2");
    UTF16toUTF8("utf16", "utf8-2");
    
    return 0;
}
#endif /* __PROGTEST__ */

