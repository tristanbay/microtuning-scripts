/*
Breaks a rational interval in the form a/b into its prime factorization.

Numbers in "[>" are the powers of the prime factors
and the numbers in "()" are the powers' respective primes.

Written in 2023 or 2024 by Tristan Bay, public domain code
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int max(int a, int b)
{
	return (a > b) ? a : b;
}

int min(int a, int b)
{
	return (a > b) ? b : a;
}

void simplify(int* n, int* d)
{
	int a = max(*n, *d), b = min(*n, *d), gcd;
	while (b != 0) {
		gcd = a % b;
		a = b;
		b = gcd;
	}
	gcd = a;
	*n /= gcd;
	*d /= gcd;
}

int count_factors(int n, int d)
{
	long t = (long)n * d;
	_Bool divisible = false;
	int count = 0;
	for (long i = 2; i > 0 && t != 1; i += 2) {
		while (t % i == 0) {
			divisible = true;
			t /= i;
		}
		if (divisible) {
			++count;
			divisible = false;
		}
		if (i == 2)
			--i;
	}
	return count;
}

void calc_monzo(int* factors, int* exponents, int f_count, int n, int d)
{
	int count = 0;
	for (int i = 2; n * d != 1 && i > 0; i += 2) {
		if (n % i == 0) {
			*(factors + count) = i;
			*(exponents + count) = 0;
			while (n % i == 0) {
				n /= i;
				++(*(exponents + count));
			}
			++count;
		} else if (d % i == 0) {
			*(factors + count) = i;
			*(exponents + count) = 0;
			while (d % i == 0) {
				d /= i;
				--(*(exponents + count));
			}
			++count;
		}
		if (i == 2)
			--i;
	}
}

int main(int argc, char** argv)
{
	if (argc == 1) {
		printf("Monzocalc: a CLI tool for calculating interval monzos\n");
		printf("by Tristan Bay\n");
	}
	if (argc != 2) {
		printf("Format: monzocalc numerator/denominator\n");
		return 1;
	}
	int n, d;
	sscanf(*(argv + 1), "%d/%d", &n, &d);
	simplify(&n, &d);
	int f_count = count_factors(n, d);
	int factors[f_count], exponents[f_count];
	calc_monzo(factors, exponents, f_count, n, d);
	printf("[");
	for (int i = 0; i < f_count; ++i) {
		printf("%d", *(exponents + i));
		if (i != f_count - 1)
			printf(" ");
	}
	printf("> (");
	for (int i = 0; i < f_count; ++i) {
		printf("%d", *(factors + i));
		if (i != f_count - 1)
			printf(".");
	}
	printf(")\n");
	return 0;
}
