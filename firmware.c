/*
 *  PicoSoC - A simple example SoC using PicoRV32
 *
 *  Copyright (C) 2017  Clifford Wolf <clifford@clifford.at>
 *
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "brlwe.h"
#include "brlwe.c"

#include "alloc.h"
#include "alloc.c"

#define alloc_printf  print

#ifdef ICEBREAKER
#  define MEM_TOTAL 0x20000 /* 128 KB */
#elif HX8KDEMO
#  define MEM_TOTAL 0x200 /* 2 KB */
#else
#  error "Set -DICEBREAKER or -DHX8KDEMO when compiling firmware.c"
#endif

 // a pointer to this is a null pointer, but the compiler does not
 // know that because "sram" is a linker symbol from sections.lds.
extern uint32_t sram;

#define reg_spictrl (*(volatile uint32_t*)0x02000000)
#define reg_uart_clkdiv (*(volatile uint32_t*)0x02000004)
#define reg_uart_data (*(volatile uint32_t*)0x02000008)
#define reg_leds (*(volatile uint32_t*)0x03000000)

#define reg_rng_data (*(volatile uint32_t*)0x03001000)

// --------------------------------------------------------

	#if defined(RBINLWEENC1) && (RBINLWEENC1 == 1)
		#define BRLWE_N 256 // n = 256 : polynomials length
		#define BRLWE_Q 128 // q = 128 : log2(q) = coeffidences data length; causing 1 bit of each byte wasted when q = 128
	#elif defined(RBINLWEENC2) && (RBINLWEENC2 == 1)
	//test bench for configure2(N = 256, Q = 256)
		#define BRLWE_N 256
		#define BRLWE_Q 256
	static const uint8_t test_1[BRLWE_N] = { \
		(uint8_t)43,(uint8_t)98,(uint8_t)100,(uint8_t)95,(uint8_t)218,(uint8_t)37,(uint8_t)156,(uint8_t)50\
		,(uint8_t)45,(uint8_t)89,(uint8_t)128,(uint8_t)74,(uint8_t)14,(uint8_t)182,(uint8_t)53,(uint8_t)216\
		,(uint8_t)235,(uint8_t)220,(uint8_t)90,(uint8_t)98,(uint8_t)41,(uint8_t)129,(uint8_t)116,(uint8_t)44\
		,(uint8_t)105,(uint8_t)116,(uint8_t)127,(uint8_t)124,(uint8_t)75,(uint8_t)53,(uint8_t)70,(uint8_t)171\
		,(uint8_t)178,(uint8_t)155,(uint8_t)53,(uint8_t)123,(uint8_t)148,(uint8_t)39,(uint8_t)228,(uint8_t)233\
		,(uint8_t)20,(uint8_t)75,(uint8_t)82,(uint8_t)238,(uint8_t)91,(uint8_t)155,(uint8_t)80,(uint8_t)108\
		,(uint8_t)69,(uint8_t)39,(uint8_t)82,(uint8_t)90,(uint8_t)226,(uint8_t)200,(uint8_t)181,(uint8_t)154\
		,(uint8_t)21,(uint8_t)29,(uint8_t)195,(uint8_t)42,(uint8_t)18,(uint8_t)81,(uint8_t)113,(uint8_t)22\
		,(uint8_t)146,(uint8_t)211,(uint8_t)71,(uint8_t)194,(uint8_t)136,(uint8_t)140,(uint8_t)48,(uint8_t)165\
		,(uint8_t)111,(uint8_t)46,(uint8_t)167,(uint8_t)11,(uint8_t)26,(uint8_t)39,(uint8_t)121,(uint8_t)134\
		,(uint8_t)27,(uint8_t)198,(uint8_t)36,(uint8_t)37,(uint8_t)230,(uint8_t)44,(uint8_t)117,(uint8_t)1\
		,(uint8_t)156,(uint8_t)165,(uint8_t)147,(uint8_t)226,(uint8_t)15,(uint8_t)200,(uint8_t)2,(uint8_t)53\
		,(uint8_t)94,(uint8_t)123,(uint8_t)224,(uint8_t)103,(uint8_t)0,(uint8_t)29,(uint8_t)57,(uint8_t)23\
		,(uint8_t)88,(uint8_t)168,(uint8_t)58,(uint8_t)189,(uint8_t)134,(uint8_t)244,(uint8_t)146,(uint8_t)81\
		,(uint8_t)49,(uint8_t)239,(uint8_t)243,(uint8_t)6,(uint8_t)110,(uint8_t)31,(uint8_t)225,(uint8_t)51\
		,(uint8_t)17,(uint8_t)13,(uint8_t)221,(uint8_t)1,(uint8_t)197,(uint8_t)253,(uint8_t)68,(uint8_t)26\
		,(uint8_t)69,(uint8_t)171,(uint8_t)80,(uint8_t)40,(uint8_t)174,(uint8_t)130,(uint8_t)203,(uint8_t)74\
		,(uint8_t)208,(uint8_t)234,(uint8_t)103,(uint8_t)142,(uint8_t)141,(uint8_t)120,(uint8_t)173,(uint8_t)189\
		,(uint8_t)92,(uint8_t)28,(uint8_t)14,(uint8_t)31,(uint8_t)78,(uint8_t)157,(uint8_t)99,(uint8_t)154\
		,(uint8_t)64,(uint8_t)111,(uint8_t)38,(uint8_t)11,(uint8_t)122,(uint8_t)130,(uint8_t)245,(uint8_t)177\
		,(uint8_t)89,(uint8_t)203,(uint8_t)133,(uint8_t)255,(uint8_t)76,(uint8_t)131,(uint8_t)216,(uint8_t)218\
		,(uint8_t)81,(uint8_t)92,(uint8_t)183,(uint8_t)254,(uint8_t)89,(uint8_t)234,(uint8_t)244,(uint8_t)52\
		,(uint8_t)125,(uint8_t)150,(uint8_t)20,(uint8_t)93,(uint8_t)165,(uint8_t)175,(uint8_t)172,(uint8_t)89\
		,(uint8_t)123,(uint8_t)50,(uint8_t)207,(uint8_t)107,(uint8_t)224,(uint8_t)12,(uint8_t)250,(uint8_t)138\
		,(uint8_t)227,(uint8_t)116,(uint8_t)34,(uint8_t)94,(uint8_t)85,(uint8_t)194,(uint8_t)203,(uint8_t)139\
		,(uint8_t)71,(uint8_t)75,(uint8_t)83,(uint8_t)11,(uint8_t)8,(uint8_t)121,(uint8_t)26,(uint8_t)217\
		,(uint8_t)98,(uint8_t)241,(uint8_t)140,(uint8_t)114,(uint8_t)101,(uint8_t)221,(uint8_t)127,(uint8_t)180\
		,(uint8_t)169,(uint8_t)250,(uint8_t)189,(uint8_t)21,(uint8_t)166,(uint8_t)240,(uint8_t)227,(uint8_t)73\
		,(uint8_t)40,(uint8_t)118,(uint8_t)128,(uint8_t)80,(uint8_t)181,(uint8_t)199,(uint8_t)187,(uint8_t)245\
		,(uint8_t)120,(uint8_t)224,(uint8_t)61,(uint8_t)153,(uint8_t)71,(uint8_t)166,(uint8_t)56,(uint8_t)248\
		,(uint8_t)211,(uint8_t)169,(uint8_t)39,(uint8_t)245,(uint8_t)55,(uint8_t)90,(uint8_t)219,(uint8_t)95\
		,(uint8_t)106,(uint8_t)202,(uint8_t)94,(uint8_t)15,(uint8_t)53,(uint8_t)227,(uint8_t)165,(uint8_t)69};
	static const uint8_t test_2[BRLWE_N] = { \
		  (uint8_t)1, (uint8_t)0, (uint8_t)1, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)1 \
		, (uint8_t)0, (uint8_t)1, (uint8_t)1, (uint8_t)0, (uint8_t)1, (uint8_t)0, (uint8_t)1, (uint8_t)1 \
		, (uint8_t)0, (uint8_t)1, (uint8_t)0, (uint8_t)0, (uint8_t)1, (uint8_t)1, (uint8_t)1, (uint8_t)0 \
		, (uint8_t)1, (uint8_t)1, (uint8_t)1, (uint8_t)1, (uint8_t)0, (uint8_t)0, (uint8_t)1, (uint8_t)1 \
		, (uint8_t)1, (uint8_t)0, (uint8_t)1, (uint8_t)0, (uint8_t)1, (uint8_t)1, (uint8_t)0, (uint8_t)1 \
		, (uint8_t)1, (uint8_t)1, (uint8_t)0, (uint8_t)0, (uint8_t)1, (uint8_t)0, (uint8_t)0, (uint8_t)0 \
		, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)1 \
		, (uint8_t)1, (uint8_t)1, (uint8_t)0, (uint8_t)1, (uint8_t)0, (uint8_t)1, (uint8_t)0, (uint8_t)0 \
		, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)1 \
		, (uint8_t)1, (uint8_t)0, (uint8_t)1, (uint8_t)1, (uint8_t)0, (uint8_t)1, (uint8_t)0, (uint8_t)0 \
		, (uint8_t)1, (uint8_t)0, (uint8_t)1, (uint8_t)0, (uint8_t)1, (uint8_t)0, (uint8_t)0, (uint8_t)1 \
		, (uint8_t)1, (uint8_t)0, (uint8_t)0, (uint8_t)1, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0 \
		, (uint8_t)0, (uint8_t)1, (uint8_t)1, (uint8_t)0, (uint8_t)0, (uint8_t)1, (uint8_t)1, (uint8_t)0 \
		, (uint8_t)1, (uint8_t)1, (uint8_t)0, (uint8_t)1, (uint8_t)0, (uint8_t)0, (uint8_t)1, (uint8_t)0 \
		, (uint8_t)1, (uint8_t)1, (uint8_t)0, (uint8_t)0, (uint8_t)1, (uint8_t)1, (uint8_t)0, (uint8_t)0 \
		, (uint8_t)0, (uint8_t)0, (uint8_t)1, (uint8_t)0, (uint8_t)1, (uint8_t)0, (uint8_t)0, (uint8_t)1 \
		, (uint8_t)1, (uint8_t)1, (uint8_t)1, (uint8_t)1, (uint8_t)1, (uint8_t)1, (uint8_t)1, (uint8_t)1 \
		, (uint8_t)0, (uint8_t)0, (uint8_t)1, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)1 \
		, (uint8_t)0, (uint8_t)0, (uint8_t)1, (uint8_t)0, (uint8_t)1, (uint8_t)0, (uint8_t)1, (uint8_t)1 \
		, (uint8_t)0, (uint8_t)1, (uint8_t)0, (uint8_t)1, (uint8_t)0, (uint8_t)1, (uint8_t)1, (uint8_t)0 \
		, (uint8_t)1, (uint8_t)1, (uint8_t)1, (uint8_t)1, (uint8_t)1, (uint8_t)1, (uint8_t)0, (uint8_t)0 \
		, (uint8_t)1, (uint8_t)1, (uint8_t)1, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)1, (uint8_t)0 \
		, (uint8_t)1, (uint8_t)1, (uint8_t)1, (uint8_t)0, (uint8_t)1, (uint8_t)1, (uint8_t)1, (uint8_t)1 \
		, (uint8_t)1, (uint8_t)1, (uint8_t)0, (uint8_t)1, (uint8_t)0, (uint8_t)1, (uint8_t)0, (uint8_t)0 \
		, (uint8_t)1, (uint8_t)0, (uint8_t)0, (uint8_t)1, (uint8_t)1, (uint8_t)1, (uint8_t)1, (uint8_t)1 \
		, (uint8_t)0, (uint8_t)0, (uint8_t)1, (uint8_t)1, (uint8_t)1, (uint8_t)1, (uint8_t)1, (uint8_t)0 \
		, (uint8_t)0, (uint8_t)1, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)1, (uint8_t)0 \
		, (uint8_t)1, (uint8_t)1, (uint8_t)0, (uint8_t)1, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)1 \
		, (uint8_t)1, (uint8_t)0, (uint8_t)0, (uint8_t)1, (uint8_t)0, (uint8_t)1, (uint8_t)1, (uint8_t)0 \
		, (uint8_t)1, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)1, (uint8_t)1, (uint8_t)0 \
		, (uint8_t)0, (uint8_t)0, (uint8_t)1, (uint8_t)1, (uint8_t)0, (uint8_t)0, (uint8_t)1, (uint8_t)0 \
		, (uint8_t)1, (uint8_t)1, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)1, (uint8_t)1 };
#elif defined(RBINLWEENCT) && (RBINLWEENCT == 1)
	//test bench for configure test(N = 4, Q = 256)
	#define BRLWE_N 4
	#define BRLWE_Q 256
	uint8_t test_1[4] = { (uint8_t)30, (uint8_t)20, (uint8_t)150 , (uint8_t)80 };
	uint8_t test_2[4] = { (uint8_t)1, (uint8_t)0, (uint8_t)1, (uint8_t)1 };
#else
	#define BRLWE_N 512
	#define BRLWE_Q 256
#endif

//--------------------------------------------------------------------------
extern uint32_t flashio_worker_begin;
extern uint32_t flashio_worker_end;

void flashio(uint8_t *data, int len, uint8_t wrencmd)
{
	uint32_t func[&flashio_worker_end - &flashio_worker_begin];

	uint32_t *src_ptr = &flashio_worker_begin;
	uint32_t *dst_ptr = func;

	while (src_ptr != &flashio_worker_end)
		*(dst_ptr++) = *(src_ptr++);

	((void(*)(uint8_t*, uint32_t, uint32_t))func)(data, len, wrencmd);
}

#ifdef HX8KDEMO
void set_flash_qspi_flag()
{
	uint8_t buffer[8];
	uint32_t addr_cr1v = 0x800002;

	// Read Any Register (RDAR 65h)
	buffer[0] = 0x65;
	buffer[1] = addr_cr1v >> 16;
	buffer[2] = addr_cr1v >> 8;
	buffer[3] = addr_cr1v;
	buffer[4] = 0; // dummy
	buffer[5] = 0; // rdata
	flashio(buffer, 6, 0);
	uint8_t cr1v = buffer[5];

	// Write Enable (WREN 06h) + Write Any Register (WRAR 71h)
	buffer[0] = 0x71;
	buffer[1] = addr_cr1v >> 16;
	buffer[2] = addr_cr1v >> 8;
	buffer[3] = addr_cr1v;
	buffer[4] = cr1v | 2; // Enable QSPI
	flashio(buffer, 5, 0x06);
}

void set_flash_latency(uint8_t value)
{
	reg_spictrl = (reg_spictrl & ~0x007f0000) | ((value & 15) << 16);

	uint32_t addr = 0x800004;
	uint8_t buffer_wr[5] = { 0x71, addr >> 16, addr >> 8, addr, 0x70 | value };
	flashio(buffer_wr, 5, 0x06);
}

void set_flash_mode_spi()
{
	reg_spictrl = (reg_spictrl & ~0x00700000) | 0x00000000;
}

void set_flash_mode_dual()
{
	reg_spictrl = (reg_spictrl & ~0x00700000) | 0x00400000;
}

void set_flash_mode_quad()
{
	reg_spictrl = (reg_spictrl & ~0x00700000) | 0x00200000;
}

void set_flash_mode_qddr()
{
	reg_spictrl = (reg_spictrl & ~0x00700000) | 0x00600000;
}
#endif

#ifdef ICEBREAKER
void set_flash_qspi_flag()
{
	uint8_t buffer[8];

	// Read Configuration Registers (RDCR1 35h)
	buffer[0] = 0x35;
	buffer[1] = 0x00; // rdata
	flashio(buffer, 2, 0);
	uint8_t sr2 = buffer[1];

	// Write Enable Volatile (50h) + Write Status Register 2 (31h)
	buffer[0] = 0x31;
	buffer[1] = sr2 | 2; // Enable QSPI
	flashio(buffer, 2, 0x50);
}

void set_flash_mode_spi()
{
	reg_spictrl = (reg_spictrl & ~0x007f0000) | 0x00000000;
}

void set_flash_mode_dual()
{
	reg_spictrl = (reg_spictrl & ~0x007f0000) | 0x00400000;
}

void set_flash_mode_quad()
{
	reg_spictrl = (reg_spictrl & ~0x007f0000) | 0x00240000;
}

void set_flash_mode_qddr()
{
	reg_spictrl = (reg_spictrl & ~0x007f0000) | 0x00670000;
}

void enable_flash_crm()
{
	reg_spictrl |= 0x00100000;
}
#endif

// --------------------------------------------------------

//count the total number of error occur in the system
//return the error times, length(str1)=length(str2)=BRLWE_N;
static int counterr(uint8_t* str1, uint8_t* str2) {
	int i;
	int count = 0;
	for (i = 0; i < BRLWE_N; ++i) {
		if (str1[i] == str2[i])
			count++;
	}
	return (BRLWE_N - count);
}

void putchar(char c)
{
	if (c == '\n')
		putchar('\r');
	reg_uart_data = c;
}

void print(const char *p)
{
	while (*p)
		putchar(*(p++));
}

void print_hex(uint8_t v, int digits)
{
	for (int i = 7; i >= 0; i--) {
		char c = "0123456789abcdef"[(v >> (4 * i)) & 15];
		if (c == '0' && i >= digits) continue;
		putchar(c);
		digits = i;
	}
}

static void phex(uint8_t* str)
{
	int i, j;
	for (i = 0, j = 1; i < BRLWE_N; ++i, ++j) {
		print_hex(str[i],2);//updated, original:printf("%.2x", str[i]);
		
		if (j == 16) {
			print("\r\n");
			j = 0;
		}
		else if (j%4==0 && j!=0){
			print(" ");
		}
	}

	print("\r\n");
}

void print_dec(uint32_t v)
{
	if (v >= 10000000000) {
		print(">=10Billion");
		return;
	}
	
	if (v >= 9000000000) { putchar('9'); v -= 9000000000; }
	else if (v >= 8000000000) { putchar('8'); v -= 8000000000; }
	else if (v >= 7000000000) { putchar('7'); v -= 7000000000; }
	else if (v >= 6000000000) { putchar('6'); v -= 6000000000; }
	else if (v >= 5000000000) { putchar('5'); v -= 5000000000; }
	else if (v >= 4000000000) { putchar('4'); v -= 4000000000; }
	else if (v >= 3000000000) { putchar('3'); v -= 3000000000; }
	else if (v >= 2000000000) { putchar('2'); v -= 2000000000; }
	else if (v >= 1000000000) { putchar('1'); v -= 1000000000; }
	
	if (v >= 900000000) { putchar('9'); v -= 900000000; }
	else if (v >= 800000000) { putchar('8'); v -= 800000000; }
	else if (v >= 700000000) { putchar('7'); v -= 700000000; }
	else if (v >= 600000000) { putchar('6'); v -= 600000000; }
	else if (v >= 500000000) { putchar('5'); v -= 500000000; }
	else if (v >= 400000000) { putchar('4'); v -= 400000000; }
	else if (v >= 300000000) { putchar('3'); v -= 300000000; }
	else if (v >= 200000000) { putchar('2'); v -= 200000000; }
	else if (v >= 100000000) { putchar('1'); v -= 100000000; }
	
	if (v >= 90000000) { putchar('9'); v -= 90000000; }
	else if (v >= 80000000) { putchar('8'); v -= 80000000; }
	else if (v >= 70000000) { putchar('7'); v -= 70000000; }
	else if (v >= 60000000) { putchar('6'); v -= 60000000; }
	else if (v >= 50000000) { putchar('5'); v -= 50000000; }
	else if (v >= 40000000) { putchar('4'); v -= 40000000; }
	else if (v >= 30000000) { putchar('3'); v -= 30000000; }
	else if (v >= 20000000) { putchar('2'); v -= 20000000; }
	else if (v >= 10000000) { putchar('1'); v -= 10000000; }	
	
	if (v >= 9000000) { putchar('9'); v -= 9000000; }
	else if (v >= 8000000) { putchar('8'); v -= 8000000; }
	else if (v >= 7000000) { putchar('7'); v -= 7000000; }
	else if (v >= 6000000) { putchar('6'); v -= 6000000; }
	else if (v >= 5000000) { putchar('5'); v -= 5000000; }
	else if (v >= 4000000) { putchar('4'); v -= 4000000; }
	else if (v >= 3000000) { putchar('3'); v -= 3000000; }
	else if (v >= 2000000) { putchar('2'); v -= 2000000; }
	else if (v >= 1000000) { putchar('1'); v -= 1000000; }
	
	if (v >= 900000) { putchar('9'); v -= 900000; }
	else if (v >= 800000) { putchar('8'); v -= 800000; }
	else if (v >= 700000) { putchar('7'); v -= 700000; }
	else if (v >= 600000) { putchar('6'); v -= 600000; }
	else if (v >= 500000) { putchar('5'); v -= 500000; }
	else if (v >= 400000) { putchar('4'); v -= 400000; }
	else if (v >= 300000) { putchar('3'); v -= 300000; }
	else if (v >= 200000) { putchar('2'); v -= 200000; }
	else if (v >= 100000) { putchar('1'); v -= 100000; }
	
	if (v >= 90000) { putchar('9'); v -= 90000; }
	else if (v >= 80000) { putchar('8'); v -= 80000; }
	else if (v >= 70000) { putchar('7'); v -= 70000; }
	else if (v >= 60000) { putchar('6'); v -= 60000; }
	else if (v >= 50000) { putchar('5'); v -= 50000; }
	else if (v >= 40000) { putchar('4'); v -= 40000; }
	else if (v >= 30000) { putchar('3'); v -= 30000; }
	else if (v >= 20000) { putchar('2'); v -= 20000; }
	else if (v >= 10000) { putchar('1'); v -= 10000; }
	
	if (v >= 9000) { putchar('9'); v -= 9000; }
	else if (v >= 8000) { putchar('8'); v -= 8000; }
	else if (v >= 7000) { putchar('7'); v -= 7000; }
	else if (v >= 6000) { putchar('6'); v -= 6000; }
	else if (v >= 5000) { putchar('5'); v -= 5000; }
	else if (v >= 4000) { putchar('4'); v -= 4000; }
	else if (v >= 3000) { putchar('3'); v -= 3000; }
	else if (v >= 2000) { putchar('2'); v -= 2000; }
	else if (v >= 1000) { putchar('1'); v -= 1000; }

	if (v >= 900) { putchar('9'); v -= 900; }
	else if (v >= 800) { putchar('8'); v -= 800; }
	else if (v >= 700) { putchar('7'); v -= 700; }
	else if (v >= 600) { putchar('6'); v -= 600; }
	else if (v >= 500) { putchar('5'); v -= 500; }
	else if (v >= 400) { putchar('4'); v -= 400; }
	else if (v >= 300) { putchar('3'); v -= 300; }
	else if (v >= 200) { putchar('2'); v -= 200; }
	else if (v >= 100) { putchar('1'); v -= 100; }

	if (v >= 90) { putchar('9'); v -= 90; }
	else if (v >= 80) { putchar('8'); v -= 80; }
	else if (v >= 70) { putchar('7'); v -= 70; }
	else if (v >= 60) { putchar('6'); v -= 60; }
	else if (v >= 50) { putchar('5'); v -= 50; }
	else if (v >= 40) { putchar('4'); v -= 40; }
	else if (v >= 30) { putchar('3'); v -= 30; }
	else if (v >= 20) { putchar('2'); v -= 20; }
	else if (v >= 10) { putchar('1'); v -= 10; }

	if (v >= 9) { putchar('9'); v -= 9; }
	else if (v >= 8) { putchar('8'); v -= 8; }
	else if (v >= 7) { putchar('7'); v -= 7; }
	else if (v >= 6) { putchar('6'); v -= 6; }
	else if (v >= 5) { putchar('5'); v -= 5; }
	else if (v >= 4) { putchar('4'); v -= 4; }
	else if (v >= 3) { putchar('3'); v -= 3; }
	else if (v >= 2) { putchar('2'); v -= 2; }
	else if (v >= 1) { putchar('1'); v -= 1; }
	else putchar('0');
}

char getchar_prompt(char *prompt)
{
	//read the uart recv register while sending string.
	int32_t c = -1;

	uint32_t cycles_begin, cycles_now, cycles;
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_begin));
	//insert the asymbly code of c language
	
	reg_leds = ~0;

	if (prompt)
		print(prompt);

	while (c == -1) {
		__asm__ volatile ("rdcycle %0" : "=r"(cycles_now));
		cycles = cycles_now - cycles_begin;
		if (cycles > 12000000) {
			if (prompt)
				print(prompt);
			cycles_begin = cycles_now;
			reg_leds = ~reg_leds;
		}
		c = reg_uart_data;
	}

	reg_leds = 0;
	return c;
}

char getchar()
{
	return getchar_prompt(0);
}

void cmd_print_spi_state()
{
	print("SPI State:\n");

	print("  LATENCY ");
	print_dec((reg_spictrl >> 16) & 15);
	print("\n");

	print("  DDR ");
	if ((reg_spictrl & (1 << 22)) != 0)
		print("ON\n");
	else
		print("OFF\n");

	print("  QSPI ");
	if ((reg_spictrl & (1 << 21)) != 0)
		print("ON\n");
	else
		print("OFF\n");

	print("  CRM ");
	if ((reg_spictrl & (1 << 20)) != 0)
		print("ON\n");
	else
		print("OFF\n");
}

uint32_t xorshift32(uint32_t *state)
{
	/* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
	uint32_t x = *state;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	*state = x;

	return x;
}

void cmd_memtest()
{
	int cyc_count = 5;
	int stride = 256;
	uint32_t state;

	volatile uint32_t *base_word = (uint32_t *)0;
	volatile uint8_t *base_byte = (uint8_t *)0;

	print("Running memtest ");

	// Walk in stride increments, word access
	for (int i = 1; i <= cyc_count; i++) {
		state = i;

		for (int word = 0; word < MEM_TOTAL / sizeof(int); word += stride) {
			*(base_word + word) = xorshift32(&state);
		}

		state = i;

		for (int word = 0; word < MEM_TOTAL / sizeof(int); word += stride) {
			if (*(base_word + word) != xorshift32(&state)) {
				print(" ***FAILED WORD*** at ");
				print_hex(4 * word, 4);
				print("\n");
				return;
			}
		}

		print(".");
	}

	// Byte access
	for (int byte = 0; byte < 128; byte++) {
		*(base_byte + byte) = (uint8_t)byte;
	}

	for (int byte = 0; byte < 128; byte++) {
		if (*(base_byte + byte) != (uint8_t)byte) {
			print(" ***FAILED BYTE*** at ");
			print_hex(byte, 4);
			print("\n");
			return;
		}
	}

	print(" passed\n");
}


/*
********************************************************************************
*                                   内存打印函数                     
*
* 描述    : 打印内存系统中每一个内存块的信息
*
* 参数  : 无
*
* 返回  : 无
********************************************************************************
*/
void mem_print(void)
{
    unsigned int i = 0;
    mem_block     *head_node, *tmp_node;
 
    if(mem_init_flag < 0)
    {
        print("未初始化,先初始化.\r\n");
        mem_init();
    }
    head_node = tmp_node = (mem_block *)HEAD_NODE;
    print("\r\n#############################\r\n");
    while(1)
    {
        print("\r\nNO.");
		print_dec(i++);
		print(":\r\n");
        print("blk_ptr:0x");
        print_hex(tmp_node,8);
        print("\r\n");
        print("mem_ptr:0x");
		print_hex(tmp_node->mem_ptr,8);
		print("\r\n");
        print("nxt_ptr:0x");
		print_hex(tmp_node->nxt_ptr,8);
		print("\r\n");
        print("mem_size:");
		print_dec(tmp_node->mem_size);
		print("\r\n");
        print("mem_sta:");
		print_dec(tmp_node->mem_sta);
		print("\r\n");
 
        tmp_node = tmp_node->nxt_ptr;
        if(tmp_node == head_node)
        {
            break;
        }
    }
    print("\r\n#############################\r\n");
}
 
void buff_print(unsigned char *buf,unsigned int len)
{
    unsigned int i;
 
    print("\r\n");
    for(i=0;i<len;i++)
    {
        if(i%16 == 0 && i != 0)
        {
            print("\r\n");
        }
        print("0x");
		print_hex(buf[i],2);
		print(",");
        //print("%c",buf[i]);
    }
    print("\r\n");
}
 /*
void *m_malloc(unsigned nbytes)
{
    return malloc(nbytes);
}
 
void m_free(void *ap)
{
    free(ap);
}*/
 
typedef char (*array)[4];
/*
********************************************************************************
*                                   内存分配函数测试                     
*
* 描述    : 测试内存分配系统中每一个函数的功能
*
* 参数  : 无
*
* 返回  : 无
********************************************************************************
*/
void alloc_test(void)
{  
    
	array ptr = NULL;
    unsigned int i,j;
 
    print("Ptr1:");
	print_dec(sizeof(ptr));
	print("\r\n");
    ptr = m_malloc(4);
    if(ptr == NULL)
    {
        print("malloc failed.\r\n");
        return;
    }
	alloc_printf("malloc mark 2.\r\n");
    mem_print();
    
    for(i=0;i<4;i++)
    {
        for(j=0;j<4;j++)
        {
            ptr[i][j] = i;
        }
    }
    m_free(ptr);
    mem_print();
    buff_print((unsigned char *)ptr, 16);
}

/*------------rng component------------------*/
static void setseed32(uint8_t* str)
{	
	int i;
	uint32_t tmp = 0x00000000;
	for (i = 0; i < 4; i++) {
		tmp = tmp << 8;
		tmp = tmp + str[i];
	}//for uint8_t hex to uint32_t
	
	reg_rng_data = tmp;
}

static int getrandom(uint8_t* str)
{	
	uint32_t tmp = 0xffffffff;//impossible value of the RNG output
	
		while (tmp == 0xffffffff) {
			tmp = reg_rng_data;
		}// if RNG is not ready(tmp=0xffff_ffff), wait.
		
		int i = 0;
		int j = 0;
		for (j = 0; j < BRLWE_N/4; j++){
			while (tmp == 0xffffffff) {
				tmp = reg_rng_data;
			}// if RNG is not ready(tmp=0xffff_ffff), wait.
			
			for (i = 0; i < 4; i++)	{
				str[i]=tmp/0x01000000;
				tmp = tmp << 8;
			}//for uint32_t hex to uint8_t
			
			tmp = 0xffffffff;
		}

	return 1;
}

static void debug_rdcycle()
{
	uint32_t cycles_begin;
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_begin));
	
	uint32_t tmp = cycles_begin;
	
	print("Cycle now in dec is : ");
	print_dec(tmp);
	print("\r\n");
	
	/*
	uint8_t str[4] = { (uint8_t)0, (uint8_t)0, (uint8_t)0 , (uint8_t)0 };
	int i = 0;
	for (i = 0; i < 4; i++)	{
		str[i]=tmp/0x01000000;
		tmp = tmp << 8;
	}//for uint32_t hex to uint8_t
	print("Cycle now in hex is : 0x");
	phex(str);
	print("\r\n");*/
	
}

// --------------------------------------------------------

void cmd_read_flash_id()
{
	uint8_t buffer[17] = { 0x9F, /* zeros */ };
	flashio(buffer, 17, 0);

	for (int i = 1; i <= 16; i++) {
		putchar(' ');
		print_hex(buffer[i], 2);
	}
	putchar('\n');
}

// --------------------------------------------------------

#ifdef HX8KDEMO
uint8_t cmd_read_flash_regs_print(uint32_t addr, const char *name)
{
	set_flash_latency(8);

	uint8_t buffer[6] = { 0x65, addr >> 16, addr >> 8, addr, 0, 0 };
	flashio(buffer, 6, 0);

	print("0x");
	print_hex(addr, 6);
	print(" ");
	print(name);
	print(" 0x");
	print_hex(buffer[5], 2);
	print("\n");

	return buffer[5];
}

void cmd_read_flash_regs()
{
	print("\n");
	uint8_t sr1v = cmd_read_flash_regs_print(0x800000, "SR1V");
	uint8_t sr2v = cmd_read_flash_regs_print(0x800001, "SR2V");
	uint8_t cr1v = cmd_read_flash_regs_print(0x800002, "CR1V");
	uint8_t cr2v = cmd_read_flash_regs_print(0x800003, "CR2V");
	uint8_t cr3v = cmd_read_flash_regs_print(0x800004, "CR3V");
	uint8_t vdlp = cmd_read_flash_regs_print(0x800005, "VDLP");
}
#endif

#ifdef ICEBREAKER
uint8_t cmd_read_flash_reg(uint8_t cmd)
{
	uint8_t buffer[2] = { cmd, 0 };
	flashio(buffer, 2, 0);
	return buffer[1];
}

void print_reg_bit(int val, const char *name)
{
	for (int i = 0; i < 12; i++) {
		if (*name == 0)
			putchar(' ');
		else
			putchar(*(name++));
	}

	putchar(val ? '1' : '0');
	putchar('\n');
}

void cmd_read_flash_regs()
{
	putchar('\n');

	uint8_t sr1 = cmd_read_flash_reg(0x05);
	uint8_t sr2 = cmd_read_flash_reg(0x35);
	uint8_t sr3 = cmd_read_flash_reg(0x15);

	print_reg_bit(sr1 & 0x01, "S0  (BUSY)");
	print_reg_bit(sr1 & 0x02, "S1  (WEL)");
	print_reg_bit(sr1 & 0x04, "S2  (BP0)");
	print_reg_bit(sr1 & 0x08, "S3  (BP1)");
	print_reg_bit(sr1 & 0x10, "S4  (BP2)");
	print_reg_bit(sr1 & 0x20, "S5  (TB)");
	print_reg_bit(sr1 & 0x40, "S6  (SEC)");
	print_reg_bit(sr1 & 0x80, "S7  (SRP)");
	putchar('\n');

	print_reg_bit(sr2 & 0x01, "S8  (SRL)");
	print_reg_bit(sr2 & 0x02, "S9  (QE)");
	print_reg_bit(sr2 & 0x04, "S10 ----");
	print_reg_bit(sr2 & 0x08, "S11 (LB1)");
	print_reg_bit(sr2 & 0x10, "S12 (LB2)");
	print_reg_bit(sr2 & 0x20, "S13 (LB3)");
	print_reg_bit(sr2 & 0x40, "S14 (CMP)");
	print_reg_bit(sr2 & 0x80, "S15 (SUS)");
	putchar('\n');

	print_reg_bit(sr3 & 0x01, "S16 ----");
	print_reg_bit(sr3 & 0x02, "S17 ----");
	print_reg_bit(sr3 & 0x04, "S18 (WPS)");
	print_reg_bit(sr3 & 0x08, "S19 ----");
	print_reg_bit(sr3 & 0x10, "S20 ----");
	print_reg_bit(sr3 & 0x20, "S21 (DRV0)");
	print_reg_bit(sr3 & 0x40, "S22 (DRV1)");
	print_reg_bit(sr3 & 0x80, "S23 (HOLD)");
	putchar('\n');
}
#endif

// --------------------------------------------------------

uint32_t cmd_benchmark(bool verbose, uint32_t *instns_p)
{
	uint8_t data[256];
	uint32_t *words = (void*)data;

	uint32_t x32 = 314159265;

	uint32_t cycles_begin, cycles_end;
	uint32_t instns_begin, instns_end;
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_begin));
	__asm__ volatile ("rdinstret %0" : "=r"(instns_begin));

	for (int i = 0; i < 20; i++)
	{
		for (int k = 0; k < 256; k++)
		{
			x32 ^= x32 << 13;
			x32 ^= x32 >> 17;
			x32 ^= x32 << 5;
			data[k] = x32;
		}

		for (int k = 0, p = 0; k < 256; k++)
		{
			if (data[k])
				data[p++] = k;
		}

		for (int k = 0, p = 0; k < 64; k++)
		{
			x32 = x32 ^ words[k];
		}
	}

	__asm__ volatile ("rdcycle %0" : "=r"(cycles_end));
	__asm__ volatile ("rdinstret %0" : "=r"(instns_end));

	if (verbose)
	{
		print("Cycles: 0x");
		print_hex(cycles_end - cycles_begin, 8);
		putchar('\n');

		print("Instns: 0x");
		print_hex(instns_end - instns_begin, 8);
		putchar('\n');

		print("Chksum: 0x");
		print_hex(x32, 8);
		putchar('\n');
	}

	if (instns_p)
		*instns_p = instns_end - instns_begin;

	return cycles_end - cycles_begin;
}

// --------------------------------------------------------

#ifdef HX8KDEMO
void cmd_benchmark_all()
{
	uint32_t instns = 0;

	print("default        ");
	reg_spictrl = (reg_spictrl & ~0x00700000) | 0x00000000;
	print(": ");
	print_hex(cmd_benchmark(false, &instns), 8);
	putchar('\n');

	for (int i = 8; i > 0; i--)
	{
		print("dspi-");
		print_dec(i);
		print("         ");

		set_flash_latency(i);
		reg_spictrl = (reg_spictrl & ~0x00700000) | 0x00400000;

		print(": ");
		print_hex(cmd_benchmark(false, &instns), 8);
		putchar('\n');
	}

	for (int i = 8; i > 0; i--)
	{
		print("dspi-crm-");
		print_dec(i);
		print("     ");

		set_flash_latency(i);
		reg_spictrl = (reg_spictrl & ~0x00700000) | 0x00500000;

		print(": ");
		print_hex(cmd_benchmark(false, &instns), 8);
		putchar('\n');
	}

	for (int i = 8; i > 0; i--)
	{
		print("qspi-");
		print_dec(i);
		print("         ");

		set_flash_latency(i);
		reg_spictrl = (reg_spictrl & ~0x00700000) | 0x00200000;

		print(": ");
		print_hex(cmd_benchmark(false, &instns), 8);
		putchar('\n');
	}

	for (int i = 8; i > 0; i--)
	{
		print("qspi-crm-");
		print_dec(i);
		print("     ");

		set_flash_latency(i);
		reg_spictrl = (reg_spictrl & ~0x00700000) | 0x00300000;

		print(": ");
		print_hex(cmd_benchmark(false, &instns), 8);
		putchar('\n');
	}

	for (int i = 8; i > 0; i--)
	{
		print("qspi-ddr-");
		print_dec(i);
		print("     ");

		set_flash_latency(i);
		reg_spictrl = (reg_spictrl & ~0x00700000) | 0x00600000;

		print(": ");
		print_hex(cmd_benchmark(false, &instns), 8);
		putchar('\n');
	}

	for (int i = 8; i > 0; i--)
	{
		print("qspi-ddr-crm-");
		print_dec(i);
		print(" ");

		set_flash_latency(i);
		reg_spictrl = (reg_spictrl & ~0x00700000) | 0x00700000;

		print(": ");
		print_hex(cmd_benchmark(false, &instns), 8);
		putchar('\n');
	}

	print("instns         : ");
	print_hex(instns, 8);
	putchar('\n');
}
#endif

#ifdef ICEBREAKER
void cmd_benchmark_all()
{
	uint32_t instns = 0;

	print("default   ");
	set_flash_mode_spi();
	print_hex(cmd_benchmark(false, &instns), 8);
	putchar('\n');

	print("dual      ");
	set_flash_mode_dual();
	print_hex(cmd_benchmark(false, &instns), 8);
	putchar('\n');

	// print("dual-crm  ");
	// enable_flash_crm();
	// print_hex(cmd_benchmark(false, &instns), 8);
	// putchar('\n');

	print("quad      ");
	set_flash_mode_quad();
	print_hex(cmd_benchmark(false, &instns), 8);
	putchar('\n');

	print("quad-crm  ");
	enable_flash_crm();
	print_hex(cmd_benchmark(false, &instns), 8);
	putchar('\n');

	print("qddr      ");
	set_flash_mode_qddr();
	print_hex(cmd_benchmark(false, &instns), 8);
	putchar('\n');

	print("qddr-crm  ");
	enable_flash_crm();
	print_hex(cmd_benchmark(false, &instns), 8);
	putchar('\n');

}
#endif

void cmd_echo()
{
	print("Return to menu by sending '!'\n\n");
	char c;
	while ((c = getchar()) != '!')
		putchar(c);
}

// --------------------------------------------------------

void main()
{
	reg_uart_clkdiv = 104;
	print("Booting..\n");

	reg_leds = 63;//=0x3f=8'b0011_1111
	set_flash_qspi_flag();

	reg_leds = 127;//=0x7f=8'b0111_1111
	while (getchar_prompt("Press ENTER to continue..\n") != '\r') {  /* wait */  };	
	
	//test: memory allocate testing
	cmd_memtest();
	//print("\rmem print to ");
	//extern uint32_t _heap_start;
	//print_hex(&_heap_start,8);
	//print_hex(mem,8);
	//print_dec(sizeof(mem));
	mem_init();
	//mem_print();
	debug_rdcycle();
	alloc_test();
	debug_rdcycle();
	
	
	
	//uint8_t test_3[4] = { (uint8_t)130, (uint8_t)140, (uint8_t)210 , (uint8_t)156 };
	/*
	uint8_t test_4[4] = { (uint8_t)40, (uint8_t)80, (uint8_t)100 , (uint8_t)10 };
	uint8_t test_5[4] = { (uint8_t)0, (uint8_t)0, (uint8_t)0 , (uint8_t)0 };
	*/
	
	//test: Polynomial initialization step
	//struct BRLWE_Ring_polynomials a, m, n;
	//print("\nPolynomial initialization step:\r\n");
	//BRLWE_init_hex(&a, test_1, 0);
	//BRLWE_init_hex(&m, test_2, 0);	
	//print("test1 = \n");
	//phex(a.polynomial);
	//print("test2 = \n");
	//phex(m.polynomial);
	//print("test3 = \n");
	//phex(n.polynomial);

	
	//test: RNG generation
	//print("\nRNG generation:\r\n");
	//setseed32(test_3);
	
	//uint8_t test_4[BRLWE_N];
	//getrandom(test_4);
	//BRLWE_init_hex(&m, test_4, 0);
	//debug_rdcycle();
	//print("Random number = \n");
	//phex(m.polynomial);
	
	/*
	//test: Math-operation subfunctions
	print("\nMath-operation subfunctions:\r\n");
	print("test1 + test2 = \n");
	phex(Ring_add(a, m).polynomial);
	print("test1 - test2 = \n");
	phex(Ring_sub(a, m).polynomial);
	print("test1 * test2 = \n");
	phex(Simple_Ring_mul(a, m).polynomial);

	//test: Key Generation step
	struct BRLWE_Ring_polynomials pk, sk;
	struct BRLWE_Ring_polynomials2 key;
	print("\nKey Generation:\n");
	key = BRLWE_Key_Gen(a);
	pk=key.c1;
	sk=key.c2;
	print("public key = \n");
	phex(key.c1.polynomial);
	print("secret key = \n");
	phex(key.c2.polynomial);
	
	//test: Encryption step
	print("\nEncryption:\n");
	print("a = \n");
	phex(a.polynomial);
	print("public key = \n");
	phex(pk.polynomial);
	print("original message = \n");
	phex(m.polynomial);

	struct BRLWE_Ring_polynomials2 c;
	c = BRLWE_Encry(&a, &pk, m.polynomial);

	print("secret message = \n");
	phex(c.c1.polynomial);
	phex(c.c2.polynomial);
	
	//test: Decryption step
	print("\nDecryption:\n");
	print("a = \n");
	phex(a.polynomial);
	print("secret key = \n");
	phex(sk.polynomial);
	print("secret message = \n");
	phex(c.c1.polynomial);
	phex(c.c2.polynomial);

	uint8_t* dm = BRLWE_Decry(&c, &sk);

	print("original message = \n");
	phex(m.polynomial);

	print("decrypted message = \n");
	phex(dm);

	double count = 0.0;
	double errorprob = 0.0;
	if (my_memcmp(m.polynomial, dm, BRLWE_N) == 0) print("check: Decryption success!\n");
	else {
		print("check: Decryption is not the same!\n");
		/*count = counterr(m.polynomial, dm);
		double errorprob = count / BRLWE_N;
		print("The error count is"); print_flt(count);
		print(" , the error possibility is "); print_flt(errorprob);print(" .\n");
	}
	*/
}
