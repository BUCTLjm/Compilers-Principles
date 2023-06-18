//
// Created by OMEN on 2023/6/9.
//

#ifndef LEXICAL_MINIDFA_H
#define LEXICAL_MINIDFA_H
#include "iostream"
#include "set"
#include "DFA.h"
#include "vector"
#include "stack"
using namespace std;

// 划分状态集
struct stateSet {
    int trans_State = -1;            // 状态集转换到的状态集标号
    set<int> StateDFA;            // 该状态集中的dfa状态号
};

class miniDFA {
public:
    miniDFA(string expression) : Dfa(expression) {
        Build_miniDFA();
    }

    bool match(string);

    void print();

    void Build_miniDFA();

    DFA get_miniDFA() {
        return this->Dfa;
    };

private:
    vector<set<int>> PartSet;
    DFA Dfa;
};
#endif //LEXICAL_MINIDFA_H
