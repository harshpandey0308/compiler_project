#include<stdio.h>

int main(){
    int a;
    int b;
    int c;

    char ch = 'A';

    a = 4;
    b = 5;
    c = a + b;

    if(c < 10){
        a = 5;
    }
    else{
        b = 20;
    }

    printf("value of a before for loop = %d\n", a);
    

    for(int i=0 ; i<=5 ; i=i+1){
        a = a + 10;
    }
    

    printf("a = %d\n", a);

    printf("b = %d\n", b);

    printf("--------i am harsh-----------\n");

    printf("============hello==============\n");

    printf("ch = %c\n", ch);

    return 0;
}