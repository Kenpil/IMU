#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Sintable.h"

int main()
{
    Sintable st(10000);
    st.SinCos_init();
    float stsin = 0;
    float stcos = 0;
    stsin = st.Sin(30);
    stcos = st.Cos(30);
    printf("stsin = %f\n", stsin);
    printf("stcos = %f\n", stcos);
    return 0;
}