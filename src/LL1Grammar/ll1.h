//
// Created by OMEN on 2023/6/10.
//

#ifndef LEXICAL_LL1_H
#define LEXICAL_LL1_H

#include <utility>

#include "iostream"
#include "vector"
#include "stack"
#include "set"
#include "map"
#include "fstream"
#include "algorithm"

using namespace std;

// 根据字符c分割str
vector<string> split_string(string str, char c);


//单条表达式->一条处理后的语法规则
class Expression {
public:
    string left;
    vector<vector<string>> right;
    set<string> First;
    set<string> Follow;

    // first对应产生式
    map<string, vector<string>> first_expression;

    explicit Expression(string left_name);

    explicit Expression(vector<string> express);

    //打印 例如：T -> E T | E
    void Print_exp();

    //打印 例如：T -> E T,T -> E
    void Print_exp2();

    //判断表达式缺失
    bool judge_empty() const {
        return this->left.empty() | right.empty();
    }

};


// 以左部字母大小排序
bool exp_sort(const vector<string> &exp1, const vector<string> &exp2);


class Grammar {
private:
    // 标记first是否求解过
    map<string, bool> have_first;
    // 标记follow是否求解过
    map<string, bool> have_follow;


public:
    // 构造LL1文法表达式
    explicit Grammar(string input);

    void Print_gra();

    //消除左公因子
    void left_factor();

    //消除左递归
    void left_recursion();

    // 文法规则总集，first为表达式左侧非终结符，second为该表达式类
    map<string, Expression> expression_list;

    // 递归求解first集合
    void first_next(const string &left);

    // VN:非终结符，VT:终结符
    set<string> VN, VT;

    // 文法规则开始符号
    string start_word;

    // 预测分析表
    map<string, map<string, vector<string>>> forecast_table;

    void get_string_set();

    void first_get();

    void print_first();

    void print_follow();

    void follow_get();

    void follow_next(const string &left);

    // 判断是否符合文法要求
    bool is_ll();

    void create_forecast_table();

    void print_forecast_table();

    bool analyse_stack(vector<string> expression);
};

// 读入文件
string readAllFile(const string &pathFile) {
    ifstream infile;
    infile.open(pathFile);
    // 打开失败，路径不正确
    if (!infile)
        cout << "Open File Fail!" << endl;
    // 读取文本内容到字符串
    string readStr((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());

    return readStr;
}

#endif //LEXICAL_LL1_H
