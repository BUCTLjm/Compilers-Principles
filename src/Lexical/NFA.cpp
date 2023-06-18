//#ifndef __NFA__
//#define __NFA__
//
//#include "ReX.cpp"
//#include "bits/stdc++.h"
#include "NFA.h"
//#include "Rex.h"
//#include "Rex.cpp"
#include "iostream"

using namespace std;


// 定义NFA状态
//struct NFA_State {
//
//    // 初态'#'视为空转移,初始化NFA的节点
//    void add_New_epTrans(NFA_State *nfa) {
//        epTrans.insert(nfa->index);
//    }
//
//    set<int> closure();
//
//    void print();
//
//    int index = -1;                  // 节点序号
//    char input = '#';                 // NFA状态弧上的值，默认为“#”
//    int chTrans = -1;          // NFA状态弧转移到的状态号，默认为NULL
//    set<int> epTrans;     //  当前状态通过ε转移到的状态号集合
//};

//当前节点的ε边转移闭包
set<int> NFA_State::closure() {
    set<int> s;
    s.insert(this->index);
    bool has_newNode = true;
    while (has_newNode) {
        has_newNode = false;
        // 能够通过ε边从当前状态转移到其他状态则添加此nfa节点
        for (auto x: epTrans) {
            if (s.find(x) == s.end()) {
                has_newNode = true;
                s.insert(x);
            }
        }
    }
    return s;
}

// 输出NFA节点：有什么输出什么
void NFA_State::print() {
    std::cout << "Node index: " << index << endl;
    if (input != '#')
        std::cout << "\tchTrans: " << input << endl;
    if (chTrans != -1)
        std::cout << "\tTransNode: " << chTrans << endl;
    // 输出ε边到达的节点 格式为{ , , ,}
    if (!this->epTrans.empty()) {
        std::cout << "\tepTrans : {";
    }
    // 第一个输出不需要分隔符，用first标记
    bool first = true;
    for (auto x: this->epTrans) {
        std::cout << (first ? "" : " , ") << x;
        first = false; // 后续输出需要输出分隔符
    }
    if (!this->epTrans.empty()) {
        std::cout << "}\n";
    }
    std::cout << "\n";

}


NFA_State *NFA::getHead() { return finalNfa.head; }

NFA_State *NFA::getTail() { return finalNfa.tail; }

void NFA::print() {
    std::cout << rex.get_postfix() << endl;
    for (auto x: NFAStates) {
        if (x.index != -1) {
            x.print();
        }
    }
    // 说明开始节点和结束节点
    std::cout << " Start Node : " << finalNfa.head->index << endl;
    std::cout << " End Node : " << finalNfa.tail->index << endl;
}

void NFA::add_Node(char c) {
    // 新增一个NFA节点对应操作符，建立头尾指针
    if (NFAStates.size() <= NFA_State_Num) NFAStates.resize(NFA_State_Num + 2);
    finalNfa.head = &NFAStates[NFA_State_Num];
    finalNfa.tail = &NFAStates[NFA_State_Num + 1];
    finalNfa.head->index = NFA_State_Num;
    finalNfa.tail->index = NFA_State_Num + 1;
    finalNfa.head->input = c;
    finalNfa.head->chTrans = finalNfa.tail->index;
    if (finalNfa.head->input == '#') finalNfa.head->chTrans = -1;
    NFA_State_Num += 2;
}

void NFA::Build_NFA() {
    string expression = rex.get_postfix();
    vector<final_NFA> stack_nfa; // nfa栈

    try {
        int len = expression.length();
        for (int i = 0; i < len; ++i) {
            char next_char = expression[i];
            int next_type = getchar_type(next_char);

            //  表达式为操作数则新建NFA，转换弧值为操作数，并将此NFA入栈.
            //  n.head--(操作数)-->n.tail
            if (next_type == OPERATE_CHARACTER) {
                this->add_Node(next_char);
                final_NFA n = this->finalNfa;
                stack_nfa.push_back(n);
            }
                // *: 新建一个NFA n，从NFA栈中弹出一个元素n1连接后，将NFA n压入栈中
            else if (next_char == '*') {
                //取不出一个元素
                if (stack_nfa.size() < 1) throw REX_FORMAT_EXCEPTION();
                final_NFA n1 = stack_nfa.back();
                stack_nfa.pop_back();


                this->add_Node('#');
                final_NFA n = this->finalNfa;// #表示从head->tail只需ε边

                // NFA n和n1连接后结构:
                (*n.head).add_New_epTrans(n1.head);
                (*n1.tail).add_New_epTrans(n.tail);
                (*n1.tail).add_New_epTrans(n.head);
                (*n.head).add_New_epTrans(n.tail);

                stack_nfa.push_back(n);
            }
                // '|':新建一个NFA n，并在NFA栈中弹出两个元素n1,n2，将NFA n压入栈中
            else if (next_char == '|') {
                // 取不出两个元素
                if (stack_nfa.size() < 2) throw REX_FORMAT_EXCEPTION();

                final_NFA n1 = stack_nfa.back();
                stack_nfa.pop_back();
                final_NFA n2 = stack_nfa.back();
                stack_nfa.pop_back();

                this->add_Node('#');
                final_NFA n = this->finalNfa;  // n上转移值为ε

                (*n.head).add_New_epTrans(n1.head);
                (*n.head).add_New_epTrans(n2.head);
                (*n1.tail).add_New_epTrans(n.tail);
                (*n2.tail).add_New_epTrans(n.tail);


                stack_nfa.push_back(n);
            }
                // '&':只需在NFA栈中弹出两个元素n1，n2，改变n1 n2的头尾指针,最后将n压入栈中
            else if (next_char == '&') {
                // 取不出两个元素
                if (stack_nfa.size() < 2) throw REX_FORMAT_EXCEPTION();
                final_NFA n2 = stack_nfa.back();
                stack_nfa.pop_back();
                final_NFA n1 = stack_nfa.back();
                stack_nfa.pop_back();

                (*n1.tail).add_New_epTrans(n2.head);
                n1.tail = n2.tail;

                stack_nfa.push_back(n1);

            } else throw REX_FORMAT_EXCEPTION();
        }

        if (stack_nfa.size() != 1) throw REX_FORMAT_EXCEPTION();

        finalNfa = stack_nfa[0];

    }
    catch (const REX_FORMAT_EXCEPTION &exception) {
        std::cerr << exception.what() << endl;
        exit(0);
    }
    catch (const std::exception &exception) {
        std::cerr << exception.what() << endl;
        exit(0);
    }
}

//#ifndef __MAINPROCESS__
//
//
//int main() {
//#ifndef __LOCAL
//    freopen("D:\\CodeEdit\\C++\\Compilers Principles\\userin.txt", "r", stdin);
//    freopen("D:\\CodeEdit\\C++\\Compilers Principles\\userout.txt", "w", stdout);
//#endif
//
//    string str;
//    std::cin >> str;
//
//    NFA nfa(str);
//    nfa.print();
//}
//
//#endif
