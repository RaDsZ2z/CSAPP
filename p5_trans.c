/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    // 1. 32 32-------------------
    //  int v1, v2, v3, v4, v5, v6, v7, v0;
    //  for (int i = 0; i < 32; i += 8)
    //  {
    //      for (int j = 0; j < 32; j += 8)
    //      {
    //          for (int k = i; k < i + 8; k++)
    //          {
    //              v0 = A[k][j + 0];
    //              v1 = A[k][j + 1];
    //              v2 = A[k][j + 2];
    //              v3 = A[k][j + 3];
    //              v4 = A[k][j + 4];
    //              v5 = A[k][j + 5];
    //              v6 = A[k][j + 6];
    //              v7 = A[k][j + 7];
    //              B[j + 0][k] = v0;
    //              B[j + 1][k] = v1;
    //              B[j + 2][k] = v2;
    //              B[j + 3][k] = v3;
    //              B[j + 4][k] = v4;
    //              B[j + 5][k] = v5;
    //              B[j + 6][k] = v6;
    //              B[j + 7][k] = v7;
    //          }
    //      }
    //  }

    // 2. 64 64-------------------
    int v0, v1, v2, v3, v4, v5, v6, v7;
    int i, j, k;
    for (i = 0; i < 64; i += 8)
    {
        for (j = 0; j < 64; j += 8)
        {
            for (k = 0; k < 4; k++, i++)
            {
                v0 = A[i][j + 0];
                v1 = A[i][j + 1];
                v2 = A[i][j + 2];
                v3 = A[i][j + 3];
                v4 = A[i][j + 4];
                v5 = A[i][j + 5];
                v6 = A[i][j + 6];
                v7 = A[i][j + 7];

                B[j + 0][i] = v0;
                B[j + 1][i] = v1;
                B[j + 2][i] = v2;
                B[j + 3][i] = v3;
                B[j + 0][i + 4] = v4;
                B[j + 1][i + 4] = v5;
                B[j + 2][i + 4] = v6;
                B[j + 3][i + 4] = v7;
            }
            i -= 4;

            /*
            i += 4;
            for (k = 0; k < 4; k++, i++)
            {
                v0 = A[i][j];
                v1 = A[i][j + 1];
                v2 = A[i][j + 2];
                v3 = A[i][j + 3];
                v4 = B[j][i];
                v5 = B[j + 1][i];
                v6 = B[j + 2][i];
                v7 = B[j + 3][i];

                B[j][i] = v0;
                B[j + 1][i] = v1;
                B[j + 2][i] = v2;
                B[j + 3][i] = v3;
                B[j + 4][i - 4] = v4;
                B[j + 5][i - 4] = v5;
                B[j + 6][i - 4] = v6;
                B[j + 7][i - 4] = v7;
            }
            i -= 8;

            */
            // 把下面这个for循环换成上面那个 miss数从1180变为2660
            // 考虑其差异：
            // 由于前四行和后四行的组数是相同的，上面的写法频繁淘汰缓存导致的miss数是
            // 64*(4+(4*7)) = 2048
            // 而下面的写法导致的miss是
            // 64*(4+4) = 512
            // 这两个数的差值就约为 1180与2660的差值
            for (k = 0; k < 4; j++, k++)
            {
                v0 = A[i + 4][j];
                v1 = A[i + 5][j];
                v2 = A[i + 6][j];
                v3 = A[i + 7][j];
                v4 = B[j][i + 4];
                v5 = B[j][i + 5];
                v6 = B[j][i + 6];
                v7 = B[j][i + 7];

                B[j][i + 4] = v0;
                B[j][i + 5] = v1;
                B[j][i + 6] = v2;
                B[j][i + 7] = v3;
                B[j + 4][i] = v4;
                B[j + 4][i + 1] = v5;
                B[j + 4][i + 2] = v6;
                B[j + 4][i + 3] = v7;
            }
            j -= 4;

            i += 4;
            for (k = 0; k < 4; k++, i++)
            {
                v0 = A[i][j + 4];
                v1 = A[i][j + 5];
                v2 = A[i][j + 6];
                v3 = A[i][j + 7];
                B[j + 4][i] = v0;
                B[j + 5][i] = v1;
                B[j + 6][i] = v2;
                B[j + 7][i] = v3;
            }
            i -= 8;
        }
    }

    // int i, j, k;
    // int tmp_1, tmp_2, tmp_3, tmp_4, tmp_5, tmp_6, tmp_7, tmp_8;

    // for (i = 0; i < N; i += 8)
    // {
    //     for (j = 0; j < M; j += 8)
    //     {
    //         for (k = 0; k < 4; i++, k++)
    //         {
    //             tmp_1 = A[i][j];
    //             tmp_2 = A[i][j + 1];
    //             tmp_3 = A[i][j + 2];
    //             tmp_4 = A[i][j + 3];
    //             tmp_5 = A[i][j + 4];
    //             tmp_6 = A[i][j + 5];
    //             tmp_7 = A[i][j + 6];
    //             tmp_8 = A[i][j + 7];

    //             B[j][i] = tmp_1;
    //             B[j + 1][i] = tmp_2;
    //             B[j + 2][i] = tmp_3;
    //             B[j + 3][i] = tmp_4;
    //             B[j][i + 4] = tmp_5;
    //             B[j + 1][i + 4] = tmp_6;
    //             B[j + 2][i + 4] = tmp_7;
    //             B[j + 3][i + 4] = tmp_8;
    //         }
    //         i -= 4;

    //         for (k = 0; k < 4; j++, k++)
    //         {
    //             tmp_1 = A[i + 4][j];
    //             tmp_2 = A[i + 5][j];
    //             tmp_3 = A[i + 6][j];
    //             tmp_4 = A[i + 7][j];
    //             tmp_5 = B[j][i + 4];
    //             tmp_6 = B[j][i + 5];
    //             tmp_7 = B[j][i + 6];
    //             tmp_8 = B[j][i + 7];

    //             B[j][i + 4] = tmp_1;
    //             B[j][i + 5] = tmp_2;
    //             B[j][i + 6] = tmp_3;
    //             B[j][i + 7] = tmp_4;
    //             B[j + 4][i] = tmp_5;
    //             B[j + 4][i + 1] = tmp_6;
    //             B[j + 4][i + 2] = tmp_7;
    //             B[j + 4][i + 3] = tmp_8;
    //         }
    //         j -= 4;
    //         j += 4;

    //         for (i += 4, k = 0; k < 4; i++, k++)
    //         {
    //             tmp_1 = A[i][j];
    //             tmp_2 = A[i][j + 1];
    //             tmp_3 = A[i][j + 2];
    //             tmp_4 = A[i][j + 3];

    //             B[j][i] = tmp_1;
    //             B[j + 1][i] = tmp_2;
    //             B[j + 2][i] = tmp_3;
    //             B[j + 3][i] = tmp_4;
    //         }
    //         i -= 8;
    //         j -= 4;
    //     }
    // }

        // 3.61 67
    // 长度不是2的幂次不知道怎么分析
    // 看博客说简单的16*16分块可以拿到满分 就这样吧不写了
}

/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    // int i, j, tmp;

    // for (i = 0; i < N; i++)
    // {
    //     for (j = 0; j < M; j++)
    //     {
    //         tmp = A[i][j];
    //         B[j][i] = tmp;
    //     }
    // }
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);
}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; ++j)
        {
            if (A[i][j] != B[j][i])
            {
                return 0;
            }
        }
    }
    return 1;
}
