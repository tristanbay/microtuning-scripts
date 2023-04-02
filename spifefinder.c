// SPIFE finder code by Tristan Bay, March & April 2023, public domain

/* SPIFEs (superparticular prime-inclusive fully entangled numbers) are
rational numbers which only have consecutive prime numbers starting with 2 in
the prime factorizations of their numerator and denominator and have a numerator
that is exactly one greater than their denominator. These numbers have one or
more special properties in music as a ratio between two pitches, including
in regard to tempered tuning systems. */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_ZEROS 0 // maximum number of zeros allowed in monzos of results

_Bool divisible(long x, short* primes, short primes_length)
{
	for (short i = 0; i < primes_length; ++i) {
		while (x % *(primes + i) == 0)
			x /= *(primes + i);
	}
	if (x != 1)
		return false;
	return true;
}

short check_monzo(long x, short* primes, short primes_length)
{
	short total = 0;
	for (short i = 0; i < primes_length; ++i) {
		if (x % *(primes + i) != 0 && (x - 1) % *(primes + i) != 0)
			++total;
	}
	return total;
}

void show_monzo(long x, short* primes, short primes_length)
{
	short exp;
	long y = x - 1;
	printf("[");
	for (short i = 0; i < primes_length; ++i) {
		exp = 0;
		while (x % *(primes + i) == 0) {
			x /= *(primes + i);
			++exp;
		}
		while (y % *(primes + i) == 0) {
			y /= *(primes + i);
			--exp;
		}
		printf("%d", exp);
		if (i < primes_length - 1)
			printf(" ");
	}
	printf(">\n");
}

int main()
{
	short primes_length = 18; // change to match length of following arrays
	short primes[] = { // add or remove primes to check other subgroups/limits
		2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61
	};
		// highest numerator of any superparticular interval in the prime limit
	long search_limits[] = { // of the respective number in the previous array
		2, 9, 81, 4375, 9801, 123201, 336141, 11859211, 11859211, 177182721,
		1611308700, 3463200000, 63927525376, 421138799640, 1109496723126,
		1453579866025, 20628591204481, 31887350832897
	};
	short consecutive;
	for (short i = 1; i < primes_length; ++i) {
		printf("%d-limit:\n", *(primes + i));
		consecutive = 0;
		for (long j = 2; j <= *(search_limits + i); ++j) {
			if (divisible(j, primes, i + 1)) {
				++consecutive;
			} else {
				consecutive = 0;
			}
			if (consecutive == 2) {
				if (check_monzo(j, primes, i + 1) < 1 + MAX_ZEROS) {
					printf("\t%ld/%ld\t", j, j - 1);
					show_monzo(j, primes, i + 1);
				}
				--consecutive;
			}
		}
	}
	return 0;
}
