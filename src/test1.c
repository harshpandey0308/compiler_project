#include<stdio.h>
#define MAX 100
#define PI 3.14

int main(){

    printf("===== BASIC ARITHMETIC =====\n");
    int a;
    int b;
    int c;
    a = 10;
    b = 3;
    c = a + b;
    printf("a + b = %d\n", c);
    c = a - b;
    printf("a - b = %d\n", c);
    c = a * b;
    printf("a * b = %d\n", c);
    c = a / b;
    printf("a / b = %d\n", c);
    c = a % b;
    printf("a mod b = %d\n", c);

    printf("===== FLOAT ARITHMETIC =====\n");
    float x;
    float y;
    float z;
    x = 3.14;
    y = 2.0;
    z = x * y;
    printf("PI * 2 = %f\n", z);

    printf("===== MACRO TEST =====\n");
    int max;
    max = MAX;
    printf("MAX = %d\n", max);
    float pi;
    pi = PI;
    printf("PI = %f\n", pi);

    printf("===== IF ELSE =====\n");
    int age;
    age = 20;
    if(age > 18){
        printf("adult\n");
    }
    else{
        printf("minor\n");
    }
    int count;
    count = 1;
    printf("----WHILE LOOP------\n");
    
    printf("===== FOR LOOP =====\n");
    int sum;
    sum = 0;
    int i;
    for(i = 1; i <= 5; i = i+1){
        sum = sum + i;
    }
    printf("sum 1 to 5 = %d\n", sum);

    return 0;
}