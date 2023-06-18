//#ifndef __DFA__
//#define __DFA__

#include "iostream"
#include "DFA.h"

using namespace std;



void DFA_State::print() {
    std::cout << "Node index: " << index << endl;
    if (!closure.empty()) {
        std::cout << "\tclosure : {";
        bool first = true;
        for (auto x: this->closure) {
            std::cout << (first ? "" : " , ") << x;
            first = false; // 后续输出需要输出分隔符
        }
        std::cout << "}\n";
    }
    std::cout << "\tis End:" << isEnd << endl;
    std::cout << "\n";
}




void DFA::Build_DFA() {
    set<set<int>> s;  //用于判断集合是否出现过。如果没有出现，则需新建一个DFA状态
    queue<DFA_State> q;

    // 计算节点通过ε转移到的所有节点值
    auto n_closure = [this](int index) {
        bool flag = true;
        set<int> temp;
        temp.insert(index);
        while (flag) {
            flag = false;
            for (auto x: temp) {
                for (auto y: Nfa.NFAStates[x].epTrans) {
                    if (temp.find(y) == temp.end()) {
                        flag = true;
                        temp.insert(y);
                    }
                }
            }
        }
        return temp;
    };

    // 判断是否含有终态节点
    auto judge_isEnd = [](DFA_State d, int end_index) {
        if (find(d.closure.begin(), d.closure.end(), end_index) != d.closure.end())
            d.isEnd = true;
        else
            d.isEnd = false;
        return d.isEnd;
    };

    // 计算move（T，ch）的节点集合，在再集合里添加所有能通过的ε到达的节点闭包
    auto move = [this](DFA_State T, char c) {
        DFA_State ans;
        for (auto x: T.closure) {
            if (Nfa.NFAStates[x].input == c) ans.closure.insert(Nfa.NFAStates[x].chTrans);
        }
        bool flag = true;
        while (flag) {
            flag = false;
            for (auto x: ans.closure) {
                for (auto y: Nfa.NFAStates[x].epTrans) {
                    if (ans.closure.find(y) == ans.closure.end()) {
                        flag = true;
                        ans.closure.insert(y);
                    }
                }
            }
        }
        return ans;
    };
    //计算ε-closure(0)，令T0 = ε-closure(0)
    DFA_State T;
    T.closure = n_closure(Nfa.getHead()->index);
    T.isEnd = judge_isEnd(T, Nfa.getTail()->index);
    T.index = DFA_State_Num;
    // 添加到DFAStates里
    if (DFAStates.size() <= DFA_State_Num) DFAStates.resize(DFA_State_Num + 1);
    DFAStates[DFA_State_Num] = T;
    s.insert(DFAStates[DFA_State_Num].closure);  // T进入子集判断集合
    DFA_State_Num++;
    q.push(T);
//    for (auto x: terminator) {
//        cout << "--------------" << x << "--------------\n";
//    }
    // NFA->DFA
    while (!q.empty()) {

        T = q.front();
        q.pop();

        //对每个终结符进行ε-closure(move(T， ch))运算
        for (auto ch: terminator) {
            DFA_State temp_DFA = move(T, ch);
            temp_DFA.isEnd = judge_isEnd(temp_DFA, Nfa.getTail()->index);
            //如果子集不为空
            if (!temp_DFA.closure.empty()) {
                if (ch == '!') ch = '*';
                if(ch == '[') ch = '(';
                if(ch == ']') ch  =')';
                if(ch == '^') ch  ='\\';
                // 如果子集不在s中
                if (s.find(temp_DFA.closure) == s.end()) {
                    // 为temp_DFA新建一个dfa状态
                    temp_DFA.index = DFA_State_Num;
                    // 添加到状态转移图里
                    finalDfa.trans[T.index][ch] = temp_DFA.index;
                    if (DFAStates.size() <= DFA_State_Num) DFAStates.resize(DFA_State_Num + 1);
                    DFAStates[DFA_State_Num] = temp_DFA;
                    DFA_State_Num++;
                    s.insert(temp_DFA.closure);        //将子集temp_DFA加入子集族s中
                    q.push(temp_DFA);    //将子集temp_DFA入队列
                }
                    //该状态在子集族s中，假设标号为temp_DFA
                else {
                    // 为当前状态T新建一条弧;弧上的值为当前终结符;弧转移到的状态为标temp_DFA
                    // 计算temp_DFA的index（通过DFAStates获得）
                    for (auto x: DFAStates) {
                        if (x.closure == temp_DFA.closure) {
                            temp_DFA.index = x.index;
                            break;
                        }
                    }
                    // 添加到状态转移图里
                    finalDfa.trans[T.index][ch] = temp_DFA.index;
                }
            }
        }
    }
}

void DFA::print() {
    // Dfa.print();
    std::cout << "------------------------------------------" << endl;
    for (auto x: DFAStates) {
        if (x.index != -1) {
            x.print();
        }
    }
    // 转移图
    std::cout << "------------------------------------------" << endl;
    std::cout << "trans graph is" << endl;
    for (auto i: finalDfa.trans)
        for (auto j: i.second) {
            std::cout << i.first << ' ' << j.first << ' ' << j.second << '\n';
        }

    std::cout << "-----------------end!!!!-----------------\n";
}

bool DFA::match(string match_str) {

    int to = 0;
    // DFA为NULL
    if (DFAStates.size() == 0) return false;

    for (auto ch: match_str) {
        // 没有则不匹配
        if (!finalDfa.trans[to].count(ch)) return false;
        // 有则转移到相应状态
        to = finalDfa.trans[to][ch];
    }
    // 是终态则匹配
    return DFAStates[to].isEnd;
}



//#ifndef __MAINPROCESS__
//int main() {
//#ifndef __LOCAL
//    freopen("D:\\CodeEdit\\C++\\Compilers Principles\\userin.txt", "r", stdin);
//    freopen("D:\\CodeEdit\\C++\\Compilers Principles\\DFAout.txt", "w", stdout);
//#endif
//
//    string str;
//    std::cin >> str;
//
//    DFA dfa(str);
//    dfa.print();
//    if(dfa.match("abababaaabb")){
//        cout<<"----------------------\n"<<"\tmatched\n";
//    } else{
//        cout<<"----------------------\n"<<"\tunmatched\n";
//    }
//}
//#endif


//#endif