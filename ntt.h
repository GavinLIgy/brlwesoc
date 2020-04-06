#ifndef _NTT_H_
#define _NTT_H_

#include <stdint.h>
#include "params.h" 

//#define My_NTT 1
#define PtNTT 1

uint16_t montgomery_reduce(uint32_t a);
static uint16_t coeff_freeze(uint16_t x);

#if (BRLWE_N == 128)

#if defined(My_NTT) && (My_NTT == 1)
/*
void get_int16_half_polys(uint16_t *arr, uint8_t *poly);
void bitrev_vector_256(uint16_t* poly);
void poly_ntt(uint16_t *r);
void poly_invntt(uint16_t *r);
void poly_mul_pointwise(uint16_t *r, const uint16_t *a, const uint16_t *b);
void poly_add2(uint16_t *r, const uint16_t *a, const uint16_t *b);
void poly_sub3(uint16_t *r, const uint16_t *a, const uint16_t *b, uint16_t *c);
*/

#elif defined(PtNTT) && (PtNTT == 1)

struct ptpoly4 {
	uint16_t* poly00;
	uint16_t* poly01;
	uint16_t* poly10;
	uint16_t* poly11;
};

struct ptpoly7 {
	struct ptpoly4 poly4;
	uint16_t* poly01_s;
	uint16_t* poly10_s;
	uint16_t* poly11_s;
};

void bitrev_vector_64(uint16_t* poly);
void get_int16_polys(uint16_t *arr, uint16_t *poly);
void mul_coefficients_64(uint16_t* poly, const uint16_t* factors);
void ntt_64(uint16_t * a, const uint16_t* omega);
void poly_quarter_mul_pointwise(uint16_t *r, const uint16_t *a, const uint16_t *b);
void poly_add(uint16_t *r, const uint16_t *a, const uint16_t *b);
void poly_quarter_add(uint16_t *r, const uint16_t *a, const uint16_t *b, const uint16_t *c, const uint16_t *d);
void poly_sub(uint16_t *r, const uint16_t *a, const uint16_t *b);
int poly_equal(uint16_t *f, uint16_t *g);
int split_poly(uint16_t *f, struct ptpoly4 poly);
int shift_poly(uint16_t *f, uint16_t *g);
int recover_poly(uint16_t *f, struct ptpoly4 poly);
void poly_ntt_64(uint16_t *r);
void poly_invntt_64(uint16_t *r);
void poly_pt_ntt4(uint16_t *p, struct ptpoly4 poly);
void poly_pt_ntt7(uint16_t *p, struct ptpoly7 poly);
void pt_ntt_bowtiemultiply(uint16_t *b, struct ptpoly4 f, struct ptpoly7 g);
void poly_inv_ptntt(uint16_t *b);

#endif

#elif (BRLWE_N == 256)
void get_int16_half_polys(uint16_t *arr, uint8_t *poly);
void bitrev_vector_256(uint16_t* poly);
void poly_ntt(uint16_t *r);
void poly_invntt(uint16_t *r);
void poly_mul_pointwise(uint16_t *r, const uint16_t *a, const uint16_t *b);
void poly_add2(uint16_t *r, const uint16_t *a, const uint16_t *b);
void poly_sub3(uint16_t *r, const uint16_t *a, const uint16_t *b, uint16_t *c);

#elif (BRLWE_N == 512)



#elif (BRLWE_N == 4)

#endif

#endif