/* Reversed intervals script by Tristan Bay:
 * Finds the largest negatively-mapped superparticular interval in a given EDO
 * Written October 2024, public domain code
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#define PRIMECOUNT 173
#define SIEVEBOUND 1024.0

void sieve(int* primes, int len)
{
	int testlimit = (int)floor(sqrt(SIEVEBOUND)) - 2;
	int nums[(int)floor(SIEVEBOUND) - 2];
	for (int i = 0; i < SIEVEBOUND - 2; ++i)
		nums[i] = i + 2;
	for (int i = 2; i < testlimit; ++i) {
		for (int j = (i * 2) - 2; j < SIEVEBOUND - 2; j += i)
			if (nums[j])
				nums[j] = 0;
	}
	int counter = 0;
	for (int i = 0; i < SIEVEBOUND - 2 && counter < len; ++i) {
		if (nums[i]) {
			primes[counter] = nums[i];
			++counter;
		}
	}
}

void generateval(int* val, int* primes, int len, int edo)
{
	for (int i = 0; i < len; ++i)
		val[i] = (int)round(log((double)primes[i]) / log(2.0) * edo);
}

int main(int argc, char** argv)
{
	if (argc != 2) {
		printf("Usage: ./reversed-intervals [EDO]\n");
		return 1;
	}
	int edo;
	sscanf(argv[1], "%d", &edo);
	int primes[PRIMECOUNT];
	sieve(primes, PRIMECOUNT);
	int val[PRIMECOUNT];
	generateval(val, primes, PRIMECOUNT, edo);
	return 0;
}
