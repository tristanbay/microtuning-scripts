/*
A simple script for finding the minimal "purely consistent" EDOs
in successive odd limits. This means that the equal temperament represents
all odd harmonics up to the specified limit consistently with less than
25 percent error. This was used to calculate some of the EDOs on the list on the
Xenharmonic Wiki's "Minimal consistent EDOs" page.

Written in June 2024 by Tristan Bay, public domain code
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

long double err(int a, long b)
{
	long double c = log((long double)a) / log(2.0) * b;
	return round(c) - c;
}

int main()
{
	int recordlimit = 1, counter;
	for (long i = 5; i < 2000000000000000000; ++i) {
		counter = 3;
		while (fabs(err(counter, i)) < 0.25) {
			if (counter > recordlimit) {
				recordlimit = counter;
				printf("%d:\t%ld\n", recordlimit, i);
			}
			counter += 2;
		}
	}
	return 0;
}
