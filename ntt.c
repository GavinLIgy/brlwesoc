// Number theory transform

//#include <string.h>
#include <stdint.h>

// #include "ntt.h"
#include "alloc.h"
#include "alloc.c"

// #define DIGIT 4
// #define X     65536
// #define N (1 << 13)
// #define L 5002
// int a[N], b[N], c[N], temp[N];
// char buffer[L];

// #define P      ((27 << 26) + 1) /* Mathematica Table[PrimeQ[a*2^26+1],{a,1,32}] */
// #define PR     136              /* See report */
// #define PR_POW 26               /* pow(136, 2 << PR_POW) % P == 1 */
// #define N_REV  1811939302       /* (N_REV * pow(2, PR_POW)) % P == 1, see report */

//int add(long long x, long long y, int mod)
int add(int x, int y, int mod) {
	return (x + y) % mod;
}

//int multiply(long long x, long long y, int mod)
int multiply(int x, int y, int mod) {
	return (x * y) % mod;
}

//int square(long long x, int mod)
int square(int x, int mod) {
	return multiply(x, x, mod);
}

//int fpow(long long base, size_t expo, int mod) 
int fpow(int base, size_t expo, int mod) {
	//long long coeff = 1;
	int coeff = 1;
	if (expo == 0) return 1;
	while (expo > 1) {
		if (expo & 1) {
			coeff = multiply(coeff, base, mod);
		}
		base = square(base, mod);
		expo >>= 1;
	}
	return multiply(base, coeff, mod);
}

// int fpow(int a, int n, int b){
    // int r=1;
    // while(b){
        // if(b&1)r=(r*a)%n;
        // a=(a*a)%n;
        // b>>=1;       // b = b>>1;
    // }
    // return r;
// }

void reverse(int *first, int *last) {
	--last;
	while (first < last) {
		int temp = *first;
		*first++ = *last;
		*last-- = temp;
	}
}

/* [1,2,3,4,...,n] -> [1,3,...,n-1,2,4,...,n] */
void cross(int *first, size_t len, int *assist) {
	int *end = first + len;
	int *even_first = first;
	int *odd_first = assist;
	int *temp = first;
	++first;
	while (first < end) {
		*assist++ = *first;
		first += 2;
	}
	while (even_first < end) {
		*temp++ = *even_first;
		even_first += 2;
	}

	while (temp < end) {
		*temp++ = *odd_first++;
	}
}

/*void print(int *num, size_t sz) {
	int *end = num + sz;
	int p = 0;
	while (num < end && *num == 0) num++;
	while (num < end) {
		if (!p) {
			printf("%d", *num++);
			printf("|");
			p = 1;
		}
		else {
			printf("%0*d", DIGIT, *num++);
			printf("|");
		}
	}
	if (!p) putchar('0');

}*/

/*void print_hex(int *num, size_t sz) {
	int *end = num + sz;
	int p = 0;
	while (num < end && *num == 0) num++;
	while (num < end) {
		if (!p) {
			printf("%x", *num++);
			printf("|");
			p = 1;
		}
		else {
			printf("%0*x", DIGIT, *num++);
			printf("|");
		}
	}
	if (!p) putchar('0');

}*/

void fft(int *first, int *last, int prim_root, int mod) {
	if (last - first > 1) {
		int *mid = first + (last - first) / 2;
		int cur = 1;
		cross(first, last - first, temp);
		fft(first, mid, square(prim_root, mod), mod);
		fft(mid, last, square(prim_root, mod), mod);
		while (mid < last) {
			int x1 = *first, x2 = *mid;
			*first++ = add(x1, multiply(cur, x2, mod), mod);
			*mid++ = add(x1, multiply(mod - cur, x2, mod), mod);
			cur = multiply(cur, prim_root, mod);
		}
	}
}

void ifft(int *first, int *last, int factor, int prim_root, int mod) {
	fft(first, last, prim_root, mod);
	while (first < last) {
		*first = multiply(*first, factor, mod);
		first++;
	}
}

int get_bin_len(size_t sz) {
	int result = 0;
	while (sz) {
		sz >>= 1;
		result++;
	}
	return result > 0 ? result : 1;
}

size_t long_mul(int *result, int *num1, size_t sz1, int *num2, size_t sz2) {
	if (sz1 == 0 || sz2 == 0) {
		*result = 0;
		return 0;
	}
	else {
		int s = get_bin_len(sz1 + sz2 - 1);
		int pr = fpow(PR, 1 << (PR_POW - s), P);
		int ni = multiply(N_REV, 1 << (PR_POW - s), P);
		int pri = fpow(pr, (1 << s) - 1, P);
		int *end;
		s = 1 << s;
		end = num1 + s;
		fft(num1, num1 + s, pr, P);
		fft(num2, num2 + s, pr, P);
		while (num1 < end) {
			*result++ = multiply(*num1++, *num2++, P);
		}
		result -= s;
		end = result + s;
		ifft(result, end, ni, pri, P);
		while (result < end) {
			result[1] += result[0] / X;
			result[0] %= X;
			result++;
		}
		return s;
	}
}

//get input long number from getchar()
//arr:output array, buf: key input buffer, return value: length of arr[]
//example: if DIGIT=3, key board input = 45678, buf = [04,05,06,07,08], len=2, arr=[678, 45]=[a6 02 00 00 2d 00 00 00]
//if DIGIT=2, key board input = 45678, buf = [04,05,06,07,08], len=3, arr=[78,56,4]=[4e 00 00 00 38 00 00 00 04 00 00 00] -- not work properly

// size_t get_int(int *arr, char *buf) {
	// size_t len = 0, result = 0;
	// int ch;
	// char *begin = buf;
	// while (isspace(ch = getchar()));
	// while (ch == '0') ch = getchar();
	// while (isdigit(ch)) {
		// *buf++ = ch - '0';
		// ch = getchar();
		// len++;
	// }
	// while (len >= DIGIT) {
		// int d = 0, i = 1;
		// buf -= DIGIT;
		// d = *buf++;
		// while (i < DIGIT) d = d * 10 + *buf++, i++;
		// len -= DIGIT;
		// buf -= DIGIT;
		// *arr++ = d;
		// result++;
	// }
	// if (begin < buf) {
		// int d = 0;
		// d = *begin++;
		// while (begin < buf) {
			// d = d * 10 + *begin++;
		// }
		// *arr++ = d;
		// result++;
	// }
	// return result;
// }

//padding function from uint8_t to int.
size_t get_int_poly(int *arr, uint8_t *poly, int n) {
	size_t result = 0;
	int end = 2 * n / DIGIT;
	if (DIGIT == 6) end++;
	for (int j = end - 1; j >= 0; j--) {
		for (int i = (DIGIT >> 1) - 1; i >= 0; i--) {
			if (j*(DIGIT >> 1) + i >= n) continue;
			arr[j] = (arr[j] << 8) + (int)poly[j*(DIGIT >> 1) + i];
		}
		result++;
	}

	return result;
};

//Polynomial Modulation
//depadding function from int to uint8_t
void get_hex_poly(int *arr,int szi, uint8_t *poly, int szh, int Q) {
	int temp;
	for (int i = 0; i < szi; i++) {
		temp = arr[i];
		for (int j = 0; j < DIGIT/2; j++) {
			if (2 * i + j < szh) {
				poly[2 * i + j] = (uint8_t)(temp % Q);
			}
			else {
				poly[2 * i + j - szh] = (uint8_t)(poly[2 * i + j - szh] + (uint8_t)((Q - (temp % Q)) % Q) % Q);
			}
			temp >>= 8;
		}
	}
	return;
}

// void solve() {
	// size_t s1, s2, rs;
	// memset(a, 0, N * sizeof(int));
	// memset(b, 0, N * sizeof(int));
	// memset(c, 0, N * sizeof(int));
	// s1 = get_int(a, buffer);
	// s2 = get_int(b, buffer);
	// rs = long_mul(c, a, s1, b, s2);
	// //reverse(c, c + rs);
	// //print(c, rs);
	// //putchar('\n');
// }

size_t get_s(int sz1, int sz2) {
	return get_bin_len(sz1 + sz2 - 1);
}

size_t get_pr(int s) {
	return fpow(PR, 1 << (PR_POW - s), P);
}

size_t get_ni(int s) {
	return multiply(N_REV, 1 << (PR_POW - s), P);
}

size_t get_pri(int s, int pr) {
	return fpow(pr, (1 << s) - 1, P);
}


/*
int main() {
	int n;
	scanf("%d", &n);//Key board input = 123456, then n = (int) 123456 = 0x1e240
	while (n-- > 0) {
		solve();
	}
	return 0;
}
*/