//
// Created by OMEN on 2023/6/9.
//

#ifndef LEXICAL_DFA_H
#define LEXICAL_DFA_H
#include "iostream"
#include "set"
#include "NFA.h"
#include "vector"
#include "stack"
#include "map"
#include "queue"
#include "algorithm"
using namespace std;

// DFA节点状态
struct DFA_State {

    void print();

    bool isEnd = false;            // 是否为终态，是为true，不是为false

    int index = -1;            // DFA状态的状态号

    set<int> closure;  // NFA的ε-move()闭包
};

// 储存整个DFA结构体,定义DFA结构
struct final_DFA {
    map<int, map<char, int>> trans;        // DFA的转移矩阵
};

class DFA {
public:
    DFA(string expression) : Nfa(expression) {
        // 获取所有操作数集
        string treated_expression = Nfa.get_rex();
        for (auto termin: treated_expression) {
            int termin_type = getchar_type(termin);
            if (termin_type == OPERATE_CHARACTER) terminator.insert(termin);
            if (termin_type == ILLEGAL_CHAR) {
                cerr << "ILLEGAL CHARACTER\n";
                exit(0);
            }
        }
        Build_DFA();
    }

    int DFA_State_Num = 0;     //dfa状态总数
    vector<DFA_State> DFAStates = vector<DFA_State>(0);  //维护DFA总节点

    bool match(string);

    void print();

    set<char> getTerminator() {
        return terminator;
    }

    // 获取转移图
    final_DFA getDFA() {
        return finalDfa;
    }

    void rebuild_DFA(final_DFA new_DFA) {
        finalDfa = new_DFA;
    }

    void Build_DFA();

    NFA get_NFA() {
        Nfa.get_rex();
        return this->Nfa;
    }

private:
    final_DFA finalDfa;
    NFA Nfa;
    set<char> terminator;        //DFA的终结符集
};
#endif //LEXICAL_DFA_H
