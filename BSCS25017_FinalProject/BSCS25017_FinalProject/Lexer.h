#pragma once
#include "MyString.h"
#include "MyVector.h"
#include <iostream>
using namespace std;



enum TokenType {
    // Literals
    TOKEN_NUMBER, //1,2,3,4,....
    TOKEN_STRING, //"1133443aaasdf..."
    TOKEN_TRUE,   // true
    TOKEN_FALSE,  // false

    // Identifiers & Keywords
    TOKEN_IDENTIFIER,
    TOKEN_LET,  // let
    TOKEN_IF,   // IF
    TOKEN_ELSE, // ELSE
    TOKEN_WHILE,//WHILE
    TOKEN_PRINT,//PRINT

    // Symbols
    TOKEN_LPAREN,       // (
    TOKEN_RPAREN,       // )
    TOKEN_LBRACE,       // {
    TOKEN_RBRACE,       // }
    TOKEN_SEMICOLON,    // ;
    TOKEN_COMMA,        // ,

    // Operators
    TOKEN_ASSIGN,       // =
    TOKEN_PLUS,         // +
    TOKEN_MINUS,        // -
    TOKEN_STAR,         // *
    TOKEN_SLASH,        // /
    TOKEN_EQ,           // ==
    TOKEN_NEQ,          // !=
    TOKEN_LT,           // <
    TOKEN_GT,           // >
    TOKEN_LTE,          // <=
    TOKEN_GTE,          // >=

    // Special
    TOKEN_EOF, 
    TOKEN_ERROR
};

struct Token {
    TokenType type;
    MyString  lexeme;
    int       line;

    Token() : type(TOKEN_EOF), lexeme(""), line(0) {}
    Token(TokenType t, MyString l, int ln) : type(t), lexeme(l), line(ln) {}

    Token& operator=(const Token& other) {
        if (this == &other) return *this;
        type = other.type;
        lexeme = other.lexeme;  
        line = other.line;
        return *this;
    }
    Token(const Token& other) : type(other.type), lexeme(other.lexeme), line(other.line) {}
};

class Lexer {
    MyString  source;
    myVector<Token>  tokens;
    int current;
    int line;

    bool is_at_end() const {
        return current >= source.Length();
    }

    char peek() const {
        if (is_at_end()) return '\0';
        return source[current];
    }

    char peek_next() const {
        if (current + 1 >= source.Length()) return '\0';
        return source[current + 1];
    }

    char advance() {
        char ch = source[current];
        current++;
        return ch;
    }

    bool match(char expected) {
        if (is_at_end())             return false;
        if (source[current] != expected) return false;
        current++;
        return true;
    }

    void add_token(TokenType type, MyString lexeme) {
        tokens.push_back(Token(type, lexeme, line));
    }

    bool is_digit(char c) const {
        return c >= '0' && c <= '9';
    }

    bool is_alpha(char c) const {
        return (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            c == '_';
    }

    bool is_alnum(char c) const {
        return is_alpha(c) || is_digit(c);
    }

    void scan_string() {
        MyString value;
        while (!is_at_end() && peek() != '"') {
            if (peek() == '\n') line++;
            value += MyString(advance());
        }

        if (is_at_end()) {
            throw ZScriptError{ "Unterminated string literal", line, SYNTAX };
        }

        advance(); 
        add_token(TOKEN_STRING, value);
    }

    void scan_number() {
        MyString num;
        num += MyString(source[current - 1]); 

        while (!is_at_end() && is_digit(peek())) {
            num += MyString(advance());
        }

        if (!is_at_end() && peek() == '.' && is_digit(peek_next())) {
            num += MyString(advance()); 
            while (!is_at_end() && is_digit(peek())) {
                num += MyString(advance());
            }
        }

        add_token(TOKEN_NUMBER, num);
    }

    void scan_identifier() {
        MyString id;
        id += MyString(source[current - 1]); 

        while (!is_at_end() && is_alnum(peek())) {
            id += MyString(advance());
        }

        
        if (id == "let")   add_token(TOKEN_LET, id);
        else if (id == "if")    add_token(TOKEN_IF, id);
        else if (id == "else")  add_token(TOKEN_ELSE, id);
        else if (id == "while") add_token(TOKEN_WHILE, id);
        else if (id == "print") add_token(TOKEN_PRINT, id);
        else if (id == "true")  add_token(TOKEN_TRUE, id);
        else if (id == "false") add_token(TOKEN_FALSE, id);
        else                    add_token(TOKEN_IDENTIFIER, id);
    }

    

    void scan_next() {
        char c = advance();

        switch (c) {
        case '(': add_token(TOKEN_LPAREN, MyString("(")); break;
        case ')': add_token(TOKEN_RPAREN, MyString(")")); break;
        case '{': add_token(TOKEN_LBRACE, MyString("{")); break;
        case '}': add_token(TOKEN_RBRACE, MyString("}")); break;
        case ';': add_token(TOKEN_SEMICOLON, MyString(";")); break;
        case ',': add_token(TOKEN_COMMA, MyString(",")); break;
        case '+': add_token(TOKEN_PLUS, MyString("+")); break;
        case '-': add_token(TOKEN_MINUS, MyString("-")); break;
        case '*': add_token(TOKEN_STAR, MyString("*")); break;

        case '/':
            if (match('/')) {
                // single-line comment — skip everything until newline
                // including any non-ASCII / Unicode bytes
                while (!is_at_end() && peek() != '\n' && peek() != '\r') {
                    advance();
                }
            }
            else {
                add_token(TOKEN_SLASH, MyString("/"));
            }
            break;

        case '=':
            if (match('=')) add_token(TOKEN_EQ, MyString("=="));
            else            add_token(TOKEN_ASSIGN, MyString("="));
            break;

        case '!':
            if (match('=')) add_token(TOKEN_NEQ, MyString("!="));
            else throw ZScriptError{ "Unexpected character '!'", line, SYNTAX };
            break;

        case '<':
            if (match('=')) add_token(TOKEN_LTE, MyString("<="));
            else            add_token(TOKEN_LT, MyString("<"));
            break;

        case '>':
            if (match('=')) add_token(TOKEN_GTE, MyString(">="));
            else            add_token(TOKEN_GT, MyString(">"));
            break;

        case '"': scan_string(); break;

        case '\n': line++; break;
        case '\r': break;
        case ' ':  break;
        case '\t': break;

        default:
            if (is_digit(c)) {
                scan_number();
            }
            else if (is_alpha(c)) {
                scan_identifier();
            }
            else if ((unsigned char)c > 127) {

            }
            else {
                throw ZScriptError{
                    MyString("Unexpected character"),
                    line, SYNTAX
                };
            }
            break;
        }
    }

public:
    Lexer(const MyString& src) : source(src), current(0), line(1) {}
    
    void tokenize() {
        while (!is_at_end()) {
            scan_next();
        }
        tokens.push_back(Token(TOKEN_EOF, MyString(""), line));
    }

    const myVector<Token>& get_tokens() const {
        return tokens;
    }


    Token get(int i) const {
        return tokens[i];
    }

    int token_count() const {
        return tokens.size();
    }
};