#include "vector.h"

template <typename T, std::size_t N>
Vector<T, N>::Vector(const std::initializer_list<T> &list)
{
    // check if list size & vector dimensions are equal
    if (list.size() != N)
    {
        throw std::length_error("List size must match vector dimension");
    }

    // fill the array with the list elements
    std::size_t i = 0;
    for (const T &element : list)
    {
        data[i++] = element;
    }
}

template <typename T, std::size_t N>
T &Vector<T, N>::operator[](std::size_t index)
{
    // check for out of bounds indexes
    if (index < 0 || index >= N)
    {
        throw std::out_of_range("Index out of bounds");
    }

    return data[index];
}

template <typename T, std::size_t N>
const T &Vector<T, N>::operator[](std::size_t index) const
{
    // check for out of bounds indexes
    if (index < 0 || index >= N)
    {
        throw std::out_of_range("Index out of bounds");
    }

    return data[index];
}