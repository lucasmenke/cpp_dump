#include <string>

enum COLOR
{
    Brown,
    Grey,
    Black
};

class Horse
{
public:
    Horse(COLOR color, int height) : color(color), height(height)
    {
        printf("Horse constructor\n");
    }
    void HorseFacts() const { printf("Color: %d, Height: %d\n", color, height); }

private:
    COLOR color;
    int height;
};

class Bird
{
public:
    Bird(COLOR color, int age) : color(color), age(age)
    {
        printf("Bird constructor\n");
    }
    void BirdFacts() const { printf("Color: %d, Age: %d\n", color, age); }

private:
    COLOR color;
    int age;
};

class Pegasus : public Horse, public Bird
{
public:
    Pegasus(COLOR color, int height, int age) : Horse(color, height), Bird(color, age)
    {
        printf("Pegasus constructor\n");
    }
};

int main()
{
    Pegasus mPegasus(Brown, 100, 2);
    mPegasus.HorseFacts();
    mPegasus.BirdFacts();

    return 0;
}