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
    bool StmtList(const string& scope); //��������� ������ ���������� � ������� ������� ���������

    bool Stmt(const string& scope); //����� ��� ��������� ������ ��������� � ������� ������� ���������
 
    pair<bool, string> Type(const string& scope); //��������� ����� ������

    bool DeclareStmt(const string& scope); //��������� ���������� ���������� ���������� � �������� ������� ���������

    bool DeclareStmtList(const string& scope, const string& p, const string& q); //��������� ������ ���������� ���������� � ������� ������� ���������

    bool DeclareVarList(const string& scope, const string& p); //��������� ������ ���������� � ���������� � ������� ������� ���������

    bool InitVar(const string& scope, const string& p, const string& q); //������������� ����������

    pair<bool, string> ParamList(const string& scope); //��������� ������ ���������� �������, ���������� ���� ��������: ���������� � ������ � ����������� ��������� ����������

    pair<bool, string> ParamListList(const string& scope); //��������� ������ ����������, ���� �� ������� �� ���������� ����������
    //
    // �������� ������������ � ������ �������
    bool AssignOrCallOp(const string& scope); //����� ��� �����������, �������� �� ��������� ��������� ������������ ��� ������� ������� � ��������� ������� ������� ���������

    bool AssignOrCall(const string& scope); //��������� ���������� �������� ������������ ��� ������ �������

    bool AssignOrCallList(const string& scope, const string& p); //��������� ���������� �������� ������������ ��� ������� �������, ���� ��� ���� ���������������
    //
    // while
    bool WhileOp(const string& scope);
    //
    // "ForOp"
    bool ForOp(const string& scope); //��������� ����� for

    bool ForInit(const string& scope); //��������� ������������� ����� for

    pair<bool, string> ForExp(const string& scope); //��������� ��������� ����� for, ���������� ��������� � ���� ���� ��������

    bool ForLoop(const string& scope); //��������� �����, ����������� �� ���� ����� for
    //
    //"IfOp"
    bool IfOp(const string& scope); //�������� ��������� ��������� if

    bool ElsePart(const string& scope); //��������� ����� else, ���� �� ������������ ����� ��������� if
    //
    // block "SwitchOp"
    bool SwitchOp(const string& scope); //��������� ��������� switch

    bool Cases(const string& scope, const string& p, const string& end); //��������� ������ case ������ ��������� switch

    bool CasesList(const string& scope, const string& p, const string& end, const string& def); //��������� ������ ������ case �, ��������, ����� default

    pair<bool, string> ACase(const string& scope, const string& p, const string& end); //��������� ������ ����� case � ��������� switch
    //
    //"IN" and "OUT"
    bool IOp(const string& scope); //��������� ��������� IN, ����������� �� ����

    bool OOp(const string& scope); //��������� ��������� OUT, ����������� �� �����

    bool OOpList(const string& scope); //��������� ������ ���������� OUT
    
    //
    //funcs
    void setLexem(); //������������� ������� ������� ��� ���������
    void nextState(const int& state); //������� � ���������� ��������� �������, � ����������� �� ��������
    void backStateIt(); //���������� ��������� ������� �� ���������� ���
    void addString(const string& str); //��������� ������ � ��������� ������, ������������ ��� ���������� ��������������� ������

    Lexer lexer; //������ �������, ������� ������������ ��� ������� ������ ��������� ����
    Lexem tempLexem; //��������� �������, ������� �������� ��� �������� ���� ���������

    vector <Lexem> list; //������ ������, ���������� �� �������
    vector<Lexem>::iterator iter; //�������� ��� ����������� �� ������ ������

    //������ ������
    int treeHeight = 0; //������ ��������������� ������, ������������ ��� ��� ������������
    vector<int> states; //������ ��������� ������� �� ��������� ������� �������
    vector<int>::iterator statesIt; //�������� ��� ����������� �� ���������� �������

    ofstream outStr; //����� ������ ��� ��������������� ������
    string outPath; //���� � �����, ���� ��������� �������������� ������

    vector<string> output; //������ ��� �������� �����, ������� ����� ���������� ��� ���������� ��������������� ������

    bool printTree(bool fl); //�������� �������������� ������, ���� ������ ������ �������
    //
    //semantic block
    string newLabel(); //���������� ����� ����� ��� ����������� ���������
    string alloc(const string& scope); //�������� ����� ��������� ����������
    string addVar(const string& name, const string& scope, const string type, const string& init = "0"); //��������� ����� ���������� � ������� ��������
    string addFunc(const string& name, const string& type); //��������� ����� ������� � ������� �������� � ������ � ����� �������
    string checkVar(const string& scope, const string& name); //���������, ���������� �� ���������� � ������ ������ � ������� ������� ���������
    string checkFunc(const string& name, const string& len); //���������, ���������� �� ������� � ������ ������ � ������������ ���������� ���������� ����������

    struct object {
        string code; //��������� ��� ������� � �������
        string name; //��� �������
        string kind; //��� �������
        string type = "kwint"; //�� ������ ������� �� ���������
        string len = "-1"; //����� �������
        string value = "00"; //��������� �������� �������
        string scope = "-1"; //������� ���������, � ������� ��������� ������
        string offset = "-1"; //�������� � ������ ��� �������
    };

    vector<object> table; //������� ��������, ���������� ��� ���������� � ������� � ���������. ������������ ��� �������� ���������� � ������ ���������� � �������
    int labelCounter = 0; //������� �����, ������� ������������� ��� ��������� ����� �����
    int codeCounter = 0; //������� ����, ������� ����� ���� ����������� ��� ��������� ���������� ����� ��� ��������
    int tempVarCounter = 0; //������� ��������� ����������, ������� ��������� ��� �����������

    bool printAtoms(); //������� ��� ������ ������
    //
    //
    struct atom {
        string scope = "-1"; //������� ���������
        string operation = ""; //��������, ������� �����������
        string first = ""; 
        string second = ""; 
        string third = "";
    };

    void addAtom(atom data); //��������� ��������� ���������� � ������ ��� ����������� ��������� �������������� ����
    vector<atom> atomList; //������ ���� ��������� ����������, ������� ���� ������� �� ����� ������� � ������� ���������
    string outPathAtom; //���� � �����, � ������� ��������� �����
    //
    //
    vector<string> asmList; //������ ���������� ����������. ������������ ��� �������� ���������������� ������������� ���� �� ��� ������
    string outPathAsm; //���� � �����, � ������� ��������� ������������ ���
    stack<string> programStack; //���� ���������, ������������ ��� ���������� �������� �������, ���������� ����������� � ����������

    bool asmBlock(); //����� ��� ��������� � ��������� ����� ���������� �� ������ ��������� ����������
    void printAsm(); //�������� ��������������� ������������ ��� � ����

    void loadOp(const string& operand, const string& scope, int newOffset); //��������� ������� � ������ ����� � ������ ������� ��������� � ��������
    void saveOp(const string& operand); //��������� ��������� ���������� �������� � ������ ��� �������, ��������� ��������� �������

    //������������ �������
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