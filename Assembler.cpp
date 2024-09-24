#include "Translator.h"
bool LL::asmBlock() {
    //проверка наличия main
    bool fl = false;
    for (auto now : table) {
        if (now.name == "main") {
            fl = true;
            break;
        }
    }
    if (!fl) {
        cout << "-Assembler block: the function \'main\' is missing (Error)" << endl;
        return false;
    }

    cout << "-Assembler block: ";
    //
    // добавление глобальных переменных и других начальных
    asmList.push_back("ORG 8000H");
    for (auto now : table) {
        if (now.scope == "-1" and now.kind == "var") {
            asmList.push_back(now.name + ": DB " + now.value);
        }
    }
    asmList.push_back("ORG 0\nLXI H,0\nSPHL\nPUSH B");


    asmList.push_back("CALL main\nEND\n@MUL\n@DIV\n@PRINT\n\n");
    //
    //main block
    for (auto now : atomList) { // перебор списка атомов ызывается соответствующая функция для генерации ассемблерного кода
        if (now.operation == "ADD") ADD(now); // вызов функции по трансляции атома "ADD" 
        else if (now.operation == "SUB") SUB(now); // вызов функции по трансляции атома "SUB" 
        else if (now.operation == "MUL") MUL(now); // вызов функции по трансляции атома "MUL"
        else if (now.operation == "MOV") MOV(now); // вызов функции по трансляции атома "MOV"
        else if (now.operation == "LBL") LBL(now); // вызов функции по трансляции атома "LBL" 
        else if (now.operation == "JMP") JMP(now); // вызов функции по трансляции атома "JMP" 
        else if (now.operation == "AND") AND(now); // вызов функции по трансляции атома "AND" 
        else if (now.operation == "OR") OR(now); // вызов функции по трансляции атома "OR"
        else if (now.operation == "EQ") EQ(now); // вызов функции по трансляции атома "EQ" 
        else if (now.operation == "NE") NE(now); // вызов функции по трансляции атома "NE" 
        else if (now.operation == "LT") LT(now); // вызов функции по трансляции атома "LT" 
        else if (now.operation == "LE") LE(now); // вызов функции по трансляции атома "LE" 
        else if (now.operation == "GT") GT(now); // вызов функции по трансляции атома "GT" 
        else if (now.operation == "NOT") NOT(now); // вызов функции по трансляции атома "NOT" 
        else if (now.operation == "IN") IN(now); // вызов функции по трансляции атома "IN" 
        else if (now.operation == "OUT") OUT(now); // вызов функции по трансляции атома "OUT" 
        else if (now.operation == "PARAM") PARAM(now); // вызов функции по трансляции атома "PARAM" 
        else if (now.operation == "CALL") CALL(now); // вызов функции по трансляции атома "CALL" 
        else if (now.operation == "RET") RET(now); // вызов функции по трансляции атома "RET" 
        
        
    }
    
    printAsm();
    cout << "Assembler code has been generated successfully" << endl;
    return true;
}

void LL::printAsm() { // выводит все строки ассемблерного кода, хранящегося в asmList в указанный поток вывода (out)
    for (auto now : asmList) {
        out << now << endl;
    }
    return;
}

//функция обрабатывает различные типы операндов и загружает соответствующие значения в регистр (В зависимости от типа операнда генерируются разные команды для корректной загрузки значения)
void LL::loadOp(const string& operand, const string& scope, int newOffset = -1) { //operand - операнд, который нужно загрузить, scope - область видимости операнда, newOffset - новое смещение для переменной ((-1), используется стандартное смещение)
    if (operand[0] != '\'') { //обычная строка или число, которое напрямую можно загрузить в аккумулятор процессора
        asmList.push_back("MVI A, " + operand); //загружает значение операнда в аккумулятор
        return;
    }
    else if (operand[0] == '\"') { //Если строка начинается с символа " (строковый литерал)
        asmList.push_back("MVI A, " + operand.substr(1, operand.size() - 2)); //извлекаются символы между кавычками, MVI A загружает значение в аккумулятор
        return;
    }
                                                                           //если ' ссылку на объект из таблицы символов table, извлекает номер объекта и получает его данные из таблицы
    object item = table[stoi(operand.substr(1, operand.size() - 2)) - 1]; //Преобразование строкового представления числа в целое число для индексации таблицы table
    if (item.scope == "-1") { //находится ли объект item в глобальной области видимости
        asmList.push_back("LDA " + item.name); //загружает значение из памяти (по имени объекта item.name) в аккумулятор
    }
    else {
        if (newOffset != -1) { //Если задано новое смещение, нужно использовать его для указания адреса переменной
            asmList.push_back("LXI H, " + to_string(newOffset)); //загружает новое смещение в регистр H

        }
        else {
            asmList.push_back("LXI H, " + item.offset); //новое смещение не указано, используется стандартное смещение item.offset
        }
        asmList.push_back("DAD SP"); //добавляет значение регистра SP (указателя стека), результатом является указание на местоположение переменной в стеке
        asmList.push_back("MOV A, M"); //перемещает значение из адреса в аккумулятор A
    }
}

//сохраняет значение операнда в соответствующее место памяти
void LL::saveOp(const string& operand) {                               
    object item = table[stoi(operand.substr(1, operand.size() - 2)) - 1]; //извлекает объект item из таблицы table, основываясь на индексе, полученном из строки operand(stoi преобразует строковый индекс в число, чтобы обратиться к элементу в таблице символов)
    if (item.scope == "-1") {
        asmList.push_back("STA " + item.name); //сохраняет содержимое аккумулятора в памяти по адресу, соответствующему имени переменной item.name
    }
    else { //Если переменная не является глобальной
        asmList.push_back("LXI H, " + item.offset); //загружает смещение локальной переменной относительно указателя стека в регистры H и L
        asmList.push_back("DAD SP"); //добавляет значение указателя стека (SP) к регистрами H и L
        asmList.push_back("MOV M, A"); //перемещает значение из аккумулятора (A) в память по адресу, хранящемуся в регистрах H и L
    }
}
void LL::ADD(const LL::atom& atom) {
    asmList.push_back("\n\t; ADD block"); //указывает, что начинается блок для операции сложения
    loadOp(atom.second, atom.scope); //загружает значение из указанного атома в регистр A 
    asmList.push_back("MOV B, A"); //генерирует MOV B, A, копирует текущее значение из регистра A в регистр B
    loadOp(atom.first, atom.scope); //загружает значение первого атома в регистр A
    asmList.push_back("ADD B"); //генерирует ADD B,складывает значение в регистре A с значением в регистре B
    saveOp(atom.third); //сохраняет результат сложения
}

void LL::SUB(const LL::atom& atom) {
    asmList.push_back("\n\t; SUB block"); //указывает, что начинается блок для операции вычитания 
    loadOp(atom.second, atom.scope); //Загружает значение второго атома в регистр A
    asmList.push_back("MOV B, A"); //Сохраняет значение из регистра A в регистр B
    loadOp(atom.first, atom.scope); //Загружает значение первого атома в регистр A
    asmList.push_back("SUB B"); //Вычитает значение в регистре B из значения в регистре A
    saveOp(atom.third); //Сохраняет результат операции
}

void LL::MUL(const LL::atom& atom) {
    asmList.push_back("\n\t; MUL block"); //указывает, что начинается блок для операции умножения
    loadOp(atom.second, atom.scope);  //Загружает значение второго атома в регистр A
    asmList.push_back("MOV D, A");  //Сохраняет значение из регистра A в регистр D
    loadOp(atom.first, atom.scope); //Загружает значение первого атома в регистр A
    asmList.push_back("MOV C, A");  // Сохраняет значение из регистра A в регистр C

    asmList.push_back("CALL @MULT");

    asmList.push_back("MOV A, B"); //Сохраняет значение из регистра B в регистр A
    saveOp(atom.third); //Сохраняет результат операции
}

void LL::MOV(const LL::atom& atom) {
    asmList.push_back("\n\t; MOV block"); //указывает, что начинается блок для операции MOV
    loadOp(atom.first, atom.scope); //Загружает значение в аккумулятор
    saveOp(atom.third); ////Сохраняет результат операции

}

void LL::LBL(const LL::atom& atom) {
    asmList.push_back(atom.third + ":"); //добавляет метку в asmList. Метка создается из значения atom.third, к которому добавляется двоеточие (:). Это будет использоваться как место назначения для переходов
    if (atom.third[0] != '$') { // Проверяет, не начинается ли метка с символа $
        for (auto now : table) {  
            if (now.name == atom.third) { //ищет в таблице table объект с именем atom.third
                for (int i = 0; i < stoi(now.offset) - stoi(now.len); i++) {
                    asmList.push_back("PUSH B"); //добавляет команды PUSH B в asmList. Количество этих команд равно разности между now.offset и now.len
                }
                break;
            }
        }
    }
}

void LL::JMP(const LL::atom& atom) {
    asmList.push_back("JMP " + atom.third); //добавляет команду перехода JMP в asmList, где atom.third является меткой, на который осуществляется переход
}

// Выполняет логическую операцию "И" между двумя операндами и сохраняет результат
void LL::AND(const LL::atom& atom) {
    asmList.push_back("\n\t; AND block"); //указывает, что начинается блок для операции AND
    loadOp(atom.second, atom.scope); //загружает второй операнд в регистр A
    asmList.push_back("MOV B, A"); //Перемещает содержимое регистра A в регистр B
    loadOp(atom.first, atom.scope); //Загружает первый операнд в регистр A
    asmList.push_back("ANA B"); //Выполняет AND между значением в регистре A и значением в регистре B. Результат сохраняется в регистре A.
    saveOp(atom.third); //Сохраняет результат операции AND
}

//Выполняет логическую операцию "ИЛИ" между двумя операндами и сохраняет результат
void LL::OR(const LL::atom& atom) {
    asmList.push_back("\n\t; OR block"); //указывает, что начинается блок для операции OR
    loadOp(atom.second, atom.scope); //загружает значение второго операнда atom.second в аккумулятор (регистр A)
    asmList.push_back("MOV B, A"); //Значение из аккумулятора копируется в регистр B
    loadOp(atom.first, atom.scope); //загрузка первого операнда atom.first в аккумулятор
    asmList.push_back("ORA B"); //выполняет операцию "ИЛИ" между значением в аккумуляторе и значением в регистре B. Результат остается в аккумуляторе
    saveOp(atom.third); //результат операции OR сохраняется
}

// Сравнивает два операнда и переходит по метке, если они равны
void LL::EQ(const LL::atom& atom) {
    asmList.push_back("\n\t; EQ block"); //указывает, что начинается блок для операции EQ "равенство"
    loadOp(atom.second, atom.scope); //Загружает значение второго операнда atom.second в аккумулятор
    asmList.push_back("MOV B, A"); //Копирует значение из аккумулятора в регистр B
    loadOp(atom.first, atom.scope); //Загружает значение первого операнда atom.first в аккумулятор
    asmList.push_back("CMP B"); //сравнивает значение в аккумуляторе с тем, что в регистре B. Если значения равны, устанавливается флаг равенства
    asmList.push_back("JZ " + atom.third); //выполняет переход по метке atom.third, если флаг равенства установлен (то есть если два значения равны). JZ (Jump if Zero)
}

//Сравнивает два операнда и переходит по метке, если они не равны
void LL::NE(const LL::atom& atom) {
    asmList.push_back("\n\t; NE block"); //указывает, что начинается блок для операции NE "неравенство"
    loadOp(atom.second, atom.scope); //Загружает значение второго операнда atom.second в аккумулятор
    asmList.push_back("MOV B, A"); //Копирует значение из аккумулятора в регистр B
    loadOp(atom.first, atom.scope); //Загружает значение первого операнда atom.first в аккумулятор
    asmList.push_back("CMP B"); //Сравнивает значение в аккумуляторе с тем, что в регистре B
    asmList.push_back("JNZ " + atom.third); //выполняет переход по метке atom.third, если флаг равенства не установлен. JNZ (Jump if Not Zero)
}

//Сравнивает два операнда и переходит по метке, если первый операнд меньше второго
void LL::LT(const LL::atom& atom) {
    asmList.push_back("\n\t; LT block"); //указывает, что начинается блок для операции LT "меньше"
    loadOp(atom.second, atom.scope); //Загружает значение второго операнда atom.second в аккумулятор (регистр A)
    asmList.push_back("MOV B, A"); //Копирует значение из аккумулятора в регистр B
    loadOp(atom.first, atom.scope); //Загружает значение первого операнда atom.first в аккумулятор
    asmList.push_back("CMP B"); //сравнивает значение в аккумуляторе с тем, что в регистре B
    asmList.push_back("JM " + atom.third); //выполняет переход по метке atom.third, если результат первое значение меньше второго (флаг "отрицательный" установлен). JM (Jump if Minus)
}

//Сравнивает два операнда и переходит по метке, если первый операнд меньше или равен второму
void LL::LE(const LL::atom& atom) {
    asmList.push_back("\n\t; LE block"); //указывает, что начинается блок для операции LE "меньше или равно"
    loadOp(atom.second, atom.scope); //Загружает значение второго операнда atom.second в аккумулятор
    asmList.push_back("MOV B, A"); //Копирует значение из аккумулятора в регистр B
    loadOp(atom.first, atom.scope); //Загружает значение первого операнда atom.first в аккумулятор
    asmList.push_back("CMP B"); //Сравнивает значения в аккумуляторе и регистре B
    asmList.push_back("JZ " + atom.third); //переход по метке atom.third, если значения равны. JZ (Jump if Zero)
    asmList.push_back("JM" + atom.third); //переход по метке atom.third, если первое значение меньше второго. JM (Jump if Minus)
}

//Сравнивает два операнда и переходит по метке, если первый операнд больше второго
void LL::GT(const LL::atom& atom) {
    asmList.push_back("\n\t; GT block"); //указывает, что начинается блок для операции GT "больше"
    loadOp(atom.second, atom.scope); //Загружает значение второго операнда atom.second в аккумулятор
    asmList.push_back("MOV B, A"); //Копирует значение из аккумулятора в регистр B
    loadOp(atom.first, atom.scope); //Загружает значение первого операнда atom.first в аккумулятор
    asmList.push_back("CMP B"); //Сравнивает значения в аккумуляторе и регистре B
    asmList.push_back("JP " + atom.third); //переход по метке atom.third, если первое значение больше второго (флаг "положительный" установлен). JP (Jump if Positive)
}

//Загружает значение, инвертирует его
void LL::NOT(const LL::atom& atom) {
    asmList.push_back("\n\t; NOT block"); //указывает, что начинается блок для операции NOT
    loadOp(atom.first, atom.scope); //загружает значение первого операнда atom.first в аккумулятор (регистр A)
    asmList.push_back("CMA"); //инвертирует все биты в аккумуляторе
    saveOp(atom.third); //сохраняет результат операции NOT
}

//Считывает данные из порта 0 и сохраняет их в указанном месте
void LL::IN(const LL::atom& atom) {
    asmList.push_back("\n\t; IN block"); //указывает, что начинается блок для операции IN 
    asmList.push_back("IN 0"); //считывает данные из порта ввода с номером 0 и помещает их в аккумулятор
    saveOp(atom.third); //сохраняет результат операции IN
}

//Загружает значение из указанного места и выводит его на порт 1
void LL::OUT(const LL::atom& atom) {
    asmList.push_back("\n\t; OUT block"); //указывает, что начинается блок для операции OUT
    loadOp(atom.third, atom.scope); //загружает значение, хранящееся по адресу, указанному в atom.third, в аккумулятор
    asmList.push_back("OUT 1"); //выводит содержимое аккумулятора на порт вывода с номером 1
}

//Сохраняет параметр функции в стеке
void LL::PARAM(const LL::atom& atom) {
    programStack.push(atom.third); //добавляет значение atom.third в стек программы programStack. позволяет сохранять параметры для последующего использования при вызове функции
}

//Вызывает функцию, сохраняя параметры и восстанавливая состояние после завершения
void LL::CALL(const LL::atom& atom) {
    asmList.push_back("\n\t; CALL block"); //указывает, что начинается блок для операции CALL
    asmList.push_back("PUSH B"); //Сохраняет значение регистра B в стеке, чтобы его можно было восстановить после завершения функции

    object func = table[stoi(atom.first.substr(1, atom.first.size() - 2)) - 1]; //Извлекает информацию о функции из таблицы table по индексу, переданному в atom.first
    int n = stoi(func.len); //количество параметров функции
    int total = stoi(func.offset); //общий смещение функции
    string funcCode = func.code.substr(1, func.code.size() - 2); //код функции, который будет вызван

    for (int i = 0; i < n; i++) {
        asmList.push_back("PUSH B"); //добавляет команды PUSH B в asmList, чтобы сохранить все параметры функции перед её вызовом
    }
    for (int i = n - 1; i >= 0; i--) {
        string item = programStack.top(); //извлекаются параметры из стека программы
        programStack.pop();
        int newOffset = stoi(table[stoi(funcCode) + i].offset) - (i + 1 != n ? stoi(table[stoi(funcCode) + i + 1].offset) : 0); //вычисляется новое смещение для каждого параметра
        loadOp(item, atom.scope, newOffset); //загружается значение в аккумулятор с помощью loadOp


        asmList.push_back("LXI H, " + to_string((i + 1) * 2)); //Загружается смещение в регистр H
        asmList.push_back("DAD SP"); //Вычисляется адрес, куда будет записано значение
        asmList.push_back("MOV M, A"); //Сохраняется значение аккумулятора по вычисленному адресу
    }
    asmList.push_back("CALL " + func.name); //Добавляет команду вызова функции по имени

    for (int i = 0; i < n; i++) { //Восстанавливает параметры, которые были сохранены в стеке
        asmList.push_back("POP B");
    }
    asmList.push_back("POP B"); //Восстанавливает значение регистра B
    asmList.push_back("MOV A, C"); //Копирует значение из регистра C в аккумулятор
    saveOp(atom.third); //Сохраняет результат в указанное место atom.third
}

//Возвращает значение из функции и очищает параметры из стека
void LL::RET(const LL::atom& atom) {
    int m = stoi(table[stoi(atom.scope) - 1].offset); //количество параметров, которые нужно удалить из стека
    asmList.push_back("\n\t; RET block"); ////указывает, что начинается блок для операции RET
    loadOp(atom.third, atom.scope); //Загружает значение, которое будет возвращено, в аккумулятор
    asmList.push_back("LXI H, " + to_string(m * 2 + 2)); //Загружает смещение в регистр H
    asmList.push_back("DAD SP"); //Вычисляет адрес возврата
    asmList.push_back("MOV M, A"); //Сохраняет значение аккумулятора по адресу возврата
    for (int i = 0; i < m; i++) { //Удаляет параметры из стека, восстанавливая состояние программы после вызова функции
        asmList.push_back("POP");
    }
    asmList.push_back("RET"); //Добавляет команду RET, которая возвращает управление обратно в вызывающий код
}
