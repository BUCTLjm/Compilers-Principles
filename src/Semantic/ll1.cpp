//
// Created by OMEN on 2023/6/10.
//
#include "iostream"
#include "ll1.h"
#include <iterator>
#include "list"
#include "vector"
#include "stack"
#include "set"
#include "map"
#include "fstream"
#include "algorithm"
#include "iomanip"

using namespace std;


// test print vector
template<typename T>
void print_vector(vector<T> &vec) {
    auto iter = vec.begin();
    while (iter != vec.end()) {
//        cout << *iter++ << "\n";
        cout << *iter++ << " ";
    }
//    cout << "-----------\n";
    cout << "\n";
}

// 分割字符
vector<string> split_string(string str, char c) {
    vector<string> out;
    out.emplace_back("");
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == c) {
            // 如果不是最后一个字符
            if (i != str.length() - 1)
                out.emplace_back("");
            continue;
        }
        if (str[i] == '\n' || str[i] == '\t')
            continue;
        out.back() += str[i];
    }
    return out;
}

Expression::Expression(vector<string> express) {
    left = express[0]; //expression左部
    right.clear();
    First.clear();
    Follow.clear();
    first_expression.clear();// init
    vector<string> temp; //存储右部表达式，以“|”分界
    // right开始地方为vector[2]
    for (int i = 2; i < express.size(); i++) {
        if (express[i] != "|") {
            temp.push_back(express[i]);
            if (i == express.size() - 1) {
                right.push_back(temp);
            }
        } else {
            if (!temp.empty()) {
                right.push_back(temp);
                temp.clear();
                temp.shrink_to_fit();
            }
        }
    }
    std::sort(right.begin(), right.end(), exp_sort);
}

Expression::Expression(string left_name) {
    left = std::move(left_name);
    right.clear();
    First.clear();
    Follow.clear();
    first_expression.clear();
}

Grammar::Grammar(string input) {
    vector<string> expressions = split_string(std::move(input), '\n');
//    print_vector(expressions);
//    start_word = expressions.front();
    for (const auto &expression: expressions) {
        vector<string> single_exp = split_string(expression, ' ');
        if (expression == expressions.front()) {
            start_word = single_exp[0];
        }
        expression_list.insert(pair<string, Expression>(single_exp[0], single_exp));
    }
}

void Expression::Print_exp() {
    cout << left << " -> ";
    for (auto &row: right) {
        for (const auto &column: row)
            cout << column << " ";
        if (row != right.back()) {
            cout << "| ";
        }
    }
}

void Expression::Print_exp2() {
    cout << left << " -> ";
    for (auto &row: right) {
        for (const auto &column: row)
            cout << column << " ";
        if (row != right.back()) {
            cout << "\n";
            cout << left << " -> ";
        }
    }
}

void Grammar::Print_gra() {
    for (auto &elem: expression_list) {
//        cout<<"-----"<<elem.first<<"\n";
        elem.second.Print_exp();
        cout << "\n";
    }
}

// VN,VT
void Grammar::get_string_set() {
    set<string> temp;
    for (const auto &expression: expression_list) {
        VN.insert(expression.first);
        for (auto right_expression: expression.second.right) {
            temp.insert(right_expression.begin(), right_expression.end());
        }
    }
    for (const auto &x: temp) {
        if (x != "epsilon" && VN.count(x) == 0) {
            VT.insert(x);
        }
    }
    // 做差集
//    set_difference(VN.begin(),VN.end(),temp.begin(),temp.end(), inserter(VT,VT.begin()));
}

// 排序:以right每一行第一个string进行排序
bool exp_sort(const vector<string> &exp1, const vector<string> &exp2) {
    return exp1.front() < exp2.front();
}

//消除左递归
void Grammar::left_recursion() {
    // A -> B a,B -> C D.用B的右部替换A
    map<string, Expression> new_expression_list;
    for (auto iter_i = expression_list.begin(); iter_i != expression_list.end(); ++iter_i) {
        Expression expression_i = iter_i->second;
        for (auto iter_j = expression_list.begin(); iter_j != iter_i; ++iter_j) {
            Expression expression_j = iter_j->second;
            for (auto right_expression_i: expression_i.right) {
                // P_i的右部第一个为P_j
                if (!right_expression_i.empty()) {
                    if (right_expression_i.front() == expression_j.left) {
                        if (right_expression_i.size() > 1) {
                            //B-> C D, right_expression_j: C D + a,加入A -> C D a
                            for (const auto &right_expression_j: expression_j.right) {
                                vector<string> temp = right_expression_j;
                                temp.insert(temp.end(), right_expression_i.begin() + 1, right_expression_i.end());
                                temp.shrink_to_fit();
//                                print_vector(temp);
                                expression_i.right.insert(expression_i.right.end(), temp);
                            }
                            // 删除原来的被替换元素
                            auto iter = std::find(expression_i.right.begin(), expression_i.right.end(),
                                                  right_expression_i);
                            expression_i.right.erase(iter);
                            expression_i.right.shrink_to_fit();
                            std::sort(expression_i.right.begin(), expression_i.right.end(), exp_sort);
                            break;
                        }
                    }
                }
            }
        }
        // test part1
//        expression_i.Print_exp();
//        cout << "\n----------------\n";

        // 加入A'
        string new_left_name = expression_i.left + "1";
        Expression recursion_expression(new_left_name);
        // 添加新终结符,消除左递归式子A -> A B变成A -> B A',A' -> A' | epsilon
        bool flag = false;
        //暂时存储
        vector<vector<string>> i_right;
        // right_expression_k:修改右部后的i的表达式
        for (auto right_expression_k: expression_i.right) {
            // 消除直接左递归
            if (right_expression_k.front() == expression_i.left) {
                flag = true;
                for (auto right_expression_j: expression_i.right) {
                    if (right_expression_j.front() != expression_i.left) {
                        // 对于不是左递归的后加符号
                        vector<string> temp;
                        temp.insert(temp.end(), right_expression_j.begin(), right_expression_j.end());
                        temp.push_back(new_left_name);
//                        print_vector(temp);
                        i_right.push_back(temp);
//                        right_expression_j.push_back(left_1);
                    }
                    // else对应删除左递归，即不将其加入i_right
                }
                recursion_expression.right.push_back(right_expression_k);
            }
        }
        // 没有左递归直接压入temp：i_right中
        if (!flag) {
            for (const auto &right_expression_temp: expression_i.right)
                i_right.push_back(right_expression_temp);
        }
        vector<vector<string>>().swap(expression_i.right);
        expression_i.right.insert(expression_i.right.end(), i_right.begin(), i_right.end());


        // 存储修改后的recursion_expression的right，最后直接赋值即可
        vector<vector<string>> trans_new_right;
        // 处理recursion_expression的左递归A' -> A B 变成 A' -> B A'|epsilon
        if (flag) {
            for (auto left_recursion: recursion_expression.right) {
                vector<string> temp;
                temp.insert(temp.end(), left_recursion.begin() + 1, left_recursion.end());
                temp.push_back(new_left_name);
//                print_vector(temp);
                trans_new_right.push_back(temp);
            }
            std::sort(trans_new_right.begin(), trans_new_right.end(), exp_sort);
            trans_new_right.emplace_back(1, "epsilon");
            recursion_expression.right = trans_new_right;
            recursion_expression.right.shrink_to_fit();
            new_expression_list.insert(pair<string, Expression>(new_left_name, recursion_expression));
        }
        new_expression_list.insert(pair<string, Expression>(expression_i.left, expression_i));
    }

//    expression_list.clear();
    map<string, Expression>().swap(expression_list);
    expression_list.insert(new_expression_list.begin(), new_expression_list.end());
}

//左公因子
void Grammar::left_factor() {
    if (expression_list.empty()) return;
    // map<string,Expression>
    for (const auto &expression: expression_list) {
        set<string> first_set;
        // 对map每一个string提取其value（表达式右部的集合）的第一个元素
        for (auto right_expression: expression.second.right) {
            first_set.insert(right_expression.front());
        }
        // 左部非终结符
        string left_word = expression.first;
        // 遍历set集合，找取公共因子。
        for (const auto &right_first: first_set) {
            vector<vector<string>> same_left_factor; // 有公因子的表达式
            for (auto right_expression: expression.second.right) {
                if (right_expression.front() == right_first)
                    same_left_factor.push_back(right_expression);
            }
            // 有公因子消除
            if (same_left_factor.size() > 1) {
                string new_right_name = expression.first + "1";
                Expression temp(new_right_name); //新增T'表达式
//                string same_exp = right_first;
                //要做消除公因子的表达式右部
                auto *removed_right = &expression_list.find(expression.first)->second.right;
                for (auto have_factor_right: same_left_factor) {
                    if (have_factor_right.size() == 1) {
                        temp.right.emplace_back(1, "epsilon");
                    } else {
                        temp.right.emplace_back(have_factor_right.begin() + 1, have_factor_right.end());
                    }
                    //根据公因式的右部找到删除原来的map里面的值再将其合并
                    for (auto delete_elem = removed_right->begin();
                         delete_elem != removed_right->end(); delete_elem++) {
                        if (*delete_elem == have_factor_right) {
                            removed_right->erase(delete_elem);
                            (*removed_right).shrink_to_fit();
                            break;
                        }
                    }
                }
                vector<string> new_right_exp; // T->TT'
                new_right_exp.push_back(right_first);
                new_right_exp.push_back(new_right_name);
                removed_right->push_back(new_right_exp);
                std::sort(temp.right.begin(), temp.right.end(), exp_sort);
                // T'新表达式入map
                expression_list.insert(pair<string, Expression>(new_right_name, temp));
            }
        }
    }
}

// First集合
void Grammar::first_next(const string &left) {
    // 标记过直接退出
    if (have_first.at(left) | (expression_list.find(left) == expression_list.end())) return;
    for (auto right_expression: expression_list.at(left).right) {
        string first_word = right_expression.front();
        if (VT.count(first_word)) {
            expression_list.at(left).First.insert(first_word);
            expression_list.at(left).first_expression.insert(
                    pair<string, vector<string>>(first_word, right_expression));
        } else if (first_word == "epsilon") {
            expression_list.at(left).First.insert("epsilon");
            expression_list.at(left).first_expression.insert(
                    pair<string, vector<string>>(first_word, right_expression));
        } else {
            // 未标记递归求first
            if (!have_first.at(first_word)) {
                first_next(first_word);
            }
            // 含有epsilon
            if (expression_list.at(first_word).First.count("epsilon")) {
                // 含有时则需要去除空串
                auto new_first_set = expression_list.at(first_word).First;
                new_first_set.erase("epsilon");
                // first_word的first集合加入
                for (const auto &first_word_first: expression_list.at(first_word).First) {
                    expression_list.at(left).First.insert(first_word_first);
                    expression_list.at(left).first_expression.insert(
                            pair<string, vector<string>>(first_word_first, right_expression));
                }
                for (const auto &after_first_word: vector<string>(right_expression.begin() + 1,
                                                                  right_expression.end())) {
//                    first_next(after_first_word);
                    if (VT.count(after_first_word)) {
                        expression_list.at(left).First.insert(after_first_word);
                        expression_list.at(left).first_expression.insert(
                                pair<string, vector<string>>(after_first_word, right_expression));
                        break;
                    } else {
                        if (!have_first.at(after_first_word)) {
                            first_next(after_first_word);
                        }
//                        first_next(after_first_word);
                        if (!expression_list.at(after_first_word).First.count("epsilon")) {
                            // 不含有epsilon
                            for (const auto &after_first_word_first: expression_list.at(after_first_word).First) {
                                expression_list.at(left).First.insert(after_first_word_first);
                                expression_list.at(left).first_expression.insert(
                                        pair<string, vector<string>>(after_first_word_first, right_expression));
                            }
                            break;
                        } else {
                            // 如果是末尾含空串epsilon则需要添加epsilon进first集合里面
                            if (after_first_word == right_expression.back()) {
                                expression_list.at(left).First.insert("epsilon");
                                expression_list.at(left).first_expression.insert(
                                        pair<string, vector<string>>("epsilon", right_expression));
                            }
                            // 含有时则需要去除空串
                            auto new_after_first_set = expression_list.at(after_first_word).First;
                            new_after_first_set.erase("epsilon");
                            // first_word的first集合加入
                            for (const auto &after_first_word_first: new_after_first_set) {
                                expression_list.at(left).First.insert(after_first_word_first);
                                expression_list.at(left).first_expression.insert(
                                        pair<string, vector<string>>(after_first_word_first, right_expression));
                            }
                            continue;
                        }
                    }
                }
            } else {
                // 不含空串直接添加first集合
                for (const auto &first_word_first: expression_list.at(first_word).First) {
                    expression_list.at(left).First.insert(first_word_first);
                    expression_list.at(left).first_expression.insert(
                            pair<string, vector<string>>(first_word_first, right_expression));
                }
            }
        }
    }
    have_first.at(left) = true;
}

// 求解first
void Grammar::first_get() {
    for (const auto &left: VN) {
        have_first.insert(pair<string, bool>(left, false));
    }
    for (const auto &expression: expression_list) {
        first_next(expression.first);
    }
}

void Grammar::print_first() {
    for (const auto &map_expression: expression_list) {
        Expression temp = map_expression.second;
        cout << temp.left << " :{ ";
        for (const auto &x: temp.First) {
            cout << x << " ";
//            cout << x << " ->( ";
//            print_vector(temp.first_expression.at(x));
//            cout << "),";
        }
        cout << "}\n";
    }
}

void Grammar::follow_next(const string &left) {
//    if (have_follow.at(left)) return;
    for (const auto &vn_word: VN) {
        for (auto right_expression: expression_list.at(vn_word).right) {
            auto iter = std::find(right_expression.begin(), right_expression.end(), left);
            if (iter != right_expression.end()) {
                // 位于末尾,加入vn_word的follow集合
                if ((iter + 1) == right_expression.end() && vn_word != left) {
//                    if (!have_follow.at(vn_word))
//                        follow_next(vn_word);
                    set<string> temp = expression_list.at(vn_word).Follow;
                    expression_list.at(left).Follow.insert(temp.begin(), temp.end());
                    continue;
                }
                    // 终结符
                else if ((iter + 1) != right_expression.end() && ((*(iter + 1) == "epsilon") | VT.count(*(iter + 1)))) {
                    string left_next = *(iter + 1);
                    expression_list.at(left).Follow.insert(left_next);
                    continue;
                }
                    // 处于非末尾的非终结符
                else if ((iter + 1) != right_expression.end() && VN.count(*(iter + 1))) {
                    int left_pos_i = 1;
                    string left_next = *(iter + left_pos_i);
                    // 添加first集合除去空串
                    set<string> next_first_set = expression_list.at(left_next).First;
                    if (!next_first_set.count("epsilon")) {
                        expression_list.at(left).Follow.insert(next_first_set.begin(), next_first_set.end());
                        continue;
                    }
                        // 含有空串求解下一个
                    else {
                        next_first_set.erase("epsilon");
                        expression_list.at(left).Follow.insert(next_first_set.begin(), next_first_set.end());
                        if ((iter + left_pos_i + 1) == right_expression.end()) {
                            // 如果走到末尾了,还含有空串，将左边的follow添加至该字符
                            expression_list.at(left).Follow.insert(
                                    expression_list.at(vn_word).Follow.begin(),
                                    expression_list.at(vn_word).Follow.end());
                            continue;
                        }
                        left_pos_i++;
                        left_next = *(iter + left_pos_i);
                        if (VT.count(left_next) | left_next == "epsilon") {
                            expression_list.at(left).Follow.insert(left_next);
                            continue;
                        }
                        set<string>().swap(next_first_set);
                        next_first_set = expression_list.at(left_next).First;
                        while (next_first_set.count("epsilon")) {
                            next_first_set.erase("epsilon");
                            expression_list.at(left).Follow.insert(next_first_set.begin(), next_first_set.end());
                            if ((iter + left_pos_i + 1) == right_expression.end()) {
                                // 如果走到末尾了,还含有空串，将左边的follow添加至该字符
                                expression_list.at(left).Follow.insert(
                                        expression_list.at(vn_word).Follow.begin(),
                                        expression_list.at(vn_word).Follow.end());
                                break;
                            } else {
                                left_pos_i++;
                                left_next = *(iter + left_pos_i);
                                if (VT.count(left_next) | left_next == "epsilon") {
                                    expression_list.at(left).Follow.insert(left_next);
                                    break;
                                }
                                set<string>().swap(next_first_set);
                                next_first_set = expression_list.at(left_next).First;
                            }
                        }
                        expression_list.at(left).Follow.insert(next_first_set.begin(), next_first_set.end());
                    }
                }
            }
        }
    }
}

void Grammar::follow_get() {
    expression_list.at(start_word).Follow.insert("#");
    unsigned long old_size = 0;
    unsigned long long new_size = 0;
    for (const auto &left: VN) {
        old_size += expression_list.at(left).Follow.size();
    }
    // 扩充follow直至不在增加
    while (true) {
        for (const auto &left: VN) {
            follow_next(left);
            new_size += expression_list.at(left).Follow.size();
        }
        if (old_size == new_size) {
            break;
        }
        old_size = new_size;
        new_size = 0;
    }
}

// 打印follow集合
void Grammar::print_follow() {
    for (const auto &map_expression: expression_list) {
        Expression temp = map_expression.second;
        cout << temp.left << " :{ ";
        for (const auto &x: temp.Follow) {
            cout << x << " ";
        }
        cout << "}\n";
    }
}

//建立预测表
void Grammar::create_forecast_table() {
    map<string, map<string, vector<string>>>().swap(forecast_table);
    VT.insert("#");
    // 分析终结符
    for (const auto &vn_word: VN) {
        forecast_table.insert(make_pair(vn_word, map<string, vector<string>>()));
        for (const auto &vt_word: VT) {
            // 如果vt_word在内则直接将对应值放入表
            if (expression_list.at(vn_word).First.count(vt_word)) {
                vector<string> temp = expression_list.at(vn_word).first_expression.at(vt_word);
                forecast_table.at(vn_word).insert(make_pair(vt_word, temp));
            }
                // first集合有epsilon则需要把follow对应的元素，转换成M[A,b]:A-epsilon;
            else if ((expression_list.at(vn_word).First.count("epsilon")) &&
                     (expression_list.at(vn_word).Follow.count(vt_word))) {
                vector<string> temp(1, "epsilon");
                forecast_table.at(vn_word).insert(make_pair(vt_word, temp));
            } else {
                vector<string> temp(1, "ERROR!!");
                forecast_table.at(vn_word).insert(make_pair(vt_word, temp));
            }
        }
    }

}

void Grammar::print_forecast_table() {
    cout << setw(14) << setfill(' ') << left << "\\";
    cout << " | ";
    for (const auto &vt_word: VT) {
        cout << setw(14) << setfill(' ') << left << vt_word;
        cout << " | ";
    }
    cout << "\n";
    for (const auto &vn_word: VN) {
        cout << setw(14) << setfill(' ') << left << vn_word;
        cout << " | ";
        for (const auto &vt_word: VT) {
            auto exp = forecast_table.at(vn_word).at(vt_word);
            // 拼接vector成一个string
            string temp;
            for (const auto &exp_word: exp) {
                temp += exp_word + " ";
            }
            cout << setw(14) << setfill(' ') << left << temp;
            cout << " | ";
        }
        cout << "\n";
    }
}

bool Grammar::analyse_stack(vector<string> expression) {
    stack<string> analyse_stack;
    analyse_stack.emplace("#");
    expression.emplace_back("#");
    analyse_stack.push(start_word);
    if (expression.empty()) return true;
    auto iter_exp = expression.begin();
    string top_word = analyse_stack.top();
    while (top_word != "#") {
        if (iter_exp == expression.end()) {
            return false;
        } else {
            // 异常情况后续抛出异常
            if (analyse_stack.empty()) return false;
            else {
                // 栈顶为终结符
                if (VT.count(top_word) && VT.count(*iter_exp)) {
                    ++iter_exp;
                    analyse_stack.pop();
                    top_word = analyse_stack.top();
                    continue;
                }
                    // 栈顶为空串直接弹出即可
                else if (top_word == "epsilon") {
                    analyse_stack.pop();
                    top_word = analyse_stack.top();
                    continue;
                }
                    // 为非终结符
                else if (VN.count(top_word)) {
                    if (forecast_table.at(top_word).at(*iter_exp).front() == "ERROR!!") {
                        // 异常处理
                        return false;
                    }
                        // move[A,a]:A->Ba;
                    else {
                        auto push_word = forecast_table.at(top_word).at(*iter_exp);
                        analyse_stack.pop();
                        // 产生式逆序压入栈
                        for (auto elem = push_word.rbegin(); elem != push_word.rend(); ++elem) {
                            analyse_stack.push(*elem);
                        }
                        top_word = analyse_stack.top();
                        continue;
                    }
                } else return false;
            }
        }
    }
    if (top_word == "#" && *iter_exp == "#") {
        return true;
    } else return false;
}

bool Grammar::is_ll() {
    for (const auto &vn_word: VN) {
        if (expression_list.at(vn_word).First.count("epsilon")) {
            set<string> set_temp;
            std::set_difference(expression_list.at(vn_word).First.begin(),
                                expression_list.at(vn_word).First.end(), expression_list.at(vn_word).Follow.begin(),
                                expression_list.at(vn_word).Follow.end(), std::inserter(set_temp, set_temp.begin()));
            if (set_temp.size() < expression_list.at(vn_word).First.size()) {
                return false;
            }
        }
    }
    return true;
}


