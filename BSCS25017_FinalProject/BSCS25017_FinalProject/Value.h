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
	NumberValue(double p=0.0):num(p) {}

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
	double getNum() {
		return num;
	}
};

class StringValue :public Value {
	MyString str;
public:
	StringValue(MyString p=""):str(p){}
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

class BoolValue:public Value{
	bool boolean;
public:
	BoolValue(bool p=false):boolean(p){}
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