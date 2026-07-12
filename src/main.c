#include<stdio.h>
#include"compiler.h"

int main(int argc, char* argv[]){

    if(argc < 2){
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    return compile_file(argv[1]);

    return 0;
}