#include <iostream>
#include <fstream>
#include<string>
using namespace std;

#include "MyString.h"
#include "MyVector.h"
#include "ZSCRIPT_ERROR.h"
#include "Value.h"
#include "Expression.h"
#include "Environment.h"
#include "Statement.h"
#include "Lexer.h"
#include "Parser.h"
#include "ErrorHandler.h"

MyString read_file(const char* path) {
    ifstream file(path);
    if (!file.is_open()) {
        cout << "Error: could not open file '" << path << "'" << endl;
        return MyString("");
    }

    file.seekg(0, ios::end);
    int length = file.tellg();
    file.seekg(0, ios::beg);

    char* buffer = new char[length + 1];
    file.read(buffer, length);
    buffer[length] = '\0';
    file.close();

    MyString result(buffer);
    delete[] buffer;
    return result;
}

void run(const MyString& source) {
    ErrorHandler& handler = ErrorHandler::getInstance();
    handler.clear();
    
    try {
        Lexer lexer(source);
        lexer.tokenize();

        Parser parser(lexer);
        parser.parse();

        Environment global_env;
        for (int i = 0; i < parser.stmt_count; i++) {
            parser.statements[i]->execute(global_env);
        }

    }

    catch (ZScriptError& e) {
        handler.report(e);
    }

    if (handler.has_errors()) {
        cout << "\n--- " << handler.error_count() << " error(s) reported ---" << endl;
    }
}
int main() {
    const char* filename = "test.txt";

    cout << "========================================" << endl;
    cout << "  ZScript Interpreter" << endl;
    cout << "  Running: " << filename << endl;
    cout << "========================================" << endl;

    MyString source = read_file(filename);
    if (source.Length() == 0) return 1;

    run(source);

    cout << "========================================" << endl;
    cout << "  Done." << endl;
    cout << "========================================" << endl;

    return 0;
}