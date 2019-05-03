#include <stdio.h>
#include <stdlib.h>
#include <math.h>

class Sintable
{
  public:
    int SinCos_num = -1;
    double *SinCos_table = NULL;
    int split_num;

    Sintable(int num){
        split_num = num;
    }

    /* SinCos_init: ９０度分の cos() テーブルを作成する */
    void SinCos_init()
    {
        SinCos_table = (double *)malloc(sizeof(double) * split_num);
        SinCos_num = split_num;
        for (int i = 0; i < split_num; i++)
            SinCos_table[i] = cos(i * M_PI / 2 / split_num);
    }

    /* Cos() 関数 */
    double Cos(double x)
    {
        double sign = 1;
        int i;

        if (x < 0)
            x = -x; /*  cos(x)=cos(-x)     */
        x /= M_PI;
        x = (x - (int)(x / 2) * 2); /*  cos(x)=cos(x+2*PI) */
        if (x > 1)
            x = 2 - x; /*  cos(x)=cos(x-PI)   */
        if (x > .5)
        {
            x = 1 - x;
            sign = -1;
        } /* -cos(x)=cos(PI-x)   */
        i = (int)(x * 2 * SinCos_num);
        return (sign * SinCos_table[i]);
    }

/* Sin() 関数: sin(x)=cos(x-M_PI/2) */
#define Sin(x) Cos(x - M_PI / 2)
};