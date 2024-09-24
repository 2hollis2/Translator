#include "Translator.h"
bool LL::asmBlock() {
    //�������� ������� main
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
    // ���������� ���������� ���������� � ������ ���������
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
    for (auto now : atomList) { // ������� ������ ������ ��������� ��������������� ������� ��� ��������� ������������� ����
        if (now.operation == "ADD") ADD(now); // ����� ������� �� ���������� ����� "ADD" 
        else if (now.operation == "SUB") SUB(now); // ����� ������� �� ���������� ����� "SUB" 
        else if (now.operation == "MUL") MUL(now); // ����� ������� �� ���������� ����� "MUL"
        else if (now.operation == "MOV") MOV(now); // ����� ������� �� ���������� ����� "MOV"
        else if (now.operation == "LBL") LBL(now); // ����� ������� �� ���������� ����� "LBL" 
        else if (now.operation == "JMP") JMP(now); // ����� ������� �� ���������� ����� "JMP" 
        else if (now.operation == "AND") AND(now); // ����� ������� �� ���������� ����� "AND" 
        else if (now.operation == "OR") OR(now); // ����� ������� �� ���������� ����� "OR"
        else if (now.operation == "EQ") EQ(now); // ����� ������� �� ���������� ����� "EQ" 
        else if (now.operation == "NE") NE(now); // ����� ������� �� ���������� ����� "NE" 
        else if (now.operation == "LT") LT(now); // ����� ������� �� ���������� ����� "LT" 
        else if (now.operation == "LE") LE(now); // ����� ������� �� ���������� ����� "LE" 
        else if (now.operation == "GT") GT(now); // ����� ������� �� ���������� ����� "GT" 
        else if (now.operation == "NOT") NOT(now); // ����� ������� �� ���������� ����� "NOT" 
        else if (now.operation == "IN") IN(now); // ����� ������� �� ���������� ����� "IN" 
        else if (now.operation == "OUT") OUT(now); // ����� ������� �� ���������� ����� "OUT" 
        else if (now.operation == "PARAM") PARAM(now); // ����� ������� �� ���������� ����� "PARAM" 
        else if (now.operation == "CALL") CALL(now); // ����� ������� �� ���������� ����� "CALL" 
        else if (now.operation == "RET") RET(now); // ����� ������� �� ���������� ����� "RET" 
        
        
    }
    
    printAsm();
    cout << "Assembler code has been generated successfully" << endl;
    return true;
}

void LL::printAsm() { // ������� ��� ������ ������������� ����, ����������� � asmList � ��������� ����� ������ (out)
    for (auto now : asmList) {
        out << now << endl;
    }
    return;
}

//������� ������������ ��������� ���� ��������� � ��������� ��������������� �������� � ������� (� ����������� �� ���� �������� ������������ ������ ������� ��� ���������� �������� ��������)
void LL::loadOp(const string& operand, const string& scope, int newOffset = -1) { //operand - �������, ������� ����� ���������, scope - ������� ��������� ��������, newOffset - ����� �������� ��� ���������� ((-1), ������������ ����������� ��������)
    if (operand[0] != '\'') { //������� ������ ��� �����, ������� �������� ����� ��������� � ����������� ����������
        asmList.push_back("MVI A, " + operand); //��������� �������� �������� � �����������
        return;
    }
    else if (operand[0] == '\"') { //���� ������ ���������� � ������� " (��������� �������)
        asmList.push_back("MVI A, " + operand.substr(1, operand.size() - 2)); //����������� ������� ����� ���������, MVI A ��������� �������� � �����������
        return;
    }
                                                                           //���� ' ������ �� ������ �� ������� �������� table, ��������� ����� ������� � �������� ��� ������ �� �������
    object item = table[stoi(operand.substr(1, operand.size() - 2)) - 1]; //�������������� ���������� ������������� ����� � ����� ����� ��� ���������� ������� table
    if (item.scope == "-1") { //��������� �� ������ item � ���������� ������� ���������
        asmList.push_back("LDA " + item.name); //��������� �������� �� ������ (�� ����� ������� item.name) � �����������
    }
    else {
        if (newOffset != -1) { //���� ������ ����� ��������, ����� ������������ ��� ��� �������� ������ ����������
            asmList.push_back("LXI H, " + to_string(newOffset)); //��������� ����� �������� � ������� H

        }
        else {
            asmList.push_back("LXI H, " + item.offset); //����� �������� �� �������, ������������ ����������� �������� item.offset
        }
        asmList.push_back("DAD SP"); //��������� �������� �������� SP (��������� �����), ����������� �������� �������� �� �������������� ���������� � �����
        asmList.push_back("MOV A, M"); //���������� �������� �� ������ � ����������� A
    }
}

//��������� �������� �������� � ��������������� ����� ������
void LL::saveOp(const string& operand) {                               
    object item = table[stoi(operand.substr(1, operand.size() - 2)) - 1]; //��������� ������ item �� ������� table, ����������� �� �������, ���������� �� ������ operand(stoi ����������� ��������� ������ � �����, ����� ���������� � �������� � ������� ��������)
    if (item.scope == "-1") {
        asmList.push_back("STA " + item.name); //��������� ���������� ������������ � ������ �� ������, ���������������� ����� ���������� item.name
    }
    else { //���� ���������� �� �������� ����������
        asmList.push_back("LXI H, " + item.offset); //��������� �������� ��������� ���������� ������������ ��������� ����� � �������� H � L
        asmList.push_back("DAD SP"); //��������� �������� ��������� ����� (SP) � ���������� H � L
        asmList.push_back("MOV M, A"); //���������� �������� �� ������������ (A) � ������ �� ������, ����������� � ��������� H � L
    }
}
void LL::ADD(const LL::atom& atom) {
    asmList.push_back("\n\t; ADD block"); //���������, ��� ���������� ���� ��� �������� ��������
    loadOp(atom.second, atom.scope); //��������� �������� �� ���������� ����� � ������� A 
    asmList.push_back("MOV B, A"); //���������� MOV B, A, �������� ������� �������� �� �������� A � ������� B
    loadOp(atom.first, atom.scope); //��������� �������� ������� ����� � ������� A
    asmList.push_back("ADD B"); //���������� ADD B,���������� �������� � �������� A � ��������� � �������� B
    saveOp(atom.third); //��������� ��������� ��������
}

void LL::SUB(const LL::atom& atom) {
    asmList.push_back("\n\t; SUB block"); //���������, ��� ���������� ���� ��� �������� ��������� 
    loadOp(atom.second, atom.scope); //��������� �������� ������� ����� � ������� A
    asmList.push_back("MOV B, A"); //��������� �������� �� �������� A � ������� B
    loadOp(atom.first, atom.scope); //��������� �������� ������� ����� � ������� A
    asmList.push_back("SUB B"); //�������� �������� � �������� B �� �������� � �������� A
    saveOp(atom.third); //��������� ��������� ��������
}

void LL::MUL(const LL::atom& atom) {
    asmList.push_back("\n\t; MUL block"); //���������, ��� ���������� ���� ��� �������� ���������
    loadOp(atom.second, atom.scope);  //��������� �������� ������� ����� � ������� A
    asmList.push_back("MOV D, A");  //��������� �������� �� �������� A � ������� D
    loadOp(atom.first, atom.scope); //��������� �������� ������� ����� � ������� A
    asmList.push_back("MOV C, A");  // ��������� �������� �� �������� A � ������� C

    asmList.push_back("CALL @MULT");

    asmList.push_back("MOV A, B"); //��������� �������� �� �������� B � ������� A
    saveOp(atom.third); //��������� ��������� ��������
}

void LL::MOV(const LL::atom& atom) {
    asmList.push_back("\n\t; MOV block"); //���������, ��� ���������� ���� ��� �������� MOV
    loadOp(atom.first, atom.scope); //��������� �������� � �����������
    saveOp(atom.third); ////��������� ��������� ��������

}

void LL::LBL(const LL::atom& atom) {
    asmList.push_back(atom.third + ":"); //��������� ����� � asmList. ����� ��������� �� �������� atom.third, � �������� ����������� ��������� (:). ��� ����� �������������� ��� ����� ���������� ��� ���������
    if (atom.third[0] != '$') { // ���������, �� ���������� �� ����� � ������� $
        for (auto now : table) {  
            if (now.name == atom.third) { //���� � ������� table ������ � ������ atom.third
                for (int i = 0; i < stoi(now.offset) - stoi(now.len); i++) {
                    asmList.push_back("PUSH B"); //��������� ������� PUSH B � asmList. ���������� ���� ������ ����� �������� ����� now.offset � now.len
                }
                break;
            }
        }
    }
}

void LL::JMP(const LL::atom& atom) {
    asmList.push_back("JMP " + atom.third); //��������� ������� �������� JMP � asmList, ��� atom.third �������� ������, �� ������� �������������� �������
}

// ��������� ���������� �������� "�" ����� ����� ���������� � ��������� ���������
void LL::AND(const LL::atom& atom) {
    asmList.push_back("\n\t; AND block"); //���������, ��� ���������� ���� ��� �������� AND
    loadOp(atom.second, atom.scope); //��������� ������ ������� � ������� A
    asmList.push_back("MOV B, A"); //���������� ���������� �������� A � ������� B
    loadOp(atom.first, atom.scope); //��������� ������ ������� � ������� A
    asmList.push_back("ANA B"); //��������� AND ����� ��������� � �������� A � ��������� � �������� B. ��������� ����������� � �������� A.
    saveOp(atom.third); //��������� ��������� �������� AND
}

//��������� ���������� �������� "���" ����� ����� ���������� � ��������� ���������
void LL::OR(const LL::atom& atom) {
    asmList.push_back("\n\t; OR block"); //���������, ��� ���������� ���� ��� �������� OR
    loadOp(atom.second, atom.scope); //��������� �������� ������� �������� atom.second � ����������� (������� A)
    asmList.push_back("MOV B, A"); //�������� �� ������������ ���������� � ������� B
    loadOp(atom.first, atom.scope); //�������� ������� �������� atom.first � �����������
    asmList.push_back("ORA B"); //��������� �������� "���" ����� ��������� � ������������ � ��������� � �������� B. ��������� �������� � ������������
    saveOp(atom.third); //��������� �������� OR �����������
}

// ���������� ��� �������� � ��������� �� �����, ���� ��� �����
void LL::EQ(const LL::atom& atom) {
    asmList.push_back("\n\t; EQ block"); //���������, ��� ���������� ���� ��� �������� EQ "���������"
    loadOp(atom.second, atom.scope); //��������� �������� ������� �������� atom.second � �����������
    asmList.push_back("MOV B, A"); //�������� �������� �� ������������ � ������� B
    loadOp(atom.first, atom.scope); //��������� �������� ������� �������� atom.first � �����������
    asmList.push_back("CMP B"); //���������� �������� � ������������ � ���, ��� � �������� B. ���� �������� �����, ��������������� ���� ���������
    asmList.push_back("JZ " + atom.third); //��������� ������� �� ����� atom.third, ���� ���� ��������� ���������� (�� ���� ���� ��� �������� �����). JZ (Jump if Zero)
}

//���������� ��� �������� � ��������� �� �����, ���� ��� �� �����
void LL::NE(const LL::atom& atom) {
    asmList.push_back("\n\t; NE block"); //���������, ��� ���������� ���� ��� �������� NE "�����������"
    loadOp(atom.second, atom.scope); //��������� �������� ������� �������� atom.second � �����������
    asmList.push_back("MOV B, A"); //�������� �������� �� ������������ � ������� B
    loadOp(atom.first, atom.scope); //��������� �������� ������� �������� atom.first � �����������
    asmList.push_back("CMP B"); //���������� �������� � ������������ � ���, ��� � �������� B
    asmList.push_back("JNZ " + atom.third); //��������� ������� �� ����� atom.third, ���� ���� ��������� �� ����������. JNZ (Jump if Not Zero)
}

//���������� ��� �������� � ��������� �� �����, ���� ������ ������� ������ �������
void LL::LT(const LL::atom& atom) {
    asmList.push_back("\n\t; LT block"); //���������, ��� ���������� ���� ��� �������� LT "������"
    loadOp(atom.second, atom.scope); //��������� �������� ������� �������� atom.second � ����������� (������� A)
    asmList.push_back("MOV B, A"); //�������� �������� �� ������������ � ������� B
    loadOp(atom.first, atom.scope); //��������� �������� ������� �������� atom.first � �����������
    asmList.push_back("CMP B"); //���������� �������� � ������������ � ���, ��� � �������� B
    asmList.push_back("JM " + atom.third); //��������� ������� �� ����� atom.third, ���� ��������� ������ �������� ������ ������� (���� "�������������" ����������). JM (Jump if Minus)
}

//���������� ��� �������� � ��������� �� �����, ���� ������ ������� ������ ��� ����� �������
void LL::LE(const LL::atom& atom) {
    asmList.push_back("\n\t; LE block"); //���������, ��� ���������� ���� ��� �������� LE "������ ��� �����"
    loadOp(atom.second, atom.scope); //��������� �������� ������� �������� atom.second � �����������
    asmList.push_back("MOV B, A"); //�������� �������� �� ������������ � ������� B
    loadOp(atom.first, atom.scope); //��������� �������� ������� �������� atom.first � �����������
    asmList.push_back("CMP B"); //���������� �������� � ������������ � �������� B
    asmList.push_back("JZ " + atom.third); //������� �� ����� atom.third, ���� �������� �����. JZ (Jump if Zero)
    asmList.push_back("JM" + atom.third); //������� �� ����� atom.third, ���� ������ �������� ������ �������. JM (Jump if Minus)
}

//���������� ��� �������� � ��������� �� �����, ���� ������ ������� ������ �������
void LL::GT(const LL::atom& atom) {
    asmList.push_back("\n\t; GT block"); //���������, ��� ���������� ���� ��� �������� GT "������"
    loadOp(atom.second, atom.scope); //��������� �������� ������� �������� atom.second � �����������
    asmList.push_back("MOV B, A"); //�������� �������� �� ������������ � ������� B
    loadOp(atom.first, atom.scope); //��������� �������� ������� �������� atom.first � �����������
    asmList.push_back("CMP B"); //���������� �������� � ������������ � �������� B
    asmList.push_back("JP " + atom.third); //������� �� ����� atom.third, ���� ������ �������� ������ ������� (���� "�������������" ����������). JP (Jump if Positive)
}

//��������� ��������, ����������� ���
void LL::NOT(const LL::atom& atom) {
    asmList.push_back("\n\t; NOT block"); //���������, ��� ���������� ���� ��� �������� NOT
    loadOp(atom.first, atom.scope); //��������� �������� ������� �������� atom.first � ����������� (������� A)
    asmList.push_back("CMA"); //����������� ��� ���� � ������������
    saveOp(atom.third); //��������� ��������� �������� NOT
}

//��������� ������ �� ����� 0 � ��������� �� � ��������� �����
void LL::IN(const LL::atom& atom) {
    asmList.push_back("\n\t; IN block"); //���������, ��� ���������� ���� ��� �������� IN 
    asmList.push_back("IN 0"); //��������� ������ �� ����� ����� � ������� 0 � �������� �� � �����������
    saveOp(atom.third); //��������� ��������� �������� IN
}

//��������� �������� �� ���������� ����� � ������� ��� �� ���� 1
void LL::OUT(const LL::atom& atom) {
    asmList.push_back("\n\t; OUT block"); //���������, ��� ���������� ���� ��� �������� OUT
    loadOp(atom.third, atom.scope); //��������� ��������, ���������� �� ������, ���������� � atom.third, � �����������
    asmList.push_back("OUT 1"); //������� ���������� ������������ �� ���� ������ � ������� 1
}

//��������� �������� ������� � �����
void LL::PARAM(const LL::atom& atom) {
    programStack.push(atom.third); //��������� �������� atom.third � ���� ��������� programStack. ��������� ��������� ��������� ��� ������������ ������������� ��� ������ �������
}

//�������� �������, �������� ��������� � �������������� ��������� ����� ����������
void LL::CALL(const LL::atom& atom) {
    asmList.push_back("\n\t; CALL block"); //���������, ��� ���������� ���� ��� �������� CALL
    asmList.push_back("PUSH B"); //��������� �������� �������� B � �����, ����� ��� ����� ���� ������������ ����� ���������� �������

    object func = table[stoi(atom.first.substr(1, atom.first.size() - 2)) - 1]; //��������� ���������� � ������� �� ������� table �� �������, ����������� � atom.first
    int n = stoi(func.len); //���������� ���������� �������
    int total = stoi(func.offset); //����� �������� �������
    string funcCode = func.code.substr(1, func.code.size() - 2); //��� �������, ������� ����� ������

    for (int i = 0; i < n; i++) {
        asmList.push_back("PUSH B"); //��������� ������� PUSH B � asmList, ����� ��������� ��� ��������� ������� ����� � �������
    }
    for (int i = n - 1; i >= 0; i--) {
        string item = programStack.top(); //����������� ��������� �� ����� ���������
        programStack.pop();
        int newOffset = stoi(table[stoi(funcCode) + i].offset) - (i + 1 != n ? stoi(table[stoi(funcCode) + i + 1].offset) : 0); //����������� ����� �������� ��� ������� ���������
        loadOp(item, atom.scope, newOffset); //����������� �������� � ����������� � ������� loadOp


        asmList.push_back("LXI H, " + to_string((i + 1) * 2)); //����������� �������� � ������� H
        asmList.push_back("DAD SP"); //����������� �����, ���� ����� �������� ��������
        asmList.push_back("MOV M, A"); //����������� �������� ������������ �� ������������ ������
    }
    asmList.push_back("CALL " + func.name); //��������� ������� ������ ������� �� �����

    for (int i = 0; i < n; i++) { //��������������� ���������, ������� ���� ��������� � �����
        asmList.push_back("POP B");
    }
    asmList.push_back("POP B"); //��������������� �������� �������� B
    asmList.push_back("MOV A, C"); //�������� �������� �� �������� C � �����������
    saveOp(atom.third); //��������� ��������� � ��������� ����� atom.third
}

//���������� �������� �� ������� � ������� ��������� �� �����
void LL::RET(const LL::atom& atom) {
    int m = stoi(table[stoi(atom.scope) - 1].offset); //���������� ����������, ������� ����� ������� �� �����
    asmList.push_back("\n\t; RET block"); ////���������, ��� ���������� ���� ��� �������� RET
    loadOp(atom.third, atom.scope); //��������� ��������, ������� ����� ����������, � �����������
    asmList.push_back("LXI H, " + to_string(m * 2 + 2)); //��������� �������� � ������� H
    asmList.push_back("DAD SP"); //��������� ����� ��������
    asmList.push_back("MOV M, A"); //��������� �������� ������������ �� ������ ��������
    for (int i = 0; i < m; i++) { //������� ��������� �� �����, �������������� ��������� ��������� ����� ������ �������
        asmList.push_back("POP");
    }
    asmList.push_back("RET"); //��������� ������� RET, ������� ���������� ���������� ������� � ���������� ���
}
