#pragma once
#include"MyString.h"

class Value {
	
public:
	virtual MyString to_string() = 0;
	virtual bool is_truthy() = 0;
	virtual MyString type_name() = 0;
};

class NumberValue :public Value {
	double num;
public:
	MyString to_string() {
		MyString temp;
		temp.itos(num);
		return temp;
	}
	bool is_truthy() {
		if (num == 0.0) {
			return false;
		}
		return true;
	}
	MyString type_name() {
		return "number";
	}
};