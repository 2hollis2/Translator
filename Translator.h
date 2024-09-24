#include "Lexer.h"
#include <stack>
#include <iostream>
#include <algorithm>

using namespace std;

class LL {
private:

    // Expression Grammer

    // E -> E7
    pair<bool, string> E(const string& scope);

    // E7 -> E6 E7'
    pair<bool, string> E7(const string& scope);

    // E7' -> || E6 E7'  | epsilon
    pair<bool, string> E7list(const string& scope, const string& p);

    // E6 -> E5 E6'
    pair<bool, string> E6(const string& scope);

    // E6' -> && E5 E6'  | epsilon
    pair<bool, string> E6list(const string& scope, const string& p);

    // E5 -> E4 E5'
    pair<bool, string> E5(const string& scope);

    // E5' -> == E4  | != E4  | > E4  | >= E4  | < E4  | <= E4  | epsilon
    pair<bool, string> E5list(const string& scope, const string& p);
    
    // E4 -> E3 E4'
    pair<bool, string> E4(const string& scope);

    // E4' -> + E3 E4'  | - E3 E4'  | epsilon
    pair<bool, string> E4list(const string& scope, const string& p);

    // E3 -> E2 E3'
    pair<bool, string> E3(const string& scope);

    // E3' -> * E2 E3'  | epsilon
    pair<bool, string> E3list(const string& scope, const string& p);

    // E2 -> ! E1  | E1
    pair<bool, string> E2(const string& scope);

    // E1 -> ++ id  | ( E )  | num  | id E1'
    pair<bool, string> E1(const string& scope);

    // E1' -> ++  | ( ArgList )  | epsilon
    pair<bool, string> E1List(const string& scope, const string& p);

    // ArgList -> E ArgList'  | epsilon
    pair<bool, string> Arglist(const string& scope);

    // ArgList' -> , E ArgList'  | epsilon
    pair<bool, string> ArgListList(const string& scope);
    //
    //Entry point
    bool StmtList(const string& scope); //обработки списка операторов в текущей области видимости

    bool Stmt(const string& scope); //Метод для обработки одного оператора в текущей области видимости
 
    pair<bool, string> Type(const string& scope); //Обработка типов данных

    bool DeclareStmt(const string& scope); //Обработка операторов объявления переменных в заданной области видимости

    bool DeclareStmtList(const string& scope, const string& p, const string& q); //Обработка списка объявлений переменных в текущей области видимости

    bool DeclareVarList(const string& scope, const string& p); //Обработка списка переменных в объявлении в текущей области видимости

    bool InitVar(const string& scope, const string& p, const string& q); //Инициализация переменной

    pair<bool, string> ParamList(const string& scope); //Обработка списка параметров функции, возвращает пару значений: успешность и строку с результатом обработки параметров

    pair<bool, string> ParamListList(const string& scope); //Обработка списка параметров, если он состоит из нескольких параметров
    //
    // Операции присваивания и вызова функции
    bool AssignOrCallOp(const string& scope); //Метод для определения, является ли выражение операцией присваивания или вызовом функции в контексте текущей области видимости

    bool AssignOrCall(const string& scope); //Обработка конкретной операции присваивания или вызова функции

    bool AssignOrCallList(const string& scope, const string& p); //Обработка нескольких операций присваивания или вызовов функций, если они идут последовательно
    //
    // while
    bool WhileOp(const string& scope);
    //
    // "ForOp"
    bool ForOp(const string& scope); //Обработка цикла for

    bool ForInit(const string& scope); //Обработка инициализации цикла for

    pair<bool, string> ForExp(const string& scope); //Обработка выражения цикла for, возвращает результат в виде пары значений

    bool ForLoop(const string& scope); //Обработка блока, отвечающего за тело цикла for
    //
    //"IfOp"
    bool IfOp(const string& scope); //бработка условного оператора if

    bool ElsePart(const string& scope); //Обработка блока else, если он присутствует после оператора if
    //
    // block "SwitchOp"
    bool SwitchOp(const string& scope); //Обработка оператора switch

    bool Cases(const string& scope, const string& p, const string& end); //Обработка блоков case внутри оператора switch

    bool CasesList(const string& scope, const string& p, const string& end, const string& def); //Обработка списка блоков case и, возможно, блока default

    pair<bool, string> ACase(const string& scope, const string& p, const string& end); //Обработка одного блока case в операторе switch
    //
    //"IN" and "OUT"
    bool IOp(const string& scope); //Обработка оператора IN, отвечающего за ввод

    bool OOp(const string& scope); //Обработка оператора OUT, отвечающего за вывод

    bool OOpList(const string& scope); //Обработка списка операторов OUT
    
    //
    //funcs
    void setLexem(); //Устанавливает текущую лексему для обработки
    void nextState(const int& state); //Переход к следующему состоянию парсера, в зависимости от текущего
    void backStateIt(); //Возвращает состояние парсера на предыдущий шаг
    void addString(const string& str); //Добавляет строку к выходному потоку, используется для построения синтаксического дерева

    Lexer lexer; //Объект лексера, который используется для разбора лексем исходного кода
    Lexem tempLexem; //Временная лексема, которая хранится для текущего шага обработки

    vector <Lexem> list; //Список лексем, полученных от лексера
    vector<Lexem>::iterator iter; //Итератор для перемещения по списку лексем

    //печать дерева
    int treeHeight = 0; //Высота синтаксического дерева, используется для его визуализации
    vector<int> states; //Список состояний парсера на различных уровнях разбора
    vector<int>::iterator statesIt; //Итератор для перемещения по состояниям парсера

    ofstream outStr; //Поток вывода для синтаксического дерева
    string outPath; //Путь к файлу, куда выводится синтаксическое дерево

    vector<string> output; //Вектор для хранения строк, которые будут выводиться при построении синтаксического дерева

    bool printTree(bool fl); //Печатает синтаксическое дерево, если анализ прошел успешно
    //
    //semantic block
    string newLabel(); //Генерирует новую метку для безусловных переходов
    string alloc(const string& scope); //Выделяет новую временную переменную
    string addVar(const string& name, const string& scope, const string type, const string& init = "0"); //Добавляет новую переменную в таблицу символов
    string addFunc(const string& name, const string& type); //Добавляет новую функцию в таблицу символов с именем и типом функции
    string checkVar(const string& scope, const string& name); //Проверяет, существует ли переменная с данным именем в текущей области видимости
    string checkFunc(const string& name, const string& len); //Проверяет, существует ли функция с данным именем и правильность количества переданных параметров

    struct object {
        string code; //никальный код объекта в таблице
        string name; //Имя объекта
        string kind; //Тип объекта
        string type = "kwint"; //ип данных объекта по умолчанию
        string len = "-1"; //Длина объекта
        string value = "00"; //Начальное значение объекта
        string scope = "-1"; //Область видимости, в которой определен объект
        string offset = "-1"; //Смещение в памяти для объекта
    };

    vector<object> table; //Таблица символов, содержащая все переменные и функции в программе. Используется для хранения информации о каждой переменной и функции
    int labelCounter = 0; //Счетчик меток, который увеличивается при генерации новых меток
    int codeCounter = 0; //Счетчик кода, который может быть использован для генерации уникальных кодов для объектов
    int tempVarCounter = 0; //Счетчик временных переменных, которые создаются при вычислениях

    bool printAtoms(); //Функция для вывода атомов
    //
    //
    struct atom {
        string scope = "-1"; //Область видимости
        string operation = ""; //Операция, которая выполняется
        string first = ""; 
        string second = ""; 
        string third = "";
    };

    void addAtom(atom data); //Добавляет атомарную инструкцию в список для последующей генерации промежуточного кода
    vector<atom> atomList; //Список всех атомарных инструкций, которые были созданы во время разбора и анализа программы
    string outPathAtom; //Путь к файлу, в который выводятся атомы
    //
    //
    vector<string> asmList; //Список инструкций ассемблера. Используется для хранения сгенерированного ассемблерного кода до его вывода
    string outPathAsm; //Путь к файлу, в который выводится ассемблерный код
    stack<string> programStack; //Стек программы, используется для управления вызовами функций, временными переменными и операндами

    bool asmBlock(); //Метод для генерации и обработки блока ассемблера на основе атомарных инструкций
    void printAsm(); //Печатает сгенерированный ассемблерный код в файл

    void loadOp(const string& operand, const string& scope, int newOffset); //Загружает операнд в нужное место с учетом области видимости и смещения
    void saveOp(const string& operand); //Сохраняет результат выполнения операции в память или регистр, используя указанный операнд

    //ассемблерные команды
    void ADD(const atom& atom);

    void SUB(const atom& atom);

    void MUL(const atom& atom);

    void MOV(const atom& atom);

    void LBL(const atom& atom);

    void JMP(const atom& atom);

    void AND(const atom& atom);

    void OR(const atom& atom);

    void EQ(const atom& atom);

    void NE(const atom& atom);

    void LT(const atom& atom);

    void LE(const atom& atom);

    void GT(const atom& atom);

    void NOT(const atom& atom);

    void IN(const atom& atom);

    void OUT(const atom& atom);

    void PARAM(const atom& atom);

    void CALL(const atom& atom);

    void RET(const atom& atom);

    fstream out;
    ofstream outAtoms;

public:
    void solve();

    explicit LL(Lexer& lexer, const string&, const string&, const string&);
};