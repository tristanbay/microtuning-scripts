/* Small program for finding EDOs that are consistent in a certain
odd-prime-sum-limit (OPSL)

An OPSL is a set of rational numbers that only have (simplified) numerators and
denominators whose non-unique odd prime factors add up to a certain amount or
less. For example, 250/189 is in the 16-OPSL and above, but not the 15-OPSL or
below, because 189 = 3 * 3 * 3 * 7, and 3 + 3 + 3 + 7 = 16, and because
250 = 2 * 5 * 5 * 5, and 5 + 5 + 5 = 15.

Code by Tristan Bay | March and April 2023 | Public domain code
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct num_lll
{
    unsigned long num;
    struct num_lll* next;
};

unsigned long* odd_prime_list(unsigned long upper, unsigned long* amount)
{
    if (upper < 3) {
        *amount = 0;
        return NULL;
    }
    struct num_lll* head = calloc(1, sizeof(struct num_lll));
    (*head).num = 3;
    struct num_lll* check;
    unsigned long counter = 5, qty = 1;
    while (counter <= upper) {
        check = head;
        while (counter % (*check).num != 0) {
            if (!(*check).next) {
                (*check).next = calloc(1, sizeof(struct num_lll));
                (*(*check).next).num = counter;
                ++qty;
            }
            check = (*check).next;
        }
        counter += 2;
    }
    unsigned long* out = (unsigned long*)calloc(qty, sizeof(long));
    struct num_lll* clean_up;
    check = head;
    for (unsigned long i = 0; i < qty; ++i) {
        clean_up = check;
        *(out + i) = (*check).num;
        check = (*check).next;
        free(clean_up);
    }
    *amount = qty;
    return out;
}

void add_to(struct num_lll** curr, unsigned long value)
{
    (**curr).next = calloc(1, sizeof(struct num_lll));
    *curr = (**curr).next;
    (**curr).num = value;
}

void num_search(unsigned long upper, unsigned long* primes,
        unsigned long index, unsigned long* amount, unsigned long sum,
        unsigned long product, struct num_lll** curr)
{
    if (*(primes + index) + sum <= upper) {
        add_to(curr, *(primes + index) * product);
        ++(*amount);
        num_search(upper, primes, index, amount, *(primes + index) + sum,
            *(primes + index) * product, curr);
    }
    if (index > 0) {
        num_search(upper, primes, index - 1, amount, sum, product, curr);
    }
}

int compare(const void* x, const void* y)
{
    return *(unsigned long*)x - *(unsigned long*)y;
}

unsigned long* at_or_below_limit(unsigned long upper, unsigned long* primes,
        unsigned long number_of_primes, unsigned long* amount)
{
    struct num_lll* head = calloc(1, sizeof(struct num_lll)),
        * current = head;
    unsigned long qty = 1;
    (*head).num = 1;
    num_search(upper, primes, number_of_primes - 1, &qty, 0, 1, &current);
    unsigned long* out = (unsigned long*)calloc(qty, sizeof(long));
    struct num_lll* clean_up;
    current = head;
    for (unsigned long i = 0; i < qty; ++i) {
        clean_up = current;
        *(out + i) = (*current).num;
        current = (*current).next;
        free(clean_up);
    }
    qsort(out, qty, sizeof(long), compare);
    *amount = qty;
    return out;
}

unsigned long max(unsigned long x, unsigned long y)
{
	if (x > y)
		return x;
	return y;
}

unsigned long min_adjusted(unsigned long x, unsigned long y)
{
	unsigned long hi, lo;
	if (x > y) {
		hi = x; lo = y;
	} else {
		hi = y; lo = x;
	}
	while (hi >= lo * 2)
		lo *= 2;
	return lo;
}

void show_consistent_edos(unsigned long* harms, unsigned long harm_count,
        unsigned long max_edo)
{
    double sharp_error, flat_error, harm_error, extra;
	unsigned long sharpest_harm, flattest_harm;
    for (unsigned long i = 1; i <= max_edo; ++i) {
        sharp_error = 0; flat_error = 0;
		sharpest_harm = 1; flattest_harm = 1;
        for (unsigned long j = 1;
                j < harm_count && sharp_error + flat_error <= 0.5; ++j) {
            harm_error = modf(log(*(harms + j)) / log(2) * i, &extra);
            if (harm_error < 0.5) {
                if (harm_error > sharp_error) {
                    sharp_error = harm_error;
					sharpest_harm = *(harms + j);
                }
            } else {
                if (1 - harm_error > flat_error) {
                    flat_error = 1 - harm_error;
					flattest_harm = *(harms + j);
                }
            }
        }
        if (sharp_error + flat_error <= 0.5) {
            printf("%luedo\t%lf%% max error (at interval %lu/%lu)\n",
                i, (sharp_error + flat_error) * 100,
				max(sharpest_harm, flattest_harm),
				min_adjusted(sharpest_harm, flattest_harm));
        }
    }
}

int main()
{
    unsigned long limit, prime_count, use_in_fractions_count; 
    unsigned long* primes, * use_in_fractions;
    unsigned long max_edo;
    printf("OPSL to use: ");
    scanf("%lu", &limit);
    fflush(stdin);
    printf("\nSearch up to EDO: ");
    scanf("%lu", &max_edo);
    fflush(stdin);
    primes = odd_prime_list(limit, &prime_count);
    printf("\nOdd primes: ");
    if (primes) {
        for (unsigned long i = 0; i < prime_count - 1; ++i)
            printf("%lu, ", *(primes + i));
        printf("%lu\n", *(primes + prime_count - 1));
    } else {
        printf("None\n");
        return 0;
    }
    printf("\nNumerators/denominators: ");
    use_in_fractions = at_or_below_limit(limit, primes, prime_count,
        &use_in_fractions_count);
    for (unsigned long i = 0; i < use_in_fractions_count - 1; ++i)
        printf("%lu, ", *(use_in_fractions + i));
    printf("%lu\n", *(use_in_fractions + use_in_fractions_count - 1));
    printf("\nConsistent EDOs:\n");
    show_consistent_edos(use_in_fractions, use_in_fractions_count, max_edo);
	free(primes);
	free(use_in_fractions);
    return 0;
}
