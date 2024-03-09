#include <iostream>

class Base
{
public:
    void StartBase() const { std::cout << "Start Base"; }
};

class Aggregator
{
public:
    void StartDerived() const { base.StartBase(); }

private:
    Base base;
};

int main()
{
    Aggregator mAggregator;
    mAggregator.StartDerived();

    return 0;
}