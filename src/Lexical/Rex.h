//
// Created by OMEN on 2023/6/9.
//

#ifndef COMPILERS_PRINCIPLES_REX_H
#define COMPILERS_PRINCIPLES_REX_H

#include <utility>

#include "iostream"

using namespace std;

const int OPERATE_CHARACTER = 1;//操作符
const int OPERATE_SYMBOL = 2;//运算符
const int ILLEGAL_CHAR = -1;   //非法字符

int getchar_type(char c);

int get_priority(char c);

//REX包含非法字符错误
class REX_INPUT_EXCEPTION {
public:
    char c;

    explicit REX_INPUT_EXCEPTION(char c) {
        this->c = c;
    };

    string what() const {
        string ret;
        ret = c + "is not in the charset";
        return ret;
    };
};

//REX格式错误
class REX_FORMAT_EXCEPTION {
public:
    REX_FORMAT_EXCEPTION() = default;

    string what() const {
        string ret = "Format Error ";
        return ret;
    }
};

class REX {
public:

    explicit REX(string s) {
        expression = std::move(s);
        expression = REX_Pretreatment(expression);
//        std::cout << expression << endl;
        expression = REX_Infix2Postfix(expression);
    }

    string get_postfix() {
        return expression;
    }

private:
    string expression;

    string REX_Pretreatment(string &);

    string REX_Infix2Postfix(string &);
};

#endif //COMPILERS_PRINCIPLES_REX_H
