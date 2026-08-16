#include"parser.h"
#include"tokenizer.h"
#include"TACcode.h"
#include"semantic.h"

int tac_count = 0 ;
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

void Generate_if_tac(NODE *node , TACProgram *program){
    //printf("generate if position %d:\n", if_pos);
    char* l1 = new_label();
    char* l2 = new_label();
    //char* l3 = new_label();

    char* opr;
    char* op1;
    char* op2;

    NODE *cond_node = node->left;

    strcpy(opr , cond_node->lexeme);
    strcpy(op1 , cond_node->left->lexeme);
    strcpy(op2 , cond_node->right->lexeme);
    
    //printf("emitting if goto\n");
    emit_IF_GOTO(program , op1 , opr , op2 , l1);
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
    //printf("while tac generation starts\n");

    char* l3 = new_label();
    char* l4 = new_label();

    NODE *cond = node->cond;

    emit_LABEL(program , l3);

    char* op1 = cond->left->lexeme;
    char* opr = cond->lexeme;
    char* op2 = cond->right->lexeme;

    char* inver_opr = invert_condition(opr);

    

    emit_IF_GOTO(program , op1 , inver_opr , op2 , l4);

    NODE *current = node->body->head;

    while(current != NULL){
        Generate_TAC(current , program);
        current = current->next;
    }

    emit_GOTO(program , l3);

    emit_LABEL(program , l4);

    free(l3);
    free(l4);

}

int Generate_for_TAC(NODE *node , TACProgram *program){
    //printf("for statement started.\n");
    char* L5 = new_label();
    char* L6 = new_label();

    Generate_TAC(node->for_node->init_node , program);

    NODE *cond = node->cond;

    
    emit_LABEL(program , L5);

    char* op1 = cond->left->lexeme;
    char* opr = cond->lexeme;
    char* op2 = cond->right->lexeme;

    char* invert_opr = invert_condition(opr);

    emit_IF_GOTO(program , op1 , invert_opr , op2 , L6);


    NODE *current = node->body;

    while(current != NULL){
        Generate_TAC(current , program);
        current = current->next;
    }

    Generate_TAC(node->for_node->update_node , program);

    emit_GOTO(program , L5);

    emit_LABEL(program , L6);

    free(L5);
    free(L6);

}

char* Generate_TAC(NODE* node , TACProgram *program){
    if(node == NULL){
        return NULL;
    }

    switch(node->type){
        case AST_FOR:
            Generate_for_TAC(node , program);
            return NULL ;
        
        case AST_WHILE:
            Generate_while_tac(node , program);
            return NULL ;
        
        case AST_IF :
            Generate_if_tac(node , program);
            return NULL ;
        
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
        char count_string[10];
        sprintf(count_string , "%d", node->ARG_count);
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

        if(node->left->type == AST_DEREFERENCE && node->left != NULL){
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

    free(left_result);
    free(right_result);

    return temp;
}

void print_TAC(TACProgram *program){
    printf("===========TAC PRINT===========\n");
    for(int i=0 ; i<program->tac_count ; i++){
        printf("TAC : OP1 = %s , OP2 = %s , OPR = %s , RESULT = %s , LABEL = %s.\n",program->code[i].op1 , program->code[i].op2 , program->code[i].opr , program->code[i].result , program->code[i].label);
    }
}



