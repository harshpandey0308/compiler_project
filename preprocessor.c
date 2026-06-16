#include<stdio.h>
#include<ctype.h>
#include<string.h>
#include<stdlib.h>
#include "preproceesor.h"

DEFINE define_table[MAX_DEFINES];
int define_count = 0;

void add_define(const char* name , const char* value){
    strcpy(define_table[define_count].name , name);
    strcpy(define_table[define_count].value , value);
    define_count++;
    printf("DEFINE ADDED: %s = %s\n" , name , value);
}

void substitute(char* line , char* result){
    char temp[MAX_LINE_LEN] = " ";
    char word[50];

    int i=0 , j=0;

    while(line[i] != '\0'){
        if(isalpha(line[i]) || line[i] == '_'){
            j = 0;
            while(isalnum(line[i] ) || line[i] == '_'){
                word[j++] = line[i++];
            }
            word[j] = '\0';

            int found = 0;
            for(int k=0 ; k<define_count ; k++){
                if(strcmp(define_table[k].name , word) == 0){
                    strcat(temp , define_table[k].value);
                    found = 1;
                    break;
                }

            }

            if(!found) strcat(temp , word);
        }
        else{
            char ch[2] = {line[i++] , '\0'};
            strcat(temp , ch);
        }
    }
    strcpy(result , temp);
}

int preprocesses(const char* filename , char lines[][MAX_LINE_LEN]){
    FILE* file = fopen(filename , "r");

    if(file == NULL){
        printf("ERROR : Could not open file %s \n" , filename);
        return 0;
    }

    int line_count = 0;
    char raw[MAX_LINE_LEN];

    while(fgets(raw , MAX_LINE_LEN , file) != NULL){

        raw[strcspn(raw , "\n")] = '\0';

        char* trimmed = raw;

        while(*trimmed == ' ' || *trimmed == '\t') trimmed++;

        if(strlen(trimmed) == 0) continue;

        if(strncmp(trimmed , "#include" , 8) == 0){

            char * start = strchr(trimmed , '<');

            if(start == NULL) start = strchr(trimmed , '"');

            if(start != NULL){
                start++;
                char header[50];
                int k = 0 ;
                while(*start != '>' && *start != '"' && *start != '\0'){
                    header[k++] = *start++;
                }
                header[k] = '\0';

                printf("INCLUDE : <%s> noted\n" , header);
            }
            continue;
        }

        if(strncmp(trimmed , "#define" , 7) == 0){
            char name[50] , value[50];
            sscanf(trimmed , "#define %s %s" , name ,value);
            add_define(name , value);
            continue;
        }

        if(strncmp(trimmed , "return" , 6) == 0){
            continue;
        }

        //if(strstr(trimmed , "main") != NULL){
            //continue;
        //}

        if(strcmp(trimmed , "{") == 0 || strcmp(trimmed , "}") == 0){
            strcpy(lines[line_count++] , trimmed);
            continue;
        }

        char processed[MAX_LINE_LEN];
        substitute(trimmed , processed);

        char* final = processed;
        while(*final == ' ' || *final == '\t') final++;

        if(strlen(final)>0){
            strcpy(lines[line_count++] , final);
            //printf("processed line : %s\n" , final);
        }

        //printf("Processed line : %s\n" , processed);
        //strcpy(lines[line_count++] , processed); 
    }

    fclose(file);

    printf("Preprocessing complete : %d lines\n" , line_count);
    return line_count;
}