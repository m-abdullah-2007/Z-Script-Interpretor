#pragma once
#include "ZSCRIPT_ERROR.h"
#include "MyVector.h"
#include <iostream>
using namespace std;



class ErrorHandler {
    myVector<ZScriptError> errors;    

    ErrorHandler() {}
    ErrorHandler(const ErrorHandler&) = delete;
    ErrorHandler& operator=(const ErrorHandler&) = delete;

    MyString type_label(ErrorType t) const {
        if (t == SYNTAX)  return MyString("[Syntax Error]");
        else if (t == RUNTIME) return MyString("[Runtime Error]");
        return MyString("[Error]");
    }

    void print_error(const ZScriptError& e) const {
        cout << type_label(e.type)
            << " at line " << e.line
            << " : " << e.message
            << endl;
    }

public:   
 
    static ErrorHandler& getInstance() {
        static ErrorHandler instance;
        return instance;
    }

    void report_silent(const ZScriptError& e) {
        errors.push_back(e);
    } 
    void report(const ZScriptError& e) {
        this->report_silent(e);
        print_error(e);
    }
    void print_all() const {
        if (errors.size() == 0) {
            cout << "No errors." << endl;
            return;
        }
        cout << "\n===== ZScript Error Report =====" << endl;
        for (int i = 0; i < errors.size(); i++) {
            cout << (i + 1) << ". ";
            print_error(errors[i]);
        }
        cout << "================================" << endl;
    }
    bool has_errors() const {
        return errors.size() > 0;
    }

    int error_count() const {
        return errors.size();
    }
    void clear() {
        errors.clear();
    }
};