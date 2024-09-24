#include "graph.h"

using namespace std;

unordered_map<int, vector<E>> graph = {
        {0, // state whitespace
          {{" ", 0, false, true, ""},
             {"\n", 0, false, true, ""},
             {"\t", 0, false, true, ""},
             // state EOF
           {"", -1, false, false, "EOF"},
           // state 1
          {"(", 1, false, false, ""},
          {")", 1, false, false, ""},
          {"{", 1, false, false, ""},
          {"}", 1, false, false, ""},
          {"[", 1, false, false, ""},
          {"]", 1, false, false, ""},
          {";", 1, false, false, ""},
          {":", 1, false, false, ""},
          {",", 1, false, false, ""},
          {".", 1, false, false, ""},
          {">", 1, false, false, ""},
          {"*", 1, false, false, ""},
          // state 2
          {"<", 2, false, true, ""},
          // state 4
          {"!", 4, false, true, ""},
          // state 6
          {"=", 6, false, true, ""},
          // state 8
          {"+", 8, false, true, ""},
          // state 10
          {"|", 10, false, true, ""},
          // state 12
          {"&", 12, false, true, ""},
          // state 14
          {"\'", 14, false, true, ""},
          // state 18
          {"\"", 18, false, true, ""},
          // state 21 & letter
          {"letter", 21, false, false, ""},
          // state 22
          {"-", 22, true, true, ""},
          // state 23 & digit
          {"digit", 23, false, false, ""}
       }},

       {1, {
           {"(", 0, false, true, "lpar"},
           {")", 0, false, true, "rpar"},
           {"{", 0, false, true, "lbrace"},
           {"}", 0, false, true, "rbrace"},
           {"[", 0, false, true, "lbracket"},
           {"]", 0, false, true, "rbracket"},
           {";", 0, false, true, "semicolon"},
           {",", 0, false, true, "comma"},
           {":", 0, false, true, "colon"},
           {">", 0, false, true, "opgt"},
           {"*", 0, false, true, "opmul"}
       }},

       {2, {
               {"=", 3, false, false, ""},
               {"<", 24, false, false, ""},
               {"", 0, false, true, "oplt"}
       }},

       {3, {
               {"", 0, false, true, "ople"}
       }},

       {4, {
               {"=", 5, false, false, ""},
               {"", 0, false, true, "opnot"}
       }},

       {5, {
               {"", 0, false, true, "opne"}
       }},

       {6, {
               {"=", 7, false, false, ""},
               {"", 0, false, false, "opassign"}
       }},

       {7, {
               {"", 0, false, true, "opeq"}
       }},

       {8, {
               {"+", 9, false, false, ""},
               {"", 0, false, false, "opplus"}
       }},

       {9, {
               {"", 0, false, true, "opinc"}
       }},

       {10, {
               {"|", 11, false, false, ""},
               {"", -1, false, false, "error", true}
       }},

       {11, {
               {"", 0, false, true, "opor"}
       }},

       {12, {
               {"&", 13, false, false, ""},
               {"", -1, false, false, "error", true}
       }},

       {13, {
               {"", 0, false, true, "opand"}
       }},

       {14, {
               {"'", 15, false, true, ""},
               {"", 16, true, true, ""}
       }},

       {15, {
               {"", -1, false, false, "error", true}
       }},

       {16, {
               {"'", 17, false, false, ""},
               {"", -1, false, false, "error", true}
       }},

       {17, {
               {"", 0, false, true, "char"}
       }},

       {18, {
               {"\"", 20, false, false, ""},
               {"", 18, true, true, ""},
               {"EOF", 19, false, false, ""}
       }},

       {19, {
               {"", -1, false, false, "error", true}
       }},

       {20, {
               {"", 0, false, true, "str"}
       }},

       {21, {
               {"digit", 21, true, true, ""},
               {"letter", 21, true, true, ""},
               {"", 0, false, false, "keyword"},
       }},

       {22, {
               {"digit", 23, true, true, ""},
               {"", 0, false, false, "opminus", true}
       }},

       {23, {
               {"digit", 23, true, true, ""},
               {"", 0, false, false, "num"}
       }},

       {24, {
               {"", 0, false, true, "oplshift"}
       }},
};


unordered_map<int, vector<E>> getGraph() {
    return graph;
}