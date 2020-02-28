/*
Introduction:
BRLWE is a light-weighted noval PQC algorithm based on RLWE which supersede the Gaussian noise with Binary noise.
BRLWE scheme consists of three main phases: key generation, encryption, and decryption.


*/


/*****************************************************************************/
/* Includes:                                                                 */
/*****************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include "brlwe.h"
//#include "alloc.h"
//#include "alloc.c"

/*****************************************************************************/
/* Definition:                                                        */
/*****************************************************************************/
#define RNG_seed	 setseed32
#define RNG_rand	 getrandom_binary
//void setseed32(uint32_t seed);
//void getrandom_binary(uint8_t* str);

//typedef uint8_t *BRLWE_Ring_polynomials ;
//typedef uint8_t *BRLWE_Ring_polynomials2 ;
/*****************************************************************************/
/* Private functions:                                                        */
/*****************************************************************************/

//initialize a polynomial by sampling a uniform distribution with binary coefficients 
BRLWE_Ring_polynomials BRLWE_init_bin_sampling(BRLWE_Ring_polynomials poly) {
	int i = 0;
	int j = 0;
	
	uint32_t cycles_now;
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_now));
	RNG_seed(cycles_now);
	
	uint8_t* str = NULL;
	str = m_malloc(4);
	for (i = 0; i < BRLWE_N/4 ; i++) {
		RNG_rand(str);
		for (j = 0; j < 4 ; j++){
			*(poly+4*i+j) = (uint8_t)str[j];
		};
	};
	m_free(str);
	
	return poly;
	
	/*
	//C program on PC
	time_t t;
	int i = 0;
	int r = 0;
	srand((unsigned)time(&t));
	for (int i = 0; i < BRLWE_N; i++) {
		r = rand() % 2;//more powerful RNG required.
		poly->polynomial[i] = (uint8_t)r;
	};*/
};

//initialize a polynomial by input hex in form of string.
//pre-requirement: length(str) = BRLWE_N, rev = {0,1}^1
//rev = 1: str[n] = poly[0]; else: str[0] = poly[0]
BRLWE_Ring_polynomials BRLWE_init_hex(BRLWE_Ring_polynomials poly, uint8_t* str, int rev) {
	if (rev == 1) {
		for (int i = 0; i < BRLWE_N; i++)
			*(poly+i) = (uint8_t)(str[BRLWE_N - 1 - i] % BRLWE_Q);
	}
	else {
		for (int i = 0; i < BRLWE_N; i++)
			*(poly+i) = (uint8_t)(str[i]% BRLWE_Q);
	}; 
	return poly;
};

//initialize a polynomial with all 0.
BRLWE_Ring_polynomials BRLWE_init(BRLWE_Ring_polynomials poly) {
	for (int i = 0; i < BRLWE_N; i++)
		poly[i] = (uint8_t)0x00;
	return poly;
};

//Main Function 1: Key Generation
//a is a global parameter shared by Alice and Bob
// r1 and r2 are randomly selected binary polynomials, r2 is secret key
// p = r1 - a * r2, p is public key and would be sent to Bob after Key_Gen
BRLWE_Ring_polynomials2 BRLWE_Key_Gen(const BRLWE_Ring_polynomials a, BRLWE_Ring_polynomials2 key) {
	BRLWE_Ring_polynomials pk = key;//public key
	BRLWE_Ring_polynomials sk = key+BRLWE_N;//secret key
	
	sk = BRLWE_init_bin_sampling(sk);
	passpoly (pk , Simple_Ring_mul(a, sk, pk));
	
	int i = 0;
	int j = 0;
	
	uint32_t cycles_now;
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_now));
	RNG_seed(cycles_now);
	
	uint8_t* str = NULL;
	str = m_malloc(4);//random number buffer: uint8_t str [4]
	for (i = 0; i < BRLWE_N/4 ; i++) {
		RNG_rand(str);
		for (j = 0; j < 4 ; j++){
			*(pk+4*i+j) = (uint8_t)str[j] - *(pk+4*i+j);
		};
	};
	m_free(str);

	return key;
};

// //Main Function 2: Encryption
// //pre-requirement: length(m) = n, m belongs to {0,1}^n;
// //a is a global parameter shared by Alice and Bob, p is public key and would be sent to Bob after Key_Gen, m is the message to be crypto
// //After receiving p, Bob uses 3 error(binary) polynomials e1, e2, e3
// //m_wave = encode(m), c1 = a*e1 +e2, c2 = p*e1 + e3 + m_wave
// //[c1,c2] belonging to R_q^2 are cipertext
// struct BRLWE_Ring_polynomials2* BRLWE_Encry(const struct BRLWE_Ring_polynomials* a, const struct BRLWE_Ring_polynomials* p, uint8_t* m, struct BRLWE_Ring_polynomials2* cryptom ) {
	// struct BRLWE_Ring_polynomials* e1, e2, e3, m_wave = NULL;
	
	// BRLWE_Encode(m, &(cryptom->poly2));//m_wave = * cryptom->poly2
	
	// Ring_add(e3, )
	
	
	// m_malloc(BRLWE_N);
	
	// /*
	// uint8_t _e1[BRLWE_N] = {(uint8_t) 1, (uint8_t) 1, (uint8_t) 1 , (uint8_t) 0 };
	// uint8_t _e2[BRLWE_N] = {(uint8_t) 0, (uint8_t) 1, (uint8_t) 1 , (uint8_t) 1 };
	// uint8_t _e3[BRLWE_N] = {(uint8_t) 1, (uint8_t) 0, (uint8_t) 1 , (uint8_t) 0 };
	// BRLWE_init_hex(&e1, _e1, 0);
	// BRLWE_init_hex(&e2, _e2, 0);
	// BRLWE_init_hex(&e3, _e3, 0);*/
	
	// /*BRLWE_init_bin_sampling(&e1);
	// BRLWE_init_bin_sampling(&e2);
	// BRLWE_init_bin_sampling(&e3);*/
	// BRLWE_init(&c.c1);
	// BRLWE_init(&c.c2);
	// BRLWE_Encode(m, &m_wave);
	// c.c1 = Ring_add(Simple_Ring_mul(*a, e1), e2);
	// c.c2 = Ring_add(Ring_add(Simple_Ring_mul(*p, e1), e3), m_wave);
	// return cryptom;
// };

// //Main Function 3: Decryption
// //output m' = Decode(c1*r2+c2)
// //r2 is secret key
// uint8_t* BRLWE_Decry(struct BRLWE_Ring_polynomials2* c, struct BRLWE_Ring_polynomials* r2) {
	// return BRLWE_Decode(Ring_add(Simple_Ring_mul(c->c1, *r2), c->c2));
// };

// //Encode function 2: FPT'19
// //Encode string m into polynomial m_wave
// //pre-requirement: length(m) = n, m belongs to {0,1}^n;;
// struct BRLWE_Ring_polynomials* BRLWE_Encode(uint8_t* m, struct BRLWE_Ring_polynomials* m_wave) {
	// int i = 0;
	// for (i = 0; i < BRLWE_N; i++) {
		// if (m[i] == 0)
			// m_wave->polynomial[i] = 0;
		// else
			// m_wave->polynomial[i] = (uint8_t)(BRLWE_Q / 2);
		// m_wave->polynomial[i] += BRLWE_Q + (BRLWE_N / 2) - 1 - i;
	// };
	// return m_wave;
// };

// //Decode function 2: FPT'19
// //Decode polynomial m_wave into string m
// uint8_t* BRLWE_Decode(struct BRLWE_Ring_polynomials m_wave) {
	// uint8_t* m;
	// int i = 0;
	// for (i = 0; i < BRLWE_N; i++) {
		// if (m_wave.polynomial[i] > (BRLWE_Q / 4) && m_wave.polynomial[i] < (3 * BRLWE_Q / 4))
			// m[i] = (uint8_t)1;
		// else
			// m[i] = (uint8_t)0;
	// };
	// return m;
// };

//return value ans = a + b;
BRLWE_Ring_polynomials Ring_add(const BRLWE_Ring_polynomials a, const BRLWE_Ring_polynomials b, BRLWE_Ring_polynomials ans) {
	int i = 0;
	for (i = 0; i < BRLWE_N; i++)
		ans[i] = (a[i] + b[i]) % BRLWE_Q;
	return ans;
};

//return value = a - b;
BRLWE_Ring_polynomials Ring_sub(const BRLWE_Ring_polynomials a, const BRLWE_Ring_polynomials b, BRLWE_Ring_polynomials ans) {
	int i = 0;
	for (i = 0; i < BRLWE_N; i++) 
		ans[i] = (a[i] - b[i]) % BRLWE_Q;
	return ans;
};

//return value = a * b; b is with binary coefficiences
//i=0 is lsb, i=n-1 is msb
//most time/area consuming function(by guess) 
BRLWE_Ring_polynomials Simple_Ring_mul(const BRLWE_Ring_polynomials a, const BRLWE_Ring_polynomials b, BRLWE_Ring_polynomials ans) {
	int i = 0;
	int j = 0;
	for (i = 0; i < BRLWE_N; i++) {
		if (b[i] == 0x01) {
			for (j = 0; j < BRLWE_N; j++) {
				if (i + j <= BRLWE_N - 1)
					ans[i + j] = ans[i + j] + a[j];
				else
					ans[i + j - BRLWE_N] = ans[i + j - BRLWE_N] + BRLWE_N - a[j];
			};
		};
	};
	return ans;
};

//pass b.polynomial to a.polynomial
void passpoly (BRLWE_Ring_polynomials a, const BRLWE_Ring_polynomials b) {
	for (int i = 0; i < BRLWE_N; i++)
		a[i] = b[i];
	return;
};
