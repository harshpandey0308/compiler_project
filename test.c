#include<stdio.h>
#define PI 3.14

float calculate_area(float radius){
    return PI * radius * radius;
}

int main(){
    int radius = 5;
    float area = calculate_area(radius);
    printf("Area of the circle with radius %d is %.2f\n", radius, area);

    return 0;
}