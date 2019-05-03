#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int SinCos_num=-1;
double *SinCos_table= NULL;

/* SinCos_init: ９０度分の cos() テーブルを作成する */
void SinCos_init(int num)
{
    SinCos_table=(double*)malloc(sizeof(double)*num);
    SinCos_num=num;
    for (int i=0; i<num; i++) SinCos_table[i]=cos(i*M_PI/2/num);
}

/* Cos() 関数 */
double Cos(double x)
{
    double sign=1;
    int i;

    if (x<0) x=-x;              /*  cos(x)=cos(-x)     */
    x/=M_PI;
    x=(x-(int)(x/2)*2);         /*  cos(x)=cos(x+2*PI) */
    if (x>1) x=2-x;             /*  cos(x)=cos(x-PI)   */
    if (x>.5) {x=1-x; sign=-1;} /* -cos(x)=cos(PI-x)   */
    i=(int)(x*2*SinCos_num);
    return (sign*SinCos_table[i]);
}

/* Sin() 関数: sin(x)=cos(x-M_PI/2) */
#define Sin(x) (Cos(x-M_PI/2))

int main(){
    SinCos_init(1000);
    float stsin = 0;
    float stcos = 0;
    stsin = Sin(1.57);
    stcos = Cos(0);
    printf("stsin = %f\n", stsin);
    printf("stcos = %f\n", stcos);
    return 0;
}