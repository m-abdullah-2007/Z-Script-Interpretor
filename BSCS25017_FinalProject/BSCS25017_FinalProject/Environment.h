#pragma once
// Value ownership: every Binding that stores a Value* calls retain() on it.
// Temporaries from evaluate() are released by their caller when no longer needed.
// When a Value's ref_count reaches 0, no Binding and no in-flight expression
// evaluation still holds it, so release() deletes it safely.
#include"MyString.h"
#include"Value.h"
#include"MyVector.h"

struct Binding {
	MyString* name;
	Value* value;

	Binding() : name(nullptr), value(nullptr) {}

	Binding(MyString n, Value* v) : name(new MyString(n)), value(v) {
		if (value != nullptr) {
			value->retain();
		}
	}

	~Binding() {
		if (value != nullptr) {
			value->release();
		}
		delete name;
	}

	Binding(const Binding& other) : name(nullptr), value(nullptr) {
		if (other.name != nullptr) {
			name = new MyString(*other.name);
		}
		value = other.value;
		if (value != nullptr) {
			value->retain();
		}
	}

	Binding& operator=(const Binding& other) {
		if (this == &other) {
			return *this;
		}
		if (value != nullptr) {
			value->release();
		}
		delete name;
		name = nullptr;
		value = nullptr;
		if (other.name != nullptr) {
			name = new MyString(*other.name);
		}
		value = other.value;
		if (value != nullptr) {
			value->retain();
		}
		return *this;
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
				bindings[i].value->release();
				bindings[i].value = val;
				val->retain();
				return;
			}
		}
		bindings.push_back(Binding(_name, val));
	}

	bool assign(const MyString& name, Value* value) {
		for (int i = 0; i < bindings.size(); i++) {
			if (*bindings[i].name == name) {
				bindings[i].value->release();
				bindings[i].value = value;
				value->retain();
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
