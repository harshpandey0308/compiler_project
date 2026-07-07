#include<stdio.h>

int main(){
    int x = 3;

    int *p ;

    p = &x;

    if(x > 5){
        *p =100;
    }
    else{
        *p = 20;
    }
    
    return 0;
}