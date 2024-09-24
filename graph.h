#ifndef GRAPH_H
#define GRAPH_H

#include <unordered_map>
#include <vector>
#include <string>

struct E {
    std::string filter;
    int newState;
    bool append;
    bool read;
    std::string lexem;
    bool clear = false;
};

std::unordered_map<int, std::vector<E>> getGraph();

#endif //GRAPH_H