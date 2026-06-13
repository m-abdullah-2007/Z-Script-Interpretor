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

class StringValue :public Value {
	MyString str;
public:
	MyString to_string() {
		return str;
	}
	bool is_truthy() {
		if (str == "") {
			return false;
		}
		return true;
	}
	MyString type_name() {
		return "string";
	}
};

class StringValue :public Value {
	bool boolean;
public:
	MyString to_string() {
		if (boolean) {
			return "true";
		}
		return "false";
	}
	bool is_truthy() {
		return boolean;
	}
	MyString type_name() {
		return "bool";
	}
};