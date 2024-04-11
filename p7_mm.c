/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 *
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "ateam",
    /* First member's full name */
    "Harry Bovik",
    /* First member's email address */
    "bovik@cs.cmu.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/* Basic constants and macros */
#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1 << 12)

#define MAX(x, y) ((x) > (y) ? (x) : (y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

// 下面这两个return p
/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp) ((char *)(bp)-WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

// 下面这两个return bp
/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp)-WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp)-GET_SIZE(((char *)(bp)-DSIZE)))

static char *heap_listp;
/*
//合并内存碎片
*/
static void *coalesce(void *bp)
{
    // 见书P596 四种情况
    // 1.前面的块和后面的块都是已分配的
    // 2.前面的块是已分配的，后面的块是空闲的
    // 3.前面的块是空闲的，后面的块是已分配的
    // 4.前面的和后面的块都是空闲的

    // 下面这三行这里HDRP还是FTRP都一样
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc)
    {
        return bp;
    }
    else if (prev_alloc && !next_alloc)
    {
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    }
    else if (!prev_alloc && next_alloc)
    {
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        // 下面两行的位置不能交换
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    else
    {
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) +
                GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    return bp;
}
/*
当申请了words个字的空间，使其内存对齐到双字(8字节)
 */
static void *extend_heap(size_t words)
{
    char *bp;
    size_t size;

    // size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
    size = ALIGN(words * WSIZE);
    if ((long)(bp = mem_sbrk(size)) == -1)
        return NULL;
    // mem_sbrk返回的是p，但是下面把它变成bp了
    // 此时bp指向图9-42结尾块的末尾
    PUT(HDRP(bp), PACK(size, 0));         // 把结尾块变成自己的hdr
    PUT(FTRP(bp), PACK(size, 0));         // 紧跟着hdr添加ftr
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); // 在ftr后面添加结尾块
    return coalesce(bp);                  // 周围有内存碎片则合并
}
/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    // void *mem_sbrk(int incr)
    // 申请incr大小的内存并返回内存首地址
    if ((heap_listp = mem_sbrk(4 * WSIZE)) == (void *)-1)
        return -1;
    PUT(heap_listp, 0);
    PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1));
    PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1));
    PUT(heap_listp + (3 * WSIZE), PACK(0, 1));
    heap_listp += (2 * WSIZE);

    if (extend_heap(CHUNKSIZE / WSIZE) == NULL)
        return -1;
    return 0;
}

/*
// 遍历内存块，找到第一个能放下的则返回其地址
*/
static void *find_fit(size_t asize)
{

    void *bp;
    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp))
    {
        if (!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp))))
        {
            return bp;
        }
    }
    // 找不到适合的
    return NULL;
}
/*
在当前位置申请了asize字节空间
剩下的块大于等于16字节则分割
 */
static void place(void *bp, size_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp));

    if ((csize - asize) >= (2 * DSIZE))
    {
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize - asize, 0));
        PUT(FTRP(bp), PACK(csize - asize, 0));
    }
    else
    {
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
    }
}
/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    // rewrite
    size_t asize;
    size_t extendsize;
    char *bp;
    if (heap_listp == 0)
    {
        mm_init();
    }

    if (size == 0)
        return NULL;

    // 请求的空间必须有头部脚部，占了8字节，且必须有有效载荷满足双字对齐
    // 所以最小空间是2*DSIZE 16字节
    // 若请求的有效载荷空间大于8字节则加上头部脚部的8字节然后满足双字对齐

    /*
    if (size <= DSIZE)
        asize = 2 * DSIZE;
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);
    */
    asize = ALIGN(size);
    asize += DSIZE;

    if ((bp = find_fit(asize)) != NULL)
    {
        place(bp, asize);
        return bp;
    }

    ;
    // 找不到合适的块则需要请求额外的内存空间
    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize / WSIZE)) == NULL)
        return NULL;
    place(bp, asize);
    return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *bp)
{
    // write by myself
    if (bp == NULL)
        return;
    size_t size = GET_SIZE(HDRP(bp));
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));

    coalesce(bp);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
// void *mm_realloc(void *ptr, size_t size)
// {
//     size_t oldsize;
//     void *newptr;

//     /* If size == 0 then this is just free, and we return NULL. */
//     if (size == 0)
//     {
//         mm_free(ptr);
//         return 0;
//     }

//     /* If oldptr is NULL, then this is just malloc. */
//     if (ptr == NULL)
//     {
//         return mm_malloc(size);
//     }

//     newptr = mm_malloc(size);

//     /* If realloc() fails the original block is left untouched  */
//     if (!newptr)
//     {
//         return 0;
//     }

//     /* Copy the old data. */
//     oldsize = GET_SIZE(HDRP(ptr));
//     if (size < oldsize)
//         oldsize = size;
//     memcpy(newptr, ptr, oldsize);

//     /* Free the old block. */
//     mm_free(ptr);

//     return newptr;
// }

void *mm_realloc(void *ptr, size_t size)
{
    // ptr = coalesce(ptr);
    // 不能一开始就回收内存碎片 这样会找不到原来的数据存在哪里
    //  ptr对应的就是bp
    if (ptr == NULL)
        return mm_malloc(size);
    if (size == 0)
    {
        mm_free(ptr);
        return NULL;
    }

    size_t oldSize = GET_SIZE(HDRP(ptr));
    size_t newSize = ALIGN(size);
    newSize += DSIZE;
    void *newPtr;
    if (newSize == oldSize)
        return ptr;
    else if (newSize < oldSize)
    {
        if (oldSize - newSize >= 2 * DSIZE)
        {
            // place_alloc(ptr,oldSize,newSize);
            PUT(HDRP(ptr), PACK(newSize, 1));
            PUT(FTRP(ptr), PACK(newSize, 1));
            newPtr = NEXT_BLKP(ptr);
            PUT(HDRP(newPtr), PACK(oldSize - newSize, 0));
            PUT(FTRP(newPtr), PACK(oldSize - newSize, 0));
            coalesce(newPtr);
        }
        return ptr;
    }
    //---------------------
    // if (!GET_ALLOC(HDRP(NEXT_BLKP(ptr))))
    // {
    //     // 首先获取当前块和下一个块的大小
    //     size_t trySize = GET_SIZE(HDRP(ptr)) + GET_SIZE(HDRP(NEXT_BLKP(ptr)));
    //     // 如果合并后的大小大于等于newSize，则将两个块合并，并通过调用place_alloc函数将新块切割为大小为newSize
    //     if (trySize - newSize >= 0)
    //     {
    //         /* 合并：空闲链表中的free块并设置新块的头尾  */
    //         // freeNode(NEXT_BLKP(ptr));

    //         //-------------

    //         PUT(HDRP(ptr), PACK(trySize, 1));
    //         PUT(FTRP(ptr), PACK(trySize, 1));
    //         if (trySize - newSize >= 2 * DSIZE)
    //         {
    //             // place_alloc(ptr, trySize, newSize);
    //             PUT(HDRP(ptr), PACK(newSize, 1));
    //             PUT(FTRP(ptr), PACK(newSize, 1));
    //             newPtr = NEXT_BLKP(ptr);
    //             PUT(HDRP(newPtr), PACK(trySize - newSize, 0));
    //             PUT(FTRP(newPtr), PACK(trySize - newSize, 0));
    //             coalesce(newPtr);
    //         }
    //         return ptr;
    //     }
    // }

    // 如果两个块的合并无法满足需求，只能重新分配一个大小为newSize的新块。
    // 调用mm_malloc函数尝试分配一块新的内存，并使用memcpy函数将原来的内存中的数据拷贝到新的内存中。
    if ((newPtr = mm_malloc(newSize)) == NULL)
    {
        return NULL;
    }
    memcpy(newPtr, ptr, oldSize);

    // 最后，释放原来的内存，并返回新分配的内存的首地址。
    mm_free(ptr);
    return newPtr;

    // else
    // {
    //     // newSize > oldSize

    //     // 先试着回收内存碎片
    //     newPtr = coalesce(ptr);
    //     size_t coalSize = GET_SIZE(HDRP(ptr));
    //     // 回收内存碎片之后的大小可以满足申请
    //     if (coalSize > newSize)
    //     {
    //         if (newPtr < ptr)
    //         {

    //             // 向前扩展了 需要把数据移上去
    //             // 否则数据不需要动
    //             // 如果两段数据位置有重叠，可能得到错误的结果？
    //             memcpy(newPtr, ptr, oldSize);
    //         }

    //         if (coalSize - newSize >= 2 * DSIZE)
    //         {
    //             PUT(HDRP(newPtr), PACK(newSize, 1));
    //             PUT(FTRP(newPtr), PACK(newSize, 1));
    //             ptr = NEXT_BLKP(newPtr);
    //             PUT(HDRP(ptr), PACK(coalSize - newSize, 0));
    //             PUT(FTRP(ptr), PACK(coalSize - newSize, 0));
    //         }

    //         return newPtr;
    //     }
    //     else
    //     {
    //         // 回收内存碎片后也没有足够的空间，只能malloc
    //         newPtr = mm_malloc(newSize);
    //         if (newPtr == NULL)
    //             return NULL;
    //         memcpy(newPtr, ptr, oldSize);
    //         mm_free(ptr);
    //         return newPtr;
    //     }
    //     // newSize > oldSize
    // }
}
