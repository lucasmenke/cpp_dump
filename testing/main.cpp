#include <iostream>

class Base
{
public:
    virtual void Whisper() const { std::cout << "Base class whisper\n"; }
};

class Derived : public Base
{
public:
    void Whisper() const { std::cout << "Derived class whisper\n"; }
};

void PtrFunction(Base *pObj);
void RefFunction(Base &rObj);
void ValueFunction(Base obj);
int main()
{
    Base *pDerived = new Derived;
    PtrFunction(pDerived);
    RefFunction(*pDerived);
    ValueFunction(*pDerived);

    return 0;
}

void PtrFunction(Base *pObj) { pObj->Whisper(); }
void RefFunction(Base &rObj) { rObj.Whisper(); }
void ValueFunction(Base obj) { obj.Whisper(); }