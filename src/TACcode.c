#include"parser.h"
#include"tokenizer.h"
#include"TACcode.h"
#include"semantic.h"



TAC tac_table[500];
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

char* Label(){
    char* label = (char*)malloc(10);
    sprintf(label , "L%d" , ++label_count);
    return label;
}

void Generate_if_tac(TOKEN tokens[] , int if_pos , TACProgram *program){
    //printf("generate if position %d:\n", if_pos);
    char* l1 = Label();
    char* l2 = Label();
    //char* l3 = Label();

    int cond1_start = if_pos + 2;
    int cond1_end = cond1_start;

    while(strcmp(tokens[cond1_end].lexeme , ")") != 0){
        cond1_end++;
    }
    cond1_end--;
    //printf("Condition starts at %d and ends at %d\n",cond1_start , cond1_end);
    
    char* opr;
    char* op1;
    char* op2;

    if(strcmp(tokens[cond1_start].lexeme , "*") == 0){
        opr = tokens[cond1_start+2].lexeme;
        sprintf(op1 , "*%s" , tokens[cond1_start + 1].lexeme);
        op2 = tokens[cond1_start+3].lexeme;
    }
    else{
        op1 = tokens[cond1_start].lexeme;
        opr = tokens[cond1_start + 1].lexeme;
    //printf("Operator is %s %d\n",opr,cond1_start + 1);
        op2 = tokens[cond1_start + 2].lexeme;
    }
    
    //printf("emitting if goto\n");
    emit_IF_GOTO(program , op1 , opr , op2 , l1);
    //printf("goto emitted\n");
    int else_pos = if_pos;
    //printf("else_pos = %d\n",else_pos);

    while(!(tokens[else_pos].tokentype == TOKEN_KEYWORD && (strcmp(tokens[else_pos].lexeme , "else") == 0 || strcmp(tokens[else_pos].lexeme , "else if") == 0))){
        else_pos++;
    }
    
    //printf("else pos = %d\n",else_pos);

    int else_start = else_pos + 2;
    int else_end = else_start;
    
    while(strcmp(tokens[else_end].lexeme , ";") != 0){
        else_end++;
    }
    else_end--;

    //printf("%d %s\n",else_start , tokens[else_start].lexeme);
    //printf("else_end = %d , lexeme at %d = %s\n",else_end , else_end , tokens[else_end].lexeme);

    //printf("build ast\n");
    //printf("tokens at else start = %s\n",tokens[else_start].lexeme);
    NODE* else_ast = Build_AST(tokens , else_start , else_end);
    //printf("generate tac for else\n");
    Generate_TAC(else_ast , program);
    //printf("else tac generated\n");
    free_tree(else_ast);

    emit_GOTO(program , l2);

    emit_LABEL(program , l1);


    int if_start = if_pos;
    
    while(strcmp(tokens[if_start].lexeme , "{") != 0){
        if_start++;
    }
    if_start++;

    int if_end = if_start;
    
    while(strcmp(tokens[if_end].lexeme , ";") != 0){
        if_end++;
    }
    if_end--;

    //printf("if ends at %d\n",if_end);
    NODE* if_ast = Build_AST(tokens , if_start , if_end);
    //printf("generate tac for if");
    Generate_TAC(if_ast , program);
    free_tree(if_ast);

    emit_LABEL(program , l2); 

    free(l1);
    free(l2);

}

void Generate_while_tac(TOKEN tokens[] , int while_pos , TACProgram *program){
//printf("while tac generation starts\n");

    char* l3 = Label();
    char* l4 = Label();

    int cond_start = while_pos + 2;
    int cond_end = cond_start;

    while(strcmp(tokens[cond_end].lexeme , ")") != 0){
        cond_end++;
    }
    cond_end--;

    char* op1 = tokens[cond_start].lexeme;
    char* opr = tokens[cond_start + 1].lexeme;
    char* op2 = tokens[cond_start + 2].lexeme;

    emit_LABEL(program , l3);

    char* inver_opr = invert_condition(opr);
    emit_IF_GOTO(program , op1 , inver_opr , op2 , l4);

    int body_start = while_pos;

    //printf("while condition starts at %d and ends at %d\n",cond_start , cond_end);

    while(strcmp(tokens[body_start].lexeme , "{") != 0){
        body_start++;
    }
    body_start++;

    //printf("while body starts at %d\n",body_start);
    int body_end = body_start;

    int stmt_start = body_start;
    int j = body_start;

    while(j<token_count && strcmp(tokens[j].lexeme , "}") != 0){
        //printf("token at %d is %s\n",j , tokens[j].lexeme);
        if(strcmp(tokens[j].lexeme , ";") == 0){
            //printf("join me \n");
            //printf("building ast for statement starting at %d and ending at %d\n",stmt_start , j);
            NODE* stmt_ast = Build_AST(tokens , stmt_start , j-1);
            Generate_TAC(stmt_ast , program);
            free_tree(stmt_ast);
            stmt_start = j + 1;
        }
        j++;

        
    }

    //NODE* body_ast = Build_AST(tokens , body_start , body_end);
    //Generate_TAC(body_ast);
    //free_tree(body_ast);

    emit_GOTO(program , l3);

    emit_LABEL(program , l4);

    free(l3);
    free(l4);

    //printf("while condition starts at %d and ends at %d\n",cond_start , cond_end);

}

void Generate_for_TAC(TOKEN tokens[] , int for_pos , TACProgram *program){
    char* L5 = Label();
    char* L6 = Label();

    int init_start = for_pos + 2;
    int init_end = init_start;

    while(strcmp(tokens[init_end].lexeme , ";") != 0){
        init_end++;
    }

    if(tokens[init_start].tokentype == TOKEN_KEYWORD){
        init_start = init_start + 1;
    }

    NODE* init_ast = Build_AST(tokens , init_start , init_end - 1);
    Generate_TAC(init_ast , program);
    free_tree(init_ast);

    emit_LABEL(program , L5);

    int cond_stt = init_end + 1;
    int cond_end = cond_stt;

    if(strcmp(tokens[cond_stt - 1].lexeme , ";") == 0){
        while(strcmp(tokens[cond_end].lexeme , ";") != 0 ){
        cond_end++;
        }
    }


    //NODE* cond_ast = Build_AST(tokens , cond_stt , cond_end - 1);
    //Generate_TAC(cond_ast);
    //free_tree(cond_ast);

    char* op1 = tokens[cond_stt].lexeme;
    char* opr = tokens[cond_stt + 1].lexeme;
    char* op2 = tokens[cond_stt + 2].lexeme;

    char* invert_opr = invert_condition(opr);
    emit_IF_GOTO(program , op1 , invert_opr , op2 , L6);


    int body_begins = for_pos;
    while(strcmp(tokens[body_begins].lexeme , "{") != 0){
        body_begins++;
    }
    body_begins++;

    int stmt_stt = body_begins;
    int j = body_begins;

    while(j < token_count && strcmp(tokens[j].lexeme , "}") != 0){
        if(strcmp(tokens[j].lexeme , ";") == 0){
            NODE* body_ast = Build_AST(tokens , stmt_stt , j - 1);
            Generate_TAC(body_ast , program);
            free_tree(body_ast);
            stmt_stt = j + 1;
        }
        j++;
    }

    int up_stt = cond_end + 1;
    int up_end = up_stt;
    
    while(strcmp(tokens[up_end].lexeme , ")") != 0){
        up_end++;
    }
    

    NODE* up_ast = Build_AST(tokens , up_stt , up_end - 1);
    Generate_TAC(up_ast , program);

    printf("FOR: init_end=%d cond_stt=%d cond_end=%d up_stt=%d\n",init_end, cond_stt, cond_end, up_stt);
    printf("UPDATE tokens: %s %s %s %s\n",tokens[up_stt].lexeme, tokens[up_stt+1].lexeme,tokens[up_stt+2].lexeme, tokens[up_stt+3].lexeme);

    free_tree(up_ast);

    emit_GOTO(program , L5);

    emit_LABEL(program , L6);

    free(L5);
    free(L6);

}

char* Generate_TAC(NODE* node , TACProgram *program){
    //printf("Generating TAC for node with lexeme %s\n",node->lexeme);
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
        char *ret_labels = Label();
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



