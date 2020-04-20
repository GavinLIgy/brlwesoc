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
#include "ntt.h"

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

#if (defined(RBINLWEENC1) && (RBINLWEENC1 == 1)) || (defined(RBINLWEENC2) && (RBINLWEENC2 == 1)) || (defined(RBINLWEENC3) && (RBINLWEENC3 == 1)) || (defined(RBINLWEENCT) && (RBINLWEENCT == 1))

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
	pk = Ring_mul(a, sk, pk);
	
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
	
	c1 = Ring_mul(a, e1, c1);//c1 = a*e1
	c2 = Ring_mul(pk, e1, c2);//c2 = pk*e1
	
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
	
	recoverm = Ring_mul(c1, r2, (BRLWE_Ring_polynomials)recoverm);//recoverm = c1*r2
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
	// print("\t| ");print_dec(cycles_now - cycles_begin);//print("*");
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
	// print("\t| ");print_dec(cycles_now - cycles_begin);//print("*");
	print("\n Cycles Number for Ring_sub = ");print_dec(cycles_now - cycles_begin);
	return ans;
};

//return value = a * b; b is with binary coefficiences
//i=0 is lsb, i=n-1 is msb
//most time/area consuming function(by guess) 
BRLWE_Ring_polynomials Ring_mul(const BRLWE_Ring_polynomials a, const BRLWE_Ring_polynomials b, BRLWE_Ring_polynomials ans) {
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
					ans[i + j] = (ans[i + j] + a[j] ) % BRLWE_Q;
					//print_dec(ans[i + j]);
				}
				else
				{
					//print("\n new ans[i + j - BRLWE_N] = ");
					tmp = (BRLWE_Q - a[j]);
					ans[i + j - BRLWE_N] = (ans[i + j - BRLWE_N] + tmp ) % BRLWE_Q;
					//print_dec(ans[i + j - BRLWE_N]);
				};	
			};
		};
	};
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_now));
	// print("\t| ");print_dec(cycles_now - cycles_begin);//print("*");
	print("\n Cycles Number for Simple_Ring_mul = ");print_dec(cycles_now - cycles_begin);
	return ans;
};


#else
	
//initialize a polynomial by sampling a uniform distribution with binary coefficients 
BRLWE_Ring_polynomials BRLWE_init_bin_sampling(BRLWE_Ring_polynomials poly) {
	int i = 0;
	int j = 0;
	
	uint32_t cycles_begin, cycles_now;
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_begin));
	
	uint8_t* str = NULL;
	str = m_malloc(4);
	for (i = 0; i < (BRLWE_N>>1) ; i++) {
		RNG_rand(str);
		for (j = 0; j < 2; j++){
			poly[(i<<1)+j] = (uint16_t)str[j];
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
BRLWE_Ring_polynomials BRLWE_init_hex(BRLWE_Ring_polynomials poly, uint16_t* str, int rev) {
	
	uint32_t cycles_begin, cycles_now;
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_begin));
	
	if (rev == 1) {
		for (int i = 0; i < BRLWE_N; i++)
			poly[i] = (uint16_t)(str[BRLWE_N - 1 - i] % BRLWE_Q );
			//poly[i] = montgomery_reduce(str[BRLWE_N - 1 - i]);
	}
	else {
		for (int i = 0; i < BRLWE_N; i++)
			poly[i] = (uint16_t)(str[i] % BRLWE_Q );
			//poly[i] = montgomery_reduce(str[i]);
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
		poly[i] = (uint16_t)0x00;
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
	pk = Ring_mul(a, sk, pk);
	
	int i = 0;
	int j = 0;
	
	uint8_t* str = NULL;
	str = m_malloc(4);//random number buffer: uint8_t str [4]
	for (i = 0; i < BRLWE_N>>2 ; i++) {
		RNG_rand(str);
		for (j = 0; j < 4 ; j++){
			pk[(i<<2)+j] = (uint16_t)((uint16_t)str[j] - pk[(i<<2)+j] ) % BRLWE_Q ;
			//pk[(i<<2)+j] = montgomery_reduce((uint16_t)str[j] - pk[(i<<2)+j] );
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
BRLWE_Ring_polynomials2 BRLWE_Encry(const BRLWE_Ring_polynomials a, const BRLWE_Ring_polynomials pk, uint16_t* m, BRLWE_Ring_polynomials2 cryptom ) {
	BRLWE_Ring_polynomials c1 = cryptom;//crypto message 1
	BRLWE_Ring_polynomials c2 = cryptom + BRLWE_N;//crypto message 2
	BRLWE_Ring_polynomials e1 = NULL;
	
	e1 = m_malloc(BRLWE_N * 2);
	
	e1 = BRLWE_init_bin_sampling(e1);
	c1 = Ring_mul(a, e1, c1);//c1 = a*e1
	c2 = Ring_mul(pk, e1, c2);//c2 = pk*e1

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
			c1[(i<<2)+j] = ( c1[(i<<2)+j] + (uint16_t)str[j] ) % BRLWE_Q;
			//c1[(i<<2)+j] = montgomery_reduce( c1[(i<<2)+j] + (uint16_t)str[j] );
			//   c1     =      c1       +          e2    ;
		};
	};
	
	for (i = 0; i < BRLWE_N>>2 ; i++) {
		RNG_rand(str);
		for (j = 0; j < 4 ; j++){
			c2[(i<<2)+j] = ( c2[(i<<2)+j] + (uint16_t)str[j] ) % BRLWE_Q ;// + (uint8_t)(BRLWE_Q / 2) * (*(m+4*i+j)) + BRLWE_Q + (BRLWE_N / 2) - 1 - (4*i+j);
			//c2[(i<<2)+j] = montgomery_reduce( c2[(i<<2)+j] + (uint16_t)str[j] );
			if (m[(i<<2)+j] != 0)
				c2[(i<<2)+j] = ( c2[(i<<2)+j] + (uint16_t)(BRLWE_Q >> 1) ) % BRLWE_Q;
				//c2[(i<<2)+j] = montgomery_reduce( c2[(i<<2)+j] + (uint16_t)(BRLWE_Q >> 1) );
			c2[(i<<2)+j] = ( c2[(i<<2)+j] + BRLWE_Q + (BRLWE_N >> 1) - 1 - ( i << 2 ) - j ) % BRLWE_Q;
			//c2[(i<<2)+j] = montgomery_reduce( c2[(i<<2)+j] + BRLWE_Q + (BRLWE_N >> 1) - 1 - ( i << 2 ) - j );
			//c2=c2+e3+m_wave;                                                    ;
		};
	};
	m_free(str);
	
	return cryptom;
};

//Main Function 3: Decryption
//output m' = Decode(c1*r2+c2)
//r2 is secret key
uint16_t* BRLWE_Decry(const BRLWE_Ring_polynomials2 cryptom, const BRLWE_Ring_polynomials r2, uint16_t* recoverm) {
		BRLWE_Ring_polynomials c1 = cryptom;//crypto message 1
	BRLWE_Ring_polynomials c2 = cryptom + BRLWE_N;//crypto message 2
	
	recoverm = Ring_mul(c1, r2, (BRLWE_Ring_polynomials)recoverm);//recoverm = c1*r2
	recoverm = Ring_add((BRLWE_Ring_polynomials)recoverm, c2, (BRLWE_Ring_polynomials)recoverm);//recoverm = recoverm + c2
	
	return BRLWE_Decode(recoverm);
};

//Decode function
//Decode polynomial m_wave into string m
uint16_t* BRLWE_Decode(uint16_t* recoverm) {
	int i = 0;
	int low_th = BRLWE_Q >> 2;
	int hig_th = (BRLWE_Q + BRLWE_Q << 1) >> 2;
	for (i = 0; i < BRLWE_N; i++) {
		if (recoverm[i] > low_th && recoverm[i] < hig_th)
			recoverm[i] = (uint16_t)1;
		else
			recoverm[i] = (uint16_t)0;
	};
	return recoverm;
};

//return value ans = a + b;
BRLWE_Ring_polynomials Ring_add(const BRLWE_Ring_polynomials a, const BRLWE_Ring_polynomials b, BRLWE_Ring_polynomials ans) {
	uint32_t cycles_begin, cycles_now;
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_begin));
	int i = 0;
	for (i = 0; i < BRLWE_N; i++)
		ans[i] = (a[i] + b[i]) % BRLWE_Q;
		//ans[i] = montgomery_reduce(a[i] + b[i]);
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_now));
	// print("\t| ");print_dec(cycles_now - cycles_begin);//print("*");
	print("\n Cycles Number for Ring_add = ");print_dec(cycles_now - cycles_begin);
	return ans;
};

//return value = a - b;
BRLWE_Ring_polynomials Ring_sub(const BRLWE_Ring_polynomials a, const BRLWE_Ring_polynomials b, BRLWE_Ring_polynomials ans) {
	uint32_t cycles_begin, cycles_now;
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_begin));
	int i = 0;
	for (i = 0; i < BRLWE_N; i++) 
		//ans[i] = montgomery_reduce(a[i] + 4 * BRLWE_Q - b[i]);
		ans[i] = (a[i] + 4 * BRLWE_Q - b[i]) % BRLWE_Q;
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_now));
	// print("\t| ");print_dec(cycles_now - cycles_begin);//print("*");
	print("\n Cycles Number for Ring_sub = ");print_dec(cycles_now - cycles_begin);
	return ans;
};

BRLWE_Ring_polynomials Simple_Ring_mul_PtNTT(const BRLWE_Ring_polynomials a, const BRLWE_Ring_polynomials b, BRLWE_Ring_polynomials ans) {
	
	uint32_t cycles_begin, cycles_now;
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_begin));
	
	uint16_t* f = NULL;
	uint16_t* g = NULL;

	f = (uint16_t*)m_malloc(2 * BRLWE_N * sizeof(uint16_t));

	get_int16_polys(f, a);

	struct ptpoly4 fpoly;

	fpoly.poly00 = (uint16_t*)m_malloc(BRLWE_N / 2 * sizeof(uint16_t));
	fpoly.poly01 = (uint16_t*)m_malloc(BRLWE_N / 2 * sizeof(uint16_t));
	fpoly.poly10 = (uint16_t*)m_malloc(BRLWE_N / 2 * sizeof(uint16_t));
	fpoly.poly11 = (uint16_t*)m_malloc(BRLWE_N / 2 * sizeof(uint16_t));

	memset(fpoly.poly00, 0, BRLWE_N / 2 * sizeof(uint16_t));
	memset(fpoly.poly01, 0, BRLWE_N / 2 * sizeof(uint16_t));
	memset(fpoly.poly10, 0, BRLWE_N / 2 * sizeof(uint16_t));
	memset(fpoly.poly11, 0, BRLWE_N / 2 * sizeof(uint16_t));

	poly_pt_ntt4(f, fpoly);

	m_free(f);

	g = (uint16_t*)m_malloc(2 * BRLWE_N * sizeof(uint16_t));

	get_int16_polys(g, b);

	struct ptpoly7 gpoly;

	gpoly.poly4.poly00 = (uint16_t*)m_malloc(BRLWE_N / 2 * sizeof(uint16_t));
	gpoly.poly4.poly01 = (uint16_t*)m_malloc(BRLWE_N / 2 * sizeof(uint16_t));
	gpoly.poly4.poly10 = (uint16_t*)m_malloc(BRLWE_N / 2 * sizeof(uint16_t));
	gpoly.poly4.poly11 = (uint16_t*)m_malloc(BRLWE_N / 2 * sizeof(uint16_t));
	gpoly.poly01_s = (uint16_t*)m_malloc(BRLWE_N / 2 * sizeof(uint16_t));
	gpoly.poly10_s = (uint16_t*)m_malloc(BRLWE_N / 2 * sizeof(uint16_t));
	gpoly.poly11_s = (uint16_t*)m_malloc(BRLWE_N / 2 * sizeof(uint16_t));

	memset(gpoly.poly4.poly00, 0, BRLWE_N / 2 * sizeof(uint16_t));
	memset(gpoly.poly4.poly01, 0, BRLWE_N / 2 * sizeof(uint16_t));
	memset(gpoly.poly4.poly10, 0, BRLWE_N / 2 * sizeof(uint16_t));
	memset(gpoly.poly4.poly11, 0, BRLWE_N / 2 * sizeof(uint16_t));
	memset(gpoly.poly01_s, 0, BRLWE_N / 2 * sizeof(uint16_t));
	memset(gpoly.poly10_s, 0, BRLWE_N / 2 * sizeof(uint16_t));
	memset(gpoly.poly11_s, 0, BRLWE_N / 2 * sizeof(uint16_t));

	poly_pt_ntt7(g, gpoly);
	pt_ntt_bowtiemultiply(g, fpoly, gpoly);
	
	m_free(fpoly.poly00);
	m_free(fpoly.poly01);
	m_free(fpoly.poly10);
	m_free(fpoly.poly11);
	m_free(gpoly.poly4.poly00);
	m_free(gpoly.poly4.poly01);
	m_free(gpoly.poly4.poly10);
	m_free(gpoly.poly4.poly11);
	m_free(gpoly.poly01_s);
	m_free(gpoly.poly10_s);
	m_free(gpoly.poly11_s);

	poly_inv_ptntt(g);

	uint32_t tmp = 0;
	for (int i = 0; i < BRLWE_N * 2; i++) {
		if (i < BRLWE_N) {
			tmp = (uint32_t)g[i];
			//ans[i] = montgomery_reduce(tmp);
			ans[i] = (uint16_t)(tmp % BRLWE_Q);
		}
		else {
			tmp = (uint32_t)ans [i - BRLWE_N] + 4 * BRLWE_Q - (uint32_t)g[i];
			//ans[i - BRLWE_N] = montgomery_reduce(tmp);
			ans[i - BRLWE_N] = (uint16_t)(tmp % BRLWE_Q);
		}
	}
	
	m_free(g);
	
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_now));
	// print("\t| ");print_dec(cycles_now - cycles_begin);//print("*");
	print("\n Cycles Number for Simple_Ring_mul_PtNTT = ");print_dec(cycles_now - cycles_begin);
	return ans;
};

//return value = a * b;
BRLWE_Ring_polynomials Ring_mul(const BRLWE_Ring_polynomials a, const BRLWE_Ring_polynomials b, BRLWE_Ring_polynomials ans) {

#if (defined(RBINLWEENC1) && (RBINLWEENC1 == 1)) || (defined(RBINLWEENC2) && (RBINLWEENC2 == 1)) || (defined(RBINLWEENC3) && (RBINLWEENC3 == 1)) || (defined(RBINLWEENCT) && (RBINLWEENCT == 1))
	return Simple_Ring_mul(a, b, ans);
#else
	#if defined(My_NTT) && (My_NTT == 1)
	return Simple_Ring_mul_NBNTT(a, b, ans);
	#elif defined(PtNTT) && (PtNTT == 1)
	return Simple_Ring_mul_PtNTT(a, b, ans);
	#endif 
#endif
};

#endif