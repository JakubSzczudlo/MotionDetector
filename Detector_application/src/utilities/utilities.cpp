#include <stdint.h>
#include "utilities.hpp"

unsigned long long int add_elements_in_array(uint64_t array[], size_t size)
{
    unsigned long long int sum = 0;
    for(uint8_t i = 0; i < size; i++)
    {
        sum += array[i];
    }
    return sum;
}