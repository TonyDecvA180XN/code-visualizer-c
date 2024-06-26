/*
    This is a test file in C language indended to test all possible C features.
*/

// including system/user headers
#include <stdio.h>
#include "Math/Header.h"

// struct definition
struct Point
{
	double x, y; // multiple definitions on one line
};

// function declaration
void foo(int param1, float param2);

// function definition
int factorial(int v)
{
	if (v <= 1)
		return v;
	return v * factorial(v - 1); // recursion
}

int bar()
{
	// variables
	int i = 1;
	const float e = 2.71f;
	float sum;

	// structs
	struct Point a;
	a.x = 7.0;
	a.y = a.x * 2.0;

	// pointers
	int* p = &i;
	const int* ptc = p;
	void *const cp = &a;

	// arrays
	int numbers[3] = {1, 2, 3};
	char string[] = "Hello world";

	// branching operators
	if (i > 0)
	{
		sum = 2 * e * i;
	}
	else
		sum = 2 * (i + (int)(e));

	switch (i)
	{
	case 1:
		i++;
		break;
	default:
		break;
	}

	// loops
	for (int j = 0; j < 10; ++j)
	{
		sum += 0.5f;
	}

	while (i > 0)
	{
		int a = i + 4;
		if (i > a)
		{
			break;
		}
		i--;
	}

	do
	{
		i /= 2;
		if (sum <= e)
			continue;
	}
	while (i % 2 == 0);


	// function calls
	printf("%s", string);
	foo(i, /*6.26f*/ 1.23f /*3.27f*/);
    return abs(i);
}

// late function definition
void foo(int param1, float param2)
{
	param1 = param2 + 4;
}
