#ifndef __PROGTEST__
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <cstring>
#include <stdio.h>

using namespace std;
#endif /* __PROGTEST__ */

//    friend CBigInt operator+ (const CBigInt , const CBigInt);
//    friend CBigInt operator* (const CBigInt , const CBigInt);

class CBigInt
{
    private:
        string number;
        
        string _toString(const int value);
        string _toString(const char* value);
        string _toString(const CBigInt &value);
        
        string _sum(string a, string b);
        string _mul(string a, string b);
        string _simpleMul(string a, int b);
        
    public:
        CBigInt(const int initial = 0);
        CBigInt(const char initial);
        CBigInt(const CBigInt &initial);
        
        ~CBigInt(void);
        
        void operator=(const int stranger);
        void operator=(const char* stranger);
        void operator=(const CBigInt &stranger);
        
        CBigInt operator+(const int stranger) const;
        CBigInt operator+(const char* stranger) const;
        CBigInt operator+(const CBigInt &stranger) const;
        
        void operator+=(const int stranger);
        void operator+=(const char* stranger);
        void operator+=(const CBigInt &stranger);
        
        CBigInt operator*(const int stranger) const;
        CBigInt operator*(const char* stranger) const;
        CBigInt operator*(const CBigInt &stranger) const;
        
        void operator*=(const int stranger);
        void operator*=(const char* stranger);
        void operator*=(const CBigInt &stranger);
        
        friend ostream& operator<<(ostream& os, const CBigInt& obj);
        friend istream& operator>>(istream& is, CBigInt& obj);
};

/* CONSTRUCT ******************************************************************/

CBigInt::CBigInt(const int initial) { operator=(initial); }
CBigInt::CBigInt(const char initial) { operator=(&initial); }
CBigInt::CBigInt(const CBigInt &initial) { operator=(initial); }
CBigInt::~CBigInt(void) {}

/* ASSIGMENT ******************************************************************/

void CBigInt::operator=(const int stranger) { number = _toString(stranger); }
void CBigInt::operator=(const char* stranger) { number = _toString(stranger); }
void CBigInt::operator=(const CBigInt &stranger) { number = _toString(stranger); }

/* ADDITION *******************************************************************/

CBigInt CBigInt::operator+(const int stranger) const
{
    CBigInt new_;
    new_ = number.c_str();
    new_ += stranger;
    return new_;
}

CBigInt CBigInt::operator+(const char* stranger) const
{
    CBigInt new_;
    new_ = number.c_str();
    new_ += stranger;
    return new_;
}

CBigInt CBigInt::operator+(const CBigInt &stranger) const
{
    CBigInt new_;
    new_ = number.c_str();
    new_ += stranger;
    return new_;
}

void CBigInt::operator+=(const int stranger) { number = _sum(number, _toString(stranger)); }
void CBigInt::operator+=(const char* stranger) { number = _sum(number, _toString(stranger)); }
void CBigInt::operator+=(const CBigInt &stranger) { number = _sum(number, _toString(stranger)); }

/* MULTIPLICATION *************************************************************/

CBigInt CBigInt::operator*(const int stranger) const
{
    CBigInt new_;
    new_ = number.c_str();
    new_ *= stranger;
    return new_;
}

CBigInt CBigInt::operator*(const char* stranger) const
{
    CBigInt new_;
    new_ = number.c_str();
    new_ *= stranger;
    return new_;
}

CBigInt CBigInt::operator*(const CBigInt &stranger) const
{
    CBigInt new_;
    new_ = number.c_str();
    new_ *= stranger;
    return new_;
}

void CBigInt::operator*=(const int stranger) { number = _mul(number, _toString(stranger)); }
void CBigInt::operator*=(const char* stranger) { number = _mul(number, _toString(stranger)); }
void CBigInt::operator*=(const CBigInt &stranger) { number = _mul(number, _toString(stranger)); }

/* STREAM *********************************************************************/

ostream& operator<<(ostream& stream, const CBigInt& obj)
{
    stream << obj.number;
    return stream;
}

istream& operator>>(istream& stream, CBigInt& obj)
{
    stringstream ss;
    char c;
    bool flag = false, failbit = false;
    
    while(!stream.eof()) {
        c = stream.get();
        if(stream.eof()) break;
        
        if(c == ' ') {
            if(flag) break;
            else continue;
        }
        if(c < '0' || c > '9') {
            if(!flag) failbit = true;
            break;
        }
        
        flag = true;
        ss << c;
    }
    
    stream.clear();
    if(!failbit) {
        ss >> obj.number;
    } else {
        stream.setstate(ios::failbit);
    }
    
    return stream;
}

/* CONVERT ********************************************************************/

string CBigInt::_toString(const int value)
{
    stringstream ss;
    ss << value;
    return ss.str();
}

string CBigInt::_toString(const char* value)
{
    stringstream ss;
    ss << value;
    return ss.str();
}

string CBigInt::_toString(const CBigInt &value)
{
    stringstream ss;
    ss << value;
    return ss.str();
}

/* SUM ************************************************************************/

string CBigInt::_sum(string a, string b)
{
    stringstream ss;
    string c, foo;
    
    int n = 0;
    int la = a.length();
    int lb = b.length();
    int length = la > lb ? la : lb;
    int zero = 0;
    
    for(int i = 0; i < length; i++) {
        if(la > 0) n += (int) a.c_str()[a.length()-i-1] - 48;
        if(lb > 0) n += (int) b.c_str()[b.length()-i-1] - 48;
        
        ss.str("");
        ss << n;
        foo = ss.str();
        
        if(foo.length() == 2) {
            n = (int) foo.c_str()[0] - 48;
            
            if(foo.c_str()[1] == '0') {
                zero++;
            } else {
                for(int z = 0; z < zero; z++) c = "0" + c;
                zero = 0;
                c = foo.c_str()[1] + c;
            }
        } else {
            n = 0;
            
            if(foo.c_str()[1] != '0') {
                for(int z = 0; z < zero; z++) c = "0" + c;
                zero = 0;
                c = foo.c_str()[0] + c;
            } else {
                zero++;
            }
        }
        
        la--;
        lb--;
    }
    
    if(n > 0) {
        ss.str("");
        ss << n;
        foo = ss.str();
        c = foo.c_str()[0] + c;
    }
    
    return c;
}

/* MUL ************************************************************************/

string CBigInt::_simpleMul(string a, int b)
{
    stringstream ss;
    string c, foo;
    int n = 0;
    int zero = 0;
    
    for(unsigned int i = 0; i < a.length(); i++) {
        n += b * ((int) a.c_str()[a.length()-i-1] - 48);

        ss.str("");
        ss << n;
        foo = ss.str();

        if(foo.length() == 2) {
            n = (int) foo.c_str()[0] - 48;
            
            if(foo.c_str()[1] != '0') {
                for(int z = 0; z < zero; z++) c = "0" + c;
                zero = 0;
                c = foo.c_str()[1] + c;
            } else {
                zero++;
            }
        } else {
            n = 0;
            
            if(foo.c_str()[1] != '0') {
                for(int z = 0; z < zero; z++) c = "0" + c;
                zero = 0;
                c = foo.c_str()[0] + c;
            } else {
                zero++;
            }
        }
    }
    
    if(n > 0) {
        ss.str("");
        ss << n;
        foo = ss.str();
        for(int z = 0; z < zero; z++) c = "0" + c;
        c = foo.c_str()[0] + c;
    }
    
    return c;
}

string CBigInt::_mul(string a, string b)
{
    string c = "0", t = "0";
    int foo = b.length() - 1;
    
    if(a == "0" || b == "0") return "0";
    
    for(unsigned int i = 0; i < b.length(); i++) {
        t = _simpleMul(a, (int) b.c_str()[i] - 48);
        for(int z = 0; z < foo; z++) t += "0";
        foo--;
        c = _sum(c, t);
    }
    
    return c;
}

#ifndef __PROGTEST__
int main(void)
{
    CBigInt       a, b;
    istringstream is;
    a = 10;
    a += 20;  // a = 30 
    cout << a << " (30)" << endl;
    a *= 5;  // a = 150 
    cout << a << " (150)" << endl;  // 150
    //b = 3 + a;  // b = 153 
    b = a + 3;  // b = 153 
    cout << b << " (153)" << endl;
    b = a * 7;  // b = 1050 

    cout << b << " (1050)" << endl;
    cout << a << " (150)" << endl;  // 150

    a = "12345678901234567890";
    a += "99999999999999999999";  // a = 112345678901234567889 
    a *= "54321987654321987654";  // a = 6102840582289286403810728658624282842406 
    cout << a << " (6102840582289286403810728658624282842406)" << endl;  // 6102840582289286403810728658624282842406
    a *= 0;  // a = 0 
    cout << a << " (0)" << endl;  // 0
    a = 10;
    b = a + "400";  // b = 410 
    b = a * "15";  // b = 150 
    cout << a << " (10)" << endl;  // 10

    is . clear ();
    is . str ( " 1234" );
    is >> b; // b = 1234, is . fail() = 0
    is . clear ();
    is . str ( " 12 34" );
    is >> b; // b = 12, is . fail() = 0
    is . clear ();
    is . str ( "999z" );
    is >> b; // b = 999, is . fail() = 0
    is . clear ();
    is . str ( "abcd" );
    is >> b; // b = 999, is . fail() = 1
    
    return 0;
}
#endif /* __PROGTEST__ */
