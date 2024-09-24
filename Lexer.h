#ifndef LEXER_H
#define LEXER_H

#include "graph.h"
#include <cctype>
#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>
#include "lexem.h"


class Lexer {
private:
    std::istream& stream;
    char cache;
    std::string value;
    int state = 0;
    std::unordered_map<int, std::vector<E>> graph = getGraph();

    std::string str(const char&);
public:
    Lexer(std::istream& stream);
    std::pair<std::string, std::string> getNextLexem();
};

#endif //LEXER_H