#ifndef _NTT_H_
#define _NTT_H_

//#include <string.h>
#include <ctype.h>
#include <stdint.h>

#define DIGIT 3
#define X     1000
#define N (1 << 13)
#define L 5002
int a[N], b[N], c[N], temp[N];
char buffer[L];

#define P      ((27 << 26) + 1) /* Mathematica Table[PrimeQ[a*2^26+1],{a,1,32}] */
#define PR     136              /* See report */
#define PR_POW 26               /* pow(136, 2 << PR_POW) % P == 1 */
#define N_REV  1811939302       /* (N_REV * pow(2, PR_POW)) % P == 1, see report */


void solve();
size_t get_int(int *arr, char *buf);
size_t long_mul(int *result, int *num1, size_t sz1, int *num2, size_t sz2);
int get_bin_len(size_t sz);
void ifft(int *first, int *last, int factor, int prim_root, int mod);
void fft(int *first, int *last, int prim_root, int mod);
//void print(int *num, size_t sz);
void cross(int *first, size_t len, int *assist);
void reverse(int *first, int *last);
int fpow(long long base, size_t expo, int mod);
int square(long long x, int mod);
int multiply(long long x, long long y, int mod);
int add(long long x, long long y, int mod);

size_t get_int_poly(int *arr, uint8_t *poly, int n);
void get_hex_poly(int *arr, int szi, uint8_t *poly, int szh, int Q);

size_t get_s(int sz1, int sz2);
size_t get_pr(int s);
size_t get_ni(int s);
size_t get_pri(int s, int pr);



#endif