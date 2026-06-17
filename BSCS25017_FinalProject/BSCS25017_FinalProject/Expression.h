#pragma once
#include"Value.h"
#include"Environment.h"


class Expression {

public:
	virtual Value* evaluate(Environment& env) = 0;
};

class NumberLiteral :public Expression {
	double value;
public:
	NumberLiteral(double p = 0.0) :value(p) {}
	Value* evaluate(Environment& env) {
		return new NumberValue(value);
	}
};

class StringLiteral :public Expression {
	MyString value;
public:
	StringLiteral(MyString p = "") :value(p) {}
	Value* evaluate(Environment& env) {
		return new StringValue(value);
	}
};


class BooleanLiteral :public Expression {
	bool value;
public:
	BooleanLiteral(bool p = false) :value(p) {}
	Value* evaluate(Environment& env) {
		return new BoolValue(value);
	}
};

class VariableExpr : public Expression {
public:
	MyString name;
	int line;

	VariableExpr(MyString n, int l) : name(n), line(l) {}

    Value* evaluate(Environment& env) {
        Value* v = env.lookup(name);

        if (v == nullptr) {
            throw ZScriptError{
                MyString("Undefined variable"),
                line, RUNTIME
            };
        }

        if (v->type_name() == MyString("number")) {
            return new NumberValue(static_cast<NumberValue*>(v)->getNum());
        }
        if (v->type_name() == MyString("string")) {
            return new StringValue(static_cast<StringValue*>(v)->to_string());
        }
        if (v->type_name() == MyString("bool")) {
            return new BoolValue(static_cast<BoolValue*>(v)->is_truthy());
        }
        return nullptr;
    }
};

class BinaryExpr : public Expression {
public:
	Expression* left;
	Expression* right;
	MyString op;
	int line;

	BinaryExpr(Expression* l, MyString o, Expression* r, int ln): left(l), right(r), op(o), line(ln) {}

	Value* evaluate(Environment& env) override {
        Value* l = left->evaluate(env);
        Value* r = right->evaluate(env);

        if (op == "+" && (l->type_name() == "string" || r->type_name() == "string")) {
            Value* result = new StringValue(l->to_string() + r->to_string());
            l->release();
            r->release();
            return result;
        }

        if (op == "==" || op == "!=") {
            bool same = (l->type_name() == r->type_name()) &&(l->to_string() == r->to_string());
            Value* result;
            if (op == "==") {
                result = new BoolValue(same);
            }
            else {
                result = new BoolValue(!same);
            }
            l->release();
            r->release();
            return result;
        }

        if (l->type_name() != "number" || r->type_name() != "number") {
            throw ZScriptError{
                MyString("Type error: cannot apply '").concat_no_space(op).concat_no_space(MyString("' to ")).concat_no_space(l->type_name()).concat_no_space(MyString(" and ")).concat_no_space(r->type_name()),line, RUNTIME
            };
        }

        NumberValue* ln = static_cast<NumberValue*>(l);
        NumberValue* rn = static_cast<NumberValue*>(r);


        double a = ln->getNum();
        double b = rn->getNum();

        Value* result = nullptr;
        if (op == "+")  result = new NumberValue(a + b);
        else if (op == "-")  result = new NumberValue(a - b);
        else if (op == "*")  result = new NumberValue(a * b);
        else if (op == "/") {
            if (b == 0.0) {
                throw ZScriptError{ "Division by zero", line, RUNTIME };
            }
            result = new NumberValue(a / b);
        }
        else if (op == "<")  result = new BoolValue(a < b);
        else if (op == ">")  result = new BoolValue(a > b);
        else if (op == "<=") result = new BoolValue(a <= b);
        else if (op == ">=") result = new BoolValue(a >= b);
        else {
            throw ZScriptError{ MyString("Unknown operator"), line, RUNTIME };
        }

        l->release();
        r->release();
        return result;
	}
};
