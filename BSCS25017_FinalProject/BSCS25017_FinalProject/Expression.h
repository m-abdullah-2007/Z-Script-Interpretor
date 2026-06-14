#pragma once
#include"Value.h"
class Environment;

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
				"Undefined variable '" + name + "'",
				line,
				RUNTIME
			};
		}

		return v;
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
            return new StringValue(l->to_string() + r->to_string());
        }

        if (op == "==" || op == "!=") {
            bool same = (l->type_name() == r->type_name()) &&(l->to_string() == r->to_string());
            if (op == "==") {
                return new BoolValue(same);
            }
            else {
                return new BoolValue(!same);
            }
        }

        if (l->type_name() != "number" || r->type_name() != "number") {
            throw ZScriptError{
                "Type error: cannot apply '" + op + "' to " +
                    l->type_name() + " and " + r->type_name(),
                line, RUNTIME
            };
        }

        NumberValue* ln = static_cast<NumberValue*>(l);
        NumberValue* rn = static_cast<NumberValue*>(r);


        double a = ln->getNum();
        double b = rn->getNum();

        if (op == "+")  return new NumberValue(a + b);
        if (op == "-")  return new NumberValue(a - b);
        if (op == "*")  return new NumberValue(a * b);

        if (op == "/") {
            if (b == 0.0) {
                throw ZScriptError{ "Division by zero", line, RUNTIME };
            }
            return new NumberValue(a / b);
        }

        if (op == "<")  return new BoolValue(a < b);
        if (op == ">")  return new BoolValue(a > b);
        if (op == "<=") return new BoolValue(a <= b);
        if (op == ">=") return new BoolValue(a >= b);

        // Should not be reachable if the parser only ever produces
        // known operators -- kept as a safety net.
        throw ZScriptError{ "Unknown operator '" + op + "'", line, RUNTIME };
	}
};
