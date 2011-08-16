#ifndef __PROGTEST__
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <cstring>
#include <stdio.h>

using namespace std;
#endif /* __PROGTEST__ */


template <class T>
class CList
{
    public:
        CList(void);
        CList(const CList &cl);
        ~CList(void);

        int Size(void) const;
        bool InsBefore(int pos, const T &x);
        bool InsAfter(int pos, const T &x);
        bool InsStart(const T &x);
        bool InsEnd(const T &x);
        bool Read(int pos, T &x) const;
        bool Delete(int pos);
        void print(void) const;
        void print2(void) const;
        
        CList & operator=(const CList &cl);

    protected:
        struct TItem {
           TItem* m_Next;
           TItem* m_Prev;
           T m_Val;
        };
        
        TItem* m_First;
        TItem* m_Last;

    private:
        int count;
        
        bool _front(int pos) const;
        struct TItem* _getPos(int pos) const;
        struct TItem* _newNode(const T &x, TItem* prev, TItem* next);
};

/* CONSTRUCT ******************************************************************/

template <class T> CList<T>::CList(void)
{
    m_First = NULL;
    m_Last = NULL;
    count = 0;
}

template <class T> CList<T>::CList(const CList &cl)
{
    struct TItem* node = cl.m_First;
    struct TItem* prev = NULL;
    struct TItem* new_ = NULL;
    
    m_First = NULL;
    m_Last = NULL;
    count = 0;
    
    while(node) {
        new_ = _newNode(node->m_Val, prev, NULL);
        
        if(count == 1) m_First = new_;
        else prev->m_Next = new_;
        
        prev = new_;
        node = node->m_Next;
    }
    
    m_Last = new_;
}

template <class T> CList<T>::~CList(void)
{
    struct TItem* node = m_First;
    struct TItem* tmp;
    
    while(node) {
        tmp = node;
        node = node->m_Next;
        delete tmp;
    }
    
    m_First = NULL;
    m_Last = NULL;
    count = 0;
}

template <class T> CList<T>& CList<T>::operator=(const CList &cl)
{
    if(this == &cl)
        return *this;
    
    struct TItem* node = m_First;
    struct TItem* tmp;
    
    while(node) {
        tmp = node;
        node = node->m_Next;
        delete tmp;
    }
    
    node = cl.m_First;
    struct TItem* prev = NULL;
    struct TItem* new_ = NULL;

    m_First = NULL;
    m_Last = NULL;
    count = 0;
    
    while(node) {
        new_ = _newNode(node->m_Val, prev, NULL);
        
        if(count == 1) m_First = new_;
        else prev->m_Next = new_;
        
        prev = new_;
        node = node->m_Next;
    }
    
    m_Last = new_;
    
    return *this;
}

/* PUBLIC *********************************************************************/

template <class T> int CList<T>::Size(void) const
{
    return count;
}

template <class T> bool CList<T>::InsBefore(int pos, const T &x)
{
    struct TItem* el;
    struct TItem* tmp;
    struct TItem* new_;
    
    if(count == 0) {
        new_ = _newNode(x, NULL, NULL);
        m_First = new_;
        m_Last = new_;
        return true;
    }
    
    el = _getPos(pos);
    if(el) {
        if(el) new_ = _newNode(x, el->m_Prev, el);
        else new_ = _newNode(x, NULL, NULL);
        
        tmp = el->m_Prev;
        if(tmp) tmp->m_Next = new_;
        else m_First = new_;
        el->m_Prev = new_;
        
        return true;
    }
    
    return false;
}

template <class T> bool CList<T>::InsAfter(int pos, const T &x)
{
    struct TItem* el;
    struct TItem* tmp;
    struct TItem* new_;
    
    if(count == 0) {
        new_ = _newNode(x, NULL, NULL);
        m_First = new_;
        m_Last = new_;
        return true;
    }
    
    el = _getPos(pos);
    
    if(el) {
        if(el) new_ = _newNode(x, el, el->m_Next);
        else new_ = _newNode(x, NULL, NULL);
        
        tmp = el->m_Next;
        if(tmp) tmp->m_Prev = new_;
        else m_Last = new_;
        el->m_Next = new_;
        
        return true;
    }
    
    return false;
}

template <class T> bool CList<T>::InsStart(const T &x)
{
    return InsBefore(0, x);
}

template <class T> bool CList<T>::InsEnd(const T &x)
{
    return InsAfter(count-1, x);
}

template <class T> bool CList<T>::Read(int pos, T &x) const
{
    struct TItem* foo = _getPos(pos);
    if(foo) {
        x = foo->m_Val;
        return true;
    }
    return false;
}

template <class T> bool CList<T>::Delete(int pos)
{
    if(pos > count && count > 0) return false;

    struct TItem* foo;
    struct TItem* node;
    
    node = _getPos(pos);
    if(node) {
        foo = node->m_Prev;
        if(foo) foo->m_Next = node->m_Next;
        else m_First = node->m_Next;
        
        foo = node->m_Next;
        if(foo) foo->m_Prev = node->m_Prev;
        else m_Last = node->m_Prev;
        
        delete node;
        count--;
        return true;
    }
    
    return false;
}

/* PRIVATE ********************************************************************/

template <class T> bool CList<T>::_front(int pos) const
{
    return pos < (count / 2);
}

template <class T> struct CList<T>::TItem* CList<T>::_getPos(int pos) const
{
    if(pos > count && count > 0) return NULL;

    bool front = _front(pos);
    struct TItem* node = front ? m_First : m_Last;
    int x = front ? 0 : count-1;
    
    while(node) {
        if(x != pos) {
            node = front ? node->m_Next : node->m_Prev;
            x += front ? +1 : -1;
            continue;
        }
        return node;
    }
    
    return NULL;
}

template <class T> void CList<T>::print(void) const
{
    struct TItem* node = m_First;
    while(node) {
        cout << node->m_Val/**/ << ", ";
        node = node->m_Next;
    }
    cout << endl;
}

template <class T> void CList<T>::print2(void) const
{
    struct TItem* node = m_Last;
    while(node) {
        cout << node->m_Val/**/ << ", ";
        node = node->m_Prev;
    }
    cout << endl;
}

template <class T> struct CList<T>::TItem* CList<T>::_newNode(const T &x, TItem* prev, TItem* next)
{
    struct TItem* node = new(struct TItem);
    node->m_Val = x;
    node->m_Prev = prev;
    node->m_Next = next;
    count++;
    return node;
}


#ifndef __PROGTEST__
int main(void)
{
    bool status;
    int tmp;
    string tmps;
    
    cout << "-----" << endl << "LI1:" << endl;
    
    CList<int> LI1;
    status = LI1.InsEnd(3000);
    cout << "status (1): " << status << ", size (1): " << LI1.Size() << endl;
    status = LI1.InsEnd(5000);
    cout << "status (1): " << status << ", size (2): " << LI1.Size() << endl;
    status = LI1.InsEnd(1000);
    cout << "status (1): " << status << ", size (3): " << LI1.Size() << endl;
    status = LI1.Read(0, tmp);
    cout << "status (1): " << status << ", tmp (3000): " << tmp << endl;
    status = LI1.Read(1, tmp);
    cout << "status (1): " << status << ", tmp (5000): " << tmp << endl;
    status = LI1.Read(2, tmp);
    cout << "status (1): " << status << ", tmp (1000): " << tmp << endl;
    
    cout << "-----" << endl << "LI2:" << endl;

    CList<int>  LI2;
    status = LI2.InsStart(10); // (10)
    cout << "status (1): " << status << ", size (1): " << LI2.Size() << endl;
    status = LI2.InsAfter(0, 20); // (10, 20)
    cout << "status (1): " << status << ", size (2): " << LI2.Size() << endl;
    status = LI2.InsEnd(30); // (10, 20, 30)
    cout << "status (1): " << status << ", size (3): " << LI2.Size() << endl;
    status = LI2.InsBefore(2, 40); // (10, 20, 40, 30)
    cout << "status (1): " << status << ", size (4): " << LI2.Size() << endl;

    status = LI2.Read(0, tmp);
    cout << "status (1): " << status << ", tmp (10): " << tmp << endl;
    status = LI2.Read(1, tmp);
    cout << "status (1): " << status << ", tmp (20): " << tmp << endl;
    status = LI2.Read(2, tmp);
    cout << "status (1): " << status << ", tmp (40): " << tmp << endl;
    status = LI2.Read(3, tmp);
    cout << "status (1): " << status << ", tmp (30): " << tmp << endl;

    cout << "-----" << endl << "LI3:" << endl;

    CList<int>  LI3;
    status = LI3.InsEnd(10); // (10)
    cout << "status (1): " << status << ", size (1): " << LI3.Size() << endl;
    status = LI3.InsEnd(20); // (10, 20)
    cout << "status (1): " << status << ", size (2): " << LI3.Size() << endl;
    status = LI3.InsEnd(30); // (10, 20, 30)
    cout << "status (1): " << status << ", size (3): " << LI3.Size() << endl;
    status = LI3.InsEnd(40); // (10, 20, 30, 40)
    cout << "status (1): " << status << ", size (4): " << LI3.Size() << endl;
    status = LI3.Delete(2); // (10, 20, 40)
    cout << "status (1): " << status << ", size (3): " << LI3.Size() << endl;
    status = LI3.InsBefore(2, 50); // (10, 20, 50, 40)
    cout << "status (1): " << status << ", size (4): " << LI3.Size() << endl;
    status = LI3.Read(3, tmp);
    cout << "status (1): " << status << ", tmp (40): " << tmp << endl;
    status = LI3.InsBefore(4, 60); // (10, 20, 50, 40)
    cout << "status (0): " << status << ", size (4): " << LI3.Size() << endl;
    status = LI3.InsAfter(3, 70); // (10, 20, 50, 40, 70)
    cout << "status (1): " << status << ", size (5): " << LI3.Size() << endl;

    cout << "-----" << endl << "LS:" << endl;

    CList<string>  LS;
    status = LS.InsEnd("first"); // (first)
    cout << "status (1): " << status << ", size (1): " << LS.Size() << endl;
    status = LS.InsEnd("third"); // (first, third)
    cout << "status (1): " << status << ", size (2): " << LS.Size() << endl;
    status = LS.InsAfter(0, "second"); // (first, second, third)
    cout << "status (1): " << status << ", size (3): " << LS.Size() << endl;
    status = LS.Read(2, tmps);
    cout << "status (1): " << status << ", tmp (third): " << tmps << endl;

    cout << "-----" << endl << "LS2:" << endl;
    
    tmps = "";
    CList<string> LS2;
    LS2 = LS;
    status = LS2.Read(2, tmps);
    cout << "status (1): " << status << ", tmp (third): " << tmps << endl;
    
    cout << "-----" << endl << "LI4:" << endl;
    
    tmp = -1;
    CList<int> LI4;
    LI4.InsAfter(0, 1);
    LI4.print();
    LI4 = LI3;
    LI4.print();
    status = LI4.Read(3, tmp);
    cout << "status (1): " << status << ", tmp (40): " << tmp << endl;
    
    cout << "-----" << endl << "print:" << endl;
    
    LI3.print();
    LI4.print();
    LI3.print2();
    LI4.print2();
    
    cout << "-----" << endl << "operations:" << endl;
    
    //LI3.InsAfter(2, 15);
    //LI4.InsAfter(2, 15);
    LI3.InsAfter(4, 15);
    LI4.InsAfter(4, 15);
    
    cout << "3: ";
    LI3.print();
    cout << "4: ";
    LI4.print();
    cout << "3: ";
    LI3.print2();
    cout << "4: ";
    LI4.print2();
    
    cout << "-----" << endl;
    
    LS.print();
    LS2.print();
    LS.print2();
    LS2.print2();

    return 0;
}
#endif /* __PROGTEST__ */
