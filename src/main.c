#include<stdio.h>
#include"compiler.h"
#include"ASMCODE.h"
#include"TACcode.h"
#include"semantic.h"
#include"VM.h"

int main(int argc, char* argv[]){

    Semantic_ctxt *context = malloc(sizeof(Semantic_ctxt));
    VM *vm = (VM*)malloc(sizeof(VM));
    SymbolTable *symbol = (SymbolTable*)malloc(sizeof(symbol));
    REGISTER *registers = (REGISTER*)malloc(sizeof(REGISTER));
    TACProgram *program = (TACProgram*)malloc(sizeof(program));

    if(argc < 2){
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    return compile_file(argv[1], context , program , symbol , registers , vm);

    free(context);
    free(vm);
    free(symbol);
    free(registers);
    free(program);

    return 0;
}