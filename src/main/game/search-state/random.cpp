/* Copyright 2014 Jan Wolter */

#include <math.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include "random.h"

/* random.c is a translation of David Bau's Javascript seedrandom.js
 * version 2.0 function to C. The original is under BSD license, so I suppose
 * this can be too.
 * Translator: Jan Wolter 3-39-13
 *
 * Usage:
 *
 *   randseed('yipee');    Sets a seed
 *
 *   baurandom();	  returns the next random number as a floating point
 *   			  number between 0 and 1.
 *
 *   randint(n);	  return random number between 0 and n-1
 */

/* If unixrand is true, we use the standard Unix random number generator
 * instead of David Bau's. The baurandom() function does not work with this,
 * and the argument to randseed() must be a string represntation of an integer.
 */
int unixrand = 0;

/*
 * The following constants are related to IEEE 754 limits.
 */
#define CHUNKS 6	/* at least six RC4 outputs for each double */
#define STARTDENOM 281474976710656 /* WIDTH^CHUNKS */
#define SIGDIGIT 52	/* there are 52 significant digits in a double */
#define SIGNIF 4503599627370496 /* 2^SIGDIGIT */
#define ROVERFLOW (SIGNIF*2)


ARC4 *arc4;

/* lowbits()
 * A quick "n mod WIDTH" for WIDTH a power of 2.
 */
int lowbits(int n)
{
    return (n & (WIDTH - 1));
}


/* ARC4
 *
 * An ARC4 implementation.  makeARCr takes a key in the form of Key structure
 * containing an array of at most (WIDTH) integers that should be
 * 0 <= x < (WIDTH).
 *
 * The getnext(count) function returns a pseudorandom integer that concatenates
 * the next (count) outputs from ARC4.  Its return value is a number x
 * that is in the range 0 <= x < (WIDTH ^ count).
 */

/* The getnext (aka "g") function returns the next (count) outputs as
 * one number.
 */
long getnext(ARC4 *me, int count)
{
    int *s= me->S;
    int i= lowbits(me->i + 1);
    int t= s[i];
    int j= lowbits(me->j + t);
    int u= s[j];
    s[i]= u;
    s[j]= t;
    long r= s[lowbits(t + u)];
    while (--count)
    {
	i= lowbits(i + 1);
	t= s[i];
	j= lowbits(j + t);
	u= s[j];
	s[i]= u;
	s[j]= t;
	r= r * WIDTH + s[lowbits(t + u)];
    }
    me->i = i;
    me->j = j;
    return r;
}

ARC4 *makeARC4(Key *key)
{
    int i,t;
    ARC4 *me= (ARC4 *)malloc(sizeof(ARC4));
    me->i= me->j= me->m= 0;

    /* The empty key [] is treated as [0]. */
    if (key->n == 0) { key->n= 1; key->a[0]= 0; }

    /* Set up S using the standard key scheduling algorithm. */
    for (i= 0; i < WIDTH; i++) 
	me->S[i]= i;

    int j= 0;
    for (i= 0; i < WIDTH; i++)
    {
	t= me->S[i];
	j= lowbits(j + t + key->a[i % key->n]);
	me->S[i]= me->S[j];
	me->S[j]= t;
    }
  
    /* For robust unpredictability discard an initial batch of values.
     * See http://www.rsa.com/rsalabs/node.asp?id=2009 */
     getnext(me,WIDTH);

     return me;
}


/* mixkey()
 * Mixes a string seed into a key that is an array of WIDTH integers.
 */
void mixkey(char *seed, Key *key)
{
    int smear= 0;
    int j,i;
    key->n= 0;
    for (j= 0; j < (int) strlen(seed); j++)
    {
	i= lowbits(j);
	if (i >= key->n) key->a[i]= 0;
	key->a[i]= lowbits((smear ^= key->a[i]*19) + seed[j]);
	if (i >= key->n) key->n= i+1;
  }
}

/* randseed()
 *   Seed must always be a string.
 */
void randseed(char *seed)
{
	Key key;

	mixkey(seed,&key);

	arc4= makeARC4(&key);
}

/* This function returns a random double in [0, 1) that contains
 * randomness in every bit of the mantissa of the IEEE 754 value.
 */

double baurandom()
{
    long n= getnext(arc4,CHUNKS);       /* Start with a numerator n < 2 ^ 48 */
    double d= STARTDENOM;               /*   and denominator d = 2 ^ 48. */
    long long x= 0;                     /*   and no 'extra last byte'. */
    while (n < SIGNIF) {	        /* Fill up all significant digits by */
      n= (n + x) * WIDTH;               /*   shifting numerator and */
      d*= WIDTH;                        /*   denominator and generating a */
      x= getnext(arc4,1);               /*   new least-significant-byte. */
    }
    while (n >= ROVERFLOW) {            /* To avoid rounding up, before adding*/
      n/= 2;                            /*   last byte, shift everything */
      d/= 2;                            /*   right using integer math until */
      x= (x >> 1);                      /*   we have exactly the desired bits.*/
    }
    return (double)(n + x) / d;         /* Form the number within [0, 1). */
}

int randint(int n)
{
    if (unixrand)
	return (int)(random() % n);
    else
	return (int)(n*baurandom());
}

void set_seed(int seed) {
    std::string temp_str = std::to_string(seed);
    char* seed_str = new char[temp_str.length()];
    strcpy(seed_str, temp_str.c_str());
    randseed(seed_str);
}

int int_in_range_inclusive(int a, int b) {
    return randint(b-a+1) + a;
}
