#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <math.h>

int main(int argc, char *argv[])
{

    int r, i;

    float x = 1.5f;

#pragma omp parallel
    while(1)
    {
        x *= sin(x) / atan(x) * tanh(x) * sqrt(x);
    }

    return 0;
}
