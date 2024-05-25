/**
    Classic Matrix Multiplication Loop

    (M x D) * (D x N)

*/

#include <stdio.h>
#include <stdlib.h>

#define M 3
#define D 3
#define N 3

int main (int argc, char **argv)
{
        double m1[M][D] = {
                {1, 2, 3},
                {3, 2, 1},
                {4, 5, 6}
        };

        double m2[D][N] = {
                {4, 5, 6},
                {4, 3, 2},
                {2, 3, 4}
        };

        double m3[M][N] = { 0 };

        for (size_t m = 0; m < M; m++) {
                for (size_t n = 0; n < N; n++) {
                        for (size_t d = 0; d < D; d++) {
                                m3[m][n] += m1[m][d] * m2[d][n];
                        }
                }
        }

        return m3[1][1];
}
