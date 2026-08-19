#include"parser.h"
#include"tokenizer.h"
#include"TACcode.h"
#include"semantic.h"

int temp_count = 0;
int label_count = 0;

void emit_ASSIGN(TACProgram *program , char* result , char* op1 , char* op2 , char* opr){
    TAC t = {0};
    t.type = TAC_ASSIGN;
    strcpy(t.result , result);
    //printf("result = %s\n",result);
    strcpy(t.op1 , op1);
    //printf("op1 = %s\n",op1);
    strcpy(t.op2 , op2);
    //printf("op2 = %s\n",op2);
    strcpy(t.opr , opr);
    //printf("opr = %s\n",opr);
    program->code[program->tac_count++] = t;
}

void emit_IF_GOTO(TACProgram *program , char* op1 , char* opr , char* op2 , char* label){
    TAC t = {0};
    t.type = TAC_IF_GOTO;
    strcpy(t.op1 , op1);
    //printf(" TAC IF GOTO : t.op1 = %s\n",t.op1);
    strcpy(t.op2 , op2);
    //printf("TAC IF GOTO : t.op2 = %s\n",t.op2);
    strcpy(t.opr , opr);
    //printf("TAC IF GOTO : t.opr = %s\n",t.opr);
    strcpy(t.label , label);
    program->code[program->tac_count++] = t;
}

void emit_GOTO(TACProgram *program , char* label){
    TAC t = {0};
    t.type = TAC_GOTO;
    strcpy(t.label , label);
    program->code[program->tac_count++] = t;
}

void emit_LABEL(TACProgram *program , char* label){
    TAC t = {0};
    t.type = TAC_LABEL;
    strcpy(t.label , label);
    program->code[program->tac_count++] = t;
}

void emit_PARAM(TACProgram *program , char* lexeme){
    TAC t = {0};
    t.type = PARAM;
    strcpy(t.op1 , lexeme);
    program->code[program->tac_count++] = t;
}

void emit_param_addr(TACProgram *program , char *lexeme){
    TAC t = {0};
    t.type = TAC_PARAM_ADDR;
    strcpy(t.op1 , lexeme);
    program->code[program->tac_count++] = t;
}

void emit_CALL(TACProgram *program , char* name , int arg_count){
    TAC t = {0};
    t.type = FUNC_CALL;
    strcpy(t.op1 , name);
    sprintf(t.op2 , "%d" , arg_count);
    program->code[program->tac_count++] = t;
}

void emit_RETURN(TACProgram *program , char* lexeme){
    TAC t = {0};
    t.type = RETURN;
    strcpy(t.op1 , lexeme);
    program->code[program->tac_count++] = t;
}

void emit_PUSH(TACProgram *program , char *lexeme){
    TAC t = {0};
    t.type = TAC_PUSH;
    strcpy(t.op1 , lexeme);
    program->code[program->tac_count++] = t;
}

void emit_PULL(TACProgram *program , char *dest){
    TAC t = {0};
    t.type = TAC_POP;
    strcpy(t.result , dest);
    program->code[program->tac_count++] = t;
}

void emit_JMP_DYN(TACProgram *program , char *reg){
    TAC t = {0};
    t.type = TAC_JMP_DYNAMIC;
    strcpy(t.op1 , reg);
    program->code[program->tac_count++] = t;
}

void emit_FUNC_BEG(TACProgram *program , char *name){
    TAC t = {0};
    t.type = TAC_FUNC_BEGIN;
    strcpy(t.label , name);
    program->code[program->tac_count++] = t;
}

void emit_param_string(TACProgram *program , char *str){
    TAC t = {0};
    t.type = TAC_PARAM_STRING;
    strcpy(t.op1 , str);
    program->code[program->tac_count++] = t;
}

char* new_temp(){
    char *temp  = (char*)malloc(10);
    sprintf(temp,"t%d",++temp_count);
    return temp;
}

char* invert_condition(char* opr){
    if(strcmp(opr , "<") == 0) return ">=";
    else if(strcmp(opr , ">") == 0) return "<=";
    else if(strcmp(opr , "<=") == 0) return ">";
    else if(strcmp(opr , ">=") == 0) return "<";
    else if(strcmp(opr , "==") == 0) return "!=";
    else if(strcmp(opr , "!=") == 0) return "==";
    else return opr;
}

char* new_label(){
    char* label = (char*)malloc(10);
    sprintf(label , "L%d" , ++label_count);
    return label;
}

int Generate_if_tac(NODE *node , TACProgram *program){
    //printf("generate if position\n");
    char* l1 = new_label();
    char* l2 = new_label();
    //char* l3 = new_label();

    NODE *cond_node = node->cond;
    //printf("cond node of if = %s.\n",cond_node->lexeme);

    char* opr = cond_node->lexeme;
    char* op1 = cond_node->left->lexeme;
    char* op2 = cond_node->right->lexeme;


    char *invert = invert_condition(opr);

    printf("\nopr = %s , op1 = %s , op2 = %s and invert = %s.\n",opr , op1 , op2 , invert);
    
    //printf("emitting if goto\n");
    emit_IF_GOTO(program , op1 , invert , op2 , l1);
    //printf("goto emitted\n");

    NODE *current = node->body->head;

    while(current != NULL){
        Generate_TAC(current , program);
        current = current->next;
    }


    emit_GOTO(program , l2);

    emit_LABEL(program , l1);

    emit_LABEL(program , l2); 

    free(l1);
    free(l2);

}

int Generate_while_tac(NODE *node , TACProgram *program ){
    //printf("the node is %s.\n",node->lexeme);
    //printf("while tac generation starts\n");

    char* l3 = new_label();
    char* l4 = new_label();

    NODE *cond = node->cond;

    emit_LABEL(program , l3);
    //printf("label emitted :: %s.\n",l3);

    char* op1 = cond->left->lexeme;
    char* opr = cond->lexeme;
    char* op2 = cond->right->lexeme;

    printf("opr : %s.\n",opr);
    printf("op1 : %s.\n",op1);
    printf("op2 : %s.\n",op2);

    char* inver_opr = invert_condition(opr);
    //printf("invert operation : %s.\n",inver_opr);

    emit_IF_GOTO(program , op1 , inver_opr , op2 , l4);

    NODE *current = node->body->head;

    while(current != NULL){
        //printf("going into the generate tac.\n");
        Generate_TAC(current , program);
        current = current->next;
    }

    emit_GOTO(program , l3);

    emit_LABEL(program , l4);

    //printf("tac count at the end of the while_tac :: %d.\n",program->tac_count);

    free(l3);
    free(l4);

}

int Generate_for_TAC(NODE *node , TACProgram *program){
    //printf("for statement started.\n");
    char* L5 = new_label();
    char* L6 = new_label();

    //printf("FOR labels: %s , %s\n", L5, L6);

    //printf("Generating FOR initialization...\n");
    Generate_TAC(node->for_node->init_node , program);
    //printf("FOR initialization complete. tac_count = %d\n",program->tac_count);

    NODE *cond = node->for_node->cond_node;

    //printf("FOR condition node = %s\n", cond->lexeme);
    //printf("the cond nod address = %p\n",cond);

    emit_LABEL(program , L5);

    //printf("FOR label emitted. tac_count = %d\n",program->tac_count);

    char* op1 = cond->left->lexeme;
    char* opr = cond->lexeme;
    char* op2 = cond->right->lexeme;

    //printf("FOR condition: %s %s %s\n", op1, opr, op2);

    char* invert_opr = invert_condition(opr);
    //printf("FOR inverted condition: %s\n", invert_opr);

    emit_IF_GOTO(program , op1 , invert_opr , op2 , L6);
    //printf("FOR IF_GOTO emitted. tac_count = %d\n",program->tac_count);

    NODE *current = node->body->head;
    
    //printf("FOR body = %p\n", (void *)current);
    while(current != NULL){
        //printf("Generating FOR body node: %s\n",current->lexeme);

        Generate_TAC(current , program);
        current = current->next;
    }

    //printf("FOR body complete.\n");

    //printf("Generating FOR update...\n");

    Generate_TAC(node->for_node->update_node , program);

    //printf("FOR update complete.\n");

    emit_GOTO(program , L5);

    emit_LABEL(program , L6);

    free(L5);
    free(L6);

}

void generate_TAC_func(NODE *node , TACProgram *program){
    char *func_name = node->lexeme;
    emit_FUNC_BEG(program , func_name);

    NODE *current = node->body->head;

    while(current != NULL){
        Generate_TAC(current , program);
        current = current->next;
    }
}

char* Generate_TAC(NODE* node , TACProgram *program){
    //printf("node = %s.\n",node->lexeme);
    //printf("tac_count at the start = %d\n",program->tac_count);
    if(node == NULL){
        return NULL;
    }

    switch(node->type){
        case AST_FOR:
            Generate_for_TAC(node , program);
            return strdup("");
        
        case AST_WHILE:
            Generate_while_tac(node , program);
            return strdup("");
        
        case AST_IF :
            Generate_if_tac(node , program);
            return strdup("");

        case AST_FUNCTION:
            generate_TAC_func(node , program);
            return strdup("");
        
        default:
            break;
    }

    if(node->type == AST_ADDRESS_OF){
        char* temp = new_temp();
        TAC t = {0};

        t.type = TAC_ASSIGN;
        strcpy(t.result , temp);
        //printf("t.result : %s.\n",t.result);
        strcpy(t.op1 , node->lexeme);
        //printf("t.op1: %s.\n",t.op1);
        strcpy(t.op2 , "");
        strcpy(t.opr , "&");
        
        t.is_deref_write = 0;
        t.is_addr = 1;
        program->code[program->tac_count++] = t;

        return temp;
    }

    if(node->type == AST_DEREFERENCE){
        char* temp = new_temp();
        TAC t = {0};

        t.type = TAC_ASSIGN;
        strcpy(t.result , temp);
        strcpy(t.op1 , node->lexeme);
        strcpy(t.op2 , "");
        strcpy(t.opr , "*");
        t.is_deref_write = 0;
        program->code[program->tac_count++] = t;

        return temp;
    }

    if(node->type == AST_FUNCTION){

    }

    if(node->type == AST_FUNCTION_CALL){
        char *ret_labels = new_label();
        for(int i=0 ; i<node->ARG_count ; i++){
            if(node->ARG[i]->type == AST_STRING){
                emit_param_string(program , node->ARG[i]->lexeme);
            }
            else if(node->ARG[i]->type == AST_ADDRESS_OF){
                emit_param_addr(program , node->ARG[i]->lexeme);
            }
            else{
                char* arg_val = Generate_TAC(node->ARG[i] , program);
                emit_PARAM(program , arg_val);
                free(arg_val);
            }
            
        }
        emit_PUSH(program , ret_labels);
        //printf("the lexeme is :%s\n",node->lexeme);
        emit_CALL(program , node->lexeme , node->ARG_count);

        emit_LABEL(program , ret_labels);

        return strdup("RETVAL");
    }

    if(node->left == NULL && node->right == NULL){
        char* lexeme = (char*)malloc(strlen(node->lexeme)+1);
        strcpy(lexeme , node->lexeme);
        //printf("Leaf node : %s\n",lexeme);
        return lexeme;
    }

    if(node->lexeme[0] == '='){

        if(node->left != NULL && node->left->type == AST_DEREFERENCE){
            char* right_result = Generate_TAC(node->right , program);
            TAC t = {0};
            strcpy(t.result , node->left->lexeme);
            //printf("the left result is %s\n",node->left->lexeme);
            strcpy(t.op1 , right_result);
            //printf("the right result is %s\n",right_result);
            strcpy(t.op2,"");
            strcpy(t.opr , "*");
            t.type = TAC_ASSIGN;
            t.is_deref_write = 1;
            program->code[program->tac_count++] = t;

            //printf("*%s = %s\n",t.result , t.op1);

            //free(node->left->lexeme);
            free(right_result);
            return strdup("");
        }

        char* left_result = Generate_TAC(node->left , program);
        char* right_result = Generate_TAC(node->right , program);
        TAC t = {0};
        strcpy(t.result , left_result);
        strcpy(t.op1 , right_result);
        strcpy(t.op2,"");
        strcpy(t.opr , node->lexeme);
        t.is_addr = 1;
        if(node->right->type == AST_CHARACTER){
            t.is_char_lit = 1;
        }
        program->code[program->tac_count++] = t;
        //printf("statement : \n");
        //printf("\n%s = %s\n",left_result , right_result);

        free(left_result);
        free(right_result);
        return strdup("");
            
        
    }

    char* left_result = Generate_TAC(node->left , program);
    char* right_result = Generate_TAC(node->right , program);

    char* temp = new_temp();

    TAC t = {0};
    strcpy(t.result , temp);
    strcpy(t.op1 , left_result);
    strcpy(t.op2 , right_result);
    strcpy(t.opr , node->lexeme);
    t.is_deref_write = 0;
    program->code[program->tac_count++] = t;

    //printf(" %s = %s %s %s\n",temp , left_result , node->lexeme , right_result);

    //printf("tac count at the end = %d\n",program->tac_count);

    free(left_result);
    free(right_result);

    return temp;
}

void print_TAC(TACProgram *program){
    printf("===========TAC PRINT===========\n");
    printf("tac_count = %d.\n", program->tac_count);
    for(int i=0 ; i<program->tac_count ; i++){
        printf("at %d the tac is:\n",i);
        printf("TAC : OP1 = %s , OP2 = %s , OPR = %s , RESULT = %s , LABEL = %s.\n",program->code[i].op1 , program->code[i].op2 , program->code[i].opr , program->code[i].result , program->code[i].label);
    }
}



