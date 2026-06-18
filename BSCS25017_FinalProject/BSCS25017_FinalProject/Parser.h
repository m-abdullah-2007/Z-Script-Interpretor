#pragma once

#pragma once
#include "Lexer.h"
#include "Expression.h"
#include "Statement.h"

class Parser {
    const myVector<Token>& tokens;
    int current;


    bool is_at_end() const {
        return tokens[current].type == TOKEN_EOF;
    }

    Token peek() const {
        return tokens[current];
    }

    Token previous() const {
        return tokens[current - 1];
    }

    Token advance() {
        if (!is_at_end()) {
            current++;
        }
        return previous();
    }

    bool check(TokenType _type) const {
        if (is_at_end()) {
            return false;
        }
        return peek().type == _type;
    }

    bool match(TokenType type) {
        if (check(type)) {
            advance(); return true;
        }
        return false;
    }

    
    Token expect(TokenType type, const MyString& msg) {
        if (check(type)) {
            return advance();
        }
        throw ZScriptError{ msg, peek().line, SYNTAX };
    }

    // ── expression parsing (recursive descent) ────────────────────────────────
    //
    //  Grammar (lowest to highest precedence):
    //
    //    expression  → equality
    //    equality    → comparison ( ( "==" | "!=" ) comparison )*
    //    comparison  → term      ( ( "<" | ">" | "<=" | ">=" ) term )*
    //    term        → factor    ( ( "+" | "-" ) factor )*
    //    factor      → unary     ( ( "*" | "/" ) unary )*
    //    unary       → primary
    //    primary     → NUMBER | STRING | "true" | "false"
    //                | IDENTIFIER | "(" expression ")"

    Expression* parse_primary() {
        if (match(TOKEN_TRUE))  return new BooleanLiteral(true);
        if (match(TOKEN_FALSE)) return new BooleanLiteral(false);

        if (match(TOKEN_NUMBER)) {
            MyString lex = previous().lexeme;
            double value = 0.0;
            double frac = 0.0;
            bool   in_frac = false;
            double frac_div = 10.0;

            for (int i = 0; i < lex.Length(); i++) {
                char c = lex[i];
                if (c == '.') {
                    in_frac = true;
                }
                else if (!in_frac) {
                    value = value * 10.0 + (c - '0');
                }
                else {
                    frac += (c - '0') / frac_div;
                    frac_div *= 10.0;
                }
            }
            return new NumberLiteral(value + frac);
        }

        if (match(TOKEN_STRING)) {
            return new StringLiteral(previous().lexeme);
        }

        if (match(TOKEN_IDENTIFIER)) {
            return new VariableExpr(previous().lexeme, previous().line);
        }

        if (match(TOKEN_LPAREN)) {
            Expression* expr = parse_expression();
            expect(TOKEN_RPAREN, MyString("Expected ')' after expression"));
            return expr;
        }

        throw ZScriptError{
            MyString("Unexpected token '") + peek().lexeme + MyString("'"),
            peek().line, SYNTAX
        };
    }

    //Expression* parse_unary() {
    //    // Extend here for unary '-' or '!' if needed in the future.
    //    return parse_primary();
    //}

    Expression* parse_factor() {
        Expression* left = parse_primary();

        while (check(TOKEN_STAR) || check(TOKEN_SLASH)) {
            Token op = advance();
            Expression* right = parse_primary();
            left = new BinaryExpr(left, op.lexeme, right, op.line);
        }
        return left;
    }

    Expression* parse_term() {
        Expression* left = parse_factor();

        while (check(TOKEN_PLUS) || check(TOKEN_MINUS)) {
            Token op = advance();
            Expression* right = parse_factor();
            left = new BinaryExpr(left, op.lexeme, right, op.line);
        }
        return left;
    }

    Expression* parse_comparison() {
        Expression* left = parse_term();

        while (check(TOKEN_LT) || check(TOKEN_GT) ||
            check(TOKEN_LTE) || check(TOKEN_GTE)) {
            Token op = advance();
            Expression* right = parse_term();
            left = new BinaryExpr(left, op.lexeme, right, op.line);
        }
        return left;
    }

    Expression* parse_equality() {
        Expression* left = parse_comparison();

        while (check(TOKEN_EQ) || check(TOKEN_NEQ)) {
            Token op = advance();
            Expression* right = parse_comparison();
            left = new BinaryExpr(left, op.lexeme, right, op.line);
        }
        return left;
    }

    Expression* parse_expression() {
        return parse_equality();
    }

    
    Statement* parse_let() {
        // let <identifier> = <expression> ;
        Token name_tok = expect(TOKEN_IDENTIFIER,MyString("Expected variable name after 'let'"));
        expect(TOKEN_ASSIGN,MyString("Expected '=' after variable name in 'let'"));
        Expression* init = parse_expression();
        expect(TOKEN_SEMICOLON,MyString("Expected ';' after 'let' initialiser"));

        return new LetStmt(name_tok.lexeme, init);
    }

    Statement* parse_print() {
        // print ( <expression> ) ;
        expect(TOKEN_LPAREN,
            MyString("Expected '(' after 'print'"));
        Expression* expr = parse_expression();
        expect(TOKEN_RPAREN,
            MyString("Expected ')' after print expression"));
        expect(TOKEN_SEMICOLON,
            MyString("Expected ';' after 'print' statement"));

        return new PrintStmt(expr);
    }

    Statement* parse_block() {
        Statement* head = nullptr;
        Statement* tail = nullptr;

        while (!check(TOKEN_RBRACE) && !is_at_end()) {
            Statement* stmt = parse_statement();
            if (head == nullptr) {
                head = stmt;
                tail = stmt;
            }
            else {
                tail->setNext(stmt);  
                tail = stmt;
            }
        }

        expect(TOKEN_RBRACE, MyString("Expected '}' to close block"));
        return new BlockStmt(head);
    }

    Statement* parse_if() {
        // if ( <condition> ) <block> [ else <block> ]
        expect(TOKEN_LPAREN,
            MyString("Expected '(' after 'if'"));
        Expression* condition = parse_expression();
        expect(TOKEN_RPAREN,
            MyString("Expected ')' after if-condition"));
        expect(TOKEN_LBRACE,
            MyString("Expected '{' to open if-body"));

        Statement* then_branch = parse_block();
        Statement* else_branch = nullptr;

        if (match(TOKEN_ELSE)) {
            expect(TOKEN_LBRACE,
                MyString("Expected '{' to open else-body"));
            else_branch = parse_block();
        }

        return new IfStmt(condition, then_branch, else_branch);
    }

    Statement* parse_while() {
        // while ( <condition> ) <block>
        expect(TOKEN_LPAREN,
            MyString("Expected '(' after 'while'"));
        Expression* condition = parse_expression();
        expect(TOKEN_RPAREN,
            MyString("Expected ')' after while-condition"));
        expect(TOKEN_LBRACE,
            MyString("Expected '{' to open while-body"));

        Statement* body = parse_block();
        return new WhileStmt(condition, body);
    }

    Statement* parse_assign(const MyString& name, int line_num) {
        // <identifier> = <expression> ;   (already consumed name & '=')
        Expression* val = parse_expression();
        expect(TOKEN_SEMICOLON,
            MyString("Expected ';' after assignment"));
        return new AssignStmt(name, val, line_num);
    }

    Statement* parse_statement() {
        if (match(TOKEN_LET))   return parse_let();
        if (match(TOKEN_PRINT)) return parse_print();
        if (match(TOKEN_IF))    return parse_if();
        if (match(TOKEN_WHILE)) return parse_while();

        // Assignment:  identifier = expr ;
        if (match(TOKEN_IDENTIFIER)) {
            MyString name = previous().lexeme;
            int      ln = previous().line;
            expect(TOKEN_ASSIGN,MyString("Expected '=' after variable name '") + name + MyString("'"));
            return parse_assign(name, ln);
        }

        throw ZScriptError{
            MyString("Unexpected token '") + peek().lexeme + MyString("'"),
            peek().line, SYNTAX
        };
    }

public:
    Statement* statements[256];
    int        stmt_count;

    Parser(const Lexer& lexer): tokens(lexer.get_tokens()), current(0), stmt_count(0) {
        for (int i = 0; i < 256; i++) statements[i] = nullptr;
    }

    void parse() {
        while (!is_at_end()) {
            statements[stmt_count++] = parse_statement();
            if (stmt_count >= 256) {
                throw ZScriptError{
                    MyString("Program too large (> 256 top-level statements)"),peek().line, SYNTAX
                };
            }
        }
    }
};
