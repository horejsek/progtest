#ifndef __PROGTEST__
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <cstring>
#include <stdio.h>

#include <cassert>

using namespace std;
#endif /* __PROGTEST__ */





template <class T>
class CList
 {
   public:
                                CList           ( void );

     void                       InsertStart     ( const T & X );
     void                       InsertEnd       ( const T & X );
     void                       Merge           ( const CList<T> & a,
                                                  const CList<T> & b );
     // operator vystupu
        friend ostream& operator<<(ostream& stream, const CList<T>& obj)
        {
            stream << "[";
            
            TItem* node = obj.m_First;
            while(node) {
                stream << node->Val;
                node = node->Next;
                if(node) stream << ", ";
            }
            
            stream << "]";
            return stream;
        }


   protected:
     struct TItem
      {
        TItem                 * Next;
        T                       Val;
                                TItem           ( const T & val ) { Val = val; }
      };

     TItem                    * m_First;
     TItem                    * m_Last;
 };

template <class T>
CList<T>::CList ( void )
 {
   m_First = NULL;
   m_Last  = NULL;
 }

template <class T>
void CList<T>::InsertStart ( const T & X )
 {
   TItem * n;

   n          = new TItem ( X );
   n -> Next  = m_First;
   m_First = n;

   if ( ! m_Last ) 
     m_Last = m_First;
 }

template <class T>
void CList<T>::InsertEnd (  const T & X )
 {
   TItem * n;

   n          = new TItem ( X );

   if ( m_Last )
    {
      m_Last -> Next = n;
      m_Last = n;
    }
   else
     m_Last = m_First = n;
 }
 template <class T>
 void CList<T>::Merge(const CList<T> & a,
                                                  const CList<T> & b)
 {
    TItem* node1 = a.m_First;
    TItem* node2 = b.m_First;
    TItem* new_ = NULL;
    TItem* foo = m_First;
    TItem* bar = NULL;
    
    while(foo) {
        bar = foo;
        foo = foo->Next;
        delete bar;
    }
    m_First = NULL;
    m_Last = NULL;
    
    
    while(true) {
        if(node1) {
            new_ = new TItem(node1->Val);
            new_->Val = node1->Val;
            new_->Next = NULL;
            if(!m_First) m_First = new_;
            if(m_Last) m_Last->Next = new_;
            m_Last = new_;
            node1 = node1->Next;
        }
        
        if(node2) {
            new_ = new TItem(node2->Val);
            new_->Val = node2->Val;
            new_->Next = NULL;
            if(!m_First) m_First = new_;
            if(m_Last) m_Last->Next = new_;
            m_Last = new_;
            node2 = node2->Next;
        }
        
        if(!node1 && !node2) break;
    }
 }
 


#ifndef __PROGTEST__
int main(void)
{
CList<int> L0, L1, L2;
cout << L0 << endl;
 // []
L0 . InsertEnd ( 33 );
cout << L0 << endl;
 // [33]
L1 . InsertEnd ( 1 );
L1 . InsertEnd ( 2 );
L1 . InsertEnd ( 3 );
L1 . InsertEnd ( 4 );
L2 . InsertStart ( 60 );
L2 . InsertStart ( 50 );
L2 . InsertStart ( 40 );
L2 . InsertStart ( 30 );
L2 . InsertStart ( 20 );
L2 . InsertStart ( 10 );
cout << L1 << endl;
 // [1, 2, 3, 4]
cout << L2 << endl;
 // [10, 20, 30, 40, 50, 60]
L0 . Merge ( L1, L2 );
cout << L0 << endl;
 // [1, 10, 2, 20, 3, 30, 4, 40, 50, 60]
L0 . InsertEnd ( 33 );
cout << L0 << endl;
 // [1, 10, 2, 20, 3, 30, 4, 40, 50, 60, 33]

    
    return 0;
}
#endif
