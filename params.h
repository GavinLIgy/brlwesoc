#ifndef PARAMS_H
#define PARAMS_H

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
#elif defined(RBINLWEENC3) && (RBINLWEENC3 == 1)
	#define BRLWE_N 512
	#define BRLWE_Q 256
#else

	#define BRLWE_N 128
	#define BRLWE_Q 7681
	#define NTT_Q 7681 

#endif

#endif