//
// Created by OMEN on 2023/6/9.
//

#ifndef COMPILERS_PRINCIPLES_NFA_H
#define COMPILERS_PRINCIPLES_NFA_H

#include "iostream"
#include "set"
#include "Rex.h"
#include "vector"
#include "stack"
using namespace std;


// 定义NFA状态
struct NFA_State {

    // 初态'#'视为空转移,初始化NFA的节点
    void add_New_epTrans(NFA_State *nfa) {
        epTrans.insert(nfa->index);
    }

    set<int> closure();

    void print();

    int index = -1;                  // 节点序号
    char input = '#';                 // NFA状态弧上的值，默认为“#”
    int chTrans = -1;          // NFA状态弧转移到的状态号，默认为NULL
    set<int> epTrans;     //  当前状态通过ε转移到的状态号集合
};

// 最终的整个NFA状态图
struct final_NFA {

    void operator=(const final_NFA &ele) {
        head = ele.head;
        tail = ele.tail;
    }

    // NFA节点应该是head->tail形式
    NFA_State *head;
    NFA_State *tail;
};

// NFA结构：
class NFA {
public:
    NFA(string expression) : rex(expression){
        Build_NFA();
    }

    void print();

    void Build_NFA();

    string get_rex(){
        return this->rex.get_postfix();
    }

    NFA_State *getHead();

    NFA_State *getTail();

    vector<NFA_State> NFAStates = vector<NFA_State>(1001);     // 维护总的NFA节点

    int NFA_State_Num = 0;  // NFA节点数

private:
    final_NFA finalNfa;  // 最终的NFA图
    REX rex;   // 后缀正则表达式
    void add_Node(char c);
};

#endif //COMPILERS_PRINCIPLES_NFA_H
