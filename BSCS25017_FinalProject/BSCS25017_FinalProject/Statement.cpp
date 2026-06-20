#include "Statement.h"

void (*PrintStmt::print_handler)(const MyString&) = nullptr;