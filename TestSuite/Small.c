// This is a sample file in C language

#include "Math/Header.h"

#include <stdio.h>

int main()
{
    double r;
    scanf_s("%f", r);

    if (abs((int)(r)) != r) // negative radius
    {
        printf("Invalid input");
        return -1;
    }

    const double a = circle_area(r);
    const double treshold = 42.3;

    printf("Lowest is %f", min(a, treshold));

    return 0;
}
