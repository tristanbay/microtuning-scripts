/* Reversed intervals script by Tristan Bay:
 * Finds the largest negatively-mapped superparticular interval
 * in the patent val of a given EDO
 * Written October 2024 and October 2025, public domain code
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define PRIMECOUNT 6542
#define SIEVEBOUND 65536.0

void sieve(int* primes, int len)
{
	int testlimit = (int)floor(sqrt(SIEVEBOUND)) - 2;
	int nums[(int)SIEVEBOUND - 2];
	for (int i = 0; i < (int)SIEVEBOUND - 2; ++i)
		nums[i] = i + 2;
	for (int i = 2; i < testlimit; ++i) {
		for (int j = (i * 2) - 2; j < (int)SIEVEBOUND - 2; j += i)
			if (nums[j])
				nums[j] = 0;
	}
	int counter = 0;
	for (int i = 0; i < (int)SIEVEBOUND - 2 && counter < len; ++i) {
		if (nums[i]) {
			primes[counter] = nums[i];
			++counter;
		}
	}
}

void generate_val(int* val, int* primes, int len, int edo)
{
	for (int i = 0; i < len; ++i)
		val[i] = (int)round(log((double)primes[i]) / log(2.0) * edo);
}

void generate_harmonic_mappings(int* hm, int* val, int* primes, int len)
{
	int to_factor = 1; // current number to factor
	hm[0] = 0;
	for (int i = 1; i < len; ++i) { // each harmonic to be mapped
		to_factor = i + 1;
		hm[i] = 0;
		for (int j = 0; to_factor > 1; ++j) { // check for prime factors
			while (to_factor % primes[j] == 0) {
				to_factor /= primes[j];
				hm[i] += val[j];
			}
		}
	}
}

int first_reversal(int* hm, int len)
{
	for (int i = 1; i < len; ++i)
		if (hm[i] < hm[i - 1])
			return i;	// the denominator
	return 0; // if no reversals found
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
	generate_val(val, primes, PRIMECOUNT, edo);
	int harmonic_mappings[(int)SIEVEBOUND]; // add 1 to index to get matching harmonic
	generate_harmonic_mappings(harmonic_mappings, val, primes, (int)SIEVEBOUND);
	int f_reversal = first_reversal(harmonic_mappings, (int)SIEVEBOUND);
	if (!f_reversal) {
		printf("No superparticular reversals found\n");
		return EXIT_FAILURE;
	}
	printf("First superparticular reversal for %dedo is at %5d/%d\n",
		edo, f_reversal + 1, f_reversal);
	return EXIT_SUCCESS;
}
