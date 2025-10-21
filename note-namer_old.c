/* note-namer: A microtonal music tool by Tristan Bay
 * Provides a list of note names for a given equal division of the octave (EDO)
 * Ups and downs style notation, sometimes w/ half-accidentals, drops, and lifts
 * Written Sep 2024, public domain code
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

typedef struct Note
{
	int sharps, s_ups, s_lifts, s_nom; // sharp note name
	int flats, f_ups, f_lifts, f_nom; // flat note name
} Note;

void printnom(int nom)
{
	char c = ((nom + 2) % 7) + 65; // capital C through G, then wraps to A and B
	printf("%c", c);
}

void printupdown(int ups)
{
	if (ups < 0) {
		for (int i = 0; i > ups; --i)
			printf("v");
	} else {
		for (int i = 0; i < ups; ++i)
			printf("^");
	}
}

void printliftdrop(int lifts)
{
	if (lifts < 0) {
		for (int i = 0; i > lifts; --i)
			printf("\\");
	} else {
		for (int i = 0; i < lifts; ++i)
			printf("/");
	}
}

void printflat(int flats, _Bool half); // prototype for printsharp to reference

void printsharp(int sharps, _Bool half)
{
	if (sharps < 0) {
		printflat(sharps * -1, half);
	} else {
		if (half) {
			if (sharps % 2 != 0) {
				printf("‡");
				--sharps;
			}
			if (sharps % 4 != 0) {
				printf("#");
				sharps -= 2;
			}
			while (sharps > 0) {
				printf("x");
				sharps -= 4;
			}
		} else {
			if (sharps % 2 != 0) {
				printf("#");
				--sharps;
			}
			while (sharps > 0) {
				printf("x");
				sharps -= 2;
			}
		}
	}
}

void printflat(int flats, _Bool half)
{
	if (flats < 0) {
		printsharp(flats * -1, half);
	} else {
		if (half) {
			if (flats % 2 != 0) {
				printf("d");
				--flats;
			}
			for (int i = 0; i < flats; i += 2)
				printf("b");
		} else {
			for (int i = 0; i < flats; ++i)
				printf("b");
		}
	}
}

void printnote(Note note, _Bool halves)
{
	if (note.s_nom == note.f_nom) { // for natural notes, only print one name
		printnom(note.s_nom);
	} else {
		printliftdrop(note.s_lifts);
		printupdown(note.s_ups);
		printnom(note.s_nom);
		printsharp(note.sharps, halves);
		printf(", ");
		printliftdrop(note.f_lifts);
		printupdown(note.f_ups);
		printnom(note.f_nom);
		printflat(note.flats, halves);
	}
	printf("\n");
}

int fifth(int edo)
{
	return round(log(1.5) / log(2) * (float)edo);
}

int majsec(int edo, int p5)
{
	return (p5 * 2) - edo;
}

int apotome(int edo, int p5) // augmented unison, sharps and flats
{
	if ((edo % 7 == 0 || edo % 5 == 0) && edo < 36)
		return edo;
	return (p5 * 7) - (edo * 4);
}

int updown(int edo, int p5)
{
	if (edo < 66)
		return 1;
	if (edo == 129)
		return 3;
	int y3 = round(log(1.25) / log(2) * (float)edo);
	int sc = (p5 * 4) - (edo * 2) - y3;
	if (sc == 0)
		return 1;
	return sc;
}

_Bool verysharp(int edo, int p5) // for smaller 5n EDOs & for > 720-cent fifths
{
	if ((float)p5 / (float)edo > 0.6)
		return true;
	if (edo < 35 && edo % 5 == 0)
		return true;
	return false;
}

_Bool halfacc(int a1) // determining if there should be half-accidentals
{
	if (a1 % 2 == 0)
		return true;
	return false;
}

void basicnotes(Note notes[], int edo, int p5, int p2, _Bool penta)
{
	notes[0].s_nom = 0; // calculate natural notes one by one
	notes[p2].s_nom = 1;
	notes[edo - p5].s_nom = 3;
	notes[p5].s_nom = 4;
	notes[p5 + p2].s_nom = 5;
	notes[0].f_nom = 0;
	notes[p2].f_nom = 1;
	notes[edo - p5].f_nom = 3;
	notes[p5].f_nom = 4;
	notes[p5 + p2].f_nom = 5;
	if (!penta) {
		notes[2 * p2].s_nom = 2;
		notes[p5 + (2 * p2)].s_nom = 6;
		notes[2 * p2].f_nom = 2;
		notes[p5 + (2 * p2)].f_nom = 6;
	}
}

int trround(float x) // rounding but halfway is rounded towards 0 instead of up
{
	if (x - floor(x) == 0.5) {
		if (x > 0)
			return floor(x);
		if (x < 0)
			return ceil(x);
	}
	return round(x);
}

void setsharpcounts(int x, int ap, int ud, float* apc, float* udc, int* ldc,
		int* ra, int* ru)
{
	*apc = (float)x / (float)ap;
	*ra = trround(*apc);
	*udc = ((*ra * ap) - x) / (float)ud;
	*ru = trround(*udc);
	*ldc = (*ru * ud) - ((*ra * ap) - x);
	*ru *= -1;
}

void setsharpnotes(Note* note, int nom, int r_ap, int r_ud, int ldcount)
{
	note->s_nom = nom;
	note->sharps = r_ap;
	note->s_ups = r_ud;
	note->s_lifts = ldcount;
}

void sharpnotes(Note notes[], int edo, int p5, int p2, int ap, int ud,
		_Bool penta)
{
	float apcount = 0, udcount = 0;
	int r_ap = 0, r_ud = 0, ldcount = 0;
	for (int i = 1; i < p2; ++i) {
		setsharpcounts(i, ap, ud, &apcount, &udcount, &ldcount, &r_ap, &r_ud);
		setsharpnotes(&notes[i], 0, r_ap, r_ud, ldcount);
		setsharpnotes(&notes[edo - p5 + i], 3, r_ap, r_ud, ldcount);
		setsharpnotes(&notes[p5 + i], 4, r_ap, r_ud, ldcount);
	}
	if (penta) {
		for (int i = 1; i < (edo - p5) - p2; ++i) {
			setsharpcounts(i, ap, ud, &apcount, &udcount, &ldcount, &r_ap,
				&r_ud);
			setsharpnotes(&notes[p2 + i], 1, r_ap, r_ud, ldcount);
			setsharpnotes(&notes[p5 + p2 + i], 5, r_ap, r_ud, ldcount);
		}
	} else {
		for (int i = 1; i < p2; ++i) {
			setsharpcounts(i, ap, ud, &apcount, &udcount, &ldcount, &r_ap,
				&r_ud);
			setsharpnotes(&notes[p2 + i], 1, r_ap, r_ud, ldcount);
			setsharpnotes(&notes[p5 + p2 + i], 5, r_ap, r_ud, ldcount);
		}
		for (int i = 1; i < (edo - p5) - (p2 * 2); ++i) {
			setsharpcounts(i, ap, ud, &apcount, &udcount, &ldcount, &r_ap,
				&r_ud);
			setsharpnotes(&notes[(p2 * 2) + i], 2, r_ap, r_ud, ldcount);
			setsharpnotes(&notes[p5 + (p2 * 2) + i], 6, r_ap, r_ud, ldcount);
		}
	}
}

void setflatcounts(int x, int ap, int ud, float* apc, float* udc, int* ldc,
		int* ra, int* ru)
{
	*apc = (float)x / (float)ap;
	*ra = trround(*apc);
	*udc = ((*ra * ap) - x) / (float)ud;
	*ru = trround(*udc);
	*ldc = ((*ra * ap) - x) - (*ru * ud);
}

void setflatnotes(Note* note, int nom, int r_ap, int r_ud, int ldcount)
{
	note->f_nom = nom;
	note->flats = r_ap;
	note->f_ups = r_ud;
	note->f_lifts = ldcount;
}

void flatnotes(Note notes[], int edo, int p5, int p2, int ap, int ud,
		_Bool penta)
{
	float apcount = 0, udcount = 0;
	int r_ap = 0, r_ud = 0, ldcount = 0;
	for (int i = 1; i < p2; ++i) {
		setflatcounts(i, ap, ud, &apcount, &udcount, &ldcount, &r_ap, &r_ud);
		setflatnotes(&notes[p2 - i], 1, r_ap, r_ud, ldcount);
		setflatnotes(&notes[p5 - i], 4, r_ap, r_ud, ldcount);
		setflatnotes(&notes[p5 + p2 - i], 5, r_ap, r_ud, ldcount);
	}
	if (penta) {
		for (int i = 1; i < (edo - p5) - p2; ++i) {
			setflatcounts(i, ap, ud, &apcount, &udcount, &ldcount, &r_ap,
				&r_ud);
			setflatnotes(&notes[edo - i], 0, r_ap, r_ud, ldcount);
			setflatnotes(&notes[edo - p5 - i], 3, r_ap, r_ud, ldcount);
		}
	} else {
		for (int i = 1; i < (edo - p5) - (p2 * 2); ++i) {
			setflatcounts(i, ap, ud, &apcount, &udcount, &ldcount, &r_ap,
				&r_ud);
			setflatnotes(&notes[edo - i], 0, r_ap, r_ud, ldcount);
			setflatnotes(&notes[edo - p5 - i], 3, r_ap, r_ud, ldcount);
		}
		for (int i = 1; i < p2; ++i) {
			setflatcounts(i, ap, ud, &apcount, &udcount, &ldcount, &r_ap,
				&r_ud);
			setflatnotes(&notes[(p2 * 2) - i], 2, r_ap, r_ud, ldcount);
			setflatnotes(&notes[p5 + (p2 * 2) - i], 6, r_ap, r_ud, ldcount);
		}
	}
}

int main(int argc, char** argv)
{
	if (argc != 2) {
		printf("Usage: \"./note-namer [EDO]\"\n");
		return 1;
	}
	int edo;
	sscanf(argv[1], "%d", &edo);
	int p5 = fifth(edo);
	int p2 = majsec(edo, p5);
	int a1 = apotome(edo, p5);
	int ud = updown(edo, p5);
	_Bool penta = verysharp(edo, p5);
	_Bool halves = halfacc(a1);
	if (halves) // use half of augmented unison instead of true a1 if possible
		a1 /= 2;
	Note notes[edo];
	for (int i = 0; i < edo; ++i) {
		notes[i].sharps = 0;
		notes[i].s_ups = 0;
		notes[i].s_lifts = 0;
		notes[i].s_nom = 0;
		notes[i].flats = 0;
		notes[i].f_ups = 0;
		notes[i].f_lifts = 0;
		notes[i].f_nom = 0;
	}
	if (edo < 7 && edo != 5) {
		printf("EDO too small or subset of 12-equal.\n");	
	} else {
		basicnotes(notes, edo, p5, p2, penta);
		sharpnotes(notes, edo, p5, p2, a1, ud, penta);
		flatnotes(notes, edo, p5, p2, a1, ud, penta);
		for (int i = 0; i < edo; ++i)
			printnote(notes[i], halves);
	}
	return 0;
}
