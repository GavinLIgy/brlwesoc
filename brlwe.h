#ifndef _BRLWE_H_
#define _BRLWE_H_

#include <stdint.h>
#include "params.h" 
#include "ntt.h"
#include "alloc.h"

#if defined(hw_mul) && (hw_mul == 1)
uint32_t hard_mul(uint32_t a, uint32_t b);
uint32_t hard_mulh(uint32_t a, uint32_t b);
uint32_t hard_mulhsu(uint32_t a, uint32_t b);
uint32_t hard_mulhu(uint32_t a, uint32_t b);

#endif

#if (defined(RBINLWEENC1) && (RBINLWEENC1 == 1)) || (defined(RBINLWEENC2) && (RBINLWEENC2 == 1)) || (defined(RBINLWEENC3) && (RBINLWEENC3 == 1)) || (defined(RBINLWEENCT) && (RBINLWEENCT == 1))

typedef uint8_t *BRLWE_Ring_polynomials ;
typedef uint8_t *BRLWE_Ring_polynomials2 ;

BRLWE_Ring_polynomials BRLWE_init_bin_sampling(BRLWE_Ring_polynomials poly);//initialize a polynomial by sampling on uniform distribution with binary coefficients 
BRLWE_Ring_polynomials BRLWE_init_hex(BRLWE_Ring_polynomials poly, uint8_t* str, int rev);//initialize a polynomial by input hex in form of string.
BRLWE_Ring_polynomials BRLWE_init(BRLWE_Ring_polynomials poly);//initialize a polynomial with all 0.
BRLWE_Ring_polynomials2 BRLWE_Key_Gen(const BRLWE_Ring_polynomials a, BRLWE_Ring_polynomials2 key);
BRLWE_Ring_polynomials2 BRLWE_Encry(const BRLWE_Ring_polynomials a, const BRLWE_Ring_polynomials pk, uint8_t* m, BRLWE_Ring_polynomials2 cryptom );
uint8_t* BRLWE_Decry(const BRLWE_Ring_polynomials2 cryptom, const BRLWE_Ring_polynomials r2, uint8_t* recoverm);
uint8_t* BRLWE_Decode(uint8_t* recoverm);

BRLWE_Ring_polynomials Ring_add(const BRLWE_Ring_polynomials a, const BRLWE_Ring_polynomials b, BRLWE_Ring_polynomials ans);
BRLWE_Ring_polynomials Ring_sub(const BRLWE_Ring_polynomials a, const BRLWE_Ring_polynomials b, BRLWE_Ring_polynomials ans);
BRLWE_Ring_polynomials Ring_mul(const BRLWE_Ring_polynomials a, const BRLWE_Ring_polynomials b, BRLWE_Ring_polynomials ans);

#else
	
typedef uint16_t *BRLWE_Ring_polynomials ;
typedef uint16_t *BRLWE_Ring_polynomials2 ;

BRLWE_Ring_polynomials BRLWE_init_bin_sampling(BRLWE_Ring_polynomials poly);//initialize a polynomial by sampling on uniform distribution with binary coefficients 
BRLWE_Ring_polynomials BRLWE_init_hex(BRLWE_Ring_polynomials poly, uint16_t* str, int rev);//initialize a polynomial by input hex in form of string.
BRLWE_Ring_polynomials BRLWE_init(BRLWE_Ring_polynomials poly);//initialize a polynomial with all 0.
BRLWE_Ring_polynomials2 BRLWE_Key_Gen(const BRLWE_Ring_polynomials a, BRLWE_Ring_polynomials2 key);
BRLWE_Ring_polynomials2 BRLWE_Encry(const BRLWE_Ring_polynomials a, const BRLWE_Ring_polynomials pk, uint16_t* m, BRLWE_Ring_polynomials2 cryptom );
uint16_t* BRLWE_Decry(const BRLWE_Ring_polynomials2 cryptom, const BRLWE_Ring_polynomials r2, uint16_t* recoverm);
uint16_t* BRLWE_Decode(uint16_t* recoverm);

BRLWE_Ring_polynomials Ring_add(const BRLWE_Ring_polynomials a, const BRLWE_Ring_polynomials b, BRLWE_Ring_polynomials ans);
BRLWE_Ring_polynomials Ring_sub(const BRLWE_Ring_polynomials a, const BRLWE_Ring_polynomials b, BRLWE_Ring_polynomials ans);
BRLWE_Ring_polynomials Ring_mul(const BRLWE_Ring_polynomials a, const BRLWE_Ring_polynomials b, BRLWE_Ring_polynomials ans);

#endif
