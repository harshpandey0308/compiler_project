#include<stdio.h>
#include<math.h>
void main()
{
    int temp,sum,num,rem;
    for(num=1; num<=1000 ; num = num+1)
    {
        temp=num;
        sum=0;
        while(temp!=0)
        {
            rem=temp%10;
            sum=sum+rem*rem*rem;
            temp=temp/10;
        }
        if(sum==num)
        {
            printf("%d",num);
            printf(" ");
        }
    }
}