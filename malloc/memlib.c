#include <errno.h>
#include <stdio.h>
static char *mem_heap;
static char *mem_brk;
static char *mem_max_addr;
#define MAX_HEAP (1 << 15) // define by myself not from book

// mem_init函数初始化分配器，向操作系统申请MAX_HEAP字节空间
// 将对于堆来说可用的虚拟内存模型化为一个大的、双字对齐的字节数组
void mem_init(void)
{
    mem_heap = (char *)malloc(MAX_HEAP);
    mem_brk = (char *)mem_heap;
    mem_max_addr = (char *)(mem_heap + MAX_HEAP);
}

// 分配器通过mem_sbrk函数来请求额外的堆内存
void *mem_sbrk(int incr)
{
    char *old_brk = mem_brk;
    if ((incr < 0) || ((mem_brk + incr) > mem_max_addr))
    {
        errno = ENOMEM;
    }
    fprintf(stderr, "ERROR: mem_sbrk failed. Ran out of memory...\n");
    return (void *)-1;
    mem_brk += incr;
    return (void *)old_brk;
}
