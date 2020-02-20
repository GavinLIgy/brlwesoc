/*
********************************************************************************
*                                memory alloc 
*
*                            (c) Copyright 2018-2020;
*        All rights reserved.  Protected by international copyright laws.
*
*                                MEMORY ALLOC
*
* File    : mem_alloc.c
* By      : Recahrd.Zhang
* Version : V1.00
*
********************************************************************************
*/
//#include <string.h>
#include "alloc.h"
//#include "debug.h"
 
#define MEM_ALLOC        1
#if defined (MEM_ALLOC)&&MEM_ALLOC
#define alloc_printf         print
#else
#define alloc_printf(argv, ...)
#endif
 
#define MEM_SIZE         0x80             /*内存池的大小*/
 
extern uint32_t _heap_start, _heap_end;

#define MEM_START     & _heap_start              	  	/*定义内存池的首地址*/
#define MEM_END       & _heap_end	        /*定义内存池的尾地址*/
  
//static    char mem[MEM_SIZE];                   /*定义用来内存分配的数组*/
 
//#define MEM_START     mem              	  	/*定义内存池的首地址*/
//#define MEM_END       (mem + MEM_SIZE)	        /*定义内存池的尾地址*/

enum USE_STA{                            /*定义内存块的使用状态(UNUSED 未使用)，(USED 已使用)*/
    UNUSED = 0,
    USED   = 1
};
 
#pragma pack(1)
typedef struct mem_block{                /*定义内存管理块的数据结构*/
    void             *mem_ptr;            /*当前内存块的内存地址*/
    struct mem_block     *nxt_ptr;        /*下一个内存管理块的地址*/
    unsigned int         mem_size;        /*当前内存块的大小*/
    enum USE_STA        mem_sta;        /*当前内存块的状态*/
}mem_block;
#pragma pack()
 
 
#define BLK_SIZE    sizeof(mem_block)    /*内存管理块的大小*/
#define HEAD_NODE    (MEM_END - BLK_SIZE)/*头内存管理块的地址*/
 
static signed char  mem_init_flag = -1; /*内存分配系统初始化的标志(-1 未初始化),(1 已初始化)*/
 
/*
********************************************************************************
*                                   内存分配系统初始化                     
*
* 描述    : 初始化内存分配系统，为malloc和free做好准备工作。
*
* 参数  : 无
*
* 返回  : 无
********************************************************************************
*/
void * memcpy (void *dest, const void *src, int n)
{
	// print("**MEMCPY**\n");
	char *a = dest;
	const char *b = src;
	while (n--) *(a++) = *(b++);
	return dest;
}

int memcmp(uint8_t *str1,uint8_t *str2,int len){
	while(len--)
	{
	while(*str1==*str2)
	{
		if(*str1=='\0') return 0;
		str1++;
		str2++;
	}
	}
	if(*str1>*str2)
		return 1;
	if(*str1<*str2)
		return -1;
}

void* memset(void* s, int c, size_t n)
{    
unsigned char* p = (unsigned char*) s;
    while (n > 0) {
		*p++ = (unsigned char) c;
		--n;
    }
    return s;
}

void mem_init(void)
{
    mem_block     *node;
	
	memset(MEM_START,    0x00000000,    (MEM_END - MEM_START) );
    
    //memset(mem,    0x00000000,    sizeof(mem));
    
    node = (mem_block     *)HEAD_NODE;
    node->mem_ptr         =     MEM_START;
    node->nxt_ptr         =     (mem_block *)HEAD_NODE;
    node->mem_size        =     MEM_SIZE - BLK_SIZE;
    node->mem_sta        =    UNUSED;
    
    mem_init_flag = 1;
	alloc_printf("初始化完成.\r\n");
}
 
/*
********************************************************************************
*                                   内存申请函数                     
*
* 描述    : 内存申请函数
*
* 参数  : nbytes    要申请的内存的字节数。
*
* 返回  : 成功        返回申请到的内存的首地址
*          失败        返回NULL
********************************************************************************
*/
void *malloc(unsigned nbytes)
{
    unsigned int    suit_size = 0xFFFFFFFFUL;
    mem_block     *head_node=NULL, *tmp_node=NULL, *suit_node=NULL;
 
    if(nbytes == 0)
    {
        alloc_printf("参数非法!\r\n");
        return NULL;
    }
    if(mem_init_flag < 0)
    {
        alloc_printf("未初始化,先初始化.\r\n");
        mem_init();
    }
    
    head_node = tmp_node = (mem_block *)HEAD_NODE;
    while(1)
    {
        if(tmp_node->mem_sta == UNUSED)
        {
            if(nbytes <= tmp_node->mem_size && tmp_node->mem_size < suit_size)
            {
                suit_node = tmp_node;
                suit_size = suit_node->mem_size;
            }
        }
        tmp_node = tmp_node->nxt_ptr;
        if(tmp_node == head_node)
        {
            if(suit_node == NULL)
            {
                alloc_printf("NULL\r\n");
                return NULL;
            }
            break;
        }
    }
 
    if(nbytes <= suit_node->mem_size && (nbytes + BLK_SIZE) >= suit_node->mem_size)
    {
        suit_node->mem_sta = USED;
        return suit_node->mem_ptr;
    }
    else    if(suit_node->mem_size > (nbytes + BLK_SIZE))
    {
        tmp_node = suit_node->mem_ptr; 
        tmp_node = (mem_block *)((unsigned char *)tmp_node + nbytes);
        tmp_node->mem_ptr  = suit_node->mem_ptr;
        tmp_node->nxt_ptr  = suit_node->nxt_ptr;
        tmp_node->mem_size = nbytes;
        tmp_node->mem_sta  = USED;
        
        suit_node->mem_ptr      = (mem_block *)((unsigned char *)tmp_node + BLK_SIZE);
        suit_node->nxt_ptr   =  tmp_node;
        suit_node->mem_size -= (nbytes + BLK_SIZE);
        suit_node->mem_sta   = UNUSED;
        
        return tmp_node->mem_ptr;
    }
    else
    {
        alloc_printf("%s,size err!\r\n");
		alloc_printf(__FUNCTION__);
    }
    
    return NULL;
}
 
/*
********************************************************************************
*                                   内存释放函数                     
*
* 描述    : 内存释放函数 
*
* 参数  : ap        要释放的内存块的指针。
*
* 返回  : 无
********************************************************************************
*/
void free(void *ap)
{
    mem_block     *head_node, *tmp_node, *nxt_node;
    
    if(ap == NULL)
        return;
    if(mem_init_flag < 0)
    {
        return;
    }
    head_node = tmp_node = (mem_block *)HEAD_NODE;
    while(1)
    {
        if(tmp_node->mem_ptr == ap)
        {
            if(tmp_node->mem_sta != UNUSED)
            {
                tmp_node->mem_sta = UNUSED;
                break;
            }
            else
            {
                alloc_printf("ap:0x%08x 已经释放,无需再次释放\r\n",ap);
                return;
            }
        }
        
        tmp_node = tmp_node->nxt_ptr;
        if(tmp_node == head_node)
        {
            alloc_printf("%s,can not found ap!\r\n",__FUNCTION__);
            return ;
        }
    }
 
AGAIN:    
    head_node = tmp_node = (mem_block *)HEAD_NODE;
    while(1)
    {
        nxt_node = tmp_node->nxt_ptr;
        if(nxt_node == head_node)
        {
            break;
        }
        if(tmp_node->mem_sta == UNUSED && nxt_node->mem_sta == UNUSED)
        {
            tmp_node->mem_ptr     = nxt_node->mem_ptr;
            tmp_node->nxt_ptr     = nxt_node->nxt_ptr;
            tmp_node->mem_size += nxt_node->mem_size + BLK_SIZE;
            tmp_node->mem_sta    =  UNUSED;
            
            goto AGAIN;
        }
        tmp_node = nxt_node;
    }
}

void *m_malloc(unsigned nbytes)
{
    return malloc(nbytes);
}
 
void m_free(void *ap)
{
    free(ap);
}