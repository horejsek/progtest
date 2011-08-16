#ifndef __PROGTEST__
#include <iostream>
#include <cstdlib>
//#include <sstream>
//#include <cstring>
#include <stdio.h>
#include <string.h>

using namespace std;
#endif /* __PROGTEST__ */

#define DANGER_BLADE_LENGTH 7

#define CLOTHES 1
#define SHOES 2
#define KNIFE 3
#define GPS 4

/******************************************************************************/

class CThings
{
    public:
        CThings(void);
        virtual void print(ostream &stream) const;
        virtual int getWeight(void) const;
        virtual bool isDanger(void) const;
        virtual CThings* duplicate(void) const;
};

CThings::CThings(void) {}
void CThings::print(ostream &stream) const {}
int CThings::getWeight(void) const { return 0; }
bool CThings::isDanger(void) const { return false; }

CThings* CThings::duplicate(void) const
{
    CThings* tmp = new CThings();
    return tmp;
}

/******************************************************************************/

class CClothes: public CThings
{
    private:
        char description[256];
        
    public:
        CClothes(const char*);
        CClothes(char[256]);
        virtual void print(ostream &stream) const;
        virtual int getWeight(void) const;
        virtual CClothes* duplicate(void) const;
};

CClothes::CClothes(const char* pdescription) { strcpy(description, pdescription); }
CClothes::CClothes(char pdescription[256]) { strcpy(description, pdescription); }
int CClothes::getWeight(void) const { return 500; }

void CClothes::print(ostream &stream) const
{
    stream << "Clothes (";
    stream << description;
    stream << ")";
}

CClothes* CClothes::duplicate(void) const
{
    CClothes* tmp = new CClothes(description);
    return tmp;
}

/******************************************************************************/

class CShoes: public CThings
{
    public:
        CShoes(void);
        virtual void print(ostream &stream) const;
        virtual int getWeight(void) const;
        virtual CShoes* duplicate(void) const;
};

CShoes::CShoes(void) {}
int CShoes::getWeight(void) const { return 750; }
void CShoes::print(ostream &stream) const { stream << "Shoes"; }

CShoes* CShoes::duplicate(void) const
{
    CShoes* tmp = new CShoes();
    return tmp;
}

/******************************************************************************/

class CKnife: public CThings
{
    private:
        int length;
        
    public:
        CKnife(int);
        virtual void print(ostream &stream) const;
        virtual int getWeight(void) const;
        virtual bool isDanger(void) const;
        virtual CKnife* duplicate(void) const;
};

CKnife::CKnife(int plength) { length = plength; }
int CKnife::getWeight(void) const { return 100; }

void CKnife::print(ostream &stream) const
{
    stream << "Knife ";
    stream << length;
    stream << " cm";
}

bool CKnife::isDanger(void) const
{
    return length > DANGER_BLADE_LENGTH;
}

CKnife* CKnife::duplicate(void) const
{
    CKnife* tmp = new CKnife(length);
    return tmp;
}

/******************************************************************************/

class CGPS: public CThings
{
    public:
        CGPS(void);
        virtual void print(ostream &stream) const;
        virtual int getWeight(void) const;
        virtual CGPS* duplicate(void) const;
};

CGPS::CGPS(void) {}
int CGPS::getWeight(void) const { return 250; }
void CGPS::print(ostream &stream) const { stream << "GPS"; }
CGPS* CGPS::duplicate(void) const
{
    CGPS* tmp = new CGPS();
    return tmp;
}

/******************************************************************************/

class CSuitcase
{
    private:
        
    public:
        int count;
        int weight;
        bool danger;
        CThings** array;
        
        CSuitcase(void);
        ~CSuitcase(void);
        
        CSuitcase& Add(const CThings&);
        int Weight(void) const;
        bool Danger(void) const;
        int Count(void) const;
        
        CSuitcase(const CSuitcase &obj);
        void operator=(const CSuitcase &obj);
};

CSuitcase::CSuitcase(void)
{
    count = 0;
    weight = 2000;
    danger = false;
    array = NULL;
}


CSuitcase::~CSuitcase(void)
{
    for(int i = 0; i < count; i++) {
        free(array[i]);
    }

}


CSuitcase::CSuitcase(const CSuitcase &obj)
{
    count = 0;
    weight = 2000;
    danger = false;
    array = NULL;
    for(int i = 0; i < obj.count; i++) {
        Add(*obj.array[i]);
    }
}

void CSuitcase::operator=(const CSuitcase &obj)
{
    for(int i = 0; i < count; i++) {
        free(array[i]);
    }
    count = 0;
    weight = 2000;
    danger = false;
    array = NULL;
    
    for(int i = 0; i < obj.count; i++) {
        Add(*obj.array[i]);
    }
}


CSuitcase& CSuitcase::Add(const CThings& item)
{
    array = (CThings**)realloc(array, (count+1) * sizeof(CThings*));
    array[count] = item.duplicate();
    
    count++;
    weight += item.getWeight();
    danger = danger || item.isDanger();
    
    return *this;
}

int CSuitcase::Weight(void) const
{
    return weight;
}

bool CSuitcase::Danger(void) const
{
    return danger;
}

int CSuitcase::Count(void) const
{
    return count+1;
}

ostream & operator<<(ostream &stream, const CSuitcase &obj)
{
    stream << "Suitcase" << endl;
    
    for(int i = 0; i < obj.count; i++) {
        stream << (i+1 == obj.count ? "\\-" : "+-");
        obj.array[i]->print(stream);
        stream << endl;
    }
    
    return stream;
}

/******************************************************************************/


#ifndef __PROGTEST__
int main(void)
{

CSuitcase T0;
T0 . Add ( CKnife ( 7 ) );
T0 . Add ( CGPS () );
T0 . Add ( CClothes ( "red T-shirt" ) );
T0 . Add ( CShoes () ) ;
cout << T0;
cout << T0.Count() << " - " << T0.Weight() << " - " << T0.Danger() << endl;

cout << endl << "----" << endl << endl;

CSuitcase T1;
T1 . Add ( CKnife ( 5 ) );
T1 . Add ( CClothes ( "red T-shirt" ) );
T1 . Add ( CClothes ( "blue jeans" ) );
T1 . Add ( CShoes () );
cout << T1;
cout << T1.Count() << " - " << T1.Weight() << " - " << T1.Danger() << endl;

cout << endl << "----" << endl << endl;

CSuitcase T2;
T2 . Add ( CKnife ( 8 ) ) . Add ( CClothes ( "white underwear" ) ) . Add ( CShoes () );
T2 . Add ( CClothes ( "red T-shirt" ) );
T2 . Add ( CClothes ( "brown jacket" ) );
T2 . Add ( CShoes () );
cout << T2;
cout << T2.Count() << " - " << T2.Weight() << " - " << T2.Danger() << endl;


cout << endl << "----" << endl << endl;


CSuitcase T3(T2);
cout << T3;

    return 0;
}
#endif /* __PROGTEST__ */
