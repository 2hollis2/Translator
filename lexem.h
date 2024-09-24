#ifndef LEXEM_H
#define LEXEM_H

#include <iostream>
#include <string>
#include <vector>

using Lexem = std::pair<std::string, std::string>;

const Lexem LEX_EMPTY = { "", "" };
const Lexem LEX_ERROR = { "error", "" };
const Lexem LEX_EOF = { "EOF", "" };

#endif //LEXEM_H