#include "raylib.h"
#include "MyString.h"
#include "MyVector.h"
#include "ZSCRIPT_ERROR.h"
#include "Value.h"
#include "Expression.h"
#include "Environment.h"
#include "Statement.h"
#include "Lexer.h"
#include "Parser.h"
#include "ErrorHandler.h"
#include<string>



const int SCREEN_W = 1280;
const int SCREEN_H = 720;
const int TOOLBAR_H = 48;



Rectangle run_btn = { (float)(SCREEN_W - 110 - 12), 8, 110, 32 };


bool run_btn_hovered = false;


void update_toolbar();
void draw_toolbar();
void on_run_pressed();



int main() {

    InitWindow(SCREEN_W, SCREEN_H, "ZScript Interpretor v1.0");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {

        update_toolbar();

        
        BeginDrawing();
        ClearBackground(GetColor(0x1A1A2EFF));

        draw_toolbar();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

// update_toolbar() detects the click;
void update_toolbar() {
    Vector2 mouse = GetMousePosition();

    run_btn_hovered = CheckCollisionPointRec(mouse, run_btn);

    if (run_btn_hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        on_run_pressed();
    }
}
// draw_toolbar() renders the button.
void draw_toolbar() {
    // Toolbar background 
    DrawRectangle(0, 0, SCREEN_W, TOOLBAR_H, GetColor(0x16213EFF));

    // Thin line under the toolbar
    DrawLine(0, TOOLBAR_H - 1, SCREEN_W, TOOLBAR_H - 1, GetColor(0x2E86ABFF));

    // Run button — color swaps on hover
    Color run_color;
    if (run_btn_hovered)
    {
        run_color = GetColor(0x27AE60FF);
    }
    else
    {
        run_color = GetColor(0x1E8449FF);
    }
    DrawRectangleRounded(run_btn, 0.3f, 8, run_color);
    DrawText("Run", (int)run_btn.x + 30, (int)run_btn.y + 8, 20, WHITE);
}



void on_run_pressed() {
    cout << "[Run button clicked]" << endl;

    MyString source = MyString("let x = 10; print(x);");

    ErrorHandler& handler = ErrorHandler::getInstance();
    handler.clear();

    try {
        Lexer lexer(source);
        lexer.tokenize();

        Parser parser(lexer);
        parser.parse();

        Environment global_env;
        for (int i = 0; i < parser.stmt_count; i++) {
            parser.statements[i]->execute(global_env);
        }
    }
    catch (ZScriptError& e) {
        handler.report(e);
    }

    if (handler.has_errors()) {
        cout << "\n--- " << handler.error_count() << " error(s) reported ---" << endl;
        
    }
}