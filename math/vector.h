// TODO:
// - subscript operator overloading: check bounds
// - destructor?

#ifndef VECTOR_H
#define VECTOR_H

#include <cstddef>
#include <initializer_list>

// explanation
//      - declares template that accepts two parameters
//          - T: type (e.g. int, double)
//          - N: dimension (e.g. 2D, 3D)
template <typename T, std::size_t N>
class Vector
{
public:
    // -- CONSTRUCTOR --
    // explanation
    //      - const: constructor doesn't modifiy the list
    //      - std::initializer_list<T>: represents a list of type T
    //          - same type as defined in the template<typename T,...>
    //      - &list: reference to a list (passed by reference, not by value)
    // using constructor
    //      - example: Vector<int, 2> = v{1, 2};
    Vector(const std::initializer_list<T> &list);

    // -- OPERATOR OVERLOADING --
    // explanation
    //      - overloading subscript operator -> []
    //          -allows access to elements of the vector using array-like notation
    // 1. overload
    //      - returns element at specific index + allows modification of the element
    //          - example: v[1] = 5;
    T &operator[](std::size_t index);
    // 2. overload
    //      - provides read-only access to the element -> prevent accidental modifications
    //      - example: const Vector<int, 3> v = {1, 2}; std::cout << v[1] << std::endl;
    const T &operator[](std::size_t index) const;

    // explanation
    //      - Vector<T, N>: returns a new Vector with the same template parameters
    //      - operator+/-: overloads + / - operator
    //      - const Vector<T, N> &other: reference to a constant vector
    //          - const: ensures the reference won't be modified
    //      - const: signals that the operator doesn't modify the state of the calling object
    Vector<T, N> operator+(const Vector<T, N> &other) const;
    Vector<T, N> operator-(const Vector<T, N> &other) const;

    // -- OPERATIONS --
    T dot(const Vector<T, N> &other) const;
    T magnitude() const;

private:
    // explanation
    //      - array to store vector elements
    T data[N];
};

#endif