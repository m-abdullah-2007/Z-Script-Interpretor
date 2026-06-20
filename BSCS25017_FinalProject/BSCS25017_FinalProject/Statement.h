#pragma once
#include "expression.h"
using namespace std;
#include "Environment.h"


class Statement {
    Statement* next;
public:
    Statement() : next(nullptr) {}
    virtual ~Statement() {}
    virtual void execute(Environment& env) = 0;
    void setNext(Statement* n) { next = n; }
    virtual Statement* getNext() {
        return next;
    }
};



class PrintStmt : public Statement {
    Expression* expr;
public:
    static void (*print_handler)(const MyString&);

    static void set_handler(void (*fn)(const MyString&)) {
        print_handler = fn;
    }

    PrintStmt(Expression* e) : expr(e) {}

    void execute(Environment& env) override {
        Value* result = expr->evaluate(env);
        if (print_handler) {
            print_handler(result->to_string());
        }
        else {
            cout << result->to_string() << endl;
        }
        result->release();
    }

};

//void (*PrintStmt::print_handler)(const MyString&) = nullptr;

class LetStmt : public Statement {
public:
    MyString name;
    Expression* init;
    LetStmt(MyString n, Expression* i) : name(n), init(i) {}
    void execute(Environment& env) override {
        Value* value = init->evaluate(env);
        env.define(name, value);
    }
};

class AssignStmt : public Statement {
public:
    MyString name;
    Expression* value;
    int line;

    AssignStmt(MyString n, Expression* v, int l): name(n), value(v), line(l) {}
    void execute(Environment& env) override {
        Value* new_value = value->evaluate(env);
        env.assign(name, new_value);
    }
};


class IfStmt : public Statement {
public:
    Expression* condition;
    Statement* then_branch;
    Statement* else_branch;

    IfStmt(Expression* c, Statement* t, Statement* e = nullptr): condition(c), then_branch(t), else_branch(e) {}
    void execute(Environment& env) override {
        Value* result = condition->evaluate(env);
        if (result->is_truthy()) {
            then_branch->execute(env);
        }
        else if (else_branch != nullptr) {
            else_branch->execute(env);
        }
        result->release();
    }
};



class WhileStmt : public Statement {
public:
    Expression* condition;
    Statement* body;

    WhileStmt(Expression* c, Statement* b) : condition(c), body(b) {}
    void execute(Environment& env) override {
        while (true) {
            Value* result = condition->evaluate(env);
            bool cont = result->is_truthy();
            result->release();

            if (!cont) {
                break;
            }

            body->execute(env);
        }
    }
};


class BlockStmt : public Statement {
public:
    Statement* first;

    BlockStmt(Statement* f) : first(f) {}
    void execute(Environment& env) override {
        Environment child_env = env.create_child();

        Statement* current = first;
        while (current != nullptr) {
            current->execute(child_env);
            current = current->getNext();
        }
    }
};
