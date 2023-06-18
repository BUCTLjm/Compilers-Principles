//
// Created by OMEN on 2023/4/14.
//

//#ifndef __miniDFA__
//#define __miniDFA__

#include "miniDFA.h"
#include "iostream"

using namespace std;



void miniDFA::Build_miniDFA() {
    PartSet = vector<set<int>>(2);        //用于存储所有的划分集合,初始化为终态和非终态两个划分集合
    // 判断当前划分集合是否需要进行划分的依据为缓冲区中的元素个数.
    // 如果个数为1，表示当前划分集合中的所有元素都转移到同一个集合中，则不需要划分。
    // 反之，如果个数大于1，表示当前划分集合中的元素转移到不同集合中，则需要划分

//    Dfa.print();
//    cout << "------------------\n";
    final_DFA DFA_transGraph = Dfa.getDFA(); // 获取状态转移图


//    if (Dfa.match("float")) {
//        cout << "----------------------\n" << "\tmatched\n";
//    } else {
//        cout << "----------------------\n" << "\tunmatched\n";
//    }
    //将终态和非终态划分开
    //判断DFA中是否包含非终态。如果有则第一次划分后划分集合个数为2，如果没有则为1。
    for (auto state: Dfa.DFAStates) {
        if (!state.isEnd) {
            PartSet[0].insert(state.index);  //不是终态加入到划分集合[0]的DFAState中
        } else {
            PartSet[1].insert(state.index);  //是终态加入到划分集合[1]的DFAState中
        }
    }
//    if (!has_end) PartSet.pop_back();
    //上次产生新的划分则为true，反之为false
    bool has_newPart = true;
    //一直循环，直到上次没有产生新的划分
    while (has_newPart) {
        int cutCount = 0;
        for (auto part: PartSet)    //对每个划分集合set
        {
            bool flag = true;
            cutCount = 0;    //划分次数
            //遍历每个终结符
            for (auto ch: Dfa.getTerminator()) {
                if (ch == '!') ch = '*';
                if (ch == '[') ch = '(';
                if (ch == ']') ch = ')';
                if (ch == '^') ch = '\\';
                // 临时变量存储转移到-1的状态集合
                vector<stateSet> buffer = vector<stateSet>(); //缓冲区用来存储划分集合中的元素和该集合转移到的集合号
                stateSet null_State;
                //遍历划分集合part中的每个DFA状态state
                for (auto state: part) {
                    //判断该DFA状态是否存在与该终结符对应的弧
                    bool haveEdge = false;
                    //遍历DFA状态state的每条边edge
                    for (auto edge: DFA_transGraph.trans[state]) {
                        //如果存在某条边的输入为当前终结符
                        if (edge.first == ch) {
                            //找到该弧转换到的状态所属的划分集合号
                            //将该state加入到缓冲区中能转换到DFA序号的状态集合中;
                            int trans_num = -1;
                            for (int k = 0; k < PartSet.size(); k++) {
                                if (PartSet[k].find(edge.second) != PartSet[k].end()) {
                                    trans_num = k;
                                    break;
                                }
                            }
                            if (trans_num == part.size()) {
                                haveEdge = false;
                                break;
                            }
                            bool is_not_exist = true; // buffer里面已经有trans_State,只需加入StateDFA
                            for (int buff = 0; buff < buffer.size(); buff++) {
                                if (buffer[buff].trans_State == trans_num) {
                                    buffer[buff].StateDFA.insert(state);
                                    is_not_exist = false;
                                    break;
                                }
                            }
                            if (is_not_exist) {
                                stateSet temp;
                                temp.trans_State = trans_num;
                                temp.StateDFA.insert(state);
                                buffer.push_back(temp);   // 入buffer缓冲区
                            }
                            haveEdge = true;
                            break;
                        }
                    }
                    if (!haveEdge) {
                        // 将该state加入到缓冲区中能转换到-1的状态集合中;
                        null_State.StateDFA.insert(state);
                    }
                }
                if (!null_State.StateDFA.empty()) {
                    null_State.trans_State = -1;
                    buffer.push_back(null_State);
                }
                if (buffer.size() > 1)        //缓冲区中元素个数大于1则需要划分
                {
                    cutCount++;        //划分次数+1
                    //这里是从1开始，因为要将buffer[0]中的元素保留在原划分集合中
                    int part_num = distance(PartSet.begin(), std::find(PartSet.begin(), PartSet.end(), part));
                    PartSet[part_num] = buffer[0].StateDFA;
                    for (int i = 1; i < buffer.size(); i++) {
                        // 在原划分集合set中删除buffer[i]中的元素;为buffer[i]创建新的划分集合;
                        PartSet.push_back(buffer[i].StateDFA);
                    }
                    flag = false;
                    break;
                }
            }
            if (flag == false) {
                break;
            }
        }
        //划分次数大于0说明本次产生了新的划分
        if (cutCount == 0) {
            has_newPart = false;
        }

    }

    vector<DFA_State> new_DFA_States;
    map<int, map<char, int>> new_trans;

    // 每个PartSet建立一个DFA_State
    for (int i = 0; i < PartSet.size(); i++) {
        DFA_State temp;
        temp.index = i;
        for (auto x: PartSet[i]) {
            temp.isEnd = temp.isEnd | Dfa.DFAStates[x].isEnd;
            temp.closure.insert(Dfa.DFAStates[x].closure.begin(), Dfa.DFAStates[x].closure.end());
        }
        new_DFA_States.push_back(temp);
        for (auto ch: Dfa.getTerminator()) {
            if (ch == '!') ch = '*';
            if (ch == '[') ch = '(';
            if (ch == ']') ch = ')';
            if (ch == '^') ch = '\\';
            int self_num = *PartSet[i].begin();
            int trans_num = -1;
            if (DFA_transGraph.trans[self_num].find(ch) != DFA_transGraph.trans[self_num].end()) {
                trans_num = DFA_transGraph.trans[self_num][ch];
                if (trans_num >= 0) {
                    for (int j = 0; j < PartSet.size(); j++) {
                        if (PartSet[j].find(trans_num) != PartSet[j].end()) {
                            new_trans[i][ch] = j;
                            break;
                        }
                    }
                }
            }

        }
    }
    Dfa.DFAStates = new_DFA_States;
    Dfa.DFA_State_Num = new_DFA_States.size();
    DFA_transGraph.trans = new_trans;
    Dfa.rebuild_DFA(DFA_transGraph);
}

bool miniDFA::match(string match_str) {
    int to = 0;
    for (int i = 0; i < PartSet.size(); i++) {
        if (PartSet[i].find(0) != PartSet[i].end()) {
            to = i;
            break;
        }
    }
    // DFA为NULL
    if (Dfa.DFAStates.size() == 0) return false;

    for (auto ch: match_str) {
        // 没有则不匹配
        if (!Dfa.getDFA().trans[to].count(ch)) return false;
        // 有则转移到相应状态
        to = Dfa.getDFA().trans[to][ch];
    }
    // 是终态则匹配
    return Dfa.DFAStates[to].isEnd;
}

void miniDFA::print() {
    Dfa.print();
    cout << "---------------------------------\n";
    for (int i = 0; i < PartSet.size(); i++) {
        cout << "PartSet Num " << i;
        cout << " :  (  ";
        for (auto x: PartSet[i]) {
            cout << x << "  ";
        }
        cout << ")\n";
    }
}

//#ifndef __MAINPROCESS__
//
//int main() {
//#ifndef __LOCAL
//    freopen("D:\\CodeEdit\\C++\\Compilers Principles\\userin.txt", "r", stdin);
//    freopen("D:\\CodeEdit\\C++\\Compilers Principles\\DFAout.txt", "w", stdout);
//#endif
//
//    string str;
//    std::cin >> str;
//
//    miniDFA miniDfa(str);
//    miniDfa.print();
//    cout << "----------------------\n";
//    miniDfa.get_miniDFA().get_NFA().print();
//    if (miniDfa.match("float")) {
//        cout << "----------------------\n" << "\tmatched\n";
//    } else {
//        cout << "----------------------\n" << "\tunmatched\n";
//    }
//}
//
//#endif
//#endif
