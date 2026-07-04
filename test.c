#include<stdio.h>

int main(){
    int x = 10;

    float y = 34.6;

    int *p ;

    p = &x;

    if(*p<5){
        *p = 50;
    }
    else{
        *p = 100;
    }

    return 0;
}