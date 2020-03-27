#ifndef _BRLWE_H_
#define _BRLWE_H_

#include <stdint.h>
#include "ntt.h"
//#include "ntt.c"

//#define RBINLWEENC1 1
#define RBINLWEENC2 1
//#define RBINLWEENC3 1
//#define RBINLWEENCT 1

//#define hw_mul 1

#if defined(RBINLWEENC1) && (RBINLWEENC1 == 1)
#define BRLWE_N 256 // n = 256 : polynomials length
#define BRLWE_Q 128 // q = 128 : log2(q) = coeffidences data length; causing 1 bit of each byte wasted when q = 128
#elif defined(RBINLWEENC2) && (RBINLWEENC2 == 1)
#define BRLWE_N 256
#define BRLWE_Q 256
#elif defined(RBINLWEENCT) && (RBINLWEENCT == 1)
#define BRLWE_N 4
#define BRLWE_Q 256
#else
#define BRLWE_N 512
#define BRLWE_Q 256
#endif

typedef uint8_t *BRLWE_Ring_polynomials ;
typedef uint8_t *BRLWE_Ring_polynomials2 ;

#if defined(hw_mul) && (hw_mul == 1)
uint32_t hard_mul(uint32_t a, uint32_t b);
uint32_t hard_mulh(uint32_t a, uint32_t b);
uint32_t hard_mulhsu(uint32_t a, uint32_t b);
uint32_t hard_mulhu(uint32_t a, uint32_t b);
#endif

BRLWE_Ring_polynomials BRLWE_init_bin_sampling(BRLWE_Ring_polynomials poly);//initialize a polynomial by sampling on uniform distribution with binary coefficients 
BRLWE_Ring_polynomials BRLWE_init_hex(BRLWE_Ring_polynomials poly, uint8_t* str, int rev);//initialize a polynomial by input hex in form of string.
BRLWE_Ring_polynomials BRLWE_init(BRLWE_Ring_polynomials poly);//initialize a polynomial with all 0.
BRLWE_Ring_polynomials2 BRLWE_Key_Gen(const BRLWE_Ring_polynomials a, BRLWE_Ring_polynomials2 key);
BRLWE_Ring_polynomials2 BRLWE_Encry(const BRLWE_Ring_polynomials a, const BRLWE_Ring_polynomials pk, uint8_t* m, BRLWE_Ring_polynomials2 cryptom );
uint8_t* BRLWE_Decry(const BRLWE_Ring_polynomials2 cryptom, const BRLWE_Ring_polynomials r2, uint8_t* recoverm);
uint8_t* BRLWE_Decode(uint8_t* recoverm);

BRLWE_Ring_polynomials Ring_add(const BRLWE_Ring_polynomials a, const BRLWE_Ring_polynomials b, BRLWE_Ring_polynomials ans);
BRLWE_Ring_polynomials Ring_sub(const BRLWE_Ring_polynomials a, const BRLWE_Ring_polynomials b, BRLWE_Ring_polynomials ans);
BRLWE_Ring_polynomials Simple_Ring_mul(const BRLWE_Ring_polynomials a, const BRLWE_Ring_polynomials b, BRLWE_Ring_polynomials ans);
BRLWE_Ring_polynomials Simple_Ring_mul_NTT(const BRLWE_Ring_polynomials a, const BRLWE_Ring_polynomials b, BRLWE_Ring_polynomials ans);

#endif
