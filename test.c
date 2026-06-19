#include<stdio.h>
#define PI 3.14

float calculate_area(float radius){
    float area =  PI * radius * radius;
    return area;
}

int main(){
    float radius = 5.0;
    float area = calculate_area(5);

    int a = area/4;

    return 0;
}