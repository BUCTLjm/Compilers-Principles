//
// Created by OMEN on 2023/6/14.
//

#ifndef COMPILERS_PRINCIPLES_SEMANTIC_H
#define COMPILERS_PRINCIPLES_SEMANTIC_H

#include "iostream"
#include "semantic.h"
#include <iterator>
#include <utility>
#include "list"
#include "vector"
#include "stack"
#include "set"
#include "map"
#include "fstream"
#include "algorithm"
#include "iomanip"
#include "ll1.h"

using namespace std;

string str = readAllFile(R"(D:\Compilers Principles\src\Semantic\grammar.txt)");
Grammar gr(str);

// 抽象语法树
struct syntax_tree {
    string name;
    double value;
    double node_i; // 继承属性
    double node_s; // 综合属性
    vector<syntax_tree *> child; // 子节点，传继承属性
    string parent; // 父节点值
    string sibling; //兄弟节点值
//    vector<syntax_tree *> siblings; // 兄弟节点，传综合属性
    explicit syntax_tree(string _name, double _value = 0, double _i = 0, double _s = 0) :
            name(std::move(_name)),
            value(_value),
            node_i(_i),
            node_s(_s) {
        vector<syntax_tree *>().swap(child);
    }
};

// 符号表
class symbol_node {
private:
public:
    double node_val; // 值
    string node_name; // 符号名
    string node_type; // 符号属性
    string jump_to; // 跳转的地方到哪里去

    explicit symbol_node(double _value = 0, string _name = "", string _type = "", string _jump = "") :
            node_val(_value),
            node_name(std::move(_name)),
            node_type(std::move(_type)),
            jump_to(std::move(_jump)) {};

    // assign-stmt:最朴素的办法直接手动语义分析
    void add_assign_stmt(vector<string> input);

    // operation-stmt:建立抽象语法语法树
    void add_operation_stmt(vector<string> input);

};

// 符号表顺序存储
vector<symbol_node> table;

void print_table();

//分析程序，构造抽象语法树
void analysis_semantic();

//打印语法树
void print_syntax_tree(syntax_tree *node, int deep);

// 获取节点表达式转换关系
vector<string> get_child_expression(syntax_tree *node);

// 建立抽象语法树
void create_syntax_tree(stack<string> &analysis_stack, vector<string> &input, syntax_tree *cur_node);

//添加语义分析
void creat_op_stmt(syntax_tree *node);

#endif //COMPILERS_PRINCIPLES_SEMANTIC_H
