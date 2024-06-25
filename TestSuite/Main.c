// including headers
#include <stdio.h>

#include "Folder/Header.h"

// struct definition
struct Point
{
	double x, y; // inline comment
};

// recursion
int factorial(int v)
{
	if (v <= 1)
		return v;
	return v * factorial(v - 1);
}

void foo(int param1, float param2)
{
	param1 = param2 + 4;
}

/* Multiline one line comment */
int main(int argc, char** argv)
{
	// variables
	int var = 2;
	const float pi = 3.14f;
	float s;

	// branching
	if (var > 0)
	{
		s = 2 * pi * var;
	}
	else
	{
		var = 2 * (var + (int)(pi));
	}

	switch (var)
	{
	case 1:
		var++;
		break;
	default:
		break;
	}

	// loops
	for (int i = 0; i < 10; ++i)
	{
		var += 1;
	}

	while (var > 0)
	{
		int a = var + 4;
		if (var > a)
		{
			break;
		}
		var++;
	}

	do
	{
		var /= 2;
		if (s <= pi)
			continue;
		++var; /* some text
		Multiline comment
		some text */ --var;
	}
	while (var % 2 == 0);

	// structs
	struct Point a;
	a.x = 7.0;
	a.y = a.x * 2.0;

	// pointers
	int* p = &var;
	const int* ptc = p;
	void *const cp = &a;

	// arrays
	int numbers[3] = {1, 2, 3};
	char string[] = "Hello world";

	// lib and local function calls
	printf("%s", string);

	foo(var, /*6.26f*/ 1.23f /*3.27f*/);

	return 0;
}
