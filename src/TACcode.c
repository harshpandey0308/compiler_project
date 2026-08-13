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

int Generate_if_tac(TokenEntry *token_table , int if_pos , TACProgram *program){
    //printf("generate if position %d:\n", if_pos);
    char* l1 = new_label();
    char* l2 = new_label();
    //char* l3 = new_label();

    int cond1_start = if_pos + 2;
    int cond1_end = cond1_start;

    while(strcmp(token_table->tokens[cond1_end].lexeme , ")") != 0){
        cond1_end++;
    }
    cond1_end--;
    //printf("Condition starts at %d and ends at %d\n",cond1_start , cond1_end);
    
    char* opr;
    char* op1;
    char* op2;

    if(strcmp(token_table->tokens[cond1_start].lexeme , "*") == 0){
        opr = token_table->tokens[cond1_start+2].lexeme;
        sprintf(op1 , "*%s" , token_table->tokens[cond1_start + 1].lexeme);
        op2 = token_table->tokens[cond1_start+3].lexeme;
    }
    else{
        op1 = token_table->tokens[cond1_start].lexeme;
        opr = token_table->tokens[cond1_start + 1].lexeme;
    //printf("Operator is %s %d\n",opr,cond1_start + 1);
        op2 = token_table->tokens[cond1_start + 2].lexeme;
    }
    
    //printf("emitting if goto\n");
    emit_IF_GOTO(program , op1 , opr , op2 , l1);
    //printf("goto emitted\n");


    int else_pos = if_pos;

    
    //printf("else_pos = %d\n",else_pos);

    while(!(token_table->tokens[else_pos].tokentype == TOKEN_KEYWORD && (strcmp(token_table->tokens[else_pos].lexeme , "else") == 0 || strcmp(token_table->tokens[else_pos].lexeme , "else if") == 0))){
        else_pos++;
    }
    
    
    //printf("else pos = %d\n",else_pos);

    int else_start = else_pos + 2;

    int else_body_end = else_start;

    int if_depth = 0;

    while(strcmp(token_table->tokens[else_body_end].lexeme , "}") != 0){
        else_body_end++;
    }

    int k = else_start;

    while(k < token_table->token_count && strcmp(token_table->tokens[k].lexeme , "}") != 0){
        if(strcmp(token_table->tokens[k].lexeme , "while") == 0){
            k = Generate_while_tac(token_table , k , program);
        }
        else if(strcmp(token_table->tokens[k].lexeme , "if") == 0){
            k = Generate_if_tac(token_table , k , program);
        }
        else if(strcmp(token_table->tokens[k].lexeme , "for") == 0){
            k = Generate_for_TAC(token_table , k , program);
        }
        else{
            int nested_stt_end = k;
            while(strcmp(token_table->tokens[nested_stt_end].lexeme , ";") != 0){
                nested_stt_end++;
            }
            NODE *nested_root = build_AST(token_table , k , nested_stt_end-1);
            Generate_TAC(nested_root , program);
            free_tree(nested_root);

            k = nested_stt_end;
        }
        
    }


    emit_GOTO(program , l2);

    emit_LABEL(program , l1);


    int if_start = if_pos;
    
    while(strcmp(token_table->tokens[if_start].lexeme , "{") != 0){
        if_start++;
    }

    int if_body_end = if_start;
    while(strcmp(token_table->tokens[if_body_end].lexeme , "}") != 0){
        if_body_end++;
    }

    if_start++;
    int j = if_start;

    while(j < token_table->token_count && strcmp(token_table->tokens[j].lexeme , "}") != 0){
        if(strcmp(token_table->tokens[j].lexeme , "while") == 0){
            j = Generate_while_tac(token_table , j , program);
        }
        else if(strcmp(token_table->tokens[j].lexeme , "if") == 0){
            j = Generate_if_tac(token_table , j , program);
        }
        else if(strcmp(token_table->tokens[j].lexeme , "for") == 0){
            j = Generate_for_TAC(token_table , j , program);
        }
        else{
            int nested_stt_end = j;
            while(strcmp(token_table->tokens[nested_stt_end].lexeme , ";") != 0){
                nested_stt_end++;
            }
            NODE *nested_root = build_AST(token_table , j , nested_stt_end-1);
            Generate_TAC(nested_root , program);
            free_tree(nested_root);

            j = nested_stt_end;
        }
        
    }


    emit_LABEL(program , l2); 

    free(l1);
    free(l2);

    if(strcmp(token_table->tokens[if_body_end].lexeme , "}") == 0 && strcmp(token_table->tokens[if_body_end+1].lexeme , "else") == 0){
        return (else_body_end+1);
    }
    else{
        return (if_body_end+1);
    }

}

int Generate_while_tac( TokenEntry *token_table , int while_pos , TACProgram *program ){
    //printf("while tac generation starts\n");

    char* l3 = new_label();
    char* l4 = new_label();

    //printf("while position = %d.\n",while_pos);
    int cond_start = while_pos + 2;
    int cond_end = cond_start;

    while(strcmp(token_table->tokens[cond_end].lexeme , ")") != 0){
        cond_end++;
    }
    cond_end--;

    //printf("[DEBUG] : OP1 : %s , OP2 : %s , OP3 : %s.\n",token_table->tokens[cond_start].lexeme , token_table->tokens[cond_start].lexeme , token_table->tokens[cond_start].lexeme);
    //printf("assigning these to op variables.\n");
    char* op1 = token_table->tokens[cond_start].lexeme;
    char* opr = token_table->tokens[cond_start + 1].lexeme;
    char* op2 = token_table->tokens[cond_start + 2].lexeme;

    emit_LABEL(program , l3);
    //printf("LABEL  =  l3.\n");

    char* inver_opr = invert_condition(opr);
    emit_IF_GOTO(program , op1 , inver_opr , op2 , l4);

    //printf("ifgoto label emitted.\n");

    int body_start = while_pos;

    //printf("while condition starts at %d and ends at %d\n",cond_start , cond_end);

    while(strcmp(token_table->tokens[body_start].lexeme , "{") != 0){
        body_start++;
    }
    body_start++;

    //printf("while body starts at %d\n",body_start);
    int body_end = body_start;
    while(strcmp(token_table->tokens[body_end].lexeme , "}") != 0){
        body_end++;
    }

    int stmt_start = body_start;
    int j = body_start;

    //printf("chacking nested statements.\n");

    while(j<token_table->token_count && strcmp(token_table->tokens[j].lexeme , "}") != 0){
        //printf("token at %d is %s\n",j , token_table->tokens[j].lexeme);
        if(strcmp(token_table->tokens[j].lexeme , ";") == 0){
            //printf("join me \n");
            //printf("building ast for statement starting at %d and ending at %d\n",stmt_start , j);
            NODE* stmt_ast = build_AST(token_table , stmt_start , j-1);
            Generate_TAC(stmt_ast , program);
            free_tree(stmt_ast);
            stmt_start = j + 1;
        }
        j++;

        
    }

    //NODE* body_ast = Build_AST(token_table->tokens , body_start , body_end);
    //Generate_TAC(body_ast);
    //free_tree(body_ast);

    emit_GOTO(program , l3);

    emit_LABEL(program , l4);

    free(l3);
    free(l4);

    //printf("while condition starts at %d and ends at %d\n",cond_start , cond_end);
    return (body_end+1);

}

int Generate_for_TAC(TokenEntry *token_table , int for_pos , TACProgram *program){
    //printf("for statement started.\n");
    char* L5 = new_label();
    char* L6 = new_label();

    int init_start = for_pos + 2;
    int init_end = init_start;
    //printf("a\n");
    while(strcmp(token_table->tokens[init_end].lexeme , ";") != 0){
        init_end++;
    }
    //printf("b.\n");
    if(token_table->tokens[init_start].tokentype == TOKEN_KEYWORD){
        init_start = init_start + 1;
    }
    //printf("c\n");
    NODE* init_ast = build_AST(token_table , init_start , init_end - 1);
    Generate_TAC(init_ast , program);
    free_tree(init_ast);

    emit_LABEL(program , L5);

    int cond_stt = init_end + 1;
    int cond_end = cond_stt;

    if(strcmp(token_table->tokens[cond_stt - 1].lexeme , ";") == 0){
        while(strcmp(token_table->tokens[cond_end].lexeme , ";") != 0 ){
        cond_end++;
        }
    }


    //NODE* cond_ast = Build_AST(token_table->tokens , cond_stt , cond_end - 1);
    //Generate_TAC(cond_ast);
    //free_tree(cond_ast);

    char* op1 = token_table->tokens[cond_stt].lexeme;
    char* opr = token_table->tokens[cond_stt + 1].lexeme;
    char* op2 = token_table->tokens[cond_stt + 2].lexeme;

    char* invert_opr = invert_condition(opr);
    emit_IF_GOTO(program , op1 , invert_opr , op2 , L6);


    int body_begins = for_pos;
    while(strcmp(token_table->tokens[body_begins].lexeme , "{") != 0){
        body_begins++;
    }
    body_begins++;

    int stmt_stt = body_begins;
    int j = body_begins;
    //printf("checkng nested statements.\n");

    while(j < token_table->token_count && strcmp(token_table->tokens[j].lexeme , "}") != 0){
        //if(strcmp(token_table->tokens[j].lexeme , ";") == 0){
            //NODE* body_ast = build_AST(token_table , stmt_stt , j - 1);
            //Generate_TAC(body_ast , program);
            //free_tree(body_ast);
            //stmt_stt = j + 1;
        //}
        //printf("ENTERED WHILE LOOP.\n");
        if(strcmp(token_table->tokens[j].lexeme , "while") == 0){
            //printf("the nested statement is while loop.\n");
            j = Generate_while_tac(token_table , j , program);
        }
        else if(strcmp(token_table->tokens[j].lexeme , "if") == 0){
            //printf("the nested statement is if statement.\n");
            j = Generate_if_tac(token_table , j , program);
        }
        else if(strcmp(token_table->tokens[j].lexeme , "for") == 0){
            //printf("the nested statement is for statement.\n");
            j = Generate_for_TAC(token_table , j , program);
        }
        else{
            int nested_stt_end = j;
            //printf("normal statement.\n");
            while(strcmp(token_table->tokens[nested_stt_end].lexeme , ";") != 0){
                //printf("findinf length of the block.\n");
                nested_stt_end++;
            }
            //printf("building roots .\n");
            NODE *nested_root = build_AST(token_table , j , nested_stt_end-1);
            Generate_TAC(nested_root , program);
            free_tree(nested_root);

            j = nested_stt_end;
        }
        //printf("J inside loop = %d.\n",j);
        j++;
    }

    //printf("J = %d.\n",j);

    int body_end = j++;

    int up_stt = cond_end + 1;
    int up_end = up_stt;
    
    while(strcmp(token_table->tokens[up_end].lexeme , ")") != 0){
        up_end++;
    }
    

    NODE* up_ast = build_AST(token_table , up_stt , up_end - 1);
    Generate_TAC(up_ast , program);

    //printf("FOR: init_end=%d cond_stt=%d cond_end=%d up_stt=%d\n",init_end, cond_stt, cond_end, up_stt);
    //printf("\n");
    //printf("UPDATE token_table->tokens: %s %s %s %s\n",token_table->tokens[up_stt].lexeme, token_table->tokens[up_stt+1].lexeme,token_table->tokens[up_stt+2].lexeme, token_table->tokens[up_stt+3].lexeme);

    free_tree(up_ast);

    emit_GOTO(program , L5);

    emit_LABEL(program , L6);

    free(L5);
    free(L6);

    //printf("for stataement end.\n");

    return (body_end+1);

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



