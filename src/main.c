#include<stdio.h>
#include"compiler.h"
#include"ASMCODE.h"
#include"TACcode.h"
#include"semantic.h"
#include"VM.h"

int main(int argc, char* argv[]){
    printf("compiler is starting.......\n");

    COMPILER *compiler = calloc(1 , sizeof(COMPILER));
    TokenEntry *token_table = malloc(sizeof(TokenEntry));
    token_table->token_count = 0;

    if(argc < 2){
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    int status = compile_file(argv[1] , compiler);

    free(compiler);
    free(token_table);

    return status;
}