//
// Created by OMEN on 2023/6/14.
//
#include "iostream"
#include "semantic.h"
#include <iterator>
#include "list"
#include "vector"
#include "stack"
#include "set"
#include "map"
#include "fstream"
#include "algorithm"
#include "iomanip"
#include "ll1.h"
#include "ll1.cpp"

using namespace std;

// 建立抽象语法树
void create_syntax_tree(stack<string> &analysis_stack, vector<string> &input, syntax_tree *cur_node) {
    if (analysis_stack.empty()) return;
    if (cur_node == nullptr) return;
    if (input.empty()) return;

    // 栈顶元素
    string top_word = analysis_stack.top();

//    cout << top_word << ":\tstack:\t";
//    auto temp = analysis_stack;
//    while (!temp.empty()) {
//        cout << temp.top() << " ";
//        temp.pop();
//    }
//    cout << "\nvector:\t";
//    vector<string> test_vector(input.begin(), input.end());
//    print_vector(test_vector);
//    cout << "------------------\n";

    // 先考虑非终结符展开
    if (gr.VN.count(analysis_stack.top())) {
        auto push_word = gr.forecast_table.at(top_word).at(input.front());
        analysis_stack.pop();
        // 产生式逆序压入栈
        for (auto elem = push_word.rbegin(); elem != push_word.rend(); ++elem) {
            analysis_stack.push(*elem);
        }
        for (auto &it: push_word) {
            auto *node = new syntax_tree(it);
            cur_node->child.push_back(node);
        }
        for (int i = 0; i < push_word.size(); ++i) {
            // 对每个节点的子节点进行遍历
            create_syntax_tree(analysis_stack, input, cur_node->child[i]);
        }
    }

    //是epsilon则直接弹掉
    if (top_word == "epsilon") {
        analysis_stack.pop();
    }
    // 匹配非终结符
    while (input.begin() != input.end() && top_word == input.front()) {
        input.erase(input.begin());
        input.shrink_to_fit();
        analysis_stack.pop();
    }

    // 再次定义以防止递归找不到结束点,维护修改后的输入字符串以及栈
    if (analysis_stack.empty()) return;
    if (gr.VN.count(analysis_stack.top()))return;
}

//打印语法树
void print_syntax_tree(syntax_tree *node, int deep) {
    for (int i = 0; i <= deep; i++) {
//        cout << "\t";
        cout << "  ";
    }
    cout << deep << "." << node->name << "\n";
//    cout << node->name << "\n";

    for (auto &i: node->child) {
        print_syntax_tree(i, deep + 1);
    }
}

vector<string> get_child_expression(syntax_tree *node) {
    vector<string> res;
    if (node == nullptr) return res;
//    res.push_back(node->name);
    if (node->child.empty()) return res;
    for (auto x: node->child) {
        res.push_back(x->name);
    }
    return res;
}

//添加op-stmt语义分析
void creat_op_stmt(syntax_tree *node) {
    if (node == nullptr) return;
    if (node->child.empty()) return;
    else {
//        cout << "name : " << index->name << "\tval : " << index->value << "\n";
        vector<string> node_expression = get_child_expression(node);
        // op-stmt -> term op-stmt1 ==> op-stmt1.i=term.val , op-stmt.val=op-stmt1.s
        if (node_expression == vector<string>({"term", "op-stmt1"})) {
            creat_op_stmt(node->child[0]);
            node->child[1]->node_i = node->child[0]->value;
            creat_op_stmt(node->child[1]);
            node->value = node->child[1]->node_s;
        }
            // op-stmt1 -> + term op-stmt1' ==> op-stmt1'.i=op-stmt1.i+term.val , op-stmt1.s=op-stmt1'.s
        else if (node_expression == vector<string>({"+", "term", "op-stmt1"})) {
            creat_op_stmt(node->child[1]);
            node->child[2]->node_i = node->node_i + node->child[1]->value;
            creat_op_stmt(node->child[2]);
            node->node_s = node->child[2]->node_s;
        }
            // op-stmt1 -> - term op-stmt1' ==> op-stmt1'.i=op-stmt1.i-term.val , op-stmt1.s=op-stmt1'.s
        else if (node_expression == vector<string>({"-", "term", "op-stmt1"})) {
            creat_op_stmt(node->child[1]);
            node->child[2]->node_i = node->node_i - node->child[1]->value;
            creat_op_stmt(node->child[2]);
            node->node_s = node->child[2]->node_s;
        }
            // op-stmt1 -> epsilon ==> op-stmt1.s=op-stmt1.i
        else if (node_expression == vector<string>({"epsilon"})) {
            node->node_s = node->node_i;
        }
            // term -> factor term1 ==> term1.i=factor.val, term.val=term1.s
        else if (node_expression == vector<string>({"factor", "term1"})) {
            creat_op_stmt(node->child[0]);
            node->child[1]->node_i = node->child[0]->value;
            creat_op_stmt(node->child[1]);
            node->value = node->child[1]->node_s;
        }
            // term1 -> * factor term1' ==> term1'.i=term.i*factor.val , term.s=term'.s
        else if (node_expression == vector<string>({"*", "factor", "term1"})) {
            creat_op_stmt(node->child[1]);
            node->child[2]->node_i = node->node_i * node->child[1]->value;
            creat_op_stmt(node->child[2]);
            node->node_s = node->child[2]->node_s;
        }
            // term1 -> epsilon ==> term1.s=term1.i
//        else if (node_expression == vector<string>({"epsilon"})) {
//            node->node_s = node->node_i;
//        }
            // factor -> ( op-stmt ) ==> factor.val=op-stmt.val
        else if (node_expression == vector<string>({"(", "op-stmt", ")"})) {
            creat_op_stmt(node->child[1]);
            node->value = node->child[1]->value;
        }
            // factor -> id ==> factor.val=id.val
        else if (node_expression == vector<string>({"id"})) {
            creat_op_stmt(node->child[0]);
            node->value = node->child[0]->value;
        }
            // factor -> num ==> factor.val=num.val
        else if (node_expression == vector<string>({"num"})) {
            creat_op_stmt(node->child[0]);
            node->value = node->child[0]->value;
        }
            // num -> 0 ==> num.val = 0
        else if (node_expression == vector<string>({"0"})) {
            node->value = 0;
        }
            // num -> 1 ==> num.val = 1
        else if (node_expression == vector<string>({"1"})) {
            node->value = 1;
        }
            // num -> 2 ==> num.val = 2
        else if (node_expression == vector<string>({"2"})) {
            node->value = 2;
        }
            // num -> 3 ==> num.val = 3
        else if (node_expression == vector<string>({"3"})) {
            node->value = 3;
        }
            // num -> 4 ==> num.val = 4
        else if (node_expression == vector<string>({"4"})) {
            node->value = 4;
        }
            // num -> 5 ==> num.val = 5
        else if (node_expression == vector<string>({"5"})) {
            node->value = 5;
        }        // num -> 6 ==> num.val = 6
        else if (node_expression == vector<string>({"6"})) {
            node->value = 6;
        }
            // num -> 7 ==> num.val = 7
        else if (node_expression == vector<string>({"7"})) {
            node->value = 7;
        }
            // num -> 8 ==> num.val = 8
        else if (node_expression == vector<string>({"8"})) {
            node->value = 8;
        }
            // num -> 9 ==> num.val = 9
        else if (node_expression == vector<string>({"9"})) {
            node->value = 9;
        }
            // id -> a ==> id.val = a.val
        else if (node_expression == vector<string>({"a"})) {
            bool flag = false;
            for (const auto &x: table) {
                if (x.node_name == "a") {
                    node->value = x.node_val;
                    flag = true;
                }
            }
            if (!flag) {
                cout << "a is not defined but use it\n";
            }
        }
            // id -> b ==> id.val = b.val
        else if (node_expression == vector<string>({"b"})) {
            bool flag = false;
            for (const auto &x: table) {
                if (x.node_name == "b") {
                    node->value = x.node_val;
                    flag = true;
                }
            }
            if (!flag) {
                cout << "b is not defined but use it\n";
            }
        }
            // id -> c ==> id.val = c.val
        else if (node_expression == vector<string>({"c"})) {
            bool flag = false;
            for (const auto &x: table) {
                if (x.node_name == "c") {
                    node->value = x.node_val;
                    flag = true;
                }
            }
            if (!flag) {
                cout << "c is not defined but use it\n";
            }
        }
            // id -> d ==> id.val = d.val
        else if (node_expression == vector<string>({"d"})) {
            bool flag = false;
            for (const auto &x: table) {
                if (x.node_name == "d") {
                    node->value = x.node_val;
                    flag = true;
                }
            }
            if (!flag) {
                cout << "d is not defined but use it\n";
            }
        } else return;
    }
}


void symbol_node::add_assign_stmt(vector<string> input) {
    node_val = 0;
    node_name = input.back();
    node_type = input.front();
}

// operation-stmt:建立抽象语法语法树
void symbol_node::add_operation_stmt(vector<string> input) {
    stack<string> analysis_stack;
    auto *node = new syntax_tree("op-stmt");
    auto cur = node;
    analysis_stack.emplace("op-stmt");
    create_syntax_tree(analysis_stack, input, cur);
//    print_syntax_tree(cur, 0);
    // 对抽象语法树添加语义分析
    cur = node;
    creat_op_stmt(cur);
    node_val = node->value;
}

void analysis_semantic() {

    auto test_string = readAllFile(R"(D:\Compilers Principles\src\Semantic\code.txt)");
    vector<string> test_vector_string = split_string(test_string, '\n');
//    print_vector(test_vector_string);
    for (auto row = 0; row < test_vector_string.size(); ++row) {
        vector<string> row_code = split_string(test_vector_string.at(row), ' ');
//        print_vector(row_code);
        auto statement = gr.forecast_table.at("statement").at(row_code.front());
        if (statement == vector<string>{"assign-stmt"}) {
            auto temp_symbol = symbol_node();
            temp_symbol.add_assign_stmt(row_code);
            table.push_back(temp_symbol);
        } else if (statement == vector<string>{"operation-stmt"}) {
//            auto op_stmt = gr.forecast_table.at("operation-stmt").at(row_code.front());
            bool flag = false;
            for (int i = 0; i < table.size(); ++i) {
                if (table[i].node_name == row_code.front()) {
                    vector<string> temp(row_code.begin() + 2, row_code.end());
                    temp.emplace_back("#");
//                    print_vector(temp);
                    table[i].add_operation_stmt(temp);
                    flag = true;
                }
            }
            if (!flag) {
                cout << "-----------ERROR!!!!!!!!-----------------\n";
                cout << row_code.front() << " is not defined\n";
            }
        } else if (statement == vector<string>{"if-stmt"} | statement == vector<string>{"repeat-stmt"}) {
            cout << "row:" << row + 1 << "正在开发中....\n";
        } else {
            cout << "ERROR!!!!!!!!!!!!\n";
        }
    }
}

// 查看符号表
void print_table() {
    for (const auto &x: table) {
        cout << "id_name: " << x.node_name << "\ttype: " << x.node_type << "\tval: " << x.node_val << "\n";
    }
}

int main() {
    // 语法规则
    gr.left_recursion();
    gr.left_factor();
//    gr.Print_gra();
    gr.get_string_set();
    gr.first_get();
    gr.follow_get();
    gr.create_forecast_table();


    analysis_semantic();
    cout << "------------symbol table------------\n";
    print_table();
//    auto push_word = vector<string>{"9", "-", "(", "5", "+", "2", ")", "#"};
//    auto push_word = vector<string>{"if", "(", "a", "==", "3", ")", "{", "a", "=", "4", "}", "#"};
//    auto push_word = vector<string>{"a", "=", "5", "+", "2", "#"};
//    auto push_word = vector<string>{"a", "=", "8", "#"};

//    stack<string> test;
//    test.emplace("op-stmt");
//    auto *node = new syntax_tree("op-stmt");
//    create_syntax_tree(test, push_word, node);
//    print_syntax_tree(node, 0);

//    symbol_node table1(0, "a", "int", "");
//    table1.add_operation_stmt(vector<string>(push_word.begin() + 2, push_word.end()));
//    print_vector(push_word);
//    cout << "------------result------------\n";
//    cout << table1.node_val;

}


