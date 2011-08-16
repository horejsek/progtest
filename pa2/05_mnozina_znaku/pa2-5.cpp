#ifndef __PROGTEST__
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <cstring>
#include <stdio.h>

using namespace std;
#endif /* __PROGTEST__ */

#define COUNT 1024

class CSetWChar
{
    public:
        CSetWChar(void);
        bool Insert(int X);
        bool Delete(int X);
        bool IsSet(int X) const;
        int Size(void) const;

        CSetWChar Intersect(const CSetWChar& x) const;
        CSetWChar Union(const CSetWChar& x) const;
        bool Equals(const CSetWChar& x) const;
        bool IsSubsetOf(const CSetWChar& x) const;
        bool HasSubset(const CSetWChar& x) const;
        
        void print(void) const;

    private:
        int size;
        bool array[COUNT];
};

/* CONSTRUCT ******************************************************************/

CSetWChar::CSetWChar(void)
{
    size = 0;
    
    for(int i = 0; i < COUNT; i++)
        array[i] = false;
}

/* PUBLIC *********************************************************************/

bool CSetWChar::Insert(int X)
{
    if(X < 0 || X > 1024 || array[X]) return false;

    array[X] = true;
    size++;
    return true;
}

bool CSetWChar::Delete(int X)
{
    if(X < 0 || X > 1024 || !array[X]) return false;
    
    array[X] = false;
    size--;
    return true;
}

bool CSetWChar::IsSet(int X) const
{
    if(X < 0 || X > 1024) return false;
    return array[X];
}

int CSetWChar::Size(void) const
{
    return size;
}

CSetWChar CSetWChar::Intersect(const CSetWChar& x) const
{
    CSetWChar new_;
    for(int i = 0; i < COUNT; i++) {
        if(x.array[i] && array[i])
            new_.Insert(i);
    }
    return new_;
}

CSetWChar CSetWChar::Union(const CSetWChar& x) const
{
    CSetWChar new_;
    for(int i = 0; i < COUNT; i++) {
        if(x.array[i] || array[i])
            new_.Insert(i);
    }
    return new_;
}

bool CSetWChar::Equals(const CSetWChar& x) const
{
    for(int i = 0; i < COUNT; i++) {
        if(x.array[i] != array[i])
            return false;
    }
    return true;
}

bool CSetWChar::IsSubsetOf(const CSetWChar& x) const
{
    for(int i = 0; i < COUNT; i++) {
        if(!x.array[i] && array[i]) {
            return false;
        }
    }
    return true;
}

bool CSetWChar::HasSubset(const CSetWChar& x) const
{
    for(int i = 0; i < COUNT; i++) {
        if(x.array[i] && !array[i]) {
            return false;
        }
    }
    return true;
}

void CSetWChar::print(void) const
{
    for(int i = 0; i < COUNT; i++) {
        if(array[i]) {
            cout << i << ", ";
        }
    }
    cout << endl;
}


#ifndef __PROGTEST__
int main(void)
{
    bool status;

    cout << "-----" << endl << "S1:" << endl;
    
    CSetWChar S1;
    status = S1 . Insert ( 10 );
    // status = true, S1 . Size () => 1
    cout << "status (1): " << status << ", size (1): " << S1.Size() << endl;
    status = S1 . Insert ( 20 );
    // status = true, S1 . Size () => 2
    cout << "status (1): " << status << ", size (2): " << S1.Size() << endl;
    status = S1 . Insert ( 30 );
    // status = true, S1 . Size () => 3
    cout << "status (1): " << status << ", size (3): " << S1.Size() << endl;
    status = S1 . Insert ( 20 );
    // status = false, S1 . Size () => 3
    cout << "status (0): " << status << ", size (3): " << S1.Size() << endl;
    status = S1 . IsSet  ( 20 );
    // status = true
    cout << "status (1): " << status << endl;
    status = S1 . Delete ( 20 );
    // status = true, S1 . Size () => 2
    cout << "status (1): " << status << ", size (2): " << S1.Size() << endl;
    status = S1 . IsSet  ( 20 );
    // status = false
    cout << "status (0): " << status << endl;
    // S1 = {10, 30} 

    cout << "-----" << endl << "S2:" << endl;

    CSetWChar S2;
    status = S2 . Insert ( 10 );
    // status = true, S2 . Size () => 1
    cout << "status (1): " << status << ", size (1): " << S2.Size() << endl;
    status = S2 . Insert ( 30 );
    // status = true, S2 . Size () => 2
    cout << "status (1): " << status << ", size (2): " << S2.Size() << endl;
    status = S2 . Delete ( 20 );
    // status = false, S2 . Size () => 2
    cout << "status (0): " << status << ", size (2): " << S2.Size() << endl;
    // S2 = {10, 30}

    cout << "-----" << endl << "S3:" << endl;

    CSetWChar S3;
    status = S3 . Insert ( 10 );
    // status = true, S3 . Size () => 1
    cout << "status (1): " << status << ", size (2): " << S3.Size() << endl;
    status = S3 . Insert ( 30 );
    // status = true, S3 . Size () => 2
    cout << "status (1): " << status << ", size (2): " << S3.Size() << endl;
    status = S3 . Insert ( 50 );
    // status = true, S3 . Size () => 3
    cout << "status (1): " << status << ", size (3): " << S3.Size() << endl;
    // S3 = {10, 30, 50}

    cout << "-----" << endl << "print:" << endl;

    S1.print();
    S2.print();
    S3.print();

    cout << "-----" << endl << "S2-S2:" << endl;

    S2 . Intersect ( S2 );
    // {10, 30}
    S2 . Union ( S2 );
    // {10, 30}
    status = S2 . IsSubsetOf ( S2 );
    // status = true
    cout << "status (1): " << status << endl;
    status = S2 . HasSubset ( S2 );
    // status = true
    cout << "status (1): " << status << endl;
    status = S2 . Equals ( S2 );
    // status = true
    cout << "status (1): " << status << endl;

    cout << "-----" << endl << "S2-S3:" << endl;

    S2 . Intersect ( S3 );
    // {10, 30}
    S2 . Union ( S3 );
    // {10, 30, 50}   
    status = S2 . IsSubsetOf ( S3 );
    // status = true
    cout << "status (1): " << status << endl;
    status = S2 . HasSubset ( S3 );
    // status = false
    cout << "status (0): " << status << endl;
    status = S2 . Equals ( S3 );
    // status = false
    cout << "status (0): " << status << endl;

    cout << "-----" << endl << "S3-S2:" << endl;

    S3 . Intersect ( S2 );
    // {10, 30}
    S3 . Union ( S2 );
    // {10, 30, 50}   
    status = S3 . IsSubsetOf ( S2 );
    // status = false
    cout << "status (0): " << status << endl;
    status = S3 . HasSubset ( S2 );
    // status = true
    cout << "status (1): " << status << endl;
    status = S3 . Equals ( S2 );
    // status = false
    cout << "status (0): " << status << endl;

    cout << "-----" << endl << "S3-S3:" << endl;

    S3 . Intersect ( S3 );
    // {10, 30, 50}
    S3 . Union ( S3 );
    // {10, 30, 50}   
    status = S3 . IsSubsetOf ( S3 );
    // status = true
    cout << "status (1): " << status << endl;
    status = S3 . HasSubset ( S3 );
    // status = true
    cout << "status (1): " << status << endl;
    status = S3 . Equals ( S3 );
    // status = true
    cout << "status (1): " << status << endl;






    return 0;
}
#endif /* __PROGTEST__ */
