#include "raylib.h"
#include"../compiler.h"

#define RAYGUI_IMPLEMENTATION
#include"raygui.h"

#define FONT_SIZE 20
#define LINE_HEIGHT 25
#define SCROLL_SPEED_X 40.0f
#define SCROLL_SPEED_Y 40.0f


const int screen_width = 1600;
const int screen_height = 900;

const Color black = (Color){0, 0, 0, 255};
const Color green = (Color){0, 255, 0, 255};

const Color text_color = (Color){255, 255, 255, 255};

typedef enum{
    VIEW_AST,
    VIEW_TAC,
    VIEW_OUTPUT,
    VIEW_VM,
    VIEW_SYMBOL
}view_mode;

static int GetTextWidth(const char *text , int font_size){
    int max_width = 0;

    const char* start = text;

    while(*start != '\0'){
        int length = 0;

        const char *end = strchr(text , '\n');

        if(end){
            length = (int)(end - start);
        }
        else{
            length = strlen(start);
        }

        char line[2048];

        if(length >= sizeof(line)){
            length = sizeof(line) - 1;
        }

        memcpy(line , start , length);
        line[length] = '\0';

        int width  = MeasureText(line , font_size);

        if(width > max_width){
            max_width = width;
        }

        if(!end){
            break;
        }

        start = end+1;
    }

    return max_width;
}

void DrawScrollableTextPanel(const char *text , Rectangle panel , float *X_scroll , float *Y_scroll){
    if(text == NULL || text[0] == '\0'){
        return;
    }

    int line_count = 0;
    int max_line_width = 0;

    const char *line_start = text;

    while(*line_start != '\0'){
        const char *line_end = strchr(line_start , '\n');

        size_t line_length;

        if(line_end != NULL){
            line_length = (int)(line_end - line_start);
        }
        else{
            line_length = (int)strlen(line_start);
        }

        if(line_length > max_line_width){
            max_line_width = line_length;
        }

        line_count++;

        if(line_end == NULL){
            break;
        }

        line_start = line_end + 1;
    }

    float content_height = line_count*LINE_HEIGHT;

    float content_width = GetTextWidth(text , FONT_SIZE);

    float max_scroll_x = content_width - panel.width;

    float max_scroll_y = content_height - panel.height; 

    

    if(max_scroll_x < 0){
        max_scroll_x = 0;
    }

    if(max_scroll_y < 0){
        max_scroll_y = 0;
    }

    if(CheckCollisionPointRec(GetMousePosition() , panel)){

        float wheel = GetMouseWheelMove();

        if(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)){
            *X_scroll -= wheel*SCROLL_SPEED_X;
        }
        else{
            *Y_scroll -= wheel*SCROLL_SPEED_Y;
        }

    }

    if(*X_scroll < 0){
        *X_scroll = 0;
    }

    if(*X_scroll > max_scroll_x){
        *X_scroll = max_scroll_x;
    }

    if(*Y_scroll < 0){
        *Y_scroll = 0;
    }

    if(*Y_scroll > max_scroll_y){
        *Y_scroll = max_scroll_y;
    }

    BeginScissorMode((int)panel.x , (int)panel.y , (int)panel.width , (int)panel.height);

    line_start = text;

    float y = panel.y - *Y_scroll;

    for(const char *ch = text ; *ch != '\0' ; ch++){
        if(*ch == '\n'){
            line_count++;
        }
    }

    if(text[0] == '\0'){
        line_count++;
    }

    
    while(*line_start != '\0'){
        const char *line_end = strchr(line_start , '\n');

        size_t line_length;

        if(line_end != NULL){
            line_length = (int)(line_end - line_start);
        }
        else{
            line_length = (int)strlen(line_start);
        }

        char line[2048];

        if(line_length >= sizeof(line)){
            line_length = sizeof(line) - 1;
        }

        memcpy(line , line_start , line_length);

        line[line_length] = '\0';

        if(y + LINE_HEIGHT >= panel.y && y <= panel.y + panel.height){
            DrawText(line , (int)(panel.x + 10 - *X_scroll) , (int)y , FONT_SIZE , RAYWHITE);
        }

        y += LINE_HEIGHT;

        if(line_end == NULL){
            break;
        }

        line_start = line_end + 1;
    }

    EndScissorMode();
}

int main(){
    COMPILER compiler = {0};

    float TAC_xscroll = 0;
    float TAC_yscroll = 0;

    float ASM_xscroll = 0;
    float ASM_yscroll = 0;

    float VM_xscroll = 0;
    float VM_yscroll = 0;

    float symbol_xscroll = 0;
    float symbol_yscroll = 0;

    compiler.token_table = malloc(sizeof(TokenEntry));
    compiler.token_table->token_count = 0;

    InitWindow(screen_width , screen_height , "COMPILER IDE");

    SetTargetFPS(70);

    int header_height = 100;

    int status_height = 80;

    int left_width = screen_width*60/100;
    int right_width = screen_width - left_width;

    int panel_height = screen_height - header_height - status_height;

    int button_width = 70;
    int button_height = 30;

    Rectangle run_button = {0 , header_height-20 , button_width , button_height};
    Rectangle open_bt = {button_width , header_height-20 , button_width , button_height};
    Rectangle exit_bt = {4*button_width , header_height-20 , button_width , button_height};
    Rectangle save_bt = {3*button_width , header_height-20 , button_width , button_height};
    Rectangle AST_bt = {2*button_width , header_height-20 , button_width , button_height};
    Rectangle TAC_button = {5*button_width , header_height - 20 , button_width , button_height};

    view_mode current_view = VIEW_VM;

    while(!WindowShouldClose()){
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawRectangle(0 , 0 , screen_width , header_height , BLUE);

        DrawRectangleLines(0 , 0 , screen_width , header_height , black);

        if(GuiButton(open_bt , "OPEN")){
            printf("OPEN\n");
        }

        if(GuiButton(run_button, "RUN")){
            printf("PROGRAM RUN.\n");
            compile_file("test1.c" , &compiler);
            current_view = VIEW_SYMBOL;
        }

        if(GuiButton(AST_bt , "AST")){
            current_view = VIEW_AST;
            //printf("AST.\n");
        }

        if(GuiButton(TAC_button , "VIEW TAC")){
            current_view = VIEW_TAC;
        }

        if(GuiButton(save_bt , "SAVE")){
            printf("SAVE.\n");
        }

        if(GuiButton(exit_bt , "EXIT")){
            printf("EXIT.\n");
        }

        DrawText("COMPILER IDE" , 550 , 30 , 60 , text_color);

        DrawRectangle(0 , header_height+1 , left_width , panel_height , GRAY);

        DrawRectangleLines(0 , header_height+1 , left_width , panel_height , green);

        DrawRectangle(left_width+1 , header_height+1 , right_width , panel_height , black);

        DrawRectangleLines(left_width+1 , header_height+1 , right_width , panel_height , DARKGRAY);

        DrawRectangle(0 , 820 , screen_width , status_height , GRAY);

        DrawRectangleLines(0 , 820 , screen_width , status_height , BROWN);

        switch(current_view){
            case VIEW_AST:
                DrawText("AST_OUTPUT" , left_width+1 , header_height+1 , 30 , green);
                break;

            case VIEW_OUTPUT:
                DrawText("OUTPUT" , left_width+1 , header_height+1 , 30 , green);
                break;

            case VIEW_SYMBOL:
                DrawText("SYMBOL TABLE" , left_width+10 , header_height+10 , 25 , green);

                Rectangle Symbol_Panel = {left_width + 1 , header_height + 45 , right_width , panel_height - 45};

                DrawScrollableTextPanel(compiler.result.SYM_BUFFER , Symbol_Panel , &symbol_xscroll , &symbol_yscroll);
                break;
            
            case VIEW_TAC:
                DrawText("TAC OUTPUT" , left_width+10 , header_height+10 , 25 , green);

                Rectangle TAC_panel = {left_width + 1 , header_height + 45 , right_width , panel_height - 45};

                DrawScrollableTextPanel(compiler.result.TAC_buffer , TAC_panel , &TAC_xscroll , &TAC_yscroll);

                break;

            case VIEW_VM:
                DrawText(compiler.result.VM_buffer , left_width+1 , header_height+1 , 20 , WHITE);
                break;
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
