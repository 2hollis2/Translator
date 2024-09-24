#include "Lexer.h"
#include <unordered_set>
#include <iostream>

using namespace std;

bool is_letter(const string& word) { //Вспомогательная функция, которая проверяет, начинается ли строка с буквы или символа подчеркивания(используется для проверки идентификаторов)
    if (isalpha(word[0]) or word[0] == '_') return true;
    return false;
}

unordered_set <string> keywords = { //Лексер будет проверять, не является ли строка ключевым словом, чтобы присвоить ей соответствующую категорию
        "int",
        "char",
        "if",
        "else",
        "while",
        "for",
        "out",
        "in",
        "switch",
        "case",
        "return",
        "default"
};

Lexer::Lexer(std::istream& stream) : stream{ stream } {

    cache = stream.get();
}

std::string Lexer::str(const char& cache) { //Функция конвертирует одиночный символ cache в строку длиной 1 символ
    string new_cache(1, cache);
    return new_cache;
}

std::pair<std::string, std::string> Lexer::getNextLexem() {
    pair<std::string, std::string> lexem;
    while (lexem.first.empty()) {
        E result = { "", 0, false, false, "" };
        for (auto& now : graph[state]) {
            if (now.filter == "letter") {
                if (isalpha(cache) or cache == '_') {
                    result = now;
                    break;
                }
            }
            if (now.filter == "EOF") {
                if (cache == -1) {
                    result = now;
                    break;
                }
            }
            if (now.filter == "digit") {
                if (isdigit(cache)) {
                    result = now;
                    break;
                }
            }
            else if (now.filter[0] == cache) {
                result = now;

                break;
            }
            else if (now.filter.empty() and result.filter.empty()) {
                result = now;
            }
        }
        if (result.append) {
            value.push_back(cache);
        }
        if (result.read) {
            cache = stream.get();
        }
        if (result.clear) {
            value.clear();
        }

        if (result.lexem != "") {
            if (keywords.contains(value) and state == 21) {
                lexem.first = "kw" + value;
                lexem.second = "";
            }
            else if (state == 21) {
                lexem.first = "id";
                lexem.second = value;
            }
            else {
                lexem.first = result.lexem;
                lexem.second = value;
            }
            value.clear();
        }
        state = result.newState;
    }
    return lexem;
}


