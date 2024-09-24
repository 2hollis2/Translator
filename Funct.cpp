#include "Translator.h"


LL::LL(Lexer& lexer, const std::string& outPathstr, const string& outputAtoms, const string& outputAsm) : lexer(lexer) {
    outPath = outPathstr;
    outPathAtom = outputAtoms;
    outPathAsm = outputAsm;
}



void LL::solve() {
    //Открытие файлов
    outStr.open(outPath, std::ofstream::out | std::ofstream::trunc);
    outAtoms.open(outPathAtom, std::ofstream::out | std::ofstream::trunc);
    out.open(outPathAsm, std::ofstream::out | std::ofstream::trunc);

    setLexem(); //Устанавливается текущая лексема для начала разбора программы
    statesIt = states.begin(); //отслеживает текущее состояние разбора, устанавливается на начало списка состояний
    nextState(0);
    addString("StmtList"); //В строку для отладки добавляется информация о начале разбора списка операторов

    //Вызов функции разбора
    bool f = StmtList("-1") and *iter == LEX_EOF;

    //Вывод результатов
    cout << "Execution results:" << endl;

    if (!printTree(f)) {
        cout << "Result: fail" << endl;
    }

    else if (!printAtoms()) {
        cout << "Result: fail" << endl;
    }

    else if (!asmBlock()) {
        cout << "Result: fail" << endl;
    }
    else cout << "Result: all blocks are completed successfully" << endl;

    //Закрытие файлов
    outStr.close();
    outAtoms.close();
    out.close();
    return;
}
//
//
//печать синтакс дерева
bool LL::printTree(bool fl) {
    cout << "-Syntax block: ";

    if (fl) {
        for (const auto& now : output) {
            outStr << now << endl;
            //cout << now << endl;
        }
        outStr << endl << "Result: Accept" << endl;
        cout << "The tree was painting" << endl;
    }
    else {
        outStr << endl << "Result: Error" << endl;
        cout << "Syntax error" << endl;
        return false;
    }
    return true;
}
//
// печать атомов
bool LL::printAtoms() {

    cout << "-Semantic block: ";
    for (auto now : atomList) { //проходит по всем элементам списка atomList, который содержит атомарные инструкции
        //Если хотя бы одно из полей атомарной инструкции одержит значение $Error, то
        if (now.scope == "$Error" or now.operation == "$Error" or now.second == "$Error" or now.first == "$Error" or now.third == "$Error") {
            outAtoms.clear(); //Очищается файл для атомов
            cout << "Semantic error" << endl;
            return false;
        }
        //Если ошибок нет, текущая атомарная инструкция выводится в файл outAtoms
        outAtoms << now.scope + ": " << "(" << now.operation << ", " << now.first << ", " << now.second << ", " <<
            now.third << ")" << endl;
    }
    //Печать таблицы символов:
    outAtoms << endl << endl << "=========================================" << endl;
    outAtoms << "Code  :  Name  :  Kind  :  Type  :  Init  :  Len  :  Scope  :  Offset" << endl;

    for (auto now : table) { //Проходит по всем элементам таблицы символов (table) и выводит каждый символ в файл
        outAtoms << now.code + " " + now.name + " " + now.kind + " " + now.type + " " + now.value + " " + now.len + " " + now.scope + " " + now.offset << endl;
    }
    cout << "Atoms was printed" << endl;
    return true;
}

//отвечает за получение и перемещение по лексемам в процессе разбора
void LL::setLexem() {
    if (list.empty() or iter == list.end() - 1) { //проверка, пуст ли список лексем или достигнут ли конец списка
        list.emplace_back(lexer.getNextLexem()); // если конец списка, вызывается лексер для получения следующей лексемы и добавления её в список
        iter = list.end() - 1; //перемещает итератор на последний элемент списка (новую лексему)
       

        return;
    }
    iter++; // если список не пуст и не достигнут конец, итератор продвигается к следующей лексеме

    return;
}


//добавляет новое состояние в стек состояний, сохраняя текущее положение в разборе
void LL::nextState(const int& state) {
    states.push_back(state); //добавляет новое состояние state в вектор состояний states
    statesIt = states.end() - 1; //устанавливает итератор состояний statesIt на последнее состояние в списке (последнее добавленное)
}


//возвращает текущее состояние на шаг назад и удаляет последнее добавленное состояние
void LL::backStateIt() {
    if (statesIt > states.begin()) statesIt--; //проверяет, не находится ли итератор на самом первом состоянии, если нет, перемещает итератор на одно состояние назад
    else statesIt = states.begin(); //если итератор уже на самом первом состоянии, он остается на месте
    states.pop_back(); //удаляет последнее состояние из вектора состояний, таким образом убирая последнее добавленное состояние
}

//строит строковое представление дерева разбора, добавляя соответствующие отступы и символы для обозначения структур дерева
void LL::addString(const std::string& str) {
    string outputString; //строка, которая будет содержать текущее состояние дерева разбора
    for (auto i = states.begin(); i != states.end(); i++) { // цикл по всем состояниям для формирования отступов и структуры дерева разбора
        if (i == states.end() - 1) { //если цикл дошел до последнего состояния
            if (*i == 1) outputString += "├"; //если текущее состояние равно 1, добавляется символ "├" (ветка дерева)
            else outputString += "└"; //если состояние не 1, добавляется символ "└" (последний элемент ветки)
            break;
        }
        if (*i == 1) outputString += "│ "; //если текущее состояние равно 1, добавляется символ "│" (вертикальная линия, представляющая продолжение ветки)
        else outputString += "  "; //если состояние не 1, добавляется два пробела (для отступа)
    }
    treeHeight++; //увеличивает высоту дерева(глубину разбора) на 1
    output.push_back(outputString + str); //добавляет сформированную строку с отступами и состоянием разбора в вектор output
}

//функция генерирует новую уникальную метку
string LL::newLabel() {
    string label = to_string(labelCounter); //создается строка, содержащая текущее значение счетчика меток (labelCounter)
    labelCounter++; //увеличивается счетчик меток, чтобы каждая новая метка была уникальной
    return label; //возвращает сгенерированную строку-метку
}

//выделяет временную переменную и добавляет её в таблицу переменных
string LL::alloc(const string& scope) {
    tempVarCounter++; //увеличивается счетчик временных переменных для генерации уникального имени переменной
    return addVar("$T" + to_string(tempVarCounter), scope, "kwint"); //создается имя временной переменной $T с уникальным номером, которое добавляется в таблицу переменных с типом "kwint". Возвращается результат функции addVar, которая обрабатывает добавление переменной в таблицу символов
}

//Добавляет переменную в таблицу символов. Если переменная уже существует в таблице, обновляет её значение
string LL::addVar(const std::string& name, const string& scope, const std::string type, const std::string& init) {
    bool fl = false; //флаг, который отслеживает, нужно ли выдавать ошибку
    for (int i = 0; i < table.size(); i++) { //цикл по таблице символов, чтобы проверить, существует ли переменная с таким же именем и областью видимости
        if (table[i].name == name and table[i].scope == scope) { //если имя переменной и область видимости совпадают с тем, что уже есть в таблице

            if (table[i].kind != "var" or table[i].type != type) { //проверяется, является ли символ переменной и совпадает ли тип
                fl = true; //если не совпадают  устанавливается флаг ошибки
                break;
            }
            table[i].value = init; //если переменная существует и соответствует типу, её значение обновляется
            return table[i].code; // возвращается код переменной (уникальный идентификатор в таблице)
        }
    }
    if (fl) return "$Error"; //если была ошибка (переменная уже существует с несовпадающими типом или категорией), возвращается специальное значение "$Error"
    codeCounter++; // увеличивается счетчик для генерации уникального кода переменной
    table.push_back({ "'" + to_string(codeCounter) + "'", name, "var", type, "-1", init, scope }); //если переменная не была найдена в таблице, она добавляется в таблицу с уникальным кодом, именем, типом, значением по умолчанию и областью видимости
    return "'" + to_string(codeCounter) + "'"; // возвращается уникальный код переменной
}


//Добавляет функцию в таблицу символов, если она не была добавлена ранее
string LL::addFunc(const std::string& name, const std::string& type) {
    bool fl = false; //флаг для отслеживания, была ли функция уже добавлена в таблицу
    for (auto now : table) { //цикл по таблице символов для проверки наличия функции
        if (now.name == name) {
            fl = true; //если имя функции уже существует в таблице, устанавливается флаг ошибки
            break;
        }
    }
    if (fl) return "$Error"; // если функция уже существует, возвращается специальное значение "$Error"
    codeCounter++; //увеличивается счетчик для генерации уникального кода функции
    table.push_back({ "'" + to_string(codeCounter) + "'", name, "func", type }); //добавляется новая запись в таблицу символов для функции с уникальным кодом, именем, типом функции
    return to_string(codeCounter); //возвращается уникальный код функции
}

//существует ли переменная с заданным именем и областью видимости в таблице символов. Возвращает её код или "$Error", если переменная не найдена
string LL::checkVar(const std::string& scope, const std::string& name) {
    string tempCode = "$Error"; //Инициализируется переменная, которая будет хранить код переменной
    for (auto now : table) { //Цикл по всем элементам таблицы символов
        if ((now.scope == scope or now.scope == "-1") and now.name == name and now.kind == "var") { // Если текущая запись в таблице относится к переменной (now.kind == "var"), её имя совпадает с заданным именем (now.name == name), а область видимости совпадает с текущей областью или является глобальной
            if (now.scope == scope) { //Если текущая переменная принадлежит непосредственно заданной области видимости
                tempCode = now.code; //Присваивается код переменной
                break;
            }
            tempCode = now.code; // Если переменная принадлежит глобальной области, её код сохраняется в tempCode
        }
    }
    return tempCode; //Возвращается код переменной, если она найдена, или "$Error", если нет
}

// Проверяет, существует ли функция с заданным именем и количеством параметров в глобальной области видимости. Возвращает её код или "$Error", если функция не найдена
string LL::checkFunc(const std::string& name, const std::string& len) {
    for (auto now : table) { //Цикл по таблице символов
        if (now.scope == "-1" and now.name == name and now.kind == "func" and now.len == len) //Если текущая запись является функцией(now.kind == "func"), находится в глобальной области(now.scope == "-1"), её имя и количество параметров совпадают с заданными значениями
            return now.code; //Возвращается код функции
    }
    return "$Error"; //Если функция не найдена, возвращается "$Error"
}
//
//Добавляет атомарную операцию в список операций программы. Если любая часть данных или операция содержит ошибку, очищает список атомов и добавляет ошибочную запись
void LL::addAtom(LL::atom data) {
    if (atomList.size() == 1 and atomList[0].scope == "$Error") return; //Если список атомов содержит одну ошибочную запись,  функция просто завершает выполнение без добавления нового атома
    if (data.scope == "$Error" or data.operation == "$Error" or data.first == "$Error" or data.second == "$Error" or data.third == "$Error") { // Если хотя бы одно из полей атома указывает на ошибку ("$Error")
        atomList.clear(); //Очищается весь список атомов
        atomList.push_back({ "$Error", "$Error", "$Error", "$Error", "$Error" }); //Добавляется специальный атом с полями "$Error", указывающий на общую ошибку
    }
    atomList.push_back(data); //Если ошибок нет, добавляется новый атом в список
    return;
}


