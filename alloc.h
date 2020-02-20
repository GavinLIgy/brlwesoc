#ifndef __MEM_ALLOC_H__
#define __MEM_ALLOC_H__
 
void *m_malloc(unsigned nbytes);
void  m_free(void *ap);
int memcmp(uint8_t *str1,uint8_t *str2,int len);
void * memcpy (void *dest, const void *src, int n);
void* memset(void* s, int c, size_t n);
 
#endif