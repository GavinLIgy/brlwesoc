#include <stdint.h>
#include "ntt.h"

#include "params.h"

static const uint32_t qinv = 7679; // -inverse_mod(p,2^18)
static const uint32_t rlog = 18;

/*************************************************
* Name:        Montgomery reduction
*
* Description: Montgomery reduction; given a 32-bit integer a, computes
*              16-bit integer congruent to a * R^-1 mod q,
*              where R=2^18 (see value of rlog)
*
* Arguments:   - uint32_t a: input unsigned integer to be reduced; has to be in {0,...,1073491968}
*
* Returns:     unsigned integer in {0,...,2^14-1} congruent to a * R^-1 modulo q.
**************************************************/
uint16_t montgomery_reduce(uint32_t a)
{
	uint32_t u;

	u = (a * qinv);
	u &= ((1 << rlog) - 1);
	u *= NTT_Q;
	a = a + u;
	return a >> 18;
}

/*************************************************
* Name:        coeff_freeze
*
* Description: Fully reduces an integer modulo q in constant time
*
* Arguments:   uint16_t x: input integer to be reduced
*
* Returns integer in {0,...,q-1} congruent to x modulo q
**************************************************/
static uint16_t coeff_freeze(uint16_t x)
{
	uint16_t m, r;
	int16_t c;
	r = x & (BRLWE_Q - 1);

	m = r - BRLWE_Q;
	c = m;
	c >>= 15;
	r = m ^ ((r^m)&c);

	return r;
}

#if (BRLWE_N == 256)


/************************************************************
* Name:        omegas_bitrev_montgomery
*
* Description: Contains powers of 256th root of unity in Montgomery
*              domain with R=2^18 in bit-reversed order
************************************************************/
static const uint16_t omegas_bitrev_montgomery[128] = {
  990, 7427, 2634, 6819, 578, 3281, 2143, 1095, 484, 6362, 3336, 5382, 6086, 3823, 877, 5656,
  3583, 7010, 6414, 263, 1285, 291, 7143, 7338, 1581, 5134, 5184, 5932, 4042, 5775, 2468, 3,
  606, 729, 5383, 962, 3240, 7548, 5129, 7653, 5929, 4965, 2461, 641, 1584, 2666, 1142, 157,
  7407, 5222, 5602, 5142, 6140, 5485, 4931, 1559, 2085, 5284, 2056, 3538, 7269, 3535, 7190, 1957,
  3465, 6792, 1538, 4664, 2023, 7643, 3660, 7673, 1694, 6905, 3995, 3475, 5939, 1859, 6910, 4434,
  1019, 1492, 7087, 4761, 657, 4859, 5798, 2640, 1693, 2607, 2782, 5400, 6466, 1010, 957, 3851,
  2121, 6392, 7319, 3367, 3659, 3375, 6430, 7583, 1549, 5856, 4773, 6084, 5544, 1650, 3997, 4390,
  6722, 2915, 4245, 2635, 6128, 7676, 5737, 1616, 3457, 3132, 7196, 4702, 6239, 851, 2122, 3009
};

/************************************************************
* Name:         omegas_inv_bitrev_montgomery
*
* Description: Contains inverses of powers of 256th root of unity
*              in Montgomery domain with R=2^18 in bit-reversed order
************************************************************/
static const uint16_t omegas_inv_bitrev_montgomery[128] = {
  990, 254, 862, 5047, 6586, 5538, 4400, 7103, 2025, 6804, 3858, 1595, 2299, 4345, 1319, 7197,
  7678, 5213, 1906, 3639, 1749, 2497, 2547, 6100, 343, 538, 7390, 6396, 7418, 1267, 671, 4098,
  5724, 491, 4146, 412, 4143, 5625, 2397, 5596, 6122, 2750, 2196, 1541, 2539, 2079, 2459, 274,
  7524, 6539, 5015, 6097, 7040, 5220, 2716, 1752, 28, 2552, 133, 4441, 6719, 2298, 6952, 7075,
  4672, 5559, 6830, 1442, 2979, 485, 4549, 4224, 6065, 1944, 5, 1553, 5046, 3436, 4766, 959,
  3291, 3684, 6031, 2137, 1597, 2908, 1825, 6132, 98, 1251, 4306, 4022, 4314, 362, 1289, 5560,
  3830, 6724, 6671, 1215, 2281, 4899, 5074, 5988, 5041, 1883, 2822, 7024, 2920, 594, 6189, 6662,
  3247, 771, 5822, 1742, 4206, 3686, 776, 5987, 8, 4021, 38, 5658, 3017, 6143, 889, 4216
};

/************************************************************
* Name:        psis_bitrev_montgomery
*
* Description: Contains powers of 256th root of -1 in Montgomery
*              domain with R=2^18 in bit-reversed order
************************************************************/
static const uint16_t psis_bitrev_montgomery[256] = {
  990, 7427, 2634, 6819, 578, 3281, 2143, 1095, 484, 6362, 3336, 5382, 6086, 3823, 877, 5656,
  3583, 7010, 6414, 263, 1285, 291, 7143, 7338, 1581, 5134, 5184, 5932, 4042, 5775, 2468, 3,
  606, 729, 5383, 962, 3240, 7548, 5129, 7653, 5929, 4965, 2461, 641, 1584, 2666, 1142, 157,
  7407, 5222, 5602, 5142, 6140, 5485, 4931, 1559, 2085, 5284, 2056, 3538, 7269, 3535, 7190, 1957,
  3465, 6792, 1538, 4664, 2023, 7643, 3660, 7673, 1694, 6905, 3995, 3475, 5939, 1859, 6910, 4434,
  1019, 1492, 7087, 4761, 657, 4859, 5798, 2640, 1693, 2607, 2782, 5400, 6466, 1010, 957, 3851,
  2121, 6392, 7319, 3367, 3659, 3375, 6430, 7583, 1549, 5856, 4773, 6084, 5544, 1650, 3997, 4390,
  6722, 2915, 4245, 2635, 6128, 7676, 5737, 1616, 3457, 3132, 7196, 4702, 6239, 851, 2122, 3009,
  7613, 7295, 2007, 323, 5112, 3716, 2289, 6442, 6965, 2713, 7126, 3401, 963, 6596, 607, 5027,
  7078, 4484, 5937, 944, 2860, 2680, 5049, 1777, 5850, 3387, 6487, 6777, 4812, 4724, 7077, 186,
  6848, 6793, 3463, 5877, 1174, 7116, 3077, 5945, 6591, 590, 6643, 1337, 6036, 3991, 1675, 2053,
  6055, 1162, 1679, 3883, 4311, 2106, 6163, 4486, 6374, 5006, 4576, 4288, 5180, 4102, 282, 6119,
  7443, 6330, 3184, 4971, 2530, 5325, 4171, 7185, 5175, 5655, 1898, 382, 7211, 43, 5965, 6073,
  1730, 332, 1577, 3304, 2329, 1699, 6150, 2379, 5113, 333, 3502, 4517, 1480, 1172, 5567, 651,
  925, 4573, 599, 1367, 4109, 1863, 6929, 1605, 3866, 2065, 4048, 839, 5764, 2447, 2022, 3345,
  1990, 4067, 2036, 2069, 3567, 7371, 2368, 339, 6947, 2159, 654, 7327, 2768, 6676, 987, 2214
};

/************************************************************
* Name:        psis_inv_montgomery
*
* Description: Contains inverses of powers of 256th  root of -1
*              divided by n in Montgomery domain with R=2^18
************************************************************/
static const uint16_t psis_inv_montgomery[256] = {
  1024, 4972, 5779, 6907, 4943, 4168,  315, 5580,   90,  497, 1123,  142, 4710, 5527, 2443, 4871,
   698, 2489, 2394, 4003,  684, 2241, 2390, 7224, 5072, 2064, 4741, 1687, 6841,  482, 7441, 1235,
  2126, 4742, 2802, 5744, 6287, 4933,  699, 3604, 1297, 2127, 5857, 1705, 3868, 3779, 4397, 2177,
   159,  622, 2240, 1275,  640, 6948, 4572, 5277,  209, 2605, 1157, 7328, 5817, 3191, 1662, 2009,
  4864,  574, 2487,  164, 6197, 4436, 7257, 3462, 4268, 4281, 3414, 4515, 3170, 1290, 2003, 5855,
  7156, 6062, 7531, 1732, 3249, 4884, 7512, 3590, 1049, 2123, 1397, 6093, 3691, 6130, 6541, 3946,
  6258, 3322, 1788, 4241, 4900, 2309, 1400, 1757,  400,  502, 6698, 2338, 3011,  668, 7444, 4580,
  6516, 6795, 2959, 4136, 3040, 2279, 6355, 3943, 2913, 6613, 7416, 4084, 6508, 5556, 4054, 3782,
	61, 6567, 2212,  779,  632, 5709, 5667, 4923, 4911, 6893, 4695, 4164, 3536, 2287, 7594, 2848,
  3267, 1911, 3128,  546, 1991,  156, 4958, 5531, 6903,  483,  875,  138,  250, 2234, 2266, 7222,
  2842, 4258,  812, 6703,  232, 5207, 6650, 2585, 1900, 6225, 4932, 7265, 4701, 3173, 4635, 6393,
   227, 7313, 4454, 4284, 6759, 1224, 5223, 1447,  395, 2608, 4502, 4037,  189, 3348,   54, 6443,
  2210, 6230, 2826, 1780, 3002, 5995, 1955, 6102, 6045, 3938, 5019, 4417, 1434, 1262, 1507, 5847,
  5917, 7157, 7177, 6434, 7537,  741, 4348, 1309,  145,  374, 2236, 4496, 5028, 6771, 6923, 7421,
  1978, 1023, 3857, 6876, 1102, 7451, 4704, 6518, 1344,  765,  384, 5705, 1207, 1630, 4734, 1563,
  6839, 5933, 1954, 4987, 7142, 5814, 7527, 4953, 7637, 4707, 2182, 5734, 2818,  541, 4097, 5641
};

static const uint16_t bitrev_table_256[256] = {
0,128,64,192,32,160,96,224,16,144,80,208,48,176,112,240,
	 8,136,72,200,40,168,104,232,24,152,88,216,56,184,120,248,
	 4,132,68,196,36,164,100,228,20,148,84,212,52,180,116,244,
	 12,140,76,204,44,172,108,236,28,156,92,220,60,188,124,252,
	 2,130,66,194,34,162,98,226,18,146,82,210,50,178,114,242,
	 10,138,74,202,42,170,106,234,26,154,90,218,58,186,122,250,
	 6,134,70,198,38,166,102,230,22,150,86,214,54,182,118,246,
	 14,142,78,206,46,174,110,238,30,158,94,222,62,190,126,254,
	 1,129,65,193,33,161,97,225,17,145,81,209,49,177,113,241,
	 9,137,73,201,41,169,105,233,25,153,89,217,57,185,121,249,
	 5,133,69,197,37,165,101,229,21,149,85,213,53,181,117,245,
	 13,141,77,205,45,173,109,237,29,157,93,221,61,189,125,253,
	 3,131,67,195,35,163,99,227,19,147,83,211,51,179,115,243,
	 11,139,75,203,43,171,107,235,27,155,91,219,59,187,123,251,
	 7,135,71,199,39,167,103,231,23,151,87,215,55,183,119,247,
	 15,143,79,207,47,175,111,239,31,159,95,223,63,191,127,255 };


/*************************************************
* Name:        bitrev_vector
*
* Description: Permutes coefficients of a polynomial into bitreversed order
*
* Arguments:   - uint16_t* poly: pointer to in/output polynomial
**************************************************/

void bitrev_vector_256(uint16_t* poly)
{
	unsigned int i, r;
	uint16_t tmp;

	for (i = 0; i < 256; i++)
	{
		r = bitrev_table_256[i];
		if (i < r)
		{
			tmp = poly[i];
			poly[i] = poly[r];
			poly[r] = tmp;
		}
	}
}


/*************************************************
* Name:        get_int16_half_polys
*
* Description: spliting function from 1 uint8_t[n] to uint16_t[n/2].
*
* Arguments:    - uint16_t *arr : The pointer to output uint16_t array. No need to be memset(0) before.
*				- uint8_t *poly : The pointer to input uint8_t polynomial.
*
* Returns:     None
**************************************************/
void get_int16_half_polys(uint16_t *arr, uint8_t *poly) {
	for (int j = 0; j < BRLWE_N; j++) {
		if (j < (BRLWE_N >> 1))arr[j] = (uint16_t)poly[j];
		else arr[j] = 0;
	}
			
};


/*************************************************
* Name:        mul_coefficients
*
* Description: Performs pointwise (coefficient-wise) multiplication
*              of two polynomials
* Arguments:   - uint16_t* poly:          pointer to in/output polynomial
*              - const uint16_t* factors: pointer to input polynomial, coefficients
*                                         are assumed to be in Montgomery representation
**************************************************/
void mul_coefficients_256(uint16_t* poly, const uint16_t* factors)
{
	unsigned int i;

	for (i = 0; i < 256; i++)
		poly[i] = montgomery_reduce((poly[i] * factors[i]));
}

/*************************************************
* Name:        ntt
*
* Description: Computes number-theoretic transform (NTT) of
*              a polynomial in place; inputs assumed to be in
*              bitreversed order, output in normal order
*
* Arguments:   - uint16_t * a:          pointer to in/output polynomial
*              - const uint16_t* omega: pointer to input powers of root of unity omega;
*                                       assumed to be in Montgomery domain
**************************************************/
void ntt_256(uint16_t * a, const uint16_t* omega)
{
	int i, start, j, jTwiddle, distance;
	uint16_t temp, W;


	for (i = 0; i < 8; i += 2)
	{
		// Even level
		distance = (1 << i);
		for (start = 0; start < distance; start++)
		{
			jTwiddle = 0;
			for (j = start; j < 255; j += 2 * distance)
			{
				W = omega[jTwiddle++];
				temp = a[j];
				a[j] = (temp + a[j + distance]); // Omit reduction (be lazy)
				a[j + distance] = montgomery_reduce((W * ((uint32_t)temp + 3 * 7681 - a[j + distance])));
			}
		}

		// Odd level
		distance <<= 1;
		for (start = 0; start < distance; start++)
		{
			jTwiddle = 0;
			for (j = start; j < 255; j += 2 * distance)
			{
				W = omega[jTwiddle++];
				temp = a[j];
				a[j] = (temp + a[j + distance]) % 7681;
				a[j + distance] = montgomery_reduce((W * ((uint32_t)temp + 3 * 7681 - a[j + distance])));
			}
		}
	}
}

/*************************************************
* Name:        poly_ntt
*
* Description: Computes number-theoretic transform (NTT) of
*              a polynomial in place; inputs assumed to be in
*              bitreversed order, output in normal order
*
* Arguments:   - uint16_t * r:          pointer to in/output polynomial
**************************************************/
void poly_ntt(uint16_t *r)
{
	mul_coefficients_256(r, psis_bitrev_montgomery);
	ntt_256(r, omegas_bitrev_montgomery);
}

/*************************************************
* Name:        poly_invntt
*
* Description: Inverse NTT transform of a polynomial in place
*              Input is assumed to have coefficients in normal order
*              Output has coefficients in normal order
*
* Arguments:   - poly *r: pointer to in/output polynomial
**************************************************/
void poly_invntt(uint16_t *r)
{
	int i;
	bitrev_vector_256(r);
	ntt_256((uint16_t *)r, omegas_inv_bitrev_montgomery);
	mul_coefficients_256(r, psis_inv_montgomery);
	for (i = 0; i < 256; i++)
	{
		r[i] = coeff_freeze(r[i]);
	}
}

/*************************************************
* Name:        poly_pointwise
*
* Description: Multiply two polynomials pointwise (i.e., coefficient-wise).
*
* Arguments:   - poly_quarter *r:       pointer to output quarter-polynomial
*              - const poly_quarter *a: pointer to first input quarter-polynomial
*              - const poly_quarter *b: pointer to second input quarter-polynomial
**************************************************/
void poly_mul_pointwise(uint16_t *r, const uint16_t *a, const uint16_t *b)
{
	int i;
	uint16_t t;
	for (i = 0; i < 256; i++)
	{
		t = montgomery_reduce(4613 * b[i]); /* t is now in Montgomery domain, 3186->4613 */
		r[i] = montgomery_reduce(a[i] * t);  /* r->coeffs[i] is back in normal domain */
		//r[i] = (a[i] * b[i]) % NTT_Q;
	}
}

/*************************************************
* Name:        poly_add2
*
* Description: Add 2 polynomials
*
* Arguments:   - uint16_t *r:       pointer to output polynomial
*              - const uint16_t *a: pointer to first input polynomial
*              - const uint16_t *b: pointer to second input polynomial
**************************************************/
void poly_add2(uint16_t *r, const uint16_t *a, const uint16_t *b)
{
	int i;
	for (i = 0; i < 256; i++) {
		r[i] = (a[i] + b[i]) & (BRLWE_Q - 1);
	}

}

/*************************************************
* Name:        poly_sub3
*
* Description: Add 2 polynomials
*
* Arguments:   - uint16_t *r:       pointer to output polynomial. r = c-a-b;
*              - const uint16_t *a: pointer to first input polynomial
*              - const uint16_t *b: pointer to second input polynomial
*              - uint16_t *c: pointer to third input polynomial
**************************************************/
void poly_sub3(uint16_t *r, const uint16_t *a, const uint16_t *b, uint16_t *c)
{
	int i;
	int count = 0;
	for (i = 0; i < 256; i++) {
		r[i] = (c[i] + BRLWE_Q - a[i] + BRLWE_Q - b[i]) & (BRLWE_Q - 1);
	}
}


#elif (BRLWE_N == 512)

#elif (BRLWE_N == 4)

static uint16_t bitrev_table_4[4] = {0,2,1,3};

/*************************************************
* Name:        bitrev_vector
*
* Description: Permutes coefficients of a polynomial into bitreversed order
*
* Arguments:   - uint16_t* poly: pointer to in/output polynomial
**************************************************/

void bitrev_vector_8(uint16_t* poly)
{
	unsigned int i, r;
	uint16_t tmp;

	for (i = 0; i < 4; i++)
	{
		r = bitrev_table_4[i];
		if (i < r)
		{
			tmp = poly[i];
			poly[i] = poly[r];
			poly[r] = tmp;
		}
	}
}

/*************************************************
* Name:        mul_coefficients
*
* Description: Performs pointwise (coefficient-wise) multiplication
*              of two polynomials
* Arguments:   - uint16_t* poly:          pointer to in/output polynomial
*              - const uint16_t* factors: pointer to input polynomial, coefficients
*                                         are assumed to be in Montgomery representation
**************************************************/
void mul_coefficients_4(uint16_t* poly, const uint16_t* factors)
{
	unsigned int i;

	for (i = 0; i < 4; i++)
		poly[i] = montgomery_reduce((poly[i] * factors[i]));
}

/*************************************************
* Name:        ntt
*
* Description: Computes number-theoretic transform (NTT) of
*              a polynomial in place; inputs assumed to be in
*              bitreversed order, output in normal order
*
* Arguments:   - uint16_t * a:          pointer to in/output polynomial
*              - const uint16_t* omega: pointer to input powers of root of unity omega;
*                                       assumed to be in Montgomery domain
**************************************************/
void ntt_4(uint16_t * a, const uint16_t* omega)
{
	int i, start, j, jTwiddle, distance;
	uint16_t temp, W;


	for (i = 0; i < 2; i += 2)
	{
		// Even level
		distance = (1 << i);
		for (start = 0; start < distance; start++)
		{
			jTwiddle = 0;
			for (j = start; j < 3; j += 2 * distance)
			{
				W = omega[jTwiddle++];
				temp = a[j];
				a[j] = (temp + a[j + distance]); // Omit reduction (be lazy)
				a[j + distance] = montgomery_reduce((W * ((uint32_t)temp + 3 * 7681 - a[j + distance])));
			}
		}

		// Odd level
		distance <<= 1;
		for (start = 0; start < distance; start++)
		{
			jTwiddle = 0;
			for (j = start; j < 3; j += 2 * distance)
			{
				W = omega[jTwiddle++];
				temp = a[j];
				a[j] = (temp + a[j + distance]) % 7681;
				a[j + distance] = montgomery_reduce((W * ((uint32_t)temp + 3 * 7681 - a[j + distance])));
			}
		}
	}
}


#endif