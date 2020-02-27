#ifndef _BRLWE_H_
#define _BRLWE_H_

#include <stdint.h>

//#define RBINLWEENC1 1
#define RBINLWEENC2 1
//#define RBINLWEENC3 1
//#define RBINLWEENCT 1

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

#pragma pack(1)
struct BRLWE_Ring_polynomials
{
	uint8_t polynomial[BRLWE_N];
};
#pragma pack()

#pragma pack(1)
struct BRLWE_Ring_polynomials2
{
	struct BRLWE_Ring_polynomials c1;
	struct BRLWE_Ring_polynomials c2;
};

#pragma pack()
struct BRLWE_Ring_polynomials* BRLWE_init_bin_sampling(struct BRLWE_Ring_polynomials* poly);//initialize a polynomial by sampling on uniform distribution with binary coefficients 
struct BRLWE_Ring_polynomials * BRLWE_init_hex(struct BRLWE_Ring_polynomials * poly, uint8_t* str, int rev);//initialize a polynomial by input hex in form of string.
struct BRLWE_Ring_polynomials * BRLWE_init(struct BRLWE_Ring_polynomials * poly);//initialize a polynomial with all 0.
struct BRLWE_Ring_polynomials2 BRLWE_Key_Gen(const struct BRLWE_Ring_polynomials a);
struct BRLWE_Ring_polynomials2 BRLWE_Encry(const struct BRLWE_Ring_polynomials* a, const struct BRLWE_Ring_polynomials* p, uint8_t* m);
uint8_t* BRLWE_Decry(struct BRLWE_Ring_polynomials2* c, struct BRLWE_Ring_polynomials* r2);
void BRLWE_Encode(uint8_t* m, struct BRLWE_Ring_polynomials* m_wave);
uint8_t* BRLWE_Decode(struct BRLWE_Ring_polynomials m_wave);
//pass b.polynomial to a.polynomial
void passpoly (struct BRLWE_Ring_polynomials a, struct BRLWE_Ring_polynomials b);


struct BRLWE_Ring_polynomials* Ring_add(const struct BRLWE_Ring_polynomials* a, const struct BRLWE_Ring_polynomials* b, struct BRLWE_Ring_polynomials* ans);
struct BRLWE_Ring_polynomials* Ring_sub(const struct BRLWE_Ring_polynomials* a, const struct BRLWE_Ring_polynomials* b, struct BRLWE_Ring_polynomials* ans);
struct BRLWE_Ring_polynomials* Simple_Ring_mul(const struct BRLWE_Ring_polynomials* a, const struct BRLWE_Ring_polynomials* b, struct BRLWE_Ring_polynomials* ans);

#endif
