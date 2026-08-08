#include "raylib.h"
#include"../compiler.h"

#define RAYGUI_IMPLEMENTATION
#include"raygui.h"

#define FONT_SIZE 20
#define LINE_HEIGHT 25
#define SCROLL SPEED 40


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

void DrawScrollableTextPanel(const char *text , Rectangle panel , float *scroll){
    if(text == NULL || text[0] == '\0'){
        return;
    }

    float mouse_wheel = GetMouseWheelMove();

    if(CheckCollisionPointRec(GetMousePosition() , panel)){
        *scroll -= (mouse_wheel)*(SCROLL_SPEED);
    }

    if(*scroll < 0){
        *scroll = 0;
    }

    int line_count = 0;

    for(const char *ch = text ; *ch != '\0' ; ch++){
        if(*ch == '\n'){
            line_count++;
        }
    }

    if(text[0] == '\0'){
        line_count++;
    }

    float content_height = line_count*LINE_HEIGHT;

    float max_scroll = content_height - panel.height;

    if(max_scroll < 0){
        max_scroll = 0;
    }

    if(*scroll > max_scroll){
        *scroll = max_scroll;
    }

    BeginScissorMode((int)panel.x , (int)panel.y , (int)panel.width , (int)panel.height);

    float y = panel.y - *scroll;

    const char *line_start = text;

    while(*line_start != '\0'){
        const char *line_end = strchr(line_start , '\n');

        size_t line_length;

        if(line_end != NULL){
            line_length = line_end - line_start;
        }
        else{
            line_length = strlen(line_start);
        }

        char line[1024];

        if(line_length >= sizeof(line)){
            line_length = sizeof(line) - 1;
        }

        memcpy(line , line_start , line_length);
        line[line_length] = '\0';

        if(y + LINE_HEIGHT >= panel.y && y < panel.y + panel.height){
            DrawText(line , panel.x + 10 , y , FONT_SIZE , RAYWHITE);
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

        DrawRectangleLines(left_width+1 , header_height+1 , right_width , panel_height , GRAY);

        DrawRectangle(0 , 801 , screen_width , status_height , GRAY);

        DrawRectangleLines(0 , 801 , screen_width , status_height , BROWN);

        switch(current_view){
            case VIEW_AST:
                DrawText("AST_OUTPUT" , left_width+1 , header_height+1 , 30 , green);
                break;

            case VIEW_OUTPUT:
                DrawText("OUTPUT" , left_width+1 , header_height+1 , 30 , green);
                break;

            case VIEW_SYMBOL:
                DrawText(compiler.result.SYM_BUFFER , left_width+1 , header_height+1 , 15 , green);
                break;
            
            case VIEW_TAC:
                DrawText(compiler.result.TAC_buffer , left_width+1 , header_height+1 , 20 , green);
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
