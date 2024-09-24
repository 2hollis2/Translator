#include "Translator.h"

bool LL::StmtList(const string& scope) { // �������� ������� ��� ��������� ������� ���������� ��������� �, ��� �������� ���������, ��������� �� � ������
    if (*iter == LEX_EOF) return true; //���� ������� ������� �������� ������ �����, ����������� ��������� ����������

    nextState(1);
    addString("Stmt"); //��������� ������� ��������� � ��������� ������ "Stmt"
    auto listIt = iter;

    if (!Stmt(scope)) { //���������� ������� Stmt(scope), ������� ���������, �������� �� ������� ������� ������� ���������
        output.pop_back();
        backStateIt(); //��������� ����������� �������, � ��������� ������������ �� ��� ���� �����
        backStateIt();
        return true;
    }
    //���� �������� ������� ���������, ����������� ��������� � ����������� ������ "StmtList"
    nextState(0);
    addString("StmtList");

    if (!StmtList(scope)) return false; //���������� ���������� ���� ������� ��� ��������� ���������� ���������. ���� ���������� ��������� ���, ������������ false

    //���������� ��������� ����� � ��������� ���������
    backStateIt();
    return true;
}

bool LL::Stmt(const string& scope) {
    if (*iter == LEX_EOF) return false; //���� ������� ������� �������� ������ �����, ������� ���������� false

    if (iter->first == "kwint" or iter->first == "kwchar") { //�����������, �������� �� ������� ������� �������� ������ ���������� ����������
        nextState(0);
        addString("DeclareStmt"); 
                                    //���������� ������� ��� ��������� ���������� ���������� DeclareStmt(scope)
        if (!DeclareStmt(scope)) return false;
        backStateIt();
        return true;
    }
    //���� ������� ������� � ����� � ������� (semicolon), ��� ������ ��������������� ��� ������� (�����������), � ��������� ������� �����������
    if (iter->first == "semicolon") {
        setLexem();
        nextState(0);
        addString("semicolon");
        backStateIt();
        backStateIt();
        return true;
    }

    if (scope == "-1") return false; //���� ������� ��������� (scope) ����� �������� -1, ������� ��������� ���������, ��������� false

    //���� ������� ������� � ������������� (id), ��������������, ��� ��� ���� ������������, ���� ����� �������. ���������� ��������������� ������� AssignOrCallOp(scope)
    if (iter->first == "id") {
        nextState(0);
        addString("AssignOrCallOp");

        if (!AssignOrCallOp(scope)) return false;

        backStateIt();
        return true;
    }
    //��������� ����� while. ���� ������� ������� � �������� ����� while, ���������� ������� ��� ��������� ����� WhileOp(scope)
    if (iter->first == "kwwhile") {
        nextState(0);
        addString("kwwhile WhileOp");

        if (!WhileOp(scope)) return false;
        backStateIt();
        return true;
    }
    //��������� ����� while. ���� ������� ������� � �������� ����� while, ���������� ������� ��� ��������� ����� WhileOp(scope)
    if (iter->first == "kwfor") {
        nextState(0);
        addString("kwfor ForOp");
        if (!ForOp(scope)) return false;
        backStateIt();
        return true;
    }
    //��������� ��������� ��������� if. ���� ������� ������� � if, ���������� ������� ��� ��������� ������� IfOp(scope)
    if (iter->first == "kwif") {
        nextState(0);
        addString("kwif IfOp");
        if (!IfOp(scope)) return false;
        backStateIt();
        return true;
    }
    //��������� ��������� switch. ���� ������� ������� � �������� ����� switch, ���������� ������� ��� ��������� SwitchOp(scope)
    if (iter->first == "kwswitch") {
        nextState(0);
        addString("kwswitch SwitchOp");
        if (!SwitchOp(scope)) return false;
        backStateIt();
        return true;
    }
    //��������� ��������� ����� in
    if (iter->first == "kwin") {
        nextState(0);
        addString("kwin IOp");
        if (!IOp(scope)) return false;
        backStateIt();
        return true;
    }
    //��������� ��������� ������ out
    if (iter->first == "kwout") {
        nextState(0);
        addString("kwout OOp");
        if (!OOp(scope)) return false;
        backStateIt();
        return true;
    }
    //��������� ������ ����, ����������� � �������� ������ {}. 
    //����� �������� ������ �������������� ������ ����������, � ����� ����������� ������� ����������� ������
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
    //��������� ��������� return. ����� ��������� ����� return ��������� ��������� E(scope).
    //���� ��������� ���������, ��� ����������� � ��������� � ����� RET
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

    return false; //���� �� ���� �� ������� �� �������, ���������� false, ����������, ��� ������� ������� �� �������� ���������� ����������
}
//
//Declare block
////������� ���������, �������� �� ������� ������� ����� ������, ����� ��� int ��� char
pair<bool, string> LL::Type(const string& scope) {
    if (*iter == LEX_EOF) return { false, "" }; //���� ������� ������� � ����� ����� ���������� false � ������ ������, �������� �� ��, ��� ��� ������ �� ������

    if (iter->first == "kwint" or iter->first == "kwchar") { //���� ������� ������� �������� �������� ������ ��� ���� ������ int ��� char
        auto item = iter->first; //item ������������� �������� ������� ������� (��� ������)

        nextState(0); //��������� ����������� ���������� � ������� nextState(0) ��� ���������� ���������
        addString(iter->first); //������� ����������� � ������� addString(iter->first)
        setLexem(); // �����������
        backStateIt(); //���������� ��������� ����� ������ � ������� backStateIt()
        backStateIt();
        return { true, item }; //������� ���������� ���� ��������: true (��������, ��� ��� ������ ��� ������) � item (��� ������)
    }
    return { false, "" }; //���� ������� ������� �� �������� ����� ������, ������� ���������� ���� ��������: false � ������ ������, �������� �� ��, ��� ��� ������ �� ��� ������
}

//���������, �������� �� ������� ����������� ������������� ���������� ����������� ����������
bool LL::DeclareStmt(const string& scope) {

    //������� �������� � ��������� ��������� ����������� � ������� nextState(1) ��� ���������� �������� ��������� � ��������� ������ "Type"
    nextState(1);
    addString("Type");

    auto result = Type(scope); //����� ���������� ������� Type(scope), ������� ���������, ���������� �� ���������� � ����������� ���� ������
    if (!result.first) { return false; } //���� ��� ������ �� ��� ������, ������� ��������� ������ � ��������� false, ��� �������� ������� � ��������� ����������

    if (iter->first != "id") return false; //����� ��������� ����������� ���� ������ �����������, ������� �� �� ��� ������������� 
    auto item = iter->second; //���� �� ��� ���������������� ���������� false. � ���������� item ����������� ��� ������, ������� ��� ������

    nextState(0);
    addString(" " + item + " DeclareStmt'"); //����������� ������, ���������� ��� ������ � ����� "DeclareStmt'"
    setLexem();

    if (!DeclareStmtList(scope, result.second, item)) return false; //���������� ������� DeclareStmtList, ������� ������������ ������ ���������� � ����������

    //���� ��� ������ �������, ��������� ����������� ������������ �� ��� ����� � ������� backStateIt(),
    //� ������� ���������� true, ��������, ��� ���������� ���������� ������� ����������
    backStateIt();
    return true;

}

//������������ ���������� ������� � ����������
//��� ������� ���������� �������, ���������, ��� ���������� ���������� � ���������� ��������� �������
//������������ ������������ �������� ����������
//������� ���������� �� ������ ��������������� ������� ��� ����������� �������������
bool LL::DeclareStmtList(const string& scope, const string& p, const string& q) { // p = type, q = name
    if (*iter == LEX_EOF) return false; //���� ������� ������� � ����� ����� false
    if (iter->first == "lpar") { //���� ������� ������� � ����������� ������� ������ lpar, ��� ��������� �� ��, ��� ���������� ���������� �������
        if (scope != "-1") return false; //�����������, ��� ������� ������� ��������� � ����������, ��� ��� ������� �� ����� ���� ��������� ������ ������ �������
        setLexem(); //������� lpar ��������������� 
        nextState(1); //��������� � ������� nextState(1) ��� ���������� �������� ���������
        addString("lpar ParamList"); //����������� ������ "lpar ParamList", ����� ������� ������ ������ ���������� �������

        string codeFunc = addFunc(q, p); //��������� ���������� ����� ������� � ������ q � ����� ������������� �������� p. ������������ ���������� ��� �������

        auto result = ParamList(codeFunc); //���������� ������� ParamList(codeFunc) ��� ������� ������ ���������� �������
        if (!result.first) return false; //���� ������ ���������� �� ����� ���� ���������, ������������ false


        table[stoi(codeFunc) - 1].len = result.second; //� ������� �������� ��� ������� ������� ����������� ���������� � ����� (���������� ���������� �������)

        if (iter->first != "rpar") return false; //����������� ������� ����������� ������ rpar
        setLexem(); //�����������
        if (iter->first != "lbrace") return false; //����������� �������� ������ lbrace, ��� ��������� �� ������ ���� �������
        setLexem(); //�����������
        nextState(1);
        addString("rpar lbrace StmtList"); //����������� ������ "rpar lbrace StmtList", ����������� �� ����� ������ ���������� � ������ ���� �������

        addAtom({ codeFunc, "LBL", "", "", q }); //��������� ��������� ���������� � ������ ������� LBL
        if (!StmtList(codeFunc)) return false; //���������� ������� StmtList(codeFunc) ��� ��������� ������ ���������� ������ ���� �������
        if (iter->first != "rbrace") return false; //����������� ������� ����������� �������� ������ rbrace, ������� ��������� ���� �������
        setLexem();
        nextState(0);
        addString("rbrace");
        backStateIt(); //������������ ������ ����� � ������� backStateIt()
        backStateIt();
        
        //�������� �������� ��� ��������� ����������
        
        //����� ������������ ���������� �������� ��� ��������� ���������� ������ �������. ���������� ������ ������� ����������� � ������������ ������� ������, 
        //� ���� ���� ������������� �������� ��� ������ ����������. counter �������� �� ������� ��������, � total_cnt � �� ���������� ����������
        int counter = 0;
        int total_cnt = 0;
        for (int i = table.size() - 1; i > stoi(codeFunc) - 1 + stoi(result.second); i--) {
            if (table[i].scope == codeFunc) {
                table[i].offset = to_string(counter);
                counter += 2;
                total_cnt++;
            }
        }
        //�������� ����� ��������������� ��� ���� ��������� ���������� ������ �������. 
        //����� ���������� ����������� � ������� �������� ��� �������
        counter += 2;

        for (int i = stoi(codeFunc) + stoi(result.second) - 1; i > stoi(codeFunc) - 1; i--) {
            table[i].offset = to_string(counter);
            counter += 2;
            total_cnt++;
        }
        table[stoi(codeFunc) - 1].offset = to_string(total_cnt); //��������������� ����� ���������� ��������� ���������� ��� �������
        //
        //� ����� ���� ������� ��������� ��������� ���������� �������� RET, ����� ������� ����� ����������� ���������
        addAtom({ codeFunc, "RET", "", "", "0" });
        return true;
    }
    //��������� ������������ ����������
    else if (iter->first == "opassign") { //���� ������� ������� � �������� ������������ (opassign), ��� �����������
        setLexem();
        auto item = iter->second; //���������� item ��������� ��������, ������� ����� ��������� ����������

        if (iter->first == "num") { //���� ��������� �������� � ����� (num)
            string var = addVar(q, scope, p, item); //���������� ������� addVar(q, scope, p, item) ��� ���������� ����������
                                                   //q � ����� p � ������� ������� ��������� scope, � �� ������������� �������� item

            if (var == "'$Error'") return false; //���� ���������� �� ����� ���� ���������, ������� ���������� ������

            nextState(1);
            addString("opassign " + item + " DeclareVarList"); // ����������� ������ � ���������� ������������ � ����������
            setLexem();

            addAtom({ scope, "MOV", item, "", var }); //��������� ��������� ���������� MOV, ������� ��������� �������� ������������

            //���������� ������� DeclareVarList(scope, p) ��� ��������� ������ ����������
            //���� ��� ���������� ������ ��� �� ������ ������ semicolon ��� ���������� ��������, ��������� ����������� � �������
            if (!DeclareVarList(scope, p)) return false;
            if (iter->first != "semicolon") return false;
            setLexem();
            nextState(0);
            addString("semicolon");
            backStateIt();
            backStateIt();
            return true;
        }
        //����� ������ ���������� ����������
        else if (iter->first == "char") { //���� ������� ������� ������������� ���� char
            string var = addVar(q, scope, p, item); //����� ������� addVar(q, scope, p, item) ��������� ���������� � ������ q, � ������� ��������� scope � ���� p(� ������ ������ � char), ���������� �� �������� item
            if (var == "'$Error'") return false;

            nextState(1);
            addString("opassign " + item + " DeclareVarList"); //��������� ������ "opassign " + item + " DeclareVarList", ����� �������������, ��� ��������� ������������ �������� item ���������� � ����� ��������� ������ ����������
            setLexem(); //���������
            addAtom({ scope, "MOV", item, "", var }); //������� ��������� ���������� ��� ������������ ��������. ���������� MOV ��������� �������� item � ���������� var, ������� ���� ��������� �����
            if (!DeclareVarList(scope, p)) return false; //���������� ������� DeclareVarList, ������� ������������ ���������� ���������� ����������
            if (iter->first != "semicolon") return false; //��������� ������� ����� � ������� semicolon. ���� ����� ������������ �� ����� ����� � �������, ������������ ������
            setLexem();
            nextState(0);
            addString("semicolon"); //��������� ������ "semicolon"
            backStateIt(); //����������
            backStateIt();
            return true;
        }
        else return false;
    }
    //����� ������ ���������� ����������
    
    //���� ������� �� �������� ���������� ������������, ��� ������� ���������� ����������
    else {
        string var = addVar(q, scope, p); //� ������ �������� ���������� ���������� ��� ����������� � ������� ������� ���������
        if (var == "$Error") return false;

        nextState(1);

        //���������� ������� DeclareVarList(scope, p) ��� ��������� ������ ����������
        addString("DeclareVarList");
        if (!DeclareVarList(scope, p)) return false;

        //����������� ������� ����� � ������� semicolon
        if (iter->first != "semicolon") return false;
        setLexem();
        nextState(0);
        addString("semicolon"); //��������� ������ "semicolon"
        backStateIt(); //����������
        backStateIt();
        return true;
    }
}

//������� ������������ ������ ���������� ����������, ������� ��������� ��������.  ��� ����� ���� ������ �������� ����������, ���� ���������� � ������������
bool LL::DeclareVarList(const string& scope, const string& p) {
    if (*iter == LEX_EOF) return false; //�������� �� ����� �����

    if (iter->first == "comma") { //���� ������� ������� � ��� �������, ������, ����� ��� ������ ��������� ���������� ��������� ����������
        setLexem(); //������������� ������� ������� ��� ������������

        if (iter->first != "id") return false; //���� ����� ������� �� ���� ������������� ���������� (id), ���������� ������
        auto item = iter->second; //��������� ��� ���������� (�������������) � ���������� item

        nextState(1); //��������� � ��������� ��������� 
        addString("comma " + item + " InitVar"); //��������� ������ "comma " + item + " InitVar", ����� �������������, ��� ����� ������� ��� ���������� � � �������������
        setLexem(); //������������� ������� ������� ��� ������������
        if (!InitVar(scope, p, item)) return false; //�������� ������� InitVar, ������� ����� ���������������� ����������. ���� ������������� �� �������, ���������� false
        nextState(0); //������������ � ��������� ���������
        addString("DeclareVarList"); //��������� ������, ����������� �� ������ ������ ������ ����������
        if (!DeclareVarList(scope, p)) return false; //���������� �������� DeclareVarList, ����� ���������� ��������� ���������� � ������
        backStateIt(); //������������
        return true;
    }
    backStateIt();
    return true;
}

//������� ������������ ������������� ����������.  ��� ����� ���� ������ �������� ����������, ���� ���������� � ������������
bool LL::InitVar(const string& scope, const string& p, const string& q) {
    if (*iter == LEX_EOF) return false; //�������� �� ����� �����

    if (iter->first == "opassign") { //���� ������� ������� � �������� ������������ opassign, �� ���������� ������������� ��������
        setLexem(); //������������� ������� ������� ��� ������������

        if (iter->first != "num" and iter->first != "char") return false; //���������, ��� ����� ��������� ������������ ������ ��������� ���� �����, ���� ������
        string var = addVar(q, scope, p, iter->second); //��������� ���������� q � ������� ��������� scope � ����� p � ���������, ������� ���������� � ������� iter->second. ���� �������� ���������� �� �������, ���������� ������ "$Error"
        if (var == "'$Error'") return false; //���� �������� ������ ��� ���������� ����������, ������� ���������� false

        addAtom({ scope, "MOV", iter->second, "", var }); //������� ��������� ���������� ��� ������������ �������� ����������. 
                                                         //���������� MOV ��������� �� ����������� �������� �� ������ ����� � ���������� var

        nextState(0); //���������� ���������
        addString("opassign " + iter->second); //��������� ������, ����������� �� �������� ������������ � ��������, ������� ��������� ����������
        setLexem(); //������������� ������� ������� ��� ������������
        backStateIt(); //�������
        backStateIt();
        return true;
    }
    //���� ���������� �� ������������� �������� �����, ���������� addVar ��� ������ ���������� ���������� ��� �������������
    string var = addVar(q, scope, p);
    if (var == "'$Error'") return false;

    backStateIt();
    return true;
}

//������� ������������ ������ ���������� �������. ��� ����������� ���� ���������� � �� �����, �������� ��������������� ������ � ������� ��������
pair<bool, string> LL::ParamList(const string& scope) {
    if (*iter == LEX_EOF) return { false, "" }; //�������� �� ����� �����

    if (iter->first == "kwint" or iter->first == "kwchar") { //���� ������� ������� � �������� ����� ��� ���� ������, ��� ��������� �� ������ ���������
        nextState(1); //������� � ����� ���������
        addString("Type"); //��������� ������ "Type" ��� �������� ���� ������ ���������
        auto type = Type(scope); //�������� ������� Type, ������� ����������� � ���������� ��� ���������
        if (iter->first != "id") return { false, "" }; // ���� ����� ���� ��������� �� ���� �������������, ���������� ������
        string var = addVar(iter->second, scope, type.second); //��������� �������� � ������� ��������. iter->second � ��� ��� ���������, � type.second � ��� ���
        if (var == "'$Error'") return { false, "" }; //���� �������� ������ ��� ���������� ���������, ���������� ������

        nextState(0); //��������� ���������
        addString(" " + iter->second + " ParamList'"); //��������� ������, ����������� �� ��� ��������� � ������ ������ ������ ����������
        setLexem(); //������������� ������� ������� ��� ������������

        auto result = ParamListList(scope); //�������� ������� ParamListList, ������� ������������ ��������� �������� � ������. ���������� ���� ��������: ���������� � ���������� ����������
        if (!result.first) return { false, "" }; //���� ��������� ���������� ��������� �� �������, ���������� ������
        backStateIt(); //������� 
        return { true, to_string(stoi(result.second) + 1) }; //���������� ���� ��������: ���������� ��������� � ���������� ����������, ����������� �� 1
    }
    // ���� �� ������� ����� ���������, ���������� ���������� � 0 ����������
    backStateIt();
    return { true, "0" };
}

//������� ���������� ������������ ������ ���������� �������, ������� ��������� ��������
pair<bool, string> LL::ParamListList(const string& scope) {
    if (*iter == LEX_EOF) return { false, "" }; //�������� �� ����� �����

    if (iter->first == "comma") { //���� ������� ������� � �������, ��� ��������, ��� ��������� �������� ������ ���� ����� ��
        setLexem(); //������������� ������� ������� ��� ������������
        if (iter->first != "kwint" and iter->first != "kwchar") { //��������, ��� ����� ������� ������ ���� ��� ���������
            backStateIt(); //���� ��� �� ���, ������������ �� ��� ����� � ���������� ���� {true, "0"}, �������� �� ��, ��� ��������� �����������
            return { true, "0" };
        }
        nextState(1); 
        addString("comma Type"); //��������� ������, ����������� �� ������� � ��� ���������� ���������

        auto type = Type(scope); //�������� ������� Type ��� ������� ���� ���������
        if (iter->first != "id") return { false, "" }; //����� ���� ������ ���� �������������. ���� ��� �� ���, ���������� ������
        string var = addVar(iter->second, scope, type.second); //��������� �������� � ������� ��������� scope � ��������� �����
        if (var == "'Error'") return { false, "" }; //���� �� ������� �������� ����������, ���������� ������

        nextState(0); 
        addString(" " + iter->second + " ParamList'"); //��������� ������, ����������� �� ���������� ��������� � ����������� ������ ����������
        setLexem(); //������������� ������� ������� ��� ������������

        auto result = ParamListList(scope); //���������� �������� ���� ���� ��� ��������� ���������� ���������
        if (!result.first) return { false, "" }; // ���� ��������� ���������� ��������� �� �������, ���������� ������
        backStateIt(); //������������ �� ��� �����
        return { true, to_string(stoi(result.second) + 1) }; //���������� ����� � ���������� ����������, ����������� �� 1
    }
    //���� �� ���� ������� ������� ���������� ����� �������, ���������� ���������� � 0 ����������
    backStateIt();
    return { true, "0" };
}

//
//������� ������������ ���������, ������� ����� ���� ���� ��������� ������������, ���� ������� �������. ����������� ������ � �������
bool LL::AssignOrCallOp(const string& scope) {
    nextState(1);
    addString("AssignOrCall"); //��������� ������ � ��������� ��������: ��� ���� ������������, ���� ����� �������
    if (!AssignOrCall(scope)) return false; //�������� ������� AssignOrCall, ������� ����������, �������� �� ������� ��������� ������������� ��� ������� �������
    if (iter->first != "semicolon") return false; //����� ��������� ��������� ����� � �������. ���� � ���, ���������� ������
    nextState(0);
    addString("semicolon"); //��������� ������, ����������� �� ���������� ��������� ������ � �������
    setLexem(); //������������� ������� ������� ��� ������������

    //������������ �� ��� ���� �����
    backStateIt();
    backStateIt();
    return true;
}


//������� ����������, �������� �� ������� ��������� ������������� ���������� ��� ������� �������
bool LL::AssignOrCall(const string& scope) {
    if (iter->first != "id") return false; //���� ������� ������� � �� �������������
    auto item = iter->second; //��������� ������������� � ���������� item

    nextState(0);
    addString(item + " AssignOrCall'"); //��������� ������, ����������� �������� (������������ ��� ����� �������) � ������ ���������� ��� �������
    setLexem(); //������������� ������� ������� ��� ������������
    if (!AssignOrCallList(scope, item)) return false; //�������� ������� AssignOrCallList, ������� ���������, ��� �� ��������� ������� ������ (������������ ��� ����� �������)
    backStateIt(); //������������
    return true;
}

//��� ������� ���������, ��� ��� ����� ��������������: �������� ������������ ��� ����� �������
bool LL::AssignOrCallList(const string& scope, const string& p) {
    if (iter->first == "opassign") { //��� ������� ���������, ��� ��� ����� ��������������: �������� ������������ ��� ����� �������
        setLexem(); //������������� ������� ������� ��� ������������
        nextState(0);
        addString("opassign E"); //��������� ������, ����������� �������� ������������

        auto result = E(scope); //�������� ������� E, ������� ������������ ������ ����� ��������� (��, ��� �������������)
        if (!result.first) return false; //���� ��������� ������ ����� �� �������, ���������� ������
        backStateIt(); //������������

        auto r = checkVar(scope, p); //��������� ������������� ���������� p � ������� ��������� scope
        addAtom({ scope, "MOV", result.second, "", r }); //��������� ��������� ���������� ��� �������� ������������ (MOV), ��������� �������� �� ������ ����� � ����������

        return true;
    }
    //����� �������
    else if (iter->first == "lpar") { //���� ������� ������� � ����� ������ (, ��� ����� �������
        setLexem(); //������������� ������� ������� ��� ������������
        nextState(1); //����
        addString("lpar Arglist"); //��������� ������, ����������� �� ������ ���������� ������ �������

        auto result = Arglist(scope); //�������� ������� Arglist, ������� ������������ ������ ���������� ������ �������
        if (!result.first) return false; //���� ��������� ���������� �� �������, ���������� ������
        if (iter->first != "rpar") return false; //���� �� ������� ������ ������ ), ��������� � �������
        nextState(0);
        addString("rpar"); 
        setLexem();
        backStateIt();
        backStateIt();

        auto q = checkFunc(p, result.second); //��������� ������������� ������� � ������ p � ������������� ����� ����������
        auto r = alloc(scope); //�������� ������ ��� �������� ���������� ������ �������
        addAtom({ scope, "CALL", q, "", r }); //��������� ��������� ���������� ������ ������� (CALL)

        return true;
    }
    return false;
}//
//
//������� ������������ ����������� while
bool LL::WhileOp(const string& scope) {
    setLexem(); //������������� ������� ������� ��� ������������

    //������� ��� ����� � ��������� ��������� ���������� ��� ������ ����� label1, ������� ��������� �� ������ �����
    auto label1 = newLabel();
    auto label2 = newLabel();
    addAtom({ scope, "LBL", "", "", "$L" + label1 });

    //��������� ������� ����������� ������ ( � �������� ������� ��� ������� ���������, ������� ������ ���� � ������� �����
    if (iter->first != "lpar") return false;
    setLexem();
    nextState(1);
    addString("lpar E");

    //������� ������� E, ������� ����������� ������� while
    auto result = E(scope);
    if (!result.first) return false;

    addAtom({ scope, "EQ", result.second, "0", "$L" + label2 }); //���� ������� ����� (����� 0), ��������� ��������� ���������� �������� � ����� label2, ����� ���� �����

    //��������� ������� ����������� ������ ) � �������� ������ ���� �����
    if (iter->first != "rpar") return false;
    setLexem();
    nextState(0);
    addString("rpar Stmt");

    //�������� ������� ��� ������� ���� ����� � ��������� � �����
    if (!Stmt(scope)) return false;
    backStateIt();

    //��������� ���������� �������� ������� � ������ ����� (label1) � ���������� ����� ���������� ����� (label2)
    addAtom({ scope, "JMP", "", "", "$L" + label1 });
    addAtom({ scope, "LBL", "", "", "$L" + label2 });

    return true;
}
//
// ������� ������������ ����������� for
bool LL::ForOp(const string& scope) {
    setLexem(); //������������� ������� ������� ��� ������������

    //������� ������ �����, ������� ������������ ��� ������ �����, ����������, ���� ����� � ��������
    auto label1 = newLabel();
    auto label2 = newLabel();
    auto label3 = newLabel();
    auto label4 = newLabel();

    //��������� ������� ����������� ������ � �������� ������� ��� ������� ������������� �����
    if (iter->first != "lpar") return false;
    setLexem();
    nextState(1);
    addString("lpar ForInit");
    
    //��������� ������������� �����, � ���� �� �������, ��������� ������� ����� � ������� ����� ���
    if (!ForInit(scope)) return false;
    if (iter->first != "semicolon") return false;

    //��������� � ������� ������� � ����� � ��������� ����� ��� ������ �������� �������
    setLexem();
    backStateIt();
    nextState(1);
    addString("semicolon ForExp");
    addAtom({ scope, "LBL", "", "", "$L" + label1 });

    //�������� ������� ��� ������� ������� �, ���� ��� �����, ��������� ������� � ���������� �����
    auto result = ForExp(scope);
    if (!result.first) return false;
    addAtom({ scope, "EQ", result.second, "0", "$L" + label4 });

    //��������� ���������� ��� �������� � �������� � ����� ��� ���� �����
    addAtom({ scope, "JMP", "", "", "$L" + label3 });
    addAtom({ scope, "LBL", "", "", "$L" + label2 });

    nextState(1);
    addString("semicolon ForLoop");

    //��������� �������� ����� � ��������� ������� ������� � ������
    if (!ForLoop(scope)) return false;
    addAtom({ scope, "JMP", "", "", "$L" + label1 });
    addAtom({ scope, "LBL", "", "", "$L" + label3 });

    //��������� � ������� ���� ����� � ��������� ��� ����������
    nextState(0);
    addString("rpar Stmt");
    if (!Stmt(scope)) { return false; }

    //��������� ���������� ��� �������� � ���� ����� � ���������� �����
    addAtom({ scope, "JMP", "", "", "$L" + label2 });
    addAtom({ scope, "LBL", "", "", "$L" + label4 });

    backStateIt();
    return true;
}

//������� �������� �� ������ ���������������� ����� ����� for (��������, ������������ ���������� ����� ������� �����)
bool LL::ForInit(const string& scope) {
    if (iter->first == "id") { //���������, �������� �� ������� ������� ���������������
        //�������� ��������� ��������� ������, ��������, ��� ��������� ���� ������������, ���� ����� �������
        nextState(0);
        addString("AssignOrCall");

        //�������� ������� AssignOrCall, ������� ���������, �������� �� ��� ����������� ������������� ��� ������� �������
        if (!AssignOrCall(scope)) return false;
    }
    return true;
}

//������� ������������ ���������, ������� �������� �������� ����� (��������, i < 10)
pair<bool, string> LL::ForExp(const string& scope) {

    //���������� fl ������������ ��� ������������ ������ �������, � result ������ ��������� ������ ������� E
    bool fl = false;
    pair<bool, string> result;
    //���������, �������� �� ������� ������� ����������, ������� ����� ����������� � ������� ����� (������, �������������, �����, ������ ��� �������� ����������)
    if (iter->first == "lpar" or iter->first == "id" or iter->first == "num" or iter->first == "char" or iter->first == "opinc") {

        //�������� ��������� � ��������� ������, ����������� �� ������ ���������
        nextState(0);
        addString("E");

        //�������� ������� E, ������� ��������� ���������, � ���� ������ ��������, ������������� ���� ������
        result = E(scope);
        fl = !result.first ? true : false;
    }
    //���������, ��� ������� ������� � ����� � �������, ������� ��������� ������� �����
    if (iter->first == "semicolon") {

        //������������� ������� ������� ��� ������������ � ���������� �������� �� ���� ������� �����
        setLexem();
        backStateIt();

        //���� ������� ���� ��������� � �������, ���������� �������� "1" (����������, ��� ������� �� ��������� ��������� ��������). 
        //���� ������ ���, ���������� ��������� ������� ���������
        if (fl) return { true, "1" };
        return { true, result.second };
    }

    //���� ������ �� ������
    return { false, "" };
}

//��� ������� �������� �� ������ ������������ ����� ����� for (��������, i++ ��� ������ ������ ����� ���� �����)
bool LL::ForLoop(const string& scope) {
    if (iter->first == "opinc") { //���������, �������� �� ������� ������� ��������� ����������
        setLexem(); //������������� ������� ������� ��� ������������

        if (iter->first != "id") return false; //���� ����� ���������� �� ������� �������������

        //��������� ��� �������������� � ������������� ������� ������� ��� ������������
        auto item = iter->second;
        setLexem();

        auto p = checkVar(scope, item); //���������, ���������� �� ���������� � ������� ������� ��������� scope
        addAtom({ scope, "ADD", p, "1", p }); //��������� ��������� ���������� ��� �������� ���������� (���������� ���������� �� 1)

        //���������, ��� �� ����������� ������� ����������� ������ ), � ���� ��� ���, ��������� ��������� � ��������� ������
        if (iter->first != "rpar") return false;
        nextState(0);
        addString("opinc " + item);

        //������������� ������� ������� ��� ������������ � ���������� �������� �� ��� ������� �����.
        //���������� true, ���� ������ �������� �������
        setLexem();
        backStateIt();
        backStateIt();
        return true;
    }
    //��������� ������������ ��� ������ �������
    if (iter->first == "id") { //���� ������� ������� � �������������, ��� ����� ���� ������������ ��� ����� �������

        //�������� ��������� ������� � ��������� ������, ����������� �� ������ ������������ ��� ������ �������
        nextState(0);
        addString("AssignOrCall");

        //�������� ��������� ������� � ��������� ������, ����������� �� ������ ������������ ��� ������ �������
        if (!AssignOrCall(scope)) return false;

        //��������� ������� ����������� ������ ) ����� ������������ ��� ������ ������� � ������������� ������� ������� ��� ������������
        if (iter->first != "rpar") return false;
        setLexem();
        backStateIt();

        return true;
    }

    //���������� ������� ��������

    //���� ������� ������� � ����������� ������ ), ��������� ������ �������. ���� ��� �� ���, ���������� ������
    if (iter->first != "rpar") return false;
    setLexem();
    backStateIt();
    return true;
}
//
// �������� �� ������ �������� ����������� if �, ��������, ��������� � ��� ����������� else
bool LL::IfOp(const string& scope) {
    setLexem(); //������������� ������� ������� ��� ������������
    if (iter->first != "lpar") return false; //���������, ��� ������� ������� � ��� ����������� ������� ������ (. ���� ��� �� ���, ���������� false

    //������������� ������� ������� ��� ������������, �������� ��������� � ��������� ������, ����������� ������ ��������� (�������) � �������
    setLexem();
    nextState(1);
    addString("lpar E");

    //�������� ������� E, ������� ��������� ��������� ������ ������ (������� if). ���� ������ �� �������, ������� ���������� false
    auto result = E(scope);
    if (!result.first) return false;

    //���������, ���� �� ����� ��������� ����������� ������ ). ���� �� ���, ���������� ������
    if (iter->first != "rpar") return false;

    setLexem(); //������������� ������� ������� (����������� ������) ��� ������������

    //������� ����� ����� � ��������� ��������� ���������� ��� ��������� ���������� ��������� � 0. 
    //���� ������� ����� (��������� ��������� ����� ����), ����� ������� �� ����� label1
    auto label1 = newLabel();
    addAtom({ scope, "EQ", result.second, "0", "$L" + label1 });

    //�������� ��������� ������� � ��������� ������ ��� ������� ���������, ������� ������� �� �������� if.
    //�������� ������� Stmt, ������� ��������� ���� ��������. ���� ������ �� �������, ���������� false
    nextState(1);
    addString("rpar Stmt");
    if (!Stmt(scope)) return false;

    //������� ����� ����� ��� ������ �� ����������� if. ��������� ���������� ��� ������������ �������� (JMP) �� ����� label2,
    //������� ������������ ��� �������� ����� ����, ���� ������� ���������. 
    //��������� ����� label1 ��� ��������, ���� ������� �� ���������
    auto label2 = newLabel();
    addAtom({ scope, "JMP", "", "", "$L" + label2 });
    addAtom({ scope, "LBL", "", "", "$L" + label1 });


    //�������� ��������� ������� � ��������� ������ ��� ��������� ����� else. ���� ��������� ������� � ��� �������� ����� else, 
    //�������� ������� ElsePart, ������� ��������� ��������, ��������� � else. 
    //����� ����� ��������� ����� label2, ������� ��������� �� ���������� ����������� else
    nextState(0);
    addString("ElsePart");
    if (iter->first == "kwelse") {
        if (!ElsePart(scope)) return false;

        addAtom({ scope, "LBL", "", "", "$L" + label2 });

        backStateIt();
        return true;
    }
    addAtom({ scope, "LBL", "", "", "$L" + label2 }); //��������� ����� label2 ��� ���������� ����������� if - else, ���� else �����������

    //���������� ��������� ������� �� ��� ������� ����� � ��������� ������� �������, ���� ������ ����������� if ��� ��������
    backStateIt();
    backStateIt();
    return true;
}

//������� ElsePart ��������� ����� ���������, ������� ����������� ��� ���������� ����������� else
bool LL::ElsePart(const string& scope) {
    //�������� ��������� ������� � ��������� ������ ��� ����� else, �������� �� ������ ���������, ���������� � else
    nextState(0);
    addString("kwelse Stmt");

    //������������� ������� ������� ��� ������������ � �������� ������� Stmt, 
    //������� ��������� ��������, ��������� � else. ���� ������ �� �������, ���������� false
    setLexem();
    if (!Stmt(scope)) return false;

    //���������� ��������� ������� �� ���� ������� ����� � ��������� ������� �������, ���� ������ ����� else ������ �������
    backStateIt();
    return true;
}
//
//��������� ����������� switch
bool LL::SwitchOp(const string& scope) {

    //������������� ������� ������� ��� ������������ � ��������� ������� ����������� ������ ( ��� ��������� switch.
    //���� ������ ���, ���������� ������
    setLexem();
    if (iter->first != "lpar") return false;

    //������������� ������� ������� ��� ������������, �������� ��������� � ��������� ������, ����������� ������ ��������� (�������) � �������
    setLexem();
    nextState(1);
    addString("lpar E");

    //�������� ������� E ��� ������� ��������� ������ ������. ���� ������ �� �������, ���������� ������
    auto result = E(scope);
    if (!result.first) return false;

    //��������� ������� ����������� ������ ). ���� ������ ���, ���������� ������. ����� ������������� ������� ������� ��� ������������
    if (iter->first != "rpar") return false;
    setLexem();


    //��������� ������� ����������� �������� ������ {, ������� ��������� ���� switch. ���� �� ���, ���������� ������. 
    //����� ������������� ������� ������� ��� ������������ � ��������� ������ ��� ������� ������ case
    if (iter->first != "lbrace") return false;
    setLexem();
    nextState(1);
    addString("lpar lbrace Cases");


    //������� ����� ��� ���������� ����������� switch � �������� ������� Cases, ������� ��������� ����� case. ���� ������ �� ������, ���������� ������
    auto end = newLabel();
    if (!Cases(scope, result.second, end)) return false;


    //��������� ������� ����������� �������� ������ }, ������� ��������� ���� switch. ���� �� ���, ���������� ������. ����� ��������� ������ switch
    if (iter->first != "rbrace") return false;
    setLexem();
    nextState(0);
    addString("rbrace");
    backStateIt();
    backStateIt();

    addAtom({ scope, "LBL", "", "", "$L" + end }); //��������� ����� ��� ���������� ����������� switch
    return true;
}

//������� Cases ��������� ������ ������ case � ���� default ������ ����������� switch
bool LL::Cases(const string& scope, const string& p, const string& end) {

    //�������� ��������� ������� � ��������� ������, ������������ ������ ������ ����� case
    nextState(1);
    addString("Acase");

    //�������� ������� ACase, ������� ������������ ��������� ���� case ��� default. ���� ������ �� ������, ���������� ������
    auto result = ACase(scope, p, end);
    if (!result.first) return false;

    //�������� ���������, ��������� ������ ��� ������ ������ case, �������� ������� CasesList ��� ����������� �������.
    //���� ������ �������, ���������� true, ����� false
    nextState(0);
    addString("CasesList");
    if (!CasesList(scope, p, end, result.second)) return false;
    backStateIt();
    return true;
}

//��������� ������������������ ������ case � ��������� ������� ��������� ����� ����
bool LL::CasesList(const string& scope, const string& p, const string& end, const string& def) {

    //���������, �������� �� ������� ������� �������� ������ case ��� default. 
    //���� ��, �������� ��������� � ��������� ������ ��� ������� ����� case
    if (iter->first == "kwdefault" or iter->first == "kwcase") {
        nextState(1);
        addString("Acase");

        //�������� ������� ACase ��� ������� �������� ����� case. ���� ������ �� �������, ���������� ������.
        auto result = ACase(scope, p, end);
        if (!result.first) return false;

        //���������, ���� �� �������� ����� ������� case. ���� ��� ����� ����� ���������� ��������, ����������� ��������� ����������, ����������� �� ������
        if (stoi(def) >= 0 and stoi(result.second) >= 0) {
            addAtom({ "$Error", "$Error", "$Error", "$Error", "$Error" });
        }
        auto def2 = stoi(def) < stoi(result.second) ? result.second : def; //��������� ���������� def2, ����� ��������� ������ ���������� ����� case

        //�������� ��������� � �������� ���� ���� ��� ������� ���������� ����� case. ���� ������ �������, ���������� true
        nextState(0);
        addString("CasesList");
        if (!CasesList(scope, p, end, def2)) return false;
        backStateIt();
        return true;

    }
    //������� �������� ���������� def � ����� ����������. ��������� ��������� ���������� ��� �������� � ����� q, ������� ����� ���� def, ���� end
    cout << def << " " << end << endl;
    string q = stoi(def) >= 0 ? def : end;
    addAtom({ scope, "JMP", "", "", "$L" + q });

    //���������� ��������� ����� � ��������� �������
    backStateIt();
    return true;
}

//��������� ���� ���� case ��� default
pair<bool, string> LL::ACase(const string& scope, const string& p, const string& end) {

    //���� ������� ������� � ��� case, ������������� �� ��� ������������
    if (iter->first == "kwcase") {
        setLexem();

        //���������, ��� ����� case ���� ����� ��� ������, � ��������� ���
        if (iter->first != "num" and iter->first != "char")return { false, "" };
        auto item = iter->second;

        //������������� ������� ������� ��� ������������ � ������� ����� �����. 
        //��������� ���������� ��� ��������� �������� � ���������� p, � ���� ��� �� �����, ���������� ������� �� ����� next
        setLexem();
        string next = newLabel();
        addAtom({ scope, "NE", p, item, "$L" + next });

        //��������� ������� ��������� ����� case. ���� ��� �����������, ���������� ������. 
        //�������� ��������� � �������� ������� StmtList, ������� ������������ ��������� ������ ����� case
        if (iter->first != "colon") return { false, "" };
        nextState(0);
        addString("kwcase " + item + " colon StmtList");
        setLexem();
        if (!StmtList(scope)) return { false, "" };

        //��������� ���������� ��� ������������ �������� � ����� ���������� ����� switch (end). 
        //����� ��������� ����� next � ���������� �������� ���������
        addAtom({ scope, "JMP", "", "", "$L" + end });
        addAtom({ scope, "LBL", "", "", "$L" + next });
        backStateIt();
        return { true, "-1" };

    }
    //������������ �������� ����� default, ��������� ������� ��������� ����� ���� � �������� ������ ���������� � ������� StmtList
    else if (iter->first == "kwdefault") {
        setLexem();

        if (iter->first != "colon") return { false, "" };
        setLexem();
        nextState(0);
        addString("kwdefault colon StmtList");

        //������� ����� ��� ���������� ����� default � ��������� ��������� ���������� ��� ���������
        string next = newLabel();
        string def = newLabel();
        addAtom({ scope, "JMP", "", "", "$L" + next });
        addAtom({ scope, "LBL", "", "", "$L" + def });

        //�������� StmtList ��� ��������� ���������� ������ ����� default. 
        //����� ��������� ������� � ����� ���������� � ���������� ��������� � ������ def
        if (!StmtList(scope)) return { false, "" };
        backStateIt();
        addAtom({ scope, "JMP", "", "", "$L" + end });
        addAtom({ scope, "LBL", "", "", "$L" + next });
        return { true, def };
    }
    return { false, "" };
}
//
//������������ ���� � ����� ������
bool LL::IOp(const string& scope) {

    //������������� ������� ������� ��� ������������ � ��������� ������� ��������������.��������� �������� ����������
    setLexem();
    if (iter->first != "id") return false;
    auto item = iter->second;
    setLexem();

    //��������� ������� ����� � ������� ����� ����������. ���� �� ���, ���������� ������
    if (iter->first != "semicolon") return false;
    setLexem();

    //��������� ������ � ����������� � ������� � ���������� ��������� ������� �� ��� ������� �����
    nextState(0);
    addString(item + "semicolon");
    backStateIt();
    backStateIt();

    //��������� ������������� ���������� � ������� ��������� scope � ��������� ��������� ���������� ��� �������� ����� ������ � ���������� p
    auto p = checkVar(scope, item);
    addAtom({ scope, "IN", "", "", p });

    return true;
}

//������� ��������� �������� ������
bool LL::OOp(const string& scope) {
    setLexem(); ////������������� ������� ������� ��� ������������

    nextState(1);
    addString("OOp'"); //��������� ������ � ����������� � ���, ��� ���������� ������ ��������� ������ (OOp')
    if (!OOpList(scope)) return false; //�������� ������� OOpList, ������� ��������� ���������� �������� � ������ ������
    if (iter->first != "semicolon") return false; //���������, ��� ����� ��������� ������ ����� ����� � ������� (semicolon), ��� �������� �������������� �����������
    setLexem();
    nextState(0);
    addString("semicolon"); //��������� ������ � ����������� � ���, ��� ���� ������� ��������� ����� � �������

    //���������� ��������� �� ��� ���� �����, ����� ��������� � ���� �������, � �������� ��������� ������
    backStateIt();
    backStateIt();
    return true;
}
//��� ������� ��������� ������ ��������� ������
bool LL::OOpList(const string& scope) {
    if (iter->first == "str") { //���������, �������� �� ������� ������� ��������� ���������
        auto item = iter->second; //��������� ��������� �������� ������� �������

        //������ ��������� ������� �� ��������� � ��������� ������ ������� � ��������� �������
        nextState(0);
        addString("\"" + item + "\"");

        setLexem(); // ������������� ������� ������� ��� ������������

        //���������� ��������� ������� �� ��� ���� �����
        backStateIt();
        backStateIt();

        addAtom({ scope, "OUT", "", "", "\"" + item + "\"" }); //���������� ��������� ���������� ��� ������ ������. ��������� ���������� OUT ��������, ��� �����
        return true;
    }
    
    //���� ������� ������� �� ������
    nextState(0);
    addString("E");
    auto result = E(scope); 
    if (!result.first) return false; //���� ������ ��������� ���������� ���������, ������� ���������� false
    backStateIt(); // ����������� � ����������� ���������

    addAtom({ scope, "OUT", "", "", result.second }); //��������� ��������� ������� ��� ������ ���������� ��������� (result.second � ��������� ������������ ���������)
    return true;
}