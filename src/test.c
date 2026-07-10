#include<stdio.h>

int main(){
    int x = 3;

    printf("HELLO WORLD\n");

    int y = 4;

    int *p ;

    p = &x;

    x = y;

    *p = 56;

    return 0;
}