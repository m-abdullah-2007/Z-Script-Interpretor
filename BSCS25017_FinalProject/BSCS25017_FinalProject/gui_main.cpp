// gui_main.cpp

// ZScript IDE — Full GUI
//
// Screen flow:
//   THEME_SELECT  -> user picks Light or Dark
//   MODE_SELECT   -> user picks "Run a file" or "Write code"
//   FILENAME_ENTRY -> (only if "Run a file" was picked) type a filename,
//                     loads it into the editor if found in the same folder
//   IDE           -> editor + output panel + Run/Open buttons (matches the
//                     reference layout: toolbar -> editor -> output)
//
// Error popups (SYNTAX vs RUNTIME) use distinct colors per type, themed
// for whichever Light/Dark mode is used.
//


#include "raylib.h"
#include <cstring>

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


enum AppScreen {
    SCREEN_THEME_SELECT,
    SCREEN_MODE_SELECT,
    SCREEN_FILENAME_ENTRY,
    SCREEN_IDE
};

AppScreen current_screen = SCREEN_THEME_SELECT;

enum Theme { THEME_LIGHT, THEME_DARK };
Theme current_theme = THEME_LIGHT;

// changable and clippable
int win_w = 1000;
int win_h = 700;

//min cannot b changed
const int MIN_WIN_W = 640;
const int MIN_WIN_H = 480;



const int TOOLBAR_H = 48;
int output_h = 160;          // output panel height; floor enforced below
const int LINE_H = 22;
const int FONT_SIZE = 18;
const int GUTTER_W = 48;

Rectangle toolbar_bounds;
Rectangle editor_bounds;
Rectangle output_bounds;

void recalc_layout() {
    win_w = GetScreenWidth();
    win_h = GetScreenHeight();
    if (win_w < MIN_WIN_W) win_w = MIN_WIN_W;
    if (win_h < MIN_WIN_H) win_h = MIN_WIN_H;


    // Output panel takes roughly 22% of height
    output_h = (int)(win_h * 0.32f);

    // min / max or ouput panel
    if (output_h < 160) output_h = 160;
    if (output_h > 380) output_h = 380;

    int editor_h = win_h - TOOLBAR_H - output_h;
    // min for editor area
    if (editor_h < 120) editor_h = 120;

    toolbar_bounds = { 0, 0, (float)win_w, (float)TOOLBAR_H };
    editor_bounds = { 0, (float)TOOLBAR_H, (float)win_w, (float)editor_h };
    output_bounds = { 0, (float)(TOOLBAR_H + editor_h), (float)win_w, (float)(win_h - TOOLBAR_H - editor_h) };
}

//color palette
// One struct holding every color the IDE needs
// the user picks Light or Dark
// light-blue-tinted look for Light, and a dark-navy equivalent for Dark.

struct Palette {
    Color app_bg;
    Color panel_bg;
    Color toolbar_bg;
    Color border;
    Color gutter_bg;
    Color text_primary;
    Color text_secondary;
    Color text_muted;
    Color editor_bg;
    Color output_bg;
    Color output_header_bg;
    Color btn_default_bg;
    Color btn_default_text;
    Color btn_run_bg;
    Color btn_run_text;
    Color cursor;
};

Palette light_palette = {
    GetColor(0xEEF1FBFF), // app_bg
    GetColor(0xFFFFFFFF), // panel_bg
    GetColor(0xFAFBFEFF), // toolbar_bg
    GetColor(0xD7DCEAFF), // border
    GetColor(0xF3F5FBFF), // gutter_bg
    GetColor(0x1F2430FF), // text_primary
    GetColor(0x5B6172FF), // text_secondary
    GetColor(0x9AA0B0FF), // text_muted
    GetColor(0xFFFFFFFF), // editor_bg
    GetColor(0xEEF1FBFF), // output_bg
    GetColor(0xE3E8F7FF), // output_header_bg
    GetColor(0xEDF0F8FF), // btn_default_bg
    GetColor(0x1F2430FF), // btn_default_text
    GetColor(0x1F5FD6FF), // btn_run_bg
    GetColor(0xFFFFFFFF), // btn_run_text
    GetColor(0x1F2430FF), // cursor
};

Palette dark_palette = {
    GetColor(0x14151FFF), // app_bg
    GetColor(0x1B1D29FF), // panel_bg
    GetColor(0x1B1D29FF), // toolbar_bg
    GetColor(0x2E3142FF), // border
    GetColor(0x181A24FF), // gutter_bg
    GetColor(0xE6E8EFFF), // text_primary
    GetColor(0x9CA1B5FF), // text_secondary
    GetColor(0x6B6F82FF), // text_muted
    GetColor(0x1B1D29FF), // editor_bg
    GetColor(0x14151FFF), // output_bg
    GetColor(0x20222EFF), // output_header_bg
    GetColor(0x262838FF), // btn_default_bg
    GetColor(0xE6E8EFFF), // btn_default_text
    GetColor(0x3B7EFFFF), // btn_run_bg
    GetColor(0xFFFFFFFF), // btn_run_text
    GetColor(0xE6E8EFFF), // cursor
};

Palette& pal() {
    if (current_theme == THEME_LIGHT)
    {
        return light_palette;
    }
        return dark_palette;
}

//Error popup colors
// Syntax and Runtime errors ave differen colours
// light/dark theme choice.

struct ErrorPalette {
    Color badge_bg;
    Color badge_text;
    Color popup_border;
};

ErrorPalette syntax_error_colors_for_theme() {
    if (current_theme == THEME_LIGHT) {
        return { GetColor(0xFBE7E7FF), GetColor(0x9A2E2EFF), GetColor(0xE0A0A0FF) };
    }
    return { GetColor(0x3A1F22FF), GetColor(0xF2A0A0FF), GetColor(0x6B3438FF) };
}

ErrorPalette runtime_error_colors_for_theme() {
    if (current_theme == THEME_LIGHT) {
        return { GetColor(0xFCEEDBFF), GetColor(0x9A6113FF), GetColor(0xE8C28EFF) };
    }
    return { GetColor(0x3A2E1AFF), GetColor(0xF2C27AFF), GetColor(0x6B5430FF) };
}

const int MAX_BUFFER = 8192;
char editor_buffer[MAX_BUFFER] = "let x = 10;\nlet y = 20;\nprint(x + y);\n";
int  editor_len = (int)strlen(editor_buffer);
int  editor_cursor = editor_len;
int  editor_scroll = 0;

float cursor_blink_timer = 0.0f;
bool  cursor_visible = true;

//Output panel state

const int MAX_OUTPUT_LINES = 250;
MyString output_lines[MAX_OUTPUT_LINES];
int      output_line_count = 0;
int      output_scroll = 0;

//Filename entry state

char filename_buffer[256] = "";
int  filename_len = 0;
int  filename_cursor = 0;
MyString filename_status_message("");
bool filename_status_is_error = false;

//Error popup state
//At most one syntax error matters (lexer/parser stops at the first one)
//Runtime errors can be multiple, read from ErrorHandler::getInstance() directly when drawing.

bool popup_visible = false;
bool popup_is_syntax = false;   //syntax (single error)/runtime (multiple errors)
float popup_alpha = 0.0f;
int   popup_scroll = 0;
MyString last_syntax_message("");
int      last_syntax_line = 0;

// Function Declarations

void draw_theme_select();
void update_theme_select();

void draw_mode_select();
void update_mode_select();

void draw_filename_entry();
void update_filename_entry();
bool try_load_file(const MyString& path);

void update_toolbar();
void draw_toolbar();
void on_run_pressed();
void on_open_pressed();

void update_editor();
void draw_editor();
int  editor_count_lines();
void editor_get_line_bounds(int line_index, int& start, int& end);
void editor_insert_char(char c);
void editor_delete_char();

void output_append(const MyString& text);
void output_clear();
void update_output_panel();
void draw_output_panel();

void update_error_popup();
void draw_error_popup();
void show_syntax_popup(const MyString& message, int line);
void show_runtime_popup();


int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE); // Window can be draged by edges
    InitWindow(win_w, win_h, "ZScript IDE"); // make Window
    SetWindowMinSize(MIN_WIN_W, MIN_WIN_H);  // Minimum Size  window set Below wich it automaticall resizs
    SetTargetFPS(60);

    while (!WindowShouldClose()) {

        recalc_layout(); // After very update kayot is recalculated

        // 1) UPDATE PHASE
        switch (current_screen) {
        case SCREEN_THEME_SELECT:  update_theme_select();  break;
        case SCREEN_MODE_SELECT:   update_mode_select();   break;
        case SCREEN_FILENAME_ENTRY: update_filename_entry(); break;
        case SCREEN_IDE:
            update_toolbar();
            update_editor();
            update_output_panel();
            update_error_popup();
            break;
        }

        // ---- DRAW PHASE ----
        BeginDrawing();
        ClearBackground(pal().app_bg);

        switch (current_screen) {
        case SCREEN_THEME_SELECT:  draw_theme_select();  break;
        case SCREEN_MODE_SELECT:   draw_mode_select();   break;
        case SCREEN_FILENAME_ENTRY: draw_filename_entry(); break;
        case SCREEN_IDE:
            draw_toolbar();
            draw_editor();
            draw_output_panel();
            draw_error_popup(); // drawn at last so cod stops after it
            break;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

// Screen 1 — Theme select

Rectangle theme_light_btn;
Rectangle theme_dark_btn;
bool theme_light_hovered = false;
bool theme_dark_hovered = false;

void layout_theme_select_buttons() {
    //calaculation for button placement
    float btn_w = 220, btn_h = 64;
    float gap = 24;
    float total_w = btn_w * 2 + gap;
    float start_x = (win_w - total_w) / 2.0f;
    float y = win_h / 2.0f - btn_h / 2.0f;

    // buttons created
    theme_light_btn = { start_x, y, btn_w, btn_h };
    theme_dark_btn = { start_x + btn_w + gap, y, btn_w, btn_h };
}

void update_theme_select() {
    layout_theme_select_buttons();// theme select called first

    Vector2 mouse = GetMousePosition();
    theme_light_hovered = CheckCollisionPointRec(mouse, theme_light_btn);
    theme_dark_hovered = CheckCollisionPointRec(mouse, theme_dark_btn);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (theme_light_hovered) {
            current_theme = THEME_LIGHT;
            current_screen = SCREEN_MODE_SELECT;
        }
        else if (theme_dark_hovered) {
            current_theme = THEME_DARK;
            current_screen = SCREEN_MODE_SELECT;
        }
    }
}

void draw_theme_select() {
    const char* title = "Choose a theme!";
    int title_w = MeasureText(title, 28);
    DrawText(title, (win_w - title_w) / 2, (int)(win_h / 2.0f - 130), 28, pal().text_primary);

    //always drawn with light colors regardless of
    Color light_bg;

    if (theme_light_hovered)
    {
        light_bg = GetColor(0xE3E8F7FF);
    }
    else
    {
        light_bg = GetColor(0xFFFFFFFF);
    }
    DrawRectangleRounded(theme_light_btn, 0.18f, 8, light_bg);
    DrawRectangleLines((int)theme_light_btn.x, (int)theme_light_btn.y, (int)theme_light_btn.width, (int)theme_light_btn.height, GetColor(0xD7DCEAFF));
    const char* lLabel = "Light";
    int lw = MeasureText(lLabel, 20);
    DrawText(lLabel, (int)(theme_light_btn.x + theme_light_btn.width / 2 - lw / 2),(int)(theme_light_btn.y + theme_light_btn.height / 2 - 10), 20, GetColor(0x1F2430FF));

    //always drawn with dark colors as its own preview.
    Color dark_bg;

    if (theme_dark_hovered)
    {
        dark_bg = GetColor(0x262838FF);
    }
    else
    {
        dark_bg = GetColor(0x1B1D29FF);
    }
    DrawRectangleRounded(theme_dark_btn, 0.18f, 8, dark_bg);
    DrawRectangleLines((int)theme_dark_btn.x, (int)theme_dark_btn.y, (int)theme_dark_btn.width, (int)theme_dark_btn.height, GetColor(0x2E3142FF));
    const char* dark_label = "Dark";
    int dw = MeasureText(dark_label, 20);
    DrawText(dark_label, (int)(theme_dark_btn.x + theme_dark_btn.width / 2 - dw / 2),(int)(theme_dark_btn.y + theme_dark_btn.height / 2 - 10), 20, GetColor(0xE6E8EFFF));
}

// Screen 2 — Mode select

Rectangle mode_run_btn;
Rectangle mode_write_btn;
bool mode_run_hovered = false;
bool mode_write_hovered = false;

void layout_mode_select_buttons() {
    float btn_w = 240, btn_h = 64;
    float gap = 24;
    float total_w = btn_w * 2 + gap;
    float start_x = (win_w - total_w) / 2.0f;
    float y = (win_h / 2.0f) - (btn_h / 2.0f);

    mode_run_btn = { start_x, y, btn_w, btn_h };
    mode_write_btn = { start_x + btn_w + gap, y, btn_w, btn_h };
}

void update_mode_select() {
    layout_mode_select_buttons();
    Vector2 mouse = GetMousePosition();
    mode_run_hovered = CheckCollisionPointRec(mouse, mode_run_btn);
    mode_write_hovered = CheckCollisionPointRec(mouse, mode_write_btn);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (mode_run_hovered) {
            filename_buffer[0] = '\0';
            filename_len = 0;
            filename_cursor = 0;
            filename_status_message = MyString("");
            current_screen = SCREEN_FILENAME_ENTRY;
        }
        else if (mode_write_hovered) {
            current_screen = SCREEN_IDE;
        }
    }
}

void draw_mode_select() {
    Palette& p = pal();
    const char* title = "What would you like to do?";
    int title_w = MeasureText(title, 26);
    DrawText(title, (win_w - title_w) / 2, (int)(win_h / 2.0f - 130), 26, p.text_primary);

    Color run_bg;
    Color run_text;

    if (mode_run_hovered)
    {
        run_bg = p.btn_run_bg;
        run_text = p.btn_run_text;
    }
    else
    {
        run_bg = p.btn_default_bg;
        run_text = p.text_primary;
    }
    DrawRectangleRounded(mode_run_btn, 0.18f, 8, run_bg);
    const char* run_label = "Run a file";
    int rw = MeasureText(run_label, 18);
    DrawText(run_label, (int)(mode_run_btn.x + mode_run_btn.width / 2 - rw / 2),(int)(mode_run_btn.y + mode_run_btn.height / 2 - 9), 18, run_text);

    Color write_bg;
    Color write_text;

    if (mode_write_hovered)
    {
        write_bg = p.btn_run_bg;
        write_text = p.btn_run_text;
    }
    else
    {
        write_bg = p.btn_default_bg;
        write_text = p.text_primary;
    }
    DrawRectangleRounded(mode_write_btn, 0.18f, 8, write_bg);
    const char* write_label = "Write code";
    int ww = MeasureText(write_label, 18);
    DrawText(write_label, (int)(mode_write_btn.x + mode_write_btn.width / 2 - ww / 2),(int)(mode_write_btn.y + mode_write_btn.height / 2 - 9), 18, write_text);
}


// Screen 3 — Filename entry

#include <fstream>

bool try_load_file(const MyString& path) {
    ifstream file(path.c_str());
    if (!file.is_open()) {
        return false;
    }

    file.seekg(0, ios::end);
    long length = (long)file.tellg();
    file.seekg(0, ios::beg);

    if (length <= 0 || length >= MAX_BUFFER - 1) {
        return false; 
    }

    file.read(editor_buffer, length);
    editor_buffer[length] = '\0';
    editor_len = (int)length;
    editor_cursor = editor_len;
    editor_scroll = 0;
    return true;
}

Rectangle filename_input_box;
Rectangle filename_load_btn;
bool filename_load_hovered = false;

void layout_filename_entry() {
    float box_w = 420, box_h = 40;
    filename_input_box = { (win_w - box_w) / 2.0f, win_h / 2.0f - 60, box_w, box_h };
    filename_load_btn = { (win_w - 120) / 2.0f, win_h / 2.0f + 0, 120, 40 };
}

void update_filename_entry() {
    layout_filename_entry();

    int ch = GetCharPressed();
    while (ch > 0) {
        if (ch >= 32 && ch < 127 && filename_len < 254) {
            filename_buffer[filename_cursor] = (char)ch;
            filename_cursor++;
            filename_len++;
            filename_buffer[filename_len] = '\0';
        }
        ch = GetCharPressed();
    }

    if ((IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) && filename_cursor > 0) {
        filename_cursor--;
        filename_len--;
        filename_buffer[filename_len] = '\0';
    }

    Vector2 mouse = GetMousePosition();
    filename_load_hovered = CheckCollisionPointRec(mouse, filename_load_btn);

    bool attempt_load = (IsKeyPressed(KEY_ENTER) || (filename_load_hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)));

    if (attempt_load) {
        if (filename_len == 0) {
            filename_status_message = MyString("Type a filename first");
            filename_status_is_error = true;
        }
        else if (try_load_file(MyString(filename_buffer))) {
            filename_status_message = MyString("Loaded successfully");
            filename_status_is_error = false;
            current_screen = SCREEN_IDE;
        }
        else {
            filename_status_message = MyString("File not found in this folder");
            filename_status_is_error = true;
        }
    }
}

void draw_filename_entry() {
    Palette& p = pal();

    const char* title = "Enter a filename to run";
    int title_w = MeasureText(title, 24);
    DrawText(title, (win_w - title_w) / 2, (int)(filename_input_box.y - 50), 24, p.text_primary);

    DrawRectangleRec(filename_input_box, p.panel_bg);
    DrawRectangleLines((int)filename_input_box.x, (int)filename_input_box.y,(int)filename_input_box.width, (int)filename_input_box.height, p.border);
    DrawText(filename_buffer, (int)filename_input_box.x + 12, (int)filename_input_box.y + 10, 18, p.text_primary);

    // Blinking cursor in input box
    cursor_blink_timer += GetFrameTime();
    if (cursor_blink_timer > 0.5f) {
        cursor_visible = !cursor_visible;
        cursor_blink_timer = 0.0f;
    }
    if (cursor_visible) {
        int text_w = MeasureText(filename_buffer, 18);
        DrawRectangle((int)filename_input_box.x + 12 + text_w + 2, (int)filename_input_box.y + 8, 2, 24, p.cursor);
    }

    Color load_bg;
    Color load_text;

    if (filename_load_hovered)
    {
        load_bg = p.btn_run_bg;
        load_text = p.btn_run_text;
    }
    else
    {
        load_bg = p.btn_default_bg;
        load_text = p.text_primary;
    }
    DrawRectangleRounded(filename_load_btn, 0.2f, 8, load_bg);
    const char* load_label = "Load";
    int lw = MeasureText(load_label, 18);
    DrawText(load_label, (int)(filename_load_btn.x + filename_load_btn.width / 2 - lw / 2),(int)(filename_load_btn.y + filename_load_btn.height / 2 - 9), 18, load_text);

    if (filename_status_message.Length() > 0) {
        Color status_color = filename_status_is_error ? GetColor(0xC0392BFF) : GetColor(0x27AE60FF);
        const char* msg = filename_status_message.c_str();
        int msg_w = MeasureText(msg, 16);
        DrawText(msg, (win_w - msg_w) / 2, (int)(filename_load_btn.y + 56), 16, status_color);
    }

    const char* hint = "Press Enter or click Load";
    int hint_w = MeasureText(hint, 14);
    DrawText(hint, (win_w - hint_w) / 2, (int)(filename_load_btn.y + 90), 14, p.text_muted);
}

// IDE SCREEN
// Toolbar 

Rectangle open_btn;
Rectangle run_btn;
bool open_btn_hovered = false;
bool run_btn_hovered = false;

void layout_toolbar_buttons() {
    open_btn = { 14, 9, 110, 30 };
    run_btn = { (float)(win_w - 100 - 14), 9, 100, 30 };
}

void on_open_pressed() {
    // Re-enter the
    filename_buffer[0] = '\0';
    filename_len = 0;
    filename_cursor = 0;
    filename_status_message = MyString("");
    current_screen = SCREEN_FILENAME_ENTRY;
}

void update_toolbar() {
    layout_toolbar_buttons();
    Vector2 mouse = GetMousePosition();

    open_btn_hovered = CheckCollisionPointRec(mouse, open_btn);
    run_btn_hovered = CheckCollisionPointRec(mouse, run_btn);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (open_btn_hovered) on_open_pressed();
        else if (run_btn_hovered) on_run_pressed();
    }
}

void draw_toolbar() {
    Palette& p = pal();
    DrawRectangleRec(toolbar_bounds, p.toolbar_bg);
    DrawLine(0, TOOLBAR_H - 1, win_w, TOOLBAR_H - 1, p.border);

    Color open_bg;
    if (open_btn_hovered)
    {
        open_bg = p.btn_default_bg;
    }
    else
    {
        open_bg = p.panel_bg;
    }
    DrawRectangleRounded(open_btn, 0.25f, 6, open_bg);
    DrawRectangleLines((int)open_btn.x, (int)open_btn.y, (int)open_btn.width, (int)open_btn.height, p.border);
    DrawText("Open .txt", (int)open_btn.x + 12, (int)open_btn.y + 7, 15, p.text_primary);

    Color run_bg = p.btn_run_bg;
    DrawRectangleRounded(run_btn, 0.25f, 6, run_bg);
    DrawText("Run", (int)run_btn.x + 32, (int)run_btn.y + 7, 16, p.btn_run_text);
}

// Run — builds Lexer/Parser/Environment 

void on_run_pressed() {
    output_clear();
    ErrorHandler& handler = ErrorHandler::getInstance();
    handler.clear();
    popup_visible = false;

    PrintStmt::set_handler([](const MyString& s) {
        output_append(s);
        });

    MyString source(editor_buffer);

    bool syntax_failed = false;

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
        handler.report_silent(e);
        if (e.type == SYNTAX) {
            syntax_failed = true;
            show_syntax_popup(e.message, e.line);
        }
    }
    if (!syntax_failed && handler.has_errors()) {
        show_runtime_popup();
    }
}


// Editor

int editor_count_lines() {
    if (editor_len == 0) return 1;
    int count = 1;
    for (int i = 0; i < editor_len; i++) {
        if (editor_buffer[i] == '\n') {
            count++;
        }
    }
    return count;
}

void editor_get_line_bounds(int line_index, int& start, int& end) {
    int current_line = 0;
    start = 0;
    for (int i = 0; i < editor_len; i++) {
        if (editor_buffer[i] == '\n') {
            if (current_line == line_index) {
                end = i;
                return;
            }
            current_line++;
            start = i + 1;
        }
    }
    end = editor_len;
}

void editor_insert_char(char c) {
    if (editor_len >= MAX_BUFFER - 1) return;

    memmove(&editor_buffer[editor_cursor + 1], &editor_buffer[editor_cursor], editor_len - editor_cursor);
    editor_buffer[editor_cursor] = c;
    editor_len++;
    editor_cursor++;
    editor_buffer[editor_len] = '\0';
}

void editor_delete_char() {
    if (editor_cursor <= 0) return;
    memmove(&editor_buffer[editor_cursor - 1], &editor_buffer[editor_cursor], editor_len - editor_cursor);
    editor_len--;
    editor_cursor--;
    editor_buffer[editor_len] = '\0';
}

void update_editor() {
    // Don't steal keystrokes from the editor while the error popup is open.
    if (popup_visible) {
        return;
    }
    int ch = GetCharPressed();
    while (ch > 0) {
        if (ch >= 32 && ch < 127) editor_insert_char((char)ch);
        ch = GetCharPressed();
    }
    if (IsKeyPressed(KEY_ENTER)) editor_insert_char('\n');
    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) editor_delete_char();
    if (IsKeyPressed(KEY_LEFT) && editor_cursor > 0) editor_cursor--;
    if (IsKeyPressed(KEY_RIGHT) && editor_cursor < editor_len) editor_cursor++;

    cursor_blink_timer += GetFrameTime();
    if (cursor_blink_timer > 0.5f) {
        cursor_visible = !cursor_visible;
        cursor_blink_timer = 0.0f;
    }

    Vector2 mouse = GetMousePosition();
    if (CheckCollisionPointRec(mouse, editor_bounds)) {
        float wheel = GetMouseWheelMove();
        if (wheel != 0) {
            editor_scroll -= (int)wheel;
            int total_lines = editor_count_lines();
            int visible_lines = (int)(editor_bounds.height / LINE_H);
            int max_scroll = total_lines - visible_lines;
            if (max_scroll < 0) max_scroll = 0;
            if (editor_scroll < 0) editor_scroll = 0;
            if (editor_scroll > max_scroll) editor_scroll = max_scroll;
        }
    }
}

void draw_editor() {
    Palette& p = pal();
    DrawRectangleRec(editor_bounds, p.editor_bg);

    Rectangle gutter = { editor_bounds.x, editor_bounds.y, (float)GUTTER_W, editor_bounds.height };
    DrawRectangleRec(gutter, p.gutter_bg);
    DrawLine((int)(editor_bounds.x + GUTTER_W), (int)editor_bounds.y,
        (int)(editor_bounds.x + GUTTER_W), (int)(editor_bounds.y + editor_bounds.height), p.border);

    int total_lines = editor_count_lines();
    int visible_lines = (int)(editor_bounds.height / LINE_H);

    for (int row = 0; row < visible_lines; row++) {
        int line_index = row + editor_scroll;
        if (line_index >= total_lines) break;

        int start, end;
        editor_get_line_bounds(line_index, start, end);

        int y = (int)editor_bounds.y + 6 + row * LINE_H;

        DrawText(TextFormat("%d", line_index + 1), (int)editor_bounds.x + 8, y, FONT_SIZE - 2, p.text_muted);

        char line_text[512];
        int len = end - start;
        if (len > 510) len = 510;
        memcpy(line_text, &editor_buffer[start], len);
        line_text[len] = '\0';

        DrawText(line_text, (int)editor_bounds.x + GUTTER_W + 10, y, FONT_SIZE, p.text_primary);
    }

    if (cursor_visible && !popup_visible) {
        int cur_line = 0, cur_col = 0;
        for (int i = 0; i < editor_cursor; i++) {
            if (editor_buffer[i] == '\n') { cur_line++; cur_col = 0; }
            else cur_col++;
        }
        int row_on_screen = cur_line - editor_scroll;
        if (row_on_screen >= 0 && row_on_screen < visible_lines) {
            int cx = (int)editor_bounds.x + GUTTER_W + 10 + cur_col * 10;
            int cy = (int)editor_bounds.y + 6 + row_on_screen * LINE_H;
            DrawRectangle(cx, cy, 2, LINE_H - 4, p.cursor);
        }
    }

    if (total_lines > visible_lines) {
        float track_h = editor_bounds.height;
        float thumb_h = track_h * ((float)visible_lines / (float)total_lines);
        float thumb_y = editor_bounds.y + track_h * ((float)editor_scroll / (float)total_lines);
        DrawRectangle((int)(editor_bounds.x + editor_bounds.width - 5), (int)thumb_y, 3, (int)thumb_h, p.text_muted);
    }
}


// Output panel with proper spacing for clean output
void output_append(const MyString& text) {
    const char* str = text.c_str();
    int start = 0;
    int len = (int)strlen(str);
    for (int i = 0; i <= len; i++) {
        if (str[i] == '\n' || str[i] == '\0') {
            if (output_line_count >= MAX_OUTPUT_LINES) return;
            char buf[512];
            int chunk = i - start;
            if (chunk > 510) chunk = 510;
            memcpy(buf, str + start, chunk);
            buf[chunk] = '\0';
            output_lines[output_line_count] = MyString(buf);
            output_line_count++;
            start = i + 1;
        }
    }
    int visible_lines = (int)((output_bounds.height - 24) / LINE_H);
    int max_scroll = output_line_count - visible_lines;
    if (max_scroll < 0) max_scroll = 0;
    output_scroll = max_scroll;
}

void output_clear() {
    output_line_count = 0;
    output_scroll = 0;
}

void update_output_panel() {
    Vector2 mouse = GetMousePosition();
    if (CheckCollisionPointRec(mouse, output_bounds)) {
        float wheel = GetMouseWheelMove();
        if (wheel != 0) {
            output_scroll -= (int)wheel;
            int visible_lines = (int)((output_bounds.height - 24) / LINE_H);
            int max_scroll = output_line_count - visible_lines;
            if (max_scroll < 0) max_scroll = 0;
            if (output_scroll < 0) output_scroll = 0;
            if (output_scroll > max_scroll) output_scroll = max_scroll;
        }
    }
}

void draw_output_panel() {
    Palette& p = pal();
    DrawRectangleRec(output_bounds, p.output_bg);

    Rectangle header = { output_bounds.x, output_bounds.y, output_bounds.width, 24 };
    DrawRectangleRec(header, p.output_header_bg);
    DrawText("OUTPUT", (int)output_bounds.x + 10, (int)output_bounds.y + 4, 14, p.text_secondary);

    int visible_lines = (int)((output_bounds.height - 24) / LINE_H);
    for (int row = 0; row < visible_lines; row++) {
        int line_index = row + output_scroll;
        if (line_index >= output_line_count) break;

        DrawText(output_lines[line_index].c_str(),
            (int)output_bounds.x + 10,
            (int)output_bounds.y + 28 + row * LINE_H,
            FONT_SIZE - 2, p.text_primary);
    }

    if (output_line_count > visible_lines) {
        float track_h = output_bounds.height - 24;
        float thumb_h = track_h * ((float)visible_lines / (float)output_line_count);
        float thumb_y = output_bounds.y + 24 + track_h * ((float)output_scroll / (float)output_line_count);
        DrawRectangle((int)(output_bounds.x + output_bounds.width - 5), (int)thumb_y, 3, (int)thumb_h, p.text_muted);
    }
}

// Error popup


// True only for the single frame the popup was just opened on
bool popup_just_opened = false;

void show_syntax_popup(const MyString& message, int line) {
    last_syntax_message = message;
    last_syntax_line = line;
    popup_is_syntax = true;
    popup_visible = true;
    popup_alpha = 0.0f;
    popup_scroll = 0;
    popup_just_opened = true;
}

void show_runtime_popup() {
    popup_is_syntax = false;
    popup_visible = true;
    popup_alpha = 0.0f;
    popup_scroll = 0;
    popup_just_opened = true;
}

// Shared rectangle for every function for simulatneous click detection
Rectangle get_popup_rect() {
    Rectangle rect;
    rect.x = (float)(win_w / 2 - 280);
    rect.y = (float)(win_h / 2 - 130);
    rect.width = 560;
    rect.height = 260;
    return rect;
}

void update_error_popup() {
    if (!popup_visible) {
        return;
    }

    // Fade in
    if (popup_alpha < 1.0f) {
        popup_alpha += GetFrameTime() * 4.0f;
        if (popup_alpha > 1.0f) {
            popup_alpha = 1.0f;
        }
    }

    Rectangle popup_rect = get_popup_rect();

    // Dismiss on Escape
    if (IsKeyPressed(KEY_ESCAPE)) {
        popup_visible = false;
        popup_alpha = 0.0f;
        popup_just_opened = false;
        return;
    }

    Vector2 mouse = GetMousePosition();

    // Dismiss on clicking outside the popup 
    if (!popup_just_opened) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (!CheckCollisionPointRec(mouse, popup_rect)) {
                popup_visible = false;
                popup_alpha = 0.0f;
                return;
            }
        }
    }

    // Scroll runtime errors 
    if (!popup_is_syntax) {
        if (CheckCollisionPointRec(mouse, popup_rect)) {
            float wheel = GetMouseWheelMove();
            if (wheel != 0) {
                popup_scroll -= (int)wheel;

                int max_scroll = ErrorHandler::getInstance().error_count() - 6;
                if (max_scroll < 0) {
                    max_scroll = 0;
                }
                if (popup_scroll < 0) {
                    popup_scroll = 0;
                }
                if (popup_scroll > max_scroll) {
                    popup_scroll = max_scroll;
                }
            }
        }
    }

    // Clear the guard at the very end of update_error_popup()
    popup_just_opened = false;
}

void draw_error_popup() {
    if (!popup_visible) {
        return;
    }

    Palette& p = pal();

    // overlay
    Color overlay;
    overlay.r = 0;
    overlay.g = 0;
    overlay.b = 0;
    overlay.a = (unsigned char)(int)(120 * popup_alpha);
    DrawRectangle(0, 0, win_w, win_h, overlay);

    Rectangle popup_rect = get_popup_rect();
    int px = (int)popup_rect.x;
    int py = (int)popup_rect.y;
    int pw = (int)popup_rect.width;
    int ph = (int)popup_rect.height;

    ErrorPalette ep;
    if (popup_is_syntax) {
        ep = syntax_error_colors_for_theme();
    }
    else {
        ep = runtime_error_colors_for_theme();
    }

    // Background
    DrawRectangleRounded(popup_rect, 0.08f, 8, p.panel_bg);

    // Colored border 
    DrawRectangleLines(px, py, pw, ph, ep.popup_border);

    // Badge header
    Rectangle badge;
    badge.x = (float)(px + 20);
    badge.y = (float)(py + 18);
    badge.width = (float)(pw - 40);
    badge.height = 32;
    DrawRectangleRounded(badge, 0.3f, 6, ep.badge_bg);

    const char* badge_label;
    if (popup_is_syntax) {
        badge_label = "Syntax Error";
    }
    else {
        badge_label = "Runtime Error(s)";
    }

    int bw = MeasureText(badge_label, 16);
    DrawText(badge_label,
        (int)(badge.x + badge.width / 2 - bw / 2),
        (int)(badge.y + 8),
        16, ep.badge_text);

    // Content
    if (popup_is_syntax) {
        // Single error -- line number + message
        char line_info[64];
        snprintf(line_info, sizeof(line_info), "Line %d", last_syntax_line);
        DrawText(line_info, px + 20, py + 70, 15, p.text_secondary);

        // Word-wrap the message manually at ~66 chars per line
        // Controls overflow manually 
        const MyString& msg = last_syntax_message;
        int msg_len = msg.Length();
        int wrap = 66;
        int y_offset = py + 96;

        int i = 0;
        while (i < msg_len) {
            int chunk = msg_len - i;
            if (chunk > wrap) {
                chunk = wrap;
            }

            MyString line_piece;
            for (int j = 0; j < chunk; j++) {
                line_piece += MyString(msg[i + j]);
            }

            DrawText(line_piece.c_str(), px + 20, y_offset, FONT_SIZE - 2, p.text_primary);
            y_offset += LINE_H;
            i += chunk;
        }
    }
    else {
        // Multiple runtime errors -- scrollable list
        ErrorHandler& handler = ErrorHandler::getInstance();
        int count = handler.error_count();
        int visible_rows = 6;
        int y_start = py + 68;

        for (int i = 0; i < visible_rows; i++) {
            int idx = i + popup_scroll;
            if (idx >= count) {
                break;
            }

            const ZScriptError& e = handler.get_error(idx);

            char row[256];
            snprintf(row, sizeof(row), "Line %d : %s", e.line, e.message.c_str());
            DrawText(row, px + 20, y_start + i * LINE_H, FONT_SIZE - 3, p.text_primary);
        }

        if (count > visible_rows) {
            DrawText(TextFormat("%d / %d errors", popup_scroll + 1, count),
                px + pw - 130, py + ph - 24, 13, p.text_muted);
        }
    }

    // Dismiss hint
    const char* hint = "Press Esc or click outside to dismiss";
    int hw = MeasureText(hint, 13);
    DrawText(hint, px + pw / 2 - hw / 2, py + ph - 22, 13, p.text_muted);
}