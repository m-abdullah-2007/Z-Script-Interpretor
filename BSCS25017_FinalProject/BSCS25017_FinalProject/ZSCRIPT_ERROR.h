#pragma once
#include "MyString.h"

enum ErrorType {
    SYNTAX,     
    RUNTIME     
};


struct ZScriptError {
    MyString  message;
    int       line;
    ErrorType type;
};