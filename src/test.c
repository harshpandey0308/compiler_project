#include<stdio.h>
#define MAX 100
#define PI 3.14

int add(int a, int b){
    int result;
    result = a + b;
    return result;
}

int factorial(int n){
    int result;
    result = 1;
    int i;
    i = 1;
    while(i <= n){
        result = result * i;
        i = i + 1;
    }
    return result;
}

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

    printf("===== WHILE LOOP =====\n");
    int count;
    count = 1;
    while(count <= 5){
        printf("count = %d\n", count);
        count = count + 1;
    }

    printf("===== FOR LOOP =====\n");
    int sum;
    sum = 0;
    int i;
    for(i = 1; i <= 5; i = i + 1){
        sum = sum + i;
    }
    printf("sum 1 to 5 = %d\n", sum);

    printf("===== ARRAYS =====\n");
    int arr[5];
    arr[0] = 10;
    arr[1] = 20;
    arr[2] = 30;
    arr[3] = 40;
    arr[4] = 50;
    printf("arr[0] = %d\n", arr[0]);
    printf("arr[2] = %d\n", arr[2]);
    printf("arr[4] = %d\n", arr[4]);

    printf("===== POINTERS =====\n");
    int px;
    int* p;
    px = 42;
    p = &px;
    printf("px = %d\n", px);
    *p = 100;
    printf("after *p=100, px = %d\n", px);

    printf("===== FUNCTIONS =====\n");
    int res;
    res = add(5, 3);
    printf("add(5,3) = %d\n", res);

    printf("===== FACTORIAL =====\n");
    int fact;
    fact = factorial(5);
    printf("factorial(5) = %d\n", fact);

    printf("===== SCANF =====\n");
    int num;
    scanf("%d", &num);
    printf("you entered = %d\n", num);

    printf("===== NESTED LOOPS =====\n");
    int row;
    int col;
    int val;
    for(row = 1; row <= 3; row = row + 1){
        for(col = 1; col <= 3; col = col + 1){
            val = row * col;
            printf("%d ", val);
        }
    }

    printf("===== CONSTANT FOLDING =====\n");
    int cf;
    cf = 2 + 3 * 4;
    printf("2+3*4 = %d\n", cf);

    return 0;
}