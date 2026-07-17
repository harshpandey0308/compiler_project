#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include"raygui.h"

const int screen_width = 1500;
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

int main(){
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

    view_mode current_view;

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

        DrawRectangle(0 , header_height+1 , left_width , panel_height , black);

        DrawRectangleLines(0 , header_height+1 , left_width , panel_height , green);

        DrawRectangle(left_width+1 , header_height+1 , right_width , panel_height , DARKGRAY);

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
                DrawText("SYMBOL TABLE" , left_width+1 , header_height+1 , 30 , green);
                break;
            
            case VIEW_TAC:
                DrawText("TAC OUTPUT" , left_width+1 , header_height+1 , 30 , green);
                break;

            case VIEW_VM:
                DrawText("VM output" , left_width+1 , header_height+1 , 30 , green);
                break;
        }

        EndDrawing();
    }

    CloseWindow();
}
