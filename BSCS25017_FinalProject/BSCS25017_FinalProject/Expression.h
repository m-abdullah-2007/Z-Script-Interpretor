#pragma once
#include"Value.h"
class Environment;

class Expression {

public:
	virtual Value* evaluate(Environment& temp) = 0;
};

class NumberLiteral :public Expression {

};