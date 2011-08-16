#ifndef __PROGTEST__
#include <iostream>
#include <cstdlib>

using namespace std;

struct CComplex
{
    CComplex(void) {}
    CComplex(int re, int im) {m_Re = re; m_Im = im;}
    int m_Re;
    int m_Im;
};
#endif /* __PROGTEST__ */

#define LT -1
#define EQ 0
#define GT 1

/*****************************************************************************/
/* HEADER                                                                    */
/*****************************************************************************/

class CSetCplx
{
    private:
        CComplex* list;
        int count;
        int allocated;

        int find(CComplex el, int start, int end) const;
        int findInsert(CComplex el, int start, int end) const;
        void insert(CComplex el);
        void delete_(CComplex el);
    public:
        CSetCplx(void);
        ~CSetCplx(void);
        bool Insert(CComplex X);
        bool Delete(CComplex X);
        bool IsSet(CComplex X) const;
        int Size(void) const;
};

/*****************************************************************************/
/* CONSTRUCT                                                                 */
/*****************************************************************************/

CSetCplx::CSetCplx(void)
{
    list = NULL;
    count = 0;
    allocated = 0;
}

CSetCplx::~CSetCplx(void)
{
    count = 0;
    allocated = 0;
    free(list);
}

/*****************************************************************************/
/* PRIVATE                                                                   */
/*****************************************************************************/

int CSetCplx::find(CComplex el, int start, int end) const
{
    if(count == 0) return -1;
    
    int middle = start + (end - start) / 2;

    int cmp;
    if(el.m_Re == list[middle].m_Re && el.m_Im == list[middle].m_Im)
        cmp = EQ;
    else if((el.m_Re == list[middle].m_Re && el.m_Im < list[middle].m_Im) || el.m_Re < list[middle].m_Re)
        cmp = LT;
    else
        cmp = GT;

    if(cmp == EQ) {
        return middle;
    } else if(cmp != EQ && start == end) {
        return -1;
    } else if(cmp != EQ && middle == end) {
        return -1;
    } else if(cmp != EQ && middle == start) {
        return find(el, end, end);
    } else if(cmp == LT) {
        return find(el, start, middle);
    } else if(cmp == GT) {
        return find(el, middle, end);
    }
    return -1;
}

int CSetCplx::findInsert(CComplex el, int start, int end) const
{
    if(count == 0) return 0;
    
    int middle = start + (end - start) / 2;

    int cmp;
    if(el.m_Re == list[middle].m_Re && el.m_Im == list[middle].m_Im)
        cmp = EQ;
    else if((el.m_Re == list[middle].m_Re && el.m_Im < list[middle].m_Im) || el.m_Re < list[middle].m_Re)
        cmp = LT;
    else
        cmp = GT;

    //cout << "FIND : " << cmp << " - " << start << ", " << end << endl;

    if(cmp == EQ) {
        return -1;
    } else if(cmp != EQ && start == end) {
        return end;
    } else if(cmp != EQ && middle == end) {
        return end;
    } else if(cmp != EQ && middle == start) {
        return findInsert(el, end, end);
    } else if(cmp == LT) {
        return findInsert(el, start, middle);
    } else if(cmp == GT) {
        return findInsert(el, middle, end);
    }
    return -1;
}

void CSetCplx::insert(CComplex el)
{
    int index = findInsert(el, 0, count);
    
    /*cout << ">>>>>>" << endl;
    cout << "element: " << el.m_Re << ", " << el.m_Im << endl;
    for(int i = 0; i < count; i++) {
        cout << "i" << i << " - " << list[i].m_Re << ", " << list[i].m_Im << endl;
    }
    cout << "insert > count: " << count << " index: " << index << endl;
    cout << "-----" << endl;*/

    if(index == -1) {
        return;
    }

    list = (CComplex*)realloc(list, (count+1) * sizeof(CComplex));
    
    for(int i = count; i >= index; i--) {
        list[i] = list[i-1];
    }
    
    list[index] = el;
    count++;
    allocated++;
}

void CSetCplx::delete_(CComplex el)
{
    int index = find(el, 0, count);

    if(index == -1) return;

    for(int i = index; i < count; i++) {
        list[i] = list[i+1];
    }
    count--;
}

/*****************************************************************************/
/* PUBLIC                                                                    */
/*****************************************************************************/

bool CSetCplx::Insert(CComplex X)
{
    int foo = count;
    insert(X);
    
    return foo+1 == count;
}

bool CSetCplx::Delete(CComplex X)
{
    int foo = count;
    delete_(X);
    return foo-1 == count;
}

bool CSetCplx::IsSet(CComplex X) const
{
    /*for(int i = 0; i < count; i++) {
        cout << "i" << i << " - " << list[i].m_Re << ", " << list[i].m_Im << endl;
    }*/
    return find(X, 0, count-1) != -1;
}

int CSetCplx::Size(void) const
{
    return count;
}

#ifndef __PROGTEST__
int main(void)
{
    bool status;
    CSetCplx S1;
    
    status = S1.Insert(CComplex(10, 20));
    cout << status << " - " << S1.Size() << " (1, 1) insert(10,20)" << endl;
    status = S1. Insert(CComplex(20, 30));
    cout << status << " - " << S1.Size() << " (1, 2) insert(20,30)" << endl;
    status = S1.Insert(CComplex(10, 30));
    cout << status << " - " << S1.Size() << " (1, 3) insert(10,30)" << endl;
    
    
    status = S1.IsSet(CComplex(20, 30));
    cout << status << " - " << S1.Size() << " (1, 3) isset(20,30)" << endl;
    status = S1.IsSet(CComplex(10, 20));
    cout << status << " - " << S1.Size() << " (1, 3) isset(10,20)" << endl;
    status = S1.IsSet(CComplex(10, 30));
    cout << status << " - " << S1.Size() << " (1, 3) isset(10,30)" << endl;
    status = S1.Delete(CComplex(10, 30));
    cout << status << " - " << S1.Size() << " (1, 2) delete(10,30)" << endl;
    status = S1.IsSet(CComplex(10, 30));
    cout << status << " - " << S1.Size() << " (0, 2) isset(10,30)" << endl;
    status = S1.IsSet(CComplex(10, 20));
    cout << status << " - " << S1.Size() << " (1, 2) isset(10,20)" << endl;

    cout << endl;

    CSetCplx S2;
    status = S2.Insert(CComplex(30, 50));
    cout << status << " - " << S2.Size() << " (1, 1) insert(30,50" << endl;
    status = S2.IsSet(CComplex(30, 50));
    cout << status << " - " << S2.Size() << " (1, 1) isset(30,50)" << endl;
    status = S2.Delete(CComplex(30, 50));
    cout << status << " - " << S2.Size() << " (1, 0) delete(30,50)" << endl;
    status = S2.IsSet(CComplex(30, 50));
    cout << status << " - " << S2.Size() << " (0, 0) isset(30,50)" << endl;
    status = S2.Delete(CComplex(30, 50));
    cout << status << " - " << S2.Size() << " (0, 0) delete(30,50)" << endl;
    status = S2.Insert(CComplex(30, 50));
    cout << status << " - " << S2.Size() << " (1, 1) insert(30,50)" << endl;
    status = S2.IsSet(CComplex(30, 50));
    cout << status << " - " << S2.Size() << " (1, 1) isset(30,50)" << endl;
    status = S2.Insert(CComplex(30, 50));
    cout << status << " - " << S2.Size() << " (0, 1) insert(30,50)" << endl;
    status = S2.IsSet(CComplex(30, 50));
    cout << status << " - " << S2.Size() << " (1, 1) isset(30,50)" << endl;
    status = S2.Delete(CComplex(30, 50));
    cout << status << " - " << S2.Size() << " (1, 0) delete(30,50)" << endl;
    status = S2.IsSet(CComplex(30, 50));
    cout << status << " - " << S2.Size() << " (0, 0) isset(30,50)" << endl;

    cout << endl;

    CSetCplx S3;
    status = S3.Insert(CComplex(1, 2));
    cout << status << " - " << S3.Size() << endl;
    status = S3.Insert(CComplex(1, 2));
    cout << status << " - " << S3.Size() << endl;
    status = S3.Delete(CComplex(1, 3));
    cout << status << " - " << S3.Size() << endl;
    status = S3.Delete(CComplex(1, 2));
    cout << status << " - " << S3.Size() << endl;
    status = S3.Delete(CComplex(1, 2));
    cout << status << " - " << S3.Size() << endl;
    status = S3.Insert(CComplex(1, 2));
    cout << status << " - " << S3.Size() << endl;

    cout << endl;

    CSetCplx S4;
    S4.Insert(CComplex(10, 20));
    S4.Insert(CComplex(20, 30));
    S4.Insert(CComplex(10, 30));
    S4.Insert(CComplex(10, 40));

    return 0;
}
#endif /* __PROGTEST__ */
