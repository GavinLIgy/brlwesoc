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

#include "alloc.h"
#include "alloc.c"

#include "brlwe.h"
#include "brlwe.c"

#define alloc_printf  print

#ifdef ICEBREAKER
#  define MEM_TOTAL 0x20000 /* 128 KB */
#elif HX8KDEMO
#  define MEM_TOTAL 0x700 /* 7 KB */
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

void print_dec (uint32_t n)
{
    if (n >= 10)
    {
        print_dec(n / 10); 
        n %= 10;
    }
    putchar((char)(n + '0'));
}

static void print_Hex_32(unsigned int hex)
{
	int i = 8;
	putchar('0');
	putchar('x');
	while (i--) {
		unsigned char c = (hex & 0xF0000000) >> 28;
		putchar(c < 0xa ? c + '0' : c - 0xa + 'a');
		hex <<= 4;
	}
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
        print("blk_ptr:");
        print_Hex_32(tmp_node);
        print("\r\n");
        print("mem_ptr:");
		print_Hex_32(tmp_node->mem_ptr);
		print("\r\n");
        print("nxt_ptr:");
		print_Hex_32(tmp_node->nxt_ptr);
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
    ptr = m_malloc(16);
    if(ptr == NULL)
    {
        print("malloc failed.\r\n");
        return;
    }
    mem_print();
    
    for(i=0;i<4;i++)
    {
        for(j=0;j<4;j++)
        {
            ptr[i][j] = i;
        }
    }
	
	array ptr2 = NULL;
 
    print("Ptr2:");
	print_dec(sizeof(ptr2));
	print("\r\n");
    ptr2 = m_malloc(16);
    if(ptr2 == NULL)
    {
        print("malloc failed.\r\n");
        return;
    }
    mem_print();
    
    for(i=0;i<4;i++)
    {
        for(j=0;j<4;j++)
        {
            ptr2[i][j] = i;
        }
    }
	
	m_free(ptr);
	m_free(ptr2);
	mem_print();
    buff_print((unsigned char *)ptr, 16);
	buff_print((unsigned char *)ptr2, 16);
}

/*
********************************************************************************
*                       Random Number Seed Setting                     
*
* Description  	: Set the seed of RNG to the input number.
*
* Parameters  	: uint32_t seed: seed number, format: uint8_t str[4]
*
* Return  		: None
********************************************************************************
*/

void setseed32(uint32_t seed)
{	
	/*int i;
	uint32_t tmp = 0x00000000;
	for (i = 0; i < 4; i++) {
		tmp = tmp << 8;
		tmp = tmp + str[i];
	}//for uint8_t hex to uint32_t
	*/
	reg_rng_data = seed;
}

/*
int getrandom(uint8_t* str)
{	
	uint32_t tmp = 0xffffffff;//impossible value of the RNG output
		
		int i = 0;
		int j = 0;
		for (j = 0; j < BRLWE_N/4; j++){
			while (tmp == 0xffffffff) {
				tmp = reg_rng_data;
			}// if RNG is not ready(tmp=0xffff_ffff), wait.
			
			for (i = 0; i < 4; i++)	{
				str[4*j+i]=tmp/0x01000000;
				tmp = tmp << 8;
			}//for uint32_t hex to uint8_t
			
			tmp = 0xffffffff;
		}

	return 1;
}*/

void getrandom_binary(uint8_t* str)
{	//sizeof(str) = 4 (another form of unit32_t)
	uint32_t tmp = 0xffffffff;//impossible value of the RNG output
	int i = 0;
	
	while (tmp == 0xffffffff) {
			tmp = reg_rng_data;
		}// if RNG is not ready(tmp=0xffff_ffff), wait.
	for (i = 0; i < 4; i++)	{
			str[i]=(tmp/0x01000000)%2;
			tmp = tmp << 8;
		}//for uint32_t hex to uint8_t[4]
	/*
	//sizeof(str) = BRLWE_N
	for (j = 0; j < BRLWE_N/8; j++){
		while (tmp == 0xffffffff) {
			tmp = reg_rng_data;
		}// if RNG is not ready(tmp=0xffff_ffff), wait.
		for (i = 0; i < 8; i++)	{
			str[8*j+i]=(tmp/0x10000000)%2;
			tmp = tmp << 4;
		}//for uint32_t hex to uint8_t
		tmp = 0xffffffff;
	}
	*/
}

void debug_rdcycle()
{
	uint32_t cycles_begin;
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_begin));
	
	uint32_t tmp = cycles_begin;
	
	print("Cycle now in dec is : ");
	print_dec(tmp);
	print("\r\n");
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

void cmd_echo()
{
	print("Return to menu by sending '!'\n\n");
	char c;
	while ((c = getchar()) != '!')
		putchar(c);
}

// --------------------------------------------------------
	static const uint8_t test_3[4] = { (uint8_t)130, (uint8_t)140, (uint8_t)210 , (uint8_t)156 };
	//uint8_t test_4[4] = { (uint8_t)40, (uint8_t)80, (uint8_t)100 , (uint8_t)10 };
	//uint8_t test_5[4] = { (uint8_t)0, (uint8_t)0, (uint8_t)0 , (uint8_t)0 };
void main()
{
	reg_uart_clkdiv = 104;
	reg_leds = 63;//=0x3f=8'b0011_1111
	set_flash_qspi_flag();
	
	reg_leds = 127;//=0x7f=8'b0111_1111
	while (getchar_prompt("Press ENTER to continue..\n") != '\r') {  /* wait */ };	

	//RNG Testing
	
	// uint32_t cycles_now;
	// __asm__ volatile ("rdcycle %0" : "=r"(cycles_now));
	// setseed32(cycles_now);
	
	// uint32_t count_1 = 0;
	// uint32_t count_0 = 0;
	
	// uint8_t* str = NULL;
	// str = m_malloc(4);
	// int i = 0;
	// int count_loop = 0;
	
	// reg_leds = 0x00;
	// print("\nRNG Testing Progress : \n");
	// while (count_loop < 500000) {  
		// getrandom_binary(str);
		// for (i = 0; i < 4 ; i++){
			// //print("\nstr[");print_dec(i);print("] =");print_hex(str[i],2);
			// if (str[i] == (uint8_t)0x00) count_0++;
			// if (str[i] == (uint8_t)0x01) count_1++;
		// };
		// count_loop++;
		// print("\r");print_dec(count_loop);print(" Of 500000");
		// if ((count_0 > 0xFFFFFFF0UL) || (count_1 > 0xFFFFFFF0UL)) break;
	// };
	// reg_leds = 0xff;
	// m_free(str);
	// debug_rdcycle();
	// print("Count 0 = ");print_dec(count_0);
	// print("\nCount 1 = ");print_dec(count_1);
	// print("\nEnd of RNG testing");
	
	//test: memory allocate testing & RNG initialization
	
	print("Booting..\n");
	mem_init();
	mem_print();
	uint32_t cycles_now;
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_now));
	RNG_seed(cycles_now);
	
	//test: Polynomial initialization step
	/*
	BRLWE_Ring_polynomials a = NULL;
	a = m_malloc(BRLWE_N);
	print("\nPolynomial initialization step:\r\n");
	a = BRLWE_init_hex(a, test_1, 0);
	print("test_1 = ");
	phex(test_1);
	print("BRLWE_init_hex(test_1, 0) = ");
	phex(a);
	print("\n");
	
	
	struct BRLWE_Ring_polynomials* m = NULL;
	m = m_malloc(BRLWE_N);
	m = BRLWE_init_hex(m, test_2, 0);
	print("BRLWE_init_hex(test_2, 0) = ");
	phex(m->polynomial);
	print("\n");
	
	
	
	a = BRLWE_init_bin_sampling(a);
	print("BRLWE_init_bin_sampling() = ");
	phex(a);
	print("\n");
	a = BRLWE_init(a);
	print("BRLWE_init() = ");
	phex(a);
	print("\n");
	*/
	
	// //test: Math-operation subfunctions
	
	// struct BRLWE_Ring_polynomials* n = NULL;
	// n = m_malloc(BRLWE_N);
	// print("\nMath-operation subfunctions:\r\n");
	// print("test1 + test2 = \n");
	// phex(Ring_add(test_1, test_2, n));
	// print("test1 - test2 = \n");
	// phex(Ring_sub(test_1, test_2, n));
	// print("test1 * test2 = \n");
	// phex(Simple_Ring_mul(test_1, test_2, n));
	
	// mem_print();
	// m_free(n);
	// mem_print();

	//test: Key Generation step
	
	uint32_t cycles_begin;
	
	BRLWE_Ring_polynomials2 key = NULL;
	key = m_malloc(BRLWE_N * 2);
	//print("mem_print() 1 \n");
	//mem_print();
	//print("\n \nKey Generation:\n");
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_begin));
	key = BRLWE_Key_Gen((BRLWE_Ring_polynomials) test_1, key);
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_now));
	print("\n Cycles Number for Key Generation = ");print_dec(cycles_now - cycles_begin);
	//print("public key = \n");
	//phex(key);
	//print("secret key = \n");
	//phex(key + BRLWE_N);
	
	//m_free(key);
	
	//test: Encryption step
	
	print("\n \nEncryption:\n");
	// print("a = \n");
	// phex(test_1);
	// print("public key = \n");
	// phex(key);
	// print("original message = \n");
	// phex(test_2);

	BRLWE_Ring_polynomials2 cryptom = NULL;
	cryptom = m_malloc(BRLWE_N * 2);
	
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_begin));
	cryptom = BRLWE_Encry( (BRLWE_Ring_polynomials) test_1, (BRLWE_Ring_polynomials) key, test_2, cryptom);
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_now));
	print("\n Cycles Number for Encryption = ");print_dec(cycles_now - cycles_begin);
 
	// print("secret message 1 = \n");
	// phex(cryptom);
	// print("secret message 2 = \n");
	// phex(cryptom + BRLWE_N);
	
	//test: Decryption step
	
	print("\n \nDecryption:\n");
	// print("a = \n");
	// phex(test_1);
	// print("secret key = \n");
	// phex(key + BRLWE_N);
	// print("secret message = \n");
	// phex(cryptom);
	// phex(cryptom + BRLWE_N);
	
	uint8_t* recoverm = NULL;
	recoverm = m_malloc(BRLWE_N);
	
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_begin));
	recoverm = BRLWE_Decry(cryptom, (BRLWE_Ring_polynomials)(key + BRLWE_N), recoverm);
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_now));
	print("\n Cycles Number for Decryption = ");print_dec(cycles_now - cycles_begin);

	// print("original message = \n");
	// phex(test_2);

	// print("recovered message = \n");
	// phex(recoverm);

	int count = 0;
	if (memcmp(test_2, recoverm, BRLWE_N) == 0) 
		print("\n \ncheck: Decryption success!\n");
	else {
		print("\n \ncheck: Decryption failed!\n");
		count = counterr(test_2, recoverm);
		print("The error count is "); print_dec(count);
		print(" of N :"); print_dec(BRLWE_N);
	}
	
	mem_print();
	m_free(key);
	m_free(cryptom);
	m_free(recoverm);
	mem_print();
	
}
