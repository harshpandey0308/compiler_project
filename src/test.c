#include<stdio.h>

int main(){
    int a;
    int b;
    int c;

    a = 4;
    b = 5;
    c = a + b;

    if(c < 10){
        a = 5;
    }
    else{
        b = 20;
    }
    
    printf("hello world.\n");

    printf("a = %d\n", a);

    printf("b = %d\n", b);

    return 0;
}