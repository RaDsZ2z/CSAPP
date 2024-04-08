// L加载：读到内存   S存储：写到磁盘   M修改：读到内存后再写到磁盘
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "cachelab.h"
bool _v = false;
char t[50];
typedef struct
{
    int valid;
    int tag;
    int time; // 未被使用的时间
} CacheLine;

typedef struct
{
    int S, E, B;
    CacheLine **cacheSets;
} Cache;

Cache *cache;
int hitCnt = 0, missCnt = 0, eviCnt = 0;
void initCache(int S, int E, int B)
{
    // printf("S:%d E:%d B:%d\n", S, E, B);
    cache = (Cache *)malloc(sizeof(Cache));
    cache->S = S;
    cache->E = E;
    cache->B = B;
    cache->cacheSets = (CacheLine **)malloc(sizeof(CacheLine *) * S);
    for (int i = 0; i < S; i++)
    {
        cache->cacheSets[i] = (CacheLine *)malloc(sizeof(CacheLine) * E);
        for (int j = 0; j < E; j++)
        {
            cache->cacheSets[i][j].valid = 0;
            cache->cacheSets[i][j].tag = -1;
            cache->cacheSets[i][j].time = 0;
        }
    }
}

// 根据组索引和tag 查找被缓存到了组中的第几行
int getRowIndex(int setIndex, int tag)
{
    for (int j = 0; j < (cache->E); j++)
    {
        if (cache->cacheSets[setIndex][j].valid && cache->cacheSets[setIndex][j].tag == tag)
        {
            return j;
        }
    }
    return -1; // 没有被缓存
}

// 找该组的空行
int findEmpty(int setIndex)
{
    // printf("---findEmpty---\n");
    for (int j = 0; j < cache->E; j++)
    {
        // printf("j:%d\n", j);
        if (cache->cacheSets[setIndex][j].valid == 0)
        {
            return j;
        }
    }
    // printf("---outfindEmpty---\n");
    return -1; // 没有空行
}

// 根据替换策略，找到应该被替换的行
int lru(int setIndex)
{
    // printf("---lru---\n");
    int ret = -1;
    int maxTime = -1;
    for (int j = 0; j < cache->E; j++)
    {
        int t = cache->cacheSets[setIndex][j].time;
        if (t > maxTime)
        {
            maxTime = t;
            ret = j;
        }
    }
    return ret;
}

// 将某行替换出去
void update(int setIndex, int rowIndex, int tag)
{
    // printf("---update---\n");
    // printf("%d %d\n", setIndex, rowIndex);
    for (int j = 0; j < cache->E; j++)
    {
        cache->cacheSets[setIndex][j].time++;
    }
    cache->cacheSets[setIndex][rowIndex].valid = 1;
    cache->cacheSets[setIndex][rowIndex].tag = tag;
    cache->cacheSets[setIndex][rowIndex].time = 0;
}

// 根据地址得到参数，此处并不会实际返回数据所以不需要块偏移
void make(int setIndex, int tag)
{
    int targetRow = getRowIndex(setIndex, tag);
    // printf("targetRow:%d\n", targetRow);
    if (targetRow == -1)
    {
        // printf("miss\n");
        // 没有被缓存 miss+1
        missCnt++;
        int emptyLine = findEmpty(setIndex);

        if (emptyLine == -1)
        {
            // 没有空行 需要替换 eviction+1
            eviCnt++;
            targetRow = lru(setIndex);
            update(setIndex, targetRow, tag);
        }
        else
        {
            // printf("empty\n");
            // 有空行 放进去就行
            update(setIndex, emptyLine, tag);
        }
    }
    else
    {
        // hit+1
        hitCnt++;
    }
}

// 读入.trace文件
void read(int s, int b)
{
    FILE *p_file;
    p_file = fopen(t, "r");
    if (p_file == NULL)
    {
        exit(-1);
    }
    char identifier;
    // unsigned是32位的 而地址是36位 所以改用unsigned long long
    unsigned long long address;
    int size;
    // 这个读入方式第一次见，会无视不符合要求的行吗
    while (fscanf(p_file, " %c %llx,%d", &identifier, &address, &size) > 0)
    {
        int tag = address >> (s + b);
        // int setIndex = (address >> b) & (0xffffffffffffffff >> (64 - s));
        int setIndex = (address >> b) & ((unsigned)(-1) >> (sizeof(unsigned long long) - s));
        switch (identifier)
        {
        case 'M':
            make(setIndex, tag);
            make(setIndex, tag);
            break;
        case 'L':
            make(setIndex, tag);
            break;
        case 'S':
            make(setIndex, tag);
            break;
        }
    }
    fclose(p_file);
}

int main(int argc, char *argv[])
{
    // delete
    // delete

    char opt;
    int s, E, b;
    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1)
    {
        switch (opt)
        {
        case 'h':
            printf("use command'./csim-ref -h' if you need help\n");
            exit(0);
        case 'v':
            _v = true;
        case 's':
            s = atoi(optarg);
            break;
        case 'E':
            E = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 't':
            strcpy(t, optarg);
            break;
        default:
            printf("input format wrong!\n");
            exit(114514);
        }
    }
    initCache(1 << s, E, 1 << b);
    read(s, b);
    for (int i = 0; i < cache->S; i++)
    {
        free(cache->cacheSets[i]);
    }
    free(cache->cacheSets);

    printf("hits:%d misses:%d evictions:%d\n", hitCnt, missCnt, eviCnt); // 变量没有用到不能通过编译 先占位
    return 0;
}
