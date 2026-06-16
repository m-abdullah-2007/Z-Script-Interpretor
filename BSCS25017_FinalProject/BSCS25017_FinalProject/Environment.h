#pragma once
#include"MyString.h"
#include"Value.h"
#include"MyVector.h"

struct Binding {
	MyString* name;
	Value* value;

	Binding() : name(nullptr), value(nullptr) {}
	Binding(MyString n, Value* v) : value(v) {
		this->name = new MyString(n);
	}
};


class Environment {
	myVector<Binding> bindings;
	Environment* parent;

public:

	Environment(Environment* parent = nullptr) : parent(parent) {}
	~Environment(){}

	void define(const MyString& _name, Value* val) {
		for (int i = 0; i < bindings.size(); i++) {
			if (*bindings[i].name == _name) {
				bindings[i].value = val;
				return;
			}
		}
		bindings.push_back(Binding(_name, val));
	}

	bool assign(const MyString& name, Value* value) {
		for (int i = 0; i < bindings.size(); i++) {
			if (*bindings[i].name == name) {
				bindings[i].value = value;
				return true;
			}
		}
		if (parent != nullptr) {
			return parent->assign(name, value);
		}
		return false;
	}

	Value* lookup(const MyString& name) {
		for (int i = 0; i < bindings.size(); i++) {
			if (*bindings[i].name == name) {
				return bindings[i].value;
			}
		}
		if (parent != nullptr) {
			return parent->lookup(name);
		}

		return nullptr; 
	}
	Environment create_child() {
		return Environment(this);
	}



};