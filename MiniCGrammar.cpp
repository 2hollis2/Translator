#include "Translator.h"

bool LL::StmtList(const string& scope) { // вызывает функцию для обработки каждого отдельного оператора и, при успешной обработке, добавляет их в список
    if (*iter == LEX_EOF) return true; //Если текущая лексема является концом файла, завершается обработка операторов

    nextState(1);
    addString("Stmt"); //Сохраняем текущее состояние и добавляем строку "Stmt"
    auto listIt = iter;

    if (!Stmt(scope)) { //Вызывается функция Stmt(scope), которая проверяет, является ли текущая лексема началом оператора
        output.pop_back();
        backStateIt(); //последний добавленный элемент, и состояние возвращается на два шага назад
        backStateIt();
        return true;
    }
    //Если оператор успешно обработан, обновляется состояние и добавляется строка "StmtList"
    nextState(0);
    addString("StmtList");

    if (!StmtList(scope)) return false; //Рекурсивно вызывается сама функция для обработки следующего оператора. Если следующего оператора нет, возвращается false

    //Возвращаем состояние назад и завершаем обработку
    backStateIt();
    return true;
}

bool LL::Stmt(const string& scope) {
    if (*iter == LEX_EOF) return false; //Если текущая лексема является концом файла, функция возвращает false

    if (iter->first == "kwint" or iter->first == "kwchar") { //Проверяется, является ли текущая лексема ключевым словом объявления переменной
        nextState(0);
        addString("DeclareStmt"); 
                                    //вызывается функция для обработки объявления переменной DeclareStmt(scope)
        if (!DeclareStmt(scope)) return false;
        backStateIt();
        return true;
    }
    //Если текущая лексема — точка с запятой (semicolon), она просто устанавливается как лексема (фиксируется), и обработка успешно завершается
    if (iter->first == "semicolon") {
        setLexem();
        nextState(0);
        addString("semicolon");
        backStateIt();
        backStateIt();
        return true;
    }

    if (scope == "-1") return false; //Если область видимости (scope) имеет значение -1, функция завершает обработку, возвращая false

    //Если текущая лексема — идентификатор (id), предполагается, что это либо присваивание, либо вызов функции. Вызывается соответствующая функция AssignOrCallOp(scope)
    if (iter->first == "id") {
        nextState(0);
        addString("AssignOrCallOp");

        if (!AssignOrCallOp(scope)) return false;

        backStateIt();
        return true;
    }
    //Обработка цикла while. Если текущая лексема — ключевое слово while, вызывается функция для обработки цикла WhileOp(scope)
    if (iter->first == "kwwhile") {
        nextState(0);
        addString("kwwhile WhileOp");

        if (!WhileOp(scope)) return false;
        backStateIt();
        return true;
    }
    //Обработка цикла while. Если текущая лексема — ключевое слово while, вызывается функция для обработки цикла WhileOp(scope)
    if (iter->first == "kwfor") {
        nextState(0);
        addString("kwfor ForOp");
        if (!ForOp(scope)) return false;
        backStateIt();
        return true;
    }
    //Обработка условного оператора if. Если текущая лексема — if, вызывается функция для обработки условия IfOp(scope)
    if (iter->first == "kwif") {
        nextState(0);
        addString("kwif IfOp");
        if (!IfOp(scope)) return false;
        backStateIt();
        return true;
    }
    //Обработка оператора switch. Если текущая лексема — ключевое слово switch, вызывается функция для обработки SwitchOp(scope)
    if (iter->first == "kwswitch") {
        nextState(0);
        addString("kwswitch SwitchOp");
        if (!SwitchOp(scope)) return false;
        backStateIt();
        return true;
    }
    //Обработка оператора ввода in
    if (iter->first == "kwin") {
        nextState(0);
        addString("kwin IOp");
        if (!IOp(scope)) return false;
        backStateIt();
        return true;
    }
    //Обработка оператора вывода out
    if (iter->first == "kwout") {
        nextState(0);
        addString("kwout OOp");
        if (!OOp(scope)) return false;
        backStateIt();
        return true;
    }
    //Обработка блоков кода, заключенных в фигурные скобки {}. 
    //После открытия скобки обрабатывается список операторов, а затем проверяется наличие закрывающей скобки
    if (iter->first == "lbrace") {
        setLexem();
        nextState(1);
        addString("lbrace StmtList");
        if (!StmtList(scope)) { return false; }
        if (iter->first != "rbrace") { return false; }
        setLexem();
        nextState(0);
        addString("rbrace");
        backStateIt();
        backStateIt();
        return true;
    }
    //Обработка оператора return. После ключевого слова return ожидается выражение E(scope).
    //Если выражение корректно, оно добавляется в результат с типом RET
    if (iter->first == "kwreturn") {
        setLexem();

        nextState(1);
        addString("kwreturn E");

        auto result = E(scope);
        if (!result.first) { return false; }
        if (iter->first != "semicolon") { return false; }

        nextState(0);
        addString("semicolon");
        setLexem();
        backStateIt();
        backStateIt();

        addAtom({ scope, "RET", "", "", result.second });
        return true;
    }

    return false; //Если ни одно из условий не подошло, возвращаем false, означающее, что текущая лексема не является корректным оператором
}
//
//Declare block
////функция проверяет, является ли текущая лексема типом данных, таким как int или char
pair<bool, string> LL::Type(const string& scope) {
    if (*iter == LEX_EOF) return { false, "" }; //Если текущая лексема — конец файла возвращает false и пустую строку, указывая на то, что тип данных не найден

    if (iter->first == "kwint" or iter->first == "kwchar") { //Если текущая лексема является ключевым словом для типа данных int или char
        auto item = iter->first; //item присваивается значение текущей лексемы (тип данных)

        nextState(0); //Состояние анализатора изменяется с помощью nextState(0) для сохранения состояния
        addString(iter->first); //Лексема добавляется с помощью addString(iter->first)
        setLexem(); // фиксируется
        backStateIt(); //возвращает состояние назад дважды с помощью backStateIt()
        backStateIt();
        return { true, item }; //Функция возвращает пару значений: true (указывая, что тип данных был найден) и item (тип данных)
    }
    return { false, "" }; //Если текущая лексема не является типом данных, функция возвращает пару значений: false и пустую строку, указывая на то, что тип данных не был найден
}

//проверяет, является ли текущая конструкция синтаксически корректным объявлением переменной
bool LL::DeclareStmt(const string& scope) {

    //Функция начинает с изменения состояния анализатора с помощью nextState(1) для сохранения текущего состояния и добавляет строку "Type"
    nextState(1);
    addString("Type");

    auto result = Type(scope); //Далее вызывается функция Type(scope), которая проверяет, начинается ли объявление с корректного типа данных
    if (!result.first) { return false; } //Если тип данных не был найден, функция завершает работу с возвратом false, что означает неудачу в обработке объявления

    if (iter->first != "id") return false; //После успешного определения типа данных проверяется, следует ли за ним идентификатор 
    auto item = iter->second; //Если не явл индентификатором возвращает false. В переменную item сохраняется тип данных, который был найден

    nextState(0);
    addString(" " + item + " DeclareStmt'"); //добавляется строка, содержащая тип данных и текст "DeclareStmt'"
    setLexem();

    if (!DeclareStmtList(scope, result.second, item)) return false; //Вызывается функция DeclareStmtList, которая обрабатывает список переменных в объявлении

    //Если все прошло успешно, состояние анализатора возвращается на шаг назад с помощью backStateIt(),
    //и функция возвращает true, указывая, что объявление переменной успешно распознано
    backStateIt();
    return true;

}

//Обрабатывает объявления функций и переменных
//При встрече объявления функции, проверяет, что объявление корректное и определяет параметры функции
//Обрабатывает присваивания значений переменным
//Создает инструкции на основе синтаксического анализа для дальнейшего использования
bool LL::DeclareStmtList(const string& scope, const string& p, const string& q) { // p = type, q = name
    if (*iter == LEX_EOF) return false; //Если текущая лексема — конец файла false
    if (iter->first == "lpar") { //Если текущая лексема — открывающая круглая скобка lpar, это указывает на то, что начинается объявление функции
        if (scope != "-1") return false; //проверяется, что текущая область видимости — глобальная, так как функции не могут быть объявлены внутри других функций
        setLexem(); //Лексема lpar устанавливается 
        nextState(1); //зменяется с помощью nextState(1) для сохранения текущего состояния
        addString("lpar ParamList"); //добавляется строка "lpar ParamList", чтобы указать начало списка параметров функции

        string codeFunc = addFunc(q, p); //добавляет объявление новой функции с именем q и типом возвращаемого значения p. Возвращается уникальный код функции

        auto result = ParamList(codeFunc); //Вызывается функция ParamList(codeFunc) для анализа списка параметров функции
        if (!result.first) return false; //Если список параметров не может быть обработан, возвращается false


        table[stoi(codeFunc) - 1].len = result.second; //В таблице символов для текущей функции обновляется информация о длине (количество параметров функции)

        if (iter->first != "rpar") return false; //Проверяются наличие закрывающей скобки rpar
        setLexem(); //фиксируется
        if (iter->first != "lbrace") return false; //открывающей фигурной скобки lbrace, что указывает на начало тела функции
        setLexem(); //фиксируются
        nextState(1);
        addString("rpar lbrace StmtList"); //добавляется строка "rpar lbrace StmtList", указывающая на конец списка параметров и начало тела функции

        addAtom({ codeFunc, "LBL", "", "", q }); //Создается атомарная инструкция с меткой функции LBL
        if (!StmtList(codeFunc)) return false; //Вызывается функция StmtList(codeFunc) для обработки списка операторов внутри тела функции
        if (iter->first != "rbrace") return false; //проверяется наличие закрывающей фигурной скобки rbrace, которая завершает тело функции
        setLexem();
        nextState(0);
        addString("rbrace");
        backStateIt(); //возвращается дважды назад с помощью backStateIt()
        backStateIt();
        
        //Контроль смещений для локальных переменных
        
        //Здесь производится вычисление смещений для локальных переменных внутри функции. Переменные внутри функции размещаются в определенных ячейках памяти, 
        //и этот цикл устанавливает смещения для каждой переменной. counter отвечает за текущее смещение, а total_cnt — за количество переменных
        int counter = 0;
        int total_cnt = 0;
        for (int i = table.size() - 1; i > stoi(codeFunc) - 1 + stoi(result.second); i--) {
            if (table[i].scope == codeFunc) {
                table[i].offset = to_string(counter);
                counter += 2;
                total_cnt++;
            }
        }
        //Смещения также устанавливаются для всех локальных переменных внутри функции. 
        //Число переменных сохраняется в таблице символов для функции
        counter += 2;

        for (int i = stoi(codeFunc) + stoi(result.second) - 1; i > stoi(codeFunc) - 1; i--) {
            table[i].offset = to_string(counter);
            counter += 2;
            total_cnt++;
        }
        table[stoi(codeFunc) - 1].offset = to_string(total_cnt); //Устанавливается общее количество локальных переменных для функции
        //
        //В конце тела функции создается атомарная инструкция возврата RET, чтобы функция могла завершиться корректно
        addAtom({ codeFunc, "RET", "", "", "0" });
        return true;
    }
    //Обработка присваивания переменной
    else if (iter->first == "opassign") { //Если текущая лексема — оператор присваивания (opassign), она фиксируется
        setLexem();
        auto item = iter->second; //Переменная item сохраняет значение, которое будет присвоено переменной

        if (iter->first == "num") { //Если следующее значение — число (num)
            string var = addVar(q, scope, p, item); //вызывается функция addVar(q, scope, p, item) для добавления переменной
                                                   //q с типом p в текущую область видимости scope, и ей присваивается значение item

            if (var == "'$Error'") return false; //Если переменная не может быть добавлена, функция возвращает ошибку

            nextState(1);
            addString("opassign " + item + " DeclareVarList"); // добавляется строка с оператором присваивания и переменной
            setLexem();

            addAtom({ scope, "MOV", item, "", var }); //создается атомарная инструкция MOV, которая выполняет операцию присваивания

            //вызывается функция DeclareVarList(scope, p) для обработки списка переменных
            //Если она возвращает ошибку или не найден символ semicolon для завершения операции, обработка завершается с ошибкой
            if (!DeclareVarList(scope, p)) return false;
            if (iter->first != "semicolon") return false;
            setLexem();
            nextState(0);
            addString("semicolon");
            backStateIt();
            backStateIt();
            return true;
        }
        //Общий случай объявления переменной
        else if (iter->first == "char") { //Если текущая лексема соответствует типу char
            string var = addVar(q, scope, p, item); //Вызов функции addVar(q, scope, p, item) добавляет переменную с именем q, в области видимости scope и типа p(в данном случае — char), присваивая ей значение item
            if (var == "'$Error'") return false;

            nextState(1);
            addString("opassign " + item + " DeclareVarList"); //Добавляет строку "opassign " + item + " DeclareVarList", чтобы зафиксировать, что произошло присваивание значения item переменной и будет обработан список переменных
            setLexem(); //Фиксирует
            addAtom({ scope, "MOV", item, "", var }); //Создает атомарную инструкцию для присваивания значения. Инструкция MOV переносит значение item в переменную var, которая была объявлена ранее
            if (!DeclareVarList(scope, p)) return false; //Вызывается функция DeclareVarList, которая обрабатывает дальнейшие объявления переменных
            if (iter->first != "semicolon") return false; //Проверяет наличие точки с запятой semicolon. Если после присваивания не стоит точка с запятой, возвращается ошибка
            setLexem();
            nextState(0);
            addString("semicolon"); //Добавляет строку "semicolon"
            backStateIt(); //Возвращает
            backStateIt();
            return true;
        }
        else return false;
    }
    //Общий случай объявления переменной
    
    //Если лексема не является оператором присваивания, это обычное объявление переменной
    else {
        string var = addVar(q, scope, p); //В случае обычного объявления переменной она добавляется в текущую область видимости
        if (var == "$Error") return false;

        nextState(1);

        //вызывается функция DeclareVarList(scope, p) для обработки списка переменных
        addString("DeclareVarList");
        if (!DeclareVarList(scope, p)) return false;

        //проверяется наличие точки с запятой semicolon
        if (iter->first != "semicolon") return false;
        setLexem();
        nextState(0);
        addString("semicolon"); //Добавляет строку "semicolon"
        backStateIt(); //Возвращает
        backStateIt();
        return true;
    }
}

//функция обрабатывает список объявлений переменных, которые разделены запятыми.  Она может либо просто объявить переменную, либо обработать её присваивание
bool LL::DeclareVarList(const string& scope, const string& p) {
    if (*iter == LEX_EOF) return false; //Проверка на конец файла

    if (iter->first == "comma") { //Если текущая лексема — это запятая, значит, после нее должно следовать объявление следующей переменной
        setLexem(); //Устанавливает текущую лексему как обработанную

        if (iter->first != "id") return false; //Если после запятой не идет идентификатор переменной (id), возвращает ошибку
        auto item = iter->second; //Сохраняет имя переменной (идентификатор) в переменной item

        nextState(1); //Переходит в следующее состояние 
        addString("comma " + item + " InitVar"); //Добавляет строку "comma " + item + " InitVar", чтобы зафиксировать, что после запятой идёт переменная и её инициализация
        setLexem(); //Устанавливает текущую лексему как обработанную
        if (!InitVar(scope, p, item)) return false; //Вызывает функцию InitVar, которая будет инициализировать переменную. Если инициализация не удалась, возвращает false
        nextState(0); //Возвращается в начальное состояние
        addString("DeclareVarList"); //Добавляет строку, указывающую на начало нового списка переменных
        if (!DeclareVarList(scope, p)) return false; //Рекурсивно вызывает DeclareVarList, чтобы обработать следующую переменную в списке
        backStateIt(); //Возвращается
        return true;
    }
    backStateIt();
    return true;
}

//функция обрабатывает инициализацию переменной.  Она может либо просто объявить переменную, либо обработать её присваивание
bool LL::InitVar(const string& scope, const string& p, const string& q) {
    if (*iter == LEX_EOF) return false; //Проверка на конец файла

    if (iter->first == "opassign") { //Если текущая лексема — оператор присваивания opassign, то переменной присваивается значение
        setLexem(); //Устанавливает текущую лексему как обработанную

        if (iter->first != "num" and iter->first != "char") return false; //Проверяет, что после оператора присваивания должно следовать либо число, либо символ
        string var = addVar(q, scope, p, iter->second); //Добавляет переменную q в область видимости scope с типом p и значением, которое содержится в лексеме iter->second. Если добавить переменную не удалось, возвращает строку "$Error"
        if (var == "'$Error'") return false; //Если возникла ошибка при добавлении переменной, функция возвращает false

        addAtom({ scope, "MOV", iter->second, "", var }); //Создает атомарную инструкцию для присваивания значения переменной. 
                                                         //Инструкция MOV указывает на перемещение значения из правой части в переменную var

        nextState(0); //Возвращает состояние
        addString("opassign " + iter->second); //Добавляет строку, указывающую на оператор присваивания и значение, которое присвоено переменной
        setLexem(); //Устанавливает текущую лексему как обработанную
        backStateIt(); //возврат
        backStateIt();
        return true;
    }
    //Если переменной не присваивается значение сразу, вызывается addVar для просто объявления переменной без инициализации
    string var = addVar(q, scope, p);
    if (var == "'$Error'") return false;

    backStateIt();
    return true;
}

//функция обрабатывает список параметров функции. Она анализирует типы параметров и их имена, создавая соответствующие записи в таблице символов
pair<bool, string> LL::ParamList(const string& scope) {
    if (*iter == LEX_EOF) return { false, "" }; //Проверка на конец файла

    if (iter->first == "kwint" or iter->first == "kwchar") { //Если текущая лексема — ключевое слово для типа данных, это указывает на начало параметра
        nextState(1); //Переход в новое состояние
        addString("Type"); //Добавляет строку "Type" для фиксации типа данных параметра
        auto type = Type(scope); //Вызывает функцию Type, которая анализирует и возвращает тип параметра
        if (iter->first != "id") return { false, "" }; // Если после типа параметра не идет идентификатор, возвращает ошибку
        string var = addVar(iter->second, scope, type.second); //Добавляет параметр в таблицу символов. iter->second — это имя параметра, а type.second — его тип
        if (var == "'$Error'") return { false, "" }; //Если возникла ошибка при добавлении параметра, возвращает ошибку

        nextState(0); //начальное состояние
        addString(" " + iter->second + " ParamList'"); //Добавляет строку, указывающую на имя параметра и начало нового списка параметров
        setLexem(); //Устанавливает текущую лексему как обработанную

        auto result = ParamListList(scope); //Вызывает функцию ParamListList, которая обрабатывает следующий параметр в списке. Возвращает пару значений: успешность и количество параметров
        if (!result.first) return { false, "" }; //Если обработка следующего параметра не удалась, возвращает ошибку
        backStateIt(); //возврат 
        return { true, to_string(stoi(result.second) + 1) }; //Возвращает пару значений: успешность обработки и количество параметров, увеличенное на 1
    }
    // Если не удалось найти параметры, возвращает успешность и 0 параметров
    backStateIt();
    return { true, "0" };
}

//функция рекурсивно обрабатывает список параметров функции, которые разделены запятыми
pair<bool, string> LL::ParamListList(const string& scope) {
    if (*iter == LEX_EOF) return { false, "" }; //Проверка на конец файла

    if (iter->first == "comma") { //Если текущая лексема — запятая, это означает, что следующий параметр должен идти после неё
        setLexem(); //Устанавливает текущую лексему как обработанную
        if (iter->first != "kwint" and iter->first != "kwchar") { //Проверка, что после запятой должен идти тип параметра
            backStateIt(); //Если это не так, возвращается на шаг назад и возвращает пару {true, "0"}, указывая на то, что параметры закончились
            return { true, "0" };
        }
        nextState(1); 
        addString("comma Type"); //Добавляет строку, указывающую на запятую и тип следующего параметра

        auto type = Type(scope); //Вызывает функцию Type для анализа типа параметра
        if (iter->first != "id") return { false, "" }; //После типа должен идти идентификатор. Если это не так, возвращает ошибку
        string var = addVar(iter->second, scope, type.second); //Добавляет параметр в область видимости scope с указанным типом
        if (var == "'Error'") return { false, "" }; //Если не удалось добавить переменную, возвращает ошибку

        nextState(0); 
        addString(" " + iter->second + " ParamList'"); //Добавляет строку, указывающую на добавление параметра и продолжение списка параметров
        setLexem(); //Устанавливает текущую лексему как обработанную

        auto result = ParamListList(scope); //Рекурсивно вызывает саму себя для обработки следующего параметра
        if (!result.first) return { false, "" }; // Если обработка следующего параметра не удалась, возвращает ошибку
        backStateIt(); //Возвращается на шаг назад
        return { true, to_string(stoi(result.second) + 1) }; //Возвращает успех и количество параметров, увеличенное на 1
    }
    //Если не было найдено никаких параметров после запятой, возвращает успешность и 0 параметров
    backStateIt();
    return { true, "0" };
}

//
//функция обрабатывает выражения, которые могут быть либо операцией присваивания, либо вызовом функции. Завершается точкой с запятой
bool LL::AssignOrCallOp(const string& scope) {
    nextState(1);
    addString("AssignOrCall"); //Добавляет строку с описанием операции: это либо присваивание, либо вызов функции
    if (!AssignOrCall(scope)) return false; //Вызывает функцию AssignOrCall, которая определяет, является ли текущее выражение присваиванием или вызовом функции
    if (iter->first != "semicolon") return false; //После выражения ожидается точка с запятой. Если её нет, возвращает ошибку
    nextState(0);
    addString("semicolon"); //Добавляет строку, указывающую на завершение выражения точкой с запятой
    setLexem(); //Устанавливает текущую лексему как обработанную

    //Возвращается на два шага назад
    backStateIt();
    backStateIt();
    return true;
}


//функция определяет, является ли текущее выражение присваиванием переменной или вызовом функции
bool LL::AssignOrCall(const string& scope) {
    if (iter->first != "id") return false; //Если текущая лексема — не идентификатор
    auto item = iter->second; //Сохраняет идентификатор в переменной item

    nextState(0);
    addString(item + " AssignOrCall'"); //Добавляет строку, описывающую операцию (присваивание или вызов функции) с именем переменной или функции
    setLexem(); //Устанавливает текущую лексему как обработанную
    if (!AssignOrCallList(scope, item)) return false; //Вызывает функцию AssignOrCallList, которая проверяет, что за выражение следует дальше (присваивание или вызов функции)
    backStateIt(); //Возвращается
    return true;
}

//Эта функция проверяет, что идёт после идентификатора: операция присваивания или вызов функции
bool LL::AssignOrCallList(const string& scope, const string& p) {
    if (iter->first == "opassign") { //Эта функция проверяет, что идёт после идентификатора: операция присваивания или вызов функции
        setLexem(); //Устанавливает текущую лексему как обработанную
        nextState(0);
        addString("opassign E"); //Добавляет строку, описывающую операцию присваивания

        auto result = E(scope); //Вызывает функцию E, которая обрабатывает правую часть выражения (то, что присваивается)
        if (!result.first) return false; //Если обработка правой части не удалась, возвращает ошибку
        backStateIt(); //Возвращается

        auto r = checkVar(scope, p); //Проверяет существование переменной p в области видимости scope
        addAtom({ scope, "MOV", result.second, "", r }); //Добавляет атомарную инструкцию для операции присваивания (MOV), перемещая значение из правой части в переменную

        return true;
    }
    //Вызов функции
    else if (iter->first == "lpar") { //Если текущая лексема — левая скобка (, это вызов функции
        setLexem(); //Устанавливает текущую лексему как обработанную
        nextState(1); //след
        addString("lpar Arglist"); //Добавляет строку, указывающую на начало аргументов вызова функции

        auto result = Arglist(scope); //Вызывает функцию Arglist, которая обрабатывает список аргументов вызова функции
        if (!result.first) return false; //Если обработка аргументов не удалась, возвращает ошибку
        if (iter->first != "rpar") return false; //Если не найдена правая скобка ), завершает с ошибкой
        nextState(0);
        addString("rpar"); 
        setLexem();
        backStateIt();
        backStateIt();

        auto q = checkFunc(p, result.second); //Проверяет существование функции с именем p и совместимость типов аргументов
        auto r = alloc(scope); //Выделяет память для хранения результата вызова функции
        addAtom({ scope, "CALL", q, "", r }); //Добавляет атомарную инструкцию вызова функции (CALL)

        return true;
    }
    return false;
}//
//
//Функция обрабатывает конструкцию while
bool LL::WhileOp(const string& scope) {
    setLexem(); //Устанавливает текущую лексему как обработанную

    //Создает две метки и добавляет атомарную инструкцию для первой метки label1, которая указывает на начало цикла
    auto label1 = newLabel();
    auto label2 = newLabel();
    addAtom({ scope, "LBL", "", "", "$L" + label1 });

    //Проверяет наличие открывающей скобки ( и вызывает функцию для анализа выражения, которое должно быть в условии цикла
    if (iter->first != "lpar") return false;
    setLexem();
    nextState(1);
    addString("lpar E");

    //ызывает функцию E, которая анализирует условие while
    auto result = E(scope);
    if (!result.first) return false;

    addAtom({ scope, "EQ", result.second, "0", "$L" + label2 }); //Если условие ложно (равно 0), добавляет атомарную инструкцию перехода к метке label2, минуя тело цикла

    //Проверяет наличие закрывающей скобки ) и начинает разбор тела цикла
    if (iter->first != "rpar") return false;
    setLexem();
    nextState(0);
    addString("rpar Stmt");

    //Вызывает функцию для разбора тела цикла и проверяет её успех
    if (!Stmt(scope)) return false;
    backStateIt();

    //Добавляет инструкцию перехода обратно к началу цикла (label1) и инструкцию метки завершения цикла (label2)
    addAtom({ scope, "JMP", "", "", "$L" + label1 });
    addAtom({ scope, "LBL", "", "", "$L" + label2 });

    return true;
}
//
// Функция обрабатывает конструкцию for
bool LL::ForOp(const string& scope) {
    setLexem(); //Устанавливает текущую лексему как обработанную

    //Создает четыре метки, которые используются для начала цикла, завершения, тела цикла и итераций
    auto label1 = newLabel();
    auto label2 = newLabel();
    auto label3 = newLabel();
    auto label4 = newLabel();

    //Проверяет наличие открывающей скобки и вызывает функцию для разбора инициализации цикла
    if (iter->first != "lpar") return false;
    setLexem();
    nextState(1);
    addString("lpar ForInit");
    
    //Разбирает инициализацию цикла, и если всё успешно, проверяет наличие точки с запятой после нее
    if (!ForInit(scope)) return false;
    if (iter->first != "semicolon") return false;

    //Переходит к разбору условия в цикле и добавляет метку для начала проверки условия
    setLexem();
    backStateIt();
    nextState(1);
    addString("semicolon ForExp");
    addAtom({ scope, "LBL", "", "", "$L" + label1 });

    //Вызывает функцию для разбора условия и, если оно ложно, добавляет переход к завершению цикла
    auto result = ForExp(scope);
    if (!result.first) return false;
    addAtom({ scope, "EQ", result.second, "0", "$L" + label4 });

    //Добавляет инструкции для перехода к итерации и метку для тела цикла
    addAtom({ scope, "JMP", "", "", "$L" + label3 });
    addAtom({ scope, "LBL", "", "", "$L" + label2 });

    nextState(1);
    addString("semicolon ForLoop");

    //Разбирает итерацию цикла и добавляет переход обратно к началу
    if (!ForLoop(scope)) return false;
    addAtom({ scope, "JMP", "", "", "$L" + label1 });
    addAtom({ scope, "LBL", "", "", "$L" + label3 });

    //Переходит к разбору тела цикла и проверяет его успешность
    nextState(0);
    addString("rpar Stmt");
    if (!Stmt(scope)) { return false; }

    //Добавляет инструкции для возврата к телу цикла и завершения цикла
    addAtom({ scope, "JMP", "", "", "$L" + label2 });
    addAtom({ scope, "LBL", "", "", "$L" + label4 });

    backStateIt();
    return true;
}

//Функция отвечает за разбор инициализирующей части цикла for (например, присваивание переменной перед началом цикла)
bool LL::ForInit(const string& scope) {
    if (iter->first == "id") { //Проверяет, является ли текущая лексема идентификатором
        //Изменяет состояние добавляет строку, указывая, что ожидается либо присваивание, либо вызов функции
        nextState(0);
        addString("AssignOrCall");

        //Вызывает функцию AssignOrCall, которая разбирает, является ли эта конструкция присваиванием или вызовом функции
        if (!AssignOrCall(scope)) return false;
    }
    return true;
}

//Функция обрабатывает выражение, которое является условием цикла (например, i < 10)
pair<bool, string> LL::ForExp(const string& scope) {

    //Переменная fl используется для отслеживания ошибки разбора, а result хранит результат вызова функции E
    bool fl = false;
    pair<bool, string> result;
    //Проверяет, является ли текущая лексема выражением, которое может участвовать в условии цикла (скобка, идентификатор, число, символ или операция инкремента)
    if (iter->first == "lpar" or iter->first == "id" or iter->first == "num" or iter->first == "char" or iter->first == "opinc") {

        //Изменяет состояние и добавляет строку, указывающую на разбор выражения
        nextState(0);
        addString("E");

        //Вызывает функцию E, которая разбирает выражение, и если разбор неудачен, устанавливает флаг ошибки
        result = E(scope);
        fl = !result.first ? true : false;
    }
    //Проверяет, что текущая лексема — точка с запятой, которая завершает условие цикла
    if (iter->first == "semicolon") {

        //Устанавливает текущую лексему как обработанную и возвращает итератор на одну позицию назад
        setLexem();
        backStateIt();

        //Если условие было разобрано с ошибкой, возвращает значение "1" (означающее, что условие по умолчанию считается истинным). 
        //Если ошибок нет, возвращает результат разбора выражения
        if (fl) return { true, "1" };
        return { true, result.second };
    }

    //Если разбор не удался
    return { false, "" };
}

//Эта функция отвечает за разбор итерационной части цикла for (например, i++ или другой логики после тела цикла)
bool LL::ForLoop(const string& scope) {
    if (iter->first == "opinc") { //Проверяет, является ли текущая лексема операцией инкремента
        setLexem(); //Устанавливает текущую лексему как обработанную

        if (iter->first != "id") return false; //Если после инкремента не следует идентификатор

        //Сохраняет имя идентификатора и устанавливает текущую лексему как обработанную
        auto item = iter->second;
        setLexem();

        auto p = checkVar(scope, item); //Проверяет, существует ли переменная в текущей области видимости scope
        addAtom({ scope, "ADD", p, "1", p }); //Добавляет атомарную инструкцию для операции инкремента (увеличения переменной на 1)

        //Проверяет, что за инкрементом следует закрывающая скобка ), и если это так, обновляет состояние и добавляет строку
        if (iter->first != "rpar") return false;
        nextState(0);
        addString("opinc " + item);

        //Устанавливает текущую лексему как обработанную и возвращает итератор на две позиции назад.
        //Возвращает true, если разбор завершен успешно
        setLexem();
        backStateIt();
        backStateIt();
        return true;
    }
    //Обработка присваивания или вызова функции
    if (iter->first == "id") { //Если текущая лексема — идентификатор, это может быть присваивание или вызов функции

        //Изменяет состояние парсера и добавляет строку, указывающую на разбор присваивания или вызова функции
        nextState(0);
        addString("AssignOrCall");

        //Изменяет состояние парсера и добавляет строку, указывающую на разбор присваивания или вызова функции
        if (!AssignOrCall(scope)) return false;

        //Проверяет наличие закрывающей скобки ) после присваивания или вызова функции и устанавливает текущую лексему как обработанную
        if (iter->first != "rpar") return false;
        setLexem();
        backStateIt();

        return true;
    }

    //Завершение разбора итерации

    //Если текущая лексема — закрывающая скобка ), завершает разбор успешно. Если это не так, возвращает ошибку
    if (iter->first != "rpar") return false;
    setLexem();
    backStateIt();
    return true;
}
//
// отвечает за разбор условной конструкции if и, возможно, связанной с ней конструкции else
bool LL::IfOp(const string& scope) {
    setLexem(); //Устанавливает текущую лексему как обработанную
    if (iter->first != "lpar") return false; //Проверяет, что текущая лексема — это открывающая круглая скобка (. Если это не так, возвращает false

    //Устанавливает текущую лексему как обработанную, изменяет состояние и добавляет строку, описывающую разбор выражения (условия) в скобках
    setLexem();
    nextState(1);
    addString("lpar E");

    //Вызывает функцию E, которая разбирает выражение внутри скобок (условие if). Если разбор не удается, функция возвращает false
    auto result = E(scope);
    if (!result.first) return false;

    //Проверяет, есть ли после выражения закрывающая скобка ). Если ее нет, возвращает ошибку
    if (iter->first != "rpar") return false;

    setLexem(); //Устанавливает текущую лексему (закрывающую скобку) как обработанную

    //Создает новую метку и добавляет атомарную инструкцию для сравнения результата выражения с 0. 
    //Если условие ложно (результат выражения равен нулю), будет переход на метку label1
    auto label1 = newLabel();
    addAtom({ scope, "EQ", result.second, "0", "$L" + label1 });

    //Изменяет состояние парсера и добавляет строку для разбора оператора, который следует за условием if.
    //Вызывает функцию Stmt, которая разбирает этот оператор. Если разбор не удается, возвращает false
    nextState(1);
    addString("rpar Stmt");
    if (!Stmt(scope)) return false;

    //Создает новую метку для выхода из конструкции if. Добавляет инструкцию для безусловного перехода (JMP) на метку label2,
    //которая используется для пропуска части кода, если условие выполнено. 
    //Добавляет метку label1 для перехода, если условие не выполнено
    auto label2 = newLabel();
    addAtom({ scope, "JMP", "", "", "$L" + label2 });
    addAtom({ scope, "LBL", "", "", "$L" + label1 });


    //Изменяет состояние парсера и добавляет строку для возможной части else. Если следующая лексема — это ключевое слово else, 
    //вызывает функцию ElsePart, которая разбирает оператор, связанный с else. 
    //После этого добавляет метку label2, которая указывает на завершение конструкции else
    nextState(0);
    addString("ElsePart");
    if (iter->first == "kwelse") {
        if (!ElsePart(scope)) return false;

        addAtom({ scope, "LBL", "", "", "$L" + label2 });

        backStateIt();
        return true;
    }
    addAtom({ scope, "LBL", "", "", "$L" + label2 }); //Добавляет метку label2 для завершения конструкции if - else, если else отсутствует

    //Возвращает состояние парсера на две позиции назад и завершает функцию успешно, если разбор конструкции if был успешным
    backStateIt();
    backStateIt();
    return true;
}

//Функция ElsePart разбирает часть программы, которая выполняется при выполнении конструкции else
bool LL::ElsePart(const string& scope) {
    //Изменяет состояние парсера и добавляет строку для части else, указывая на разбор оператора, связанного с else
    nextState(0);
    addString("kwelse Stmt");

    //Устанавливает текущую лексему как обработанную и вызывает функцию Stmt, 
    //которая разбирает оператор, связанный с else. Если разбор не удается, возвращает false
    setLexem();
    if (!Stmt(scope)) return false;

    //Возвращает состояние парсера на одну позицию назад и завершает функцию успешно, если разбор части else прошел успешно
    backStateIt();
    return true;
}
//
//разбирает конструкцию switch
bool LL::SwitchOp(const string& scope) {

    //Устанавливает текущую лексему как обработанную и проверяет наличие открывающей скобки ( для выражения switch.
    //Если скобки нет, возвращает ошибку
    setLexem();
    if (iter->first != "lpar") return false;

    //Устанавливает текущую лексему как обработанную, изменяет состояние и добавляет строку, описывающую разбор выражения (условия) в скобках
    setLexem();
    nextState(1);
    addString("lpar E");

    //Вызывает функцию E для разбора выражения внутри скобок. Если разбор не удается, возвращает ошибку
    auto result = E(scope);
    if (!result.first) return false;

    //Проверяет наличие закрывающей скобки ). Если скобки нет, возвращает ошибку. Иначе устанавливает текущую лексему как обработанную
    if (iter->first != "rpar") return false;
    setLexem();


    //Проверяет наличие открывающей фигурной скобки {, которая открывает блок switch. Если ее нет, возвращает ошибку. 
    //Иначе устанавливает текущую лексему как обработанную и добавляет строку для разбора блоков case
    if (iter->first != "lbrace") return false;
    setLexem();
    nextState(1);
    addString("lpar lbrace Cases");


    //Создает метку для завершения конструкции switch и вызывает функцию Cases, которая разбирает блоки case. Если разбор не удался, возвращает ошибку
    auto end = newLabel();
    if (!Cases(scope, result.second, end)) return false;


    //Проверяет наличие закрывающей фигурной скобки }, которая завершает блок switch. Если ее нет, возвращает ошибку. Иначе завершает разбор switch
    if (iter->first != "rbrace") return false;
    setLexem();
    nextState(0);
    addString("rbrace");
    backStateIt();
    backStateIt();

    addAtom({ scope, "LBL", "", "", "$L" + end }); //Добавляет метку для завершения конструкции switch
    return true;
}

//Функция Cases разбирает список блоков case и блок default внутри конструкции switch
bool LL::Cases(const string& scope, const string& p, const string& end) {

    //Изменяет состояние парсера и добавляет строку, обозначающую разбор одного блока case
    nextState(1);
    addString("Acase");

    //Вызывает функцию ACase, которая обрабатывает отдельный блок case или default. Если разбор не удался, возвращает ошибку
    auto result = ACase(scope, p, end);
    if (!result.first) return false;

    //изменяет состояние, добавляет строку для списка блоков case, вызывает функцию CasesList для дальнейшего разбора.
    //Если разбор успешен, возвращает true, иначе false
    nextState(0);
    addString("CasesList");
    if (!CasesList(scope, p, end, result.second)) return false;
    backStateIt();
    return true;
}

//разбирает последовательность блоков case и проверяет наличие конфликта между ними
bool LL::CasesList(const string& scope, const string& p, const string& end, const string& def) {

    //Проверяет, является ли текущая лексема ключевым словом case или default. 
    //Если да, изменяет состояние и добавляет строку для разбора блока case
    if (iter->first == "kwdefault" or iter->first == "kwcase") {
        nextState(1);
        addString("Acase");

        //Вызывает функцию ACase для разбора текущего блока case. Если разбор не удается, возвращает ошибку.
        auto result = ACase(scope, p, end);
        if (!result.first) return false;

        //Проверяет, есть ли конфликт между блоками case. Если два блока имеют одинаковые значения, добавляется атомарная инструкция, указывающая на ошибку
        if (stoi(def) >= 0 and stoi(result.second) >= 0) {
            addAtom({ "$Error", "$Error", "$Error", "$Error", "$Error" });
        }
        auto def2 = stoi(def) < stoi(result.second) ? result.second : def; //Обновляет переменную def2, чтобы сохранить индекс последнего блока case

        //Изменяет состояние и вызывает саму себя для разбора следующего блока case. Если разбор успешен, возвращает true
        nextState(0);
        addString("CasesList");
        if (!CasesList(scope, p, end, def2)) return false;
        backStateIt();
        return true;

    }
    //Выводит значение переменной def и метки завершения. Добавляет атомарную инструкцию для перехода к метке q, которая будет либо def, либо end
    cout << def << " " << end << endl;
    string q = stoi(def) >= 0 ? def : end;
    addAtom({ scope, "JMP", "", "", "$L" + q });

    //Возвращает состояние назад и завершает функцию
    backStateIt();
    return true;
}

//разбирает один блок case или default
pair<bool, string> LL::ACase(const string& scope, const string& p, const string& end) {

    //Если текущая лексема — это case, устанавливает ее как обработанную
    if (iter->first == "kwcase") {
        setLexem();

        //Проверяет, что после case идет число или символ, и сохраняет его
        if (iter->first != "num" and iter->first != "char")return { false, "" };
        auto item = iter->second;

        //Устанавливает текущую лексему как обработанную и создает новую метку. 
        //Добавляет инструкцию для сравнения значения с переменной p, и если они не равны, происходит переход на метку next
        setLexem();
        string next = newLabel();
        addAtom({ scope, "NE", p, item, "$L" + next });

        //Проверяет наличие двоеточия после case. Если оно отсутствует, возвращает ошибку. 
        //Изменяет состояние и вызывает функцию StmtList, которая обрабатывает операторы внутри блока case
        if (iter->first != "colon") return { false, "" };
        nextState(0);
        addString("kwcase " + item + " colon StmtList");
        setLexem();
        if (!StmtList(scope)) return { false, "" };

        //Добавляет инструкцию для безусловного перехода к метке завершения блока switch (end). 
        //Также добавляет метку next и возвращает успешный результат
        addAtom({ scope, "JMP", "", "", "$L" + end });
        addAtom({ scope, "LBL", "", "", "$L" + next });
        backStateIt();
        return { true, "-1" };

    }
    //Обрабатывает ключевое слово default, проверяет наличие двоеточия после него и вызывает разбор операторов с помощью StmtList
    else if (iter->first == "kwdefault") {
        setLexem();

        if (iter->first != "colon") return { false, "" };
        setLexem();
        nextState(0);
        addString("kwdefault colon StmtList");

        //Создает метки для завершения блока default и добавляет атомарные инструкции для переходов
        string next = newLabel();
        string def = newLabel();
        addAtom({ scope, "JMP", "", "", "$L" + next });
        addAtom({ scope, "LBL", "", "", "$L" + def });

        //Вызывает StmtList для обработки операторов внутри блока default. 
        //Затем добавляет переход к метке завершения и возвращает результат с меткой def
        if (!StmtList(scope)) return { false, "" };
        backStateIt();
        addAtom({ scope, "JMP", "", "", "$L" + end });
        addAtom({ scope, "LBL", "", "", "$L" + next });
        return { true, def };
    }
    return { false, "" };
}
//
//обрабатывает ввод и вывод данных
bool LL::IOp(const string& scope) {

    //Устанавливает текущую лексему как обработанную и проверяет наличие идентификатора.Сохраняет значение переменной
    setLexem();
    if (iter->first != "id") return false;
    auto item = iter->second;
    setLexem();

    //Проверяет наличие точки с запятой после переменной. Если ее нет, возвращает ошибку
    if (iter->first != "semicolon") return false;
    setLexem();

    //Добавляет строку с информацией о разборе и возвращает состояние парсера на две позиции назад
    nextState(0);
    addString(item + "semicolon");
    backStateIt();
    backStateIt();

    //Проверяет существование переменной в области видимости scope и добавляет атомарную инструкцию для операции ввода данных в переменную p
    auto p = checkVar(scope, item);
    addAtom({ scope, "IN", "", "", p });

    return true;
}

//функция разбирает оператор вывода
bool LL::OOp(const string& scope) {
    setLexem(); ////Устанавливает текущую лексему как обработанную

    nextState(1);
    addString("OOp'"); //Добавляет строку с информацией о том, что начинается разбор оператора вывода (OOp')
    if (!OOpList(scope)) return false; //Вызывает функцию OOpList, которая разбирает конкретные элементы в списке вывода
    if (iter->first != "semicolon") return false; //Проверяет, что после оператора вывода стоит точка с запятой (semicolon), что является синтаксическим требованием
    setLexem();
    nextState(0);
    addString("semicolon"); //Добавляет строку с информацией о том, что была успешно разобрана точка с запятой

    //Откатывает состояние на два шага назад, чтобы вернуться к тому моменту, с которого начинался разбор
    backStateIt();
    backStateIt();
    return true;
}
//Эта функция разбирает список элементов вывода
bool LL::OOpList(const string& scope) {
    if (iter->first == "str") { //Проверяет, является ли текущая лексема строковым литералом
        auto item = iter->second; //Сохраняет строковое значение текущей лексемы

        //Меняет состояние парсера на начальное и добавляет строку отладки с выводимой строкой
        nextState(0);
        addString("\"" + item + "\"");

        setLexem(); // Устанавливает текущую лексему как обработанную

        //Откатывает состояние парсера на два шага назад
        backStateIt();
        backStateIt();

        addAtom({ scope, "OUT", "", "", "\"" + item + "\"" }); //Генерирует атомарную инструкцию для вывода строки. Атомарная инструкция OUT означает, что строк
        return true;
    }
    
    //Если текущая лексема не строка
    nextState(0);
    addString("E");
    auto result = E(scope); 
    if (!result.first) return false; //Если разбор выражения завершился неуспешно, функция возвращает false
    backStateIt(); // Возвращение к предыдущему состоянию

    addAtom({ scope, "OUT", "", "", result.second }); //Создается атомарная команда для вывода результата выражения (result.second — результат вычисленного выражения)
    return true;
}