#include"parser.h"
#include"tokenizer.h"
#include"TACcode.h"
#include"semantic.h"



TAC tac_table[100];
int tac_count = 0 ;
int temp_count = 0;
int label_count = 0;

void emit_ASSIGN(char* result , char* op1 , char* op2 , char* opr){
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
    tac_table[tac_count++] = t;
}

void emit_IF_GOTO(char* op1 , char* opr , char* op2 , char* label){
    TAC t = {0};
    t.type = TAC_IF_GOTO;
    strcpy(t.op1 , op1);
    //printf(" TAC IF GOTO : t.op1 = %s\n",t.op1);
    strcpy(t.op2 , op2);
    //printf("TAC IF GOTO : t.op2 = %s\n",t.op2);
    strcpy(t.opr , opr);
    //printf("TAC IF GOTO : t.opr = %s\n",t.opr);
    strcpy(t.label , label);
    tac_table[tac_count++] = t;
}

void emit_GOTO(char* label){
    TAC t = {0};
    t.type = TAC_GOTO;
    strcpy(t.label , label);
    tac_table[tac_count++] = t;
}

void emit_LABEL(char* label){
    TAC t = {0};
    t.type = TAC_LABEL;
    strcpy(t.label , label);
    tac_table[tac_count++] = t;
}

void emit_PARAM(char* value){
    TAC t = {0};
    t.type = PARAM;
    strcpy(t.op1 , value);
    tac_table[tac_count++] = t;
}

void emit_param_addr(char *value){
    TAC t = {0};
    t.type = TAC_PARAM_ADDR;
    strcpy(t.op1 , value);
    tac_table[tac_count++] = t;
}

void emit_CALL(char* name , int arg_count){
    TAC t = {0};
    t.type = FUNC_CALL;
    strcpy(t.op1 , name);
    sprintf(t.op2 , "%d" , arg_count);
    tac_table[tac_count++] = t;
}

void emit_RETURN(char* value){
    TAC t = {0};
    t.type = RETURN;
    strcpy(t.op1 , value);
    tac_table[tac_count++] = t;
}

void emit_PUSH(char *value){
    TAC t = {0};
    t.type = TAC_PUSH;
    strcpy(t.op1 , value);
    tac_table[tac_count++] = t;
}

void emit_PULL(char *dest){
    TAC t = {0};
    t.type = TAC_POP;
    strcpy(t.result , dest);
    tac_table[tac_count++] = t;
}

void emit_JMP_DYN(char *reg){
    TAC t = {0};
    t.type = TAC_JMP_DYNAMIC;
    strcpy(t.op1 , reg);
    tac_table[tac_count++] = t;
}

void emit_FUNC_BEG(char *name){
    TAC t = {0};
    t.type = TAC_FUNC_BEGIN;
    strcpy(t.label , name);
    tac_table[tac_count++] = t;
}

void emit_param_string(char *str){
    TAC t = {0};
    t.type = TAC_PARAM_STRING;
    strcpy(t.op1 , str);
    tac_table[tac_count++] = t;
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

void Generate_if_tac(TOKEN tokens[] , int if_pos){
    //printf("generate if position %d:\n", if_pos);
    char* l1 = Label();
    char* l2 = Label();
    //char* l3 = Label();

    int cond1_start = if_pos + 2;
    int cond1_end = cond1_start;

    while(strcmp(tokens[cond1_end].value , ")") != 0){
        cond1_end++;
    }
    cond1_end--;
    //printf("Condition starts at %d and ends at %d\n",cond1_start , cond1_end);
    
    char* opr;
    char* op1;
    char* op2;

    if(strcmp(tokens[cond1_start].value , "*") == 0){
        opr = tokens[cond1_start+2].value;
        sprintf(op1 , "*%s" , tokens[cond1_start + 1].value);
        op2 = tokens[cond1_start+3].value;
    }
    else{
        op1 = tokens[cond1_start].value;
        opr = tokens[cond1_start + 1].value;
    //printf("Operator is %s %d\n",opr,cond1_start + 1);
        op2 = tokens[cond1_start + 2].value;
    }
    
    //printf("emitting if goto\n");
    emit_IF_GOTO(op1 , opr , op2 , l1);
    //printf("goto emitted\n");
    int else_pos = if_pos;
    //printf("else_pos = %d\n",else_pos);

    while(!(tokens[else_pos].tokentype == KEYWORD && (strcmp(tokens[else_pos].value , "else") == 0 || strcmp(tokens[else_pos].value , "else if") == 0))){
        else_pos++;
    }
    
    //printf("else pos = %d\n",else_pos);

    int else_start = else_pos + 2;
    int else_end = else_start;
    
    while(strcmp(tokens[else_end].value , ";") != 0){
        else_end++;
    }
    else_end--;

    //printf("%d %s\n",else_start , tokens[else_start].value);
    //printf("else_end = %d , value at %d = %s\n",else_end , else_end , tokens[else_end].value);

    //printf("build ast\n");
    //printf("tokens at else start = %s\n",tokens[else_start].value);
    NODE* else_ast = Build_AST(tokens , else_start , else_end);
    //printf("generate tac for else\n");
    Generate_TAC(else_ast);
    //printf("else tac generated\n");
    free_tree(else_ast);

    emit_GOTO(l2);

    emit_LABEL(l1);


    int if_start = if_pos;
    
    while(strcmp(tokens[if_start].value , "{") != 0){
        if_start++;
    }
    if_start++;

    int if_end = if_start;
    
    while(strcmp(tokens[if_end].value , ";") != 0){
        if_end++;
    }
    if_end--;

    //printf("if ends at %d\n",if_end);
    NODE* if_ast = Build_AST(tokens , if_start , if_end);
    //printf("generate tac for if");
    Generate_TAC(if_ast);
    free_tree(if_ast);

    emit_LABEL(l2); 

    free(l1);
    free(l2);

}

void Generate_while_tac(TOKEN tokens[] , int while_pos){
//printf("while tac generation starts\n");

    char* l3 = Label();
    char* l4 = Label();

    int cond_start = while_pos + 2;
    int cond_end = cond_start;

    while(strcmp(tokens[cond_end].value , ")") != 0){
        cond_end++;
    }
    cond_end--;

    char* op1 = tokens[cond_start].value;
    char* opr = tokens[cond_start + 1].value;
    char* op2 = tokens[cond_start + 2].value;

    emit_LABEL(l3);

    char* inver_opr = invert_condition(opr);
    emit_IF_GOTO(op1 , inver_opr , op2 , l4);

    int body_start = while_pos;

    //printf("while condition starts at %d and ends at %d\n",cond_start , cond_end);

    while(strcmp(tokens[body_start].value , "{") != 0){
        body_start++;
    }
    body_start++;

    //printf("while body starts at %d\n",body_start);
    int body_end = body_start;

    int stmt_start = body_start;
    int j = body_start;

    while(j<token_count && strcmp(tokens[j].value , "}") != 0){
        //printf("token at %d is %s\n",j , tokens[j].value);
        if(strcmp(tokens[j].value , ";") == 0){
            //printf("join me \n");
            //printf("building ast for statement starting at %d and ending at %d\n",stmt_start , j);
            NODE* stmt_ast = Build_AST(tokens , stmt_start , j-1);
            Generate_TAC(stmt_ast);
            free_tree(stmt_ast);
            stmt_start = j + 1;
        }
        j++;

        
    }

    //NODE* body_ast = Build_AST(tokens , body_start , body_end);
    //Generate_TAC(body_ast);
    //free_tree(body_ast);

    emit_GOTO(l3);

    emit_LABEL(l4);

    free(l3);
    free(l4);

    //printf("while condition starts at %d and ends at %d\n",cond_start , cond_end);

}

void Generate_for_TAC(TOKEN tokens[] , int for_pos){
    char* L5 = Label();
    char* L6 = Label();

    int init_start = for_pos + 2;
    int init_end = init_start;

    while(strcmp(tokens[init_end].value , ";") != 0){
        init_end++;
    }

    NODE* init_ast = Build_AST(tokens , init_start , init_end - 1);
    Generate_TAC(init_ast);
    free_tree(init_ast);

    emit_LABEL(L5);

    int cond_stt = init_end + 1;
    int cond_end = cond_stt;

    if(strcmp(tokens[cond_stt - 1].value , ";") == 0){
        while(strcmp(tokens[cond_end].value , ";") != 0 ){
        cond_end++;
        }
    }


    //NODE* cond_ast = Build_AST(tokens , cond_stt , cond_end - 1);
    //Generate_TAC(cond_ast);
    //free_tree(cond_ast);

    char* op1 = tokens[cond_stt].value;
    char* opr = tokens[cond_stt + 1].value;
    char* op2 = tokens[cond_stt + 2].value;

    char* invert_opr = invert_condition(opr);
    emit_IF_GOTO(op1 , invert_opr , op2 , L6);


    int body_begins = for_pos;
    while(strcmp(tokens[body_begins].value , "{") != 0){
        body_begins++;
    }
    body_begins++;

    int stmt_stt = body_begins;
    int j = body_begins;

    while(j < token_count && strcmp(tokens[j].value , "}") != 0){
        if(strcmp(tokens[j].value , ";") == 0){
            NODE* body_ast = Build_AST(tokens , stmt_stt , j - 1);
            Generate_TAC(body_ast);
            free_tree(body_ast);
            stmt_stt = j + 1;
        }
        j++;
    }

    int up_stt = cond_end + 1;
    int up_end = up_stt;
    
    while(strcmp(tokens[up_end].value , ")") != 0){
        up_end++;
    }
    

    NODE* up_ast = Build_AST(tokens , up_stt , up_end - 1);
    Generate_TAC(up_ast);
    free_tree(up_ast);

    emit_GOTO(L5);

    emit_LABEL(L6);

    free(L5);
    free(L6);

}

char* Generate_TAC(NODE* node){
    //printf("Generating TAC for node with value %s\n",node->value);
    if(node->is_addr_of == 1){
        char* temp = new_temp();
        TAC t = {0};

        t.type = TAC_ASSIGN;
        strcpy(t.result , temp);
        //printf("t.result : %s.\n",t.result);
        strcpy(t.op1 , node->value);
        //printf("t.op1: %s.\n",t.op1);
        strcpy(t.op2 , "");
        strcpy(t.opr , "&");
        
        t.is_deref_write = 0;
        t.is_addr = 1;
        tac_table[tac_count++] = t;

        return temp;
    }

    if(node->is_deref == 1){
        char* temp = new_temp();
        TAC t = {0};

        t.type = TAC_ASSIGN;
        strcpy(t.result , temp);
        strcpy(t.op1 , node->value);
        strcpy(t.op2 , "");
        strcpy(t.opr , "*");
        t.is_deref_write = 0;
        tac_table[tac_count++] = t;

        return temp;
    }

    if(node->is_Call == 1){
        char *ret_labels = Label();
        for(int i=0 ; i<node->ARG_count ; i++){
            if(node->ARG[i]->is_string == 1){
                emit_param_string(node->ARG[i]->value);
            }
            else if(node->ARG[i]->is_addr_of == 1){
                emit_param_addr(node->ARG[i]->value);
            }
            else{
                char* arg_val = Generate_TAC(node->ARG[i]);
                emit_PARAM(arg_val);
                free(arg_val);
            }
            
        }
        emit_PUSH(ret_labels);
        char count_string[10];
        sprintf(count_string , "%d", node->ARG_count);
        //printf("the value is :%s\n",node->value);
        emit_CALL(node->value , node->ARG_count);

        emit_LABEL(ret_labels);

        return strdup("RETVAL");
    }

    if(node->left == NULL && node->right == NULL){
        char* value = (char*)malloc(50);
        strcpy(value , node->value);
        //printf("Leaf node : %s\n",value);
        return value;
    }

    

    if(node->value[0] == '='){

        if(node->left->is_deref == 1 && node->left != NULL){
            char* right_result = Generate_TAC(node->right);
            TAC t = {0};
            strcpy(t.result , node->left->value);
            //printf("the left result is %s\n",node->left->value);
            strcpy(t.op1 , right_result);
            //printf("the right result is %s\n",right_result);
            strcpy(t.op2,"");
            strcpy(t.opr , "*");
            t.type = TAC_ASSIGN;
            t.is_deref_write = 1;
            tac_table[tac_count++] = t;

            printf("*%s = %s\n",t.result , t.op1);

            //free(node->left->value);
            free(right_result);
            return strdup("");
        }

        char* left_result = Generate_TAC(node->left);
        char* right_result = Generate_TAC(node->right);
        TAC t = {0};
        strcpy(t.result , left_result);
        strcpy(t.op1 , right_result);
        strcpy(t.op2,"");
        strcpy(t.opr , node->value);
        t.is_addr = 1;
        tac_table[tac_count++] = t;
        //printf("statement : \n");
        printf("%s = %s\n",left_result , right_result);

        free(left_result);
        free(right_result);
        return strdup("");
            
        
    }

    char* left_result = Generate_TAC(node->left);
    char* right_result = Generate_TAC(node->right);

    char* temp = new_temp();

    TAC t = {0};
    strcpy(t.result , temp);
    strcpy(t.op1 , left_result);
    strcpy(t.op2 , right_result);
    strcpy(t.opr , node->value);
    t.is_deref_write = 0;
    tac_table[tac_count++] = t;

    printf(" %s = %s %s %s\n",temp , left_result , node->value , right_result);

    free(left_result);
    free(right_result);

    return temp;
}



