#include "Header.h"

/*
    This is an example math library source file.
    There are functin definitions.
*/

double min(double a, double b)
{
    if (a < b)
        return a;
    else
        return b;
}

int abs(int value)
{
    if (value < 0)
        return -value;
    return value;
}

double average(double x, double y, double z)
{
    return (x + y + z) / 3.0;
}

double circle_area(double radius)
{
    return PI * radius * radius;
}
