#ifndef _NTT_H_
#define _NTT_H_

#include <stdint.h>
#include "params.h" 


#if (BRLWE_N == 256)
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