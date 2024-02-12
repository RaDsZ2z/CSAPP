#include <stdio.h>
static char *heap_listp;
extern char *mem_heap;
extern char *mem_brk;
extern char *mem_max_addr;
// come from P599 of CSAPP
#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1 << 12) // 初始空闲块大小和堆扩展时的默认大小

#define MAX(x, y) ((x) > (y) ? (x) : (y))

#define PACK(size, alloc) ((size) | (alloc))

#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

#define HDRP(bp) ((char *)(bp)-WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

#define PREV_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp)-WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp)-GET_SIZE(((char *)(bp)-DSIZE)))
// 创建带一个初始空闲块的堆
int mm_init(void)
{
    if (heap_listp = mem_sbrk(4 * WSIZE) == (void *)-1)
        return -1;
    PUT(heap_listp, 0);
    PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1));
    PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1));
    PUT(heap_listp + (3 * WSIZE), PACK(0, 1));
    heap_listp += (2 * WSIZE);

    if (extend_heap(CHUNKSIZE / WSIZE == NULL))

        return -1;

    return 0;
}
//  用一个新的空闲块扩展堆
static void *extend_heap(size_t words)
{
    char *bp;
    size_t size;
    size = words % 2 ? (words + 1) * WSIZE : words * WSIZE;
    if ((long)(bp = mem_sbrk(size)) == -1)
        return NULL;
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));
    return coalesce(bp);
}
// mm_free:释放一个块，并使用边界标记合并将之与所有的邻接空闲块在常数时间内合并
void mm_free(void *bp)
{
    size_t size = GET_SIZE(HDRP(bp));

    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    coalesce(bp);
}
static void *coalesce(void *bp)
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc)
    {
        return bp;
    }
    else if (prev_alloc & !next_alloc)
    {
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    }
    else if (!prev_alloc && next_alloc)
    {
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    else
    {
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(HDRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    return bp;
}
// m_malloc:从空闲链表分配一个块
void *mm_malloc(size_t size)
{
    size_t asize;
    size_t extendsize;
    char *bp;

    if (size == 0)
        return NULL;
    if (size <= DSIZE)
        asize = 2 * DSIZE;
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);

    if ((bp == find_fit(asize)) != NULL)
    {
        place(bp, asize);
        return bp;
    }

    extendsize = MAX(asize, CHUNKSIZE);
    if (bp = extend_heap(extendsize / WSIZE) == NULL)
        return NULL;
    place(bp, asize);
    return bp;
}
// 练习题9.8 实现find_fit函数
// 对隐式空闲链表执行首次适配搜索
// 如果有合适的就放置这个请求块，并可选地分割多余的部分；否则返回NULL
static void *find_fit(size_t asize)
{
    // asize已经是8的倍数 并且包括了头部尾部需要的空间了
    char *bp = heap_listp + 2 * WSIZE;
    size_t size;
    int alloc;
    while (bp < mem_max_addr)
    {
        size = GET_SIZE(HDRP(bp));
        alloc = GET_ALLOC(HDRP(bp));
        // 未被分配且空间足够
        if (alloc == 0 && size >= asize)
        {
            return bp;
        }
        bp += size;
    }
    return NULL;
}
// 练习题 9.9 实现 place函数
// 将请求块放置在空闲块的起始位置，只有当剩余部分的大小等于或者超出最小块的大小时，才进行分割
static void place(void *bp, size_t asize)
{
    size_t size = GET_SIZE(HDRP(bp));
    size_t left = size - asize;
    if (left >= 2 * DSIZE)
    {
        // 进行分割
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        void *next_bp = NEXT_BLKP(bp) + WSIZE;
        PUT(HDRP(next_bp), PACK(left, 0));
        PUT(FTRP(next_bp), PACK(left, 0));
    }
}
