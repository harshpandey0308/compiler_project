#include<stdio.h>
#define PI 3.14

float calculate_area(float radius){
    float area =  PI * radius * radius;
    return area;
}

int main(){
    int radius = 5;
    float area = radius*radius;

    int a = area/4;

    return 0;
}