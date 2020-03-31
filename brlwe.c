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
// #include "alloc.h"
// #include "alloc.c"
//#include "ntt.h"
//#include "ntt.c"

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
/* Public functions:                                                        */
/*****************************************************************************/

//initialize a polynomial by sampling a uniform distribution with binary coefficients 
BRLWE_Ring_polynomials BRLWE_init_bin_sampling(BRLWE_Ring_polynomials poly) {
	int i = 0;
	int j = 0;
	
	uint32_t cycles_begin, cycles_now;
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_begin));
	
	uint8_t* str = NULL;
	str = m_malloc(4);
	for (i = 0; i < (BRLWE_N>>2) ; i++) {
		RNG_rand(str);
		for (j = 0; j < 4 ; j++){
			poly[(i<<2)+j] = (uint8_t)str[j];
		};
	};
	m_free(str);
	
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_now));
	// print("\t| ");print_dec(cycles_now - cycles_begin);
	print("\n Cycles Number for BRLWE_init_bin_sampling = ");print_dec(cycles_now - cycles_begin);
	
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
	
	uint32_t cycles_begin, cycles_now;
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_begin));
	
	if (rev == 1) {
		for (int i = 0; i < BRLWE_N; i++)
			poly[i] = (uint8_t)(str[BRLWE_N - 1 - i]  & (BRLWE_Q - 1));
	}
	else {
		for (int i = 0; i < BRLWE_N; i++)
			poly[i] = (uint8_t)(str[i] & (BRLWE_Q - 1));
	}; 
	
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_now));
	// print("\t| ");print_dec(cycles_now - cycles_begin);
	print("\n Cycles Number for BRLWE_init_hex = ");print_dec(cycles_now - cycles_begin);
	
	return poly;
};

//initialize a polynomial with all 0.
BRLWE_Ring_polynomials BRLWE_init(BRLWE_Ring_polynomials poly) {
	uint32_t cycles_begin, cycles_now;
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_begin));
	for (int i = 0; i < BRLWE_N; i++)
		poly[i] = (uint8_t)0x00;
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_now));
	// print("\t| ");print_dec(cycles_now - cycles_begin);
	print("\n Cycles Number for BRLWE_init = ");print_dec(cycles_now - cycles_begin);
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
	pk = Simple_Ring_mul_NTT(a, sk, pk);
	
	int i = 0;
	int j = 0;
	
	uint8_t* str = NULL;
	str = m_malloc(4);//random number buffer: uint8_t str [4]
	for (i = 0; i < BRLWE_N>>2 ; i++) {
		RNG_rand(str);
		for (j = 0; j < 4 ; j++){
			pk[(i<<2)+j] = (uint8_t)((uint8_t)str[j] - pk[(i<<2)+j] ) & (BRLWE_Q-1);
		};
	};
	m_free(str);

	return key;
};

//Main Function 2: Encryption
//pre-requirement: length(m) = n, m belongs to {0,1}^n;
//a is a global parameter shared by Alice and Bob, pk is public key and would be sent to Bob after Key_Gen, m is the message to be crypto
//After receiving pk, Bob uses 3 error(binary) polynomials e1, e2, e3
//m_wave = encode(m), c1 = a*e1 +e2, c2 = pk*e1 + e3 + m_wave
//cryptom = [c1,c2] belonging to R_q^2 are cipertext
BRLWE_Ring_polynomials2 BRLWE_Encry(const BRLWE_Ring_polynomials a, const BRLWE_Ring_polynomials pk, uint8_t* m, BRLWE_Ring_polynomials2 cryptom ) {
	BRLWE_Ring_polynomials c1 = cryptom;//crypto message 1
	BRLWE_Ring_polynomials c2 = cryptom + BRLWE_N;//crypto message 2
	BRLWE_Ring_polynomials e1 = NULL;
	
	e1 = m_malloc(BRLWE_N);
	
	e1 = BRLWE_init_bin_sampling(e1);
	
	c1 = Simple_Ring_mul_NTT(a, e1, c1);//c1 = a*e1
	c2 = Simple_Ring_mul_NTT(pk, e1, c2);//c2 = pk*e1
	
	m_free(e1);
	
	int i = 0;
	int j = 0;
	
	// uint32_t cycles_now;
	// __asm__ volatile ("rdcycle %0" : "=r"(cycles_now));
	// RNG_seed(cycles_now);
	
	uint8_t* str = NULL;
	str = m_malloc(4);//random number buffer: uint8_t str [4]
	
	for (i = 0; i < BRLWE_N>>2 ; i++) {
		RNG_rand(str);
		for (j = 0; j < 4 ; j++){
			c1[(i<<2)+j] = ( c1[(i<<2)+j] + (uint8_t)str[j] ) & (BRLWE_Q - 1);
			//   c1     =      c1       +          e2    ;
		};
	};
	
	for (i = 0; i < BRLWE_N>>2 ; i++) {
		RNG_rand(str);
		for (j = 0; j < 4 ; j++){
			c2[(i<<2)+j] = ( c2[(i<<2)+j] + (uint8_t)str[j] ) & (BRLWE_Q - 1);// + (uint8_t)(BRLWE_Q / 2) * (*(m+4*i+j)) + BRLWE_Q + (BRLWE_N / 2) - 1 - (4*i+j);
			
			if (m[(i<<2)+j] != 0)
				c2[(i<<2)+j] = ( c2[(i<<2)+j] + (uint8_t)(BRLWE_Q >> 1) ) & (BRLWE_Q - 1);
			c2[(i<<2)+j] = ( c2[(i<<2)+j] + BRLWE_Q + (BRLWE_N >> 1) - 1 - ( i << 2 ) - j ) & (BRLWE_Q - 1);
			//c2=c2+e3+m_wave;                                                    ;
		};
	};
	
	m_free(str);
	
	return cryptom;
};

//Main Function 3: Decryption
//output m' = Decode(c1*r2+c2)
//r2 is secret key
uint8_t* BRLWE_Decry(const BRLWE_Ring_polynomials2 cryptom, const BRLWE_Ring_polynomials r2, uint8_t* recoverm) {
		BRLWE_Ring_polynomials c1 = cryptom;//crypto message 1
	BRLWE_Ring_polynomials c2 = cryptom + BRLWE_N;//crypto message 2
	
	recoverm = Simple_Ring_mul_NTT(c1, r2, (BRLWE_Ring_polynomials)recoverm);//recoverm = c1*r2
	recoverm = Ring_add((BRLWE_Ring_polynomials)recoverm, c2, (BRLWE_Ring_polynomials)recoverm);//recoverm = recoverm + c2
	
	return BRLWE_Decode(recoverm);
};

//Decode function
//Decode polynomial m_wave into string m
uint8_t* BRLWE_Decode(uint8_t* recoverm) {
	int i = 0;
	int low_th = BRLWE_Q >> 2;
	int hig_th = (BRLWE_Q + BRLWE_Q << 1) >> 2;
	for (i = 0; i < BRLWE_N; i++) {
		if (recoverm[i] > low_th && recoverm[i] < hig_th)
			recoverm[i] = (uint8_t)1;
		else
			recoverm[i] = (uint8_t)0;
	};
	return recoverm;
};

//return value ans = a + b;
BRLWE_Ring_polynomials Ring_add(const BRLWE_Ring_polynomials a, const BRLWE_Ring_polynomials b, BRLWE_Ring_polynomials ans) {
	uint32_t cycles_begin, cycles_now;
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_begin));
	int i = 0;
	for (i = 0; i < BRLWE_N; i++)
		ans[i] = (a[i] + b[i]) & (BRLWE_Q - 1);
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_now));
	// print("\t| ");print_dec(cycles_now - cycles_begin);print("*");
	print("\n Cycles Number for Ring_add = ");print_dec(cycles_now - cycles_begin);
	return ans;
};

//return value = a - b;
BRLWE_Ring_polynomials Ring_sub(const BRLWE_Ring_polynomials a, const BRLWE_Ring_polynomials b, BRLWE_Ring_polynomials ans) {
	uint32_t cycles_begin, cycles_now;
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_begin));
	int i = 0;
	for (i = 0; i < BRLWE_N; i++) 
		ans[i] = (a[i] - b[i]) & (BRLWE_Q - 1);
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_now));
	// print("\t| ");print_dec(cycles_now - cycles_begin);print("*");
	print("\n Cycles Number for Ring_sub = ");print_dec(cycles_now - cycles_begin);
	return ans;
};

//return value = a * b; b is with binary coefficiences
//i=0 is lsb, i=n-1 is msb
//most time/area consuming function(by guess) 
BRLWE_Ring_polynomials Simple_Ring_mul(const BRLWE_Ring_polynomials a, const BRLWE_Ring_polynomials b, BRLWE_Ring_polynomials ans) {
	uint32_t cycles_begin, cycles_now;
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_begin));
	int i = 0;
	int j = 0;
	int tmp = 0;
	ans = BRLWE_init(ans);
	for (i = 0; i < BRLWE_N; i++) {
		if (b[i] != (uint8_t)0) {
			for (j = 0; j < BRLWE_N; j++) {
				//print("\n( i , j ) = \t( ");print_dec(i);print(" , ");print_dec(j);print(" )\n");
				
				if ( (i + j) <= (BRLWE_N - 1) ){
					//print("\n new ans[i + j] = ");
					ans[i + j] = (ans[i + j] + a[j] ) & (BRLWE_Q - 1) ;
					//print_dec(ans[i + j]);
				}
				else
				{
					//print("\n new ans[i + j - BRLWE_N] = ");
					tmp = (256 - a[j]);
					ans[i + j - BRLWE_N] = (ans[i + j - BRLWE_N] + tmp )  & (BRLWE_Q - 1);
					//print_dec(ans[i + j - BRLWE_N]);
				};	
			};
		};
	};
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_now));
	//print("\t| ");print_dec(cycles_now - cycles_begin);print("*");
	print("\n Cycles Number for Simple_Ring_mul = ");print_dec(cycles_now - cycles_begin);
	return ans;
};

BRLWE_Ring_polynomials Simple_Ring_mul_NTT(const BRLWE_Ring_polynomials a, const BRLWE_Ring_polynomials b, BRLWE_Ring_polynomials ans) {
	
	uint32_t cycles_begin, cycles_now;
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_begin));
	
	int* inta = NULL;
	inta = (int*)m_malloc(BRLWE_N<<4);
	memset(inta, 0, BRLWE_N<<4);
	int s1 = 0;
	s1 = get_int_poly(inta, a, BRLWE_N);

	int* intb = NULL;
	intb = (int*)m_malloc(BRLWE_N<<4);
	memset(intb, 0, BRLWE_N<<4);
	int s2 = 0;
	s2 = get_int_poly(intb, b, BRLWE_N);

	int* result = NULL;
	result = (int*)m_malloc(BRLWE_N<<4);
	memset(result, 0, BRLWE_N<<4);
	
	mem_print();
	int rs = long_mul(result, inta, s1, intb, s2);
	get_hex_poly(result, rs, ans, BRLWE_N, BRLWE_Q);
	
	mem_print();
	m_free(inta);
	m_free(intb);
	m_free(result);
	
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_now));
	//print("\t| ");print_dec(cycles_now - cycles_begin);print("*");
	print("\n Cycles Number for Simple_Ring_mul = ");print_dec(cycles_now - cycles_begin);
	return ans;
};
