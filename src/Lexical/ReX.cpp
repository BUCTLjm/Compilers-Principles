// Regular_Expression.cpp

#include "Rex.h"
#include "stack"
#include "iostream"
#include "assert.h"
using namespace std;

//判断字符类型
int getchar_type(char c) {
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')
        || (c == '.') || (c == '_') || (c == '+') || (c == '-') || (c == '\\') || (c == '{')
        || (c == '}') || (c == ';') || (c == '=') || (c == '!') || (c == '[') || (c == ']') || (c == '^'))
        return OPERATE_CHARACTER;

    if ((c == '|') || (c == '*') || (c == '&') || (c == '(') || (c == ')'))
        return OPERATE_SYMBOL;

    return ILLEGAL_CHAR;
}

//操作符优先级设定
int get_priority(char c) {
    switch (c) {
        case '*':
            return 3;
            // break;
        case '&':
            return 2;
            // break;
        case '|':
            return 1;
            // break;
        case '(':
            return 0;
            // break;
        default:
            return ILLEGAL_CHAR;
            // break;
    }
}


//检查并预处理正则表达式 添加&作为连接符
string REX::REX_Pretreatment(string &pre_expression) {
    assert(!pre_expression.empty());
    string treated_expression; //处理后表达式

    try {
        char pre_char = pre_expression[0];
        int pre_type = getchar_type(pre_char);
        //首字符输入错误
        if (pre_type == ILLEGAL_CHAR) throw REX_INPUT_EXCEPTION(pre_char);
        if (pre_char != '\\')
            treated_expression.push_back(pre_char);

        char next_char;
        int next_type;
        int len = pre_expression.length();

        for (int i = 1; i < len; ++i) {
            next_char = pre_expression[i];
            next_type = getchar_type(next_char);

            if (next_type == ILLEGAL_CHAR) throw REX_INPUT_EXCEPTION(next_char);

            if (next_char == '\\' && (pre_char == '|' || pre_char == '(')) {
                pre_char = next_char;
                pre_type = next_type;
                continue;
            }

            if (pre_char == '\\') {
                if (next_char == '(') {
                    treated_expression.push_back('[');
                } else if (next_char == ')') {
                    treated_expression.push_back(']');
                } else if (next_char == '\\') {
                    treated_expression.push_back('^');
                } else if (next_char == '*') {
                    treated_expression.push_back('!');
                } else {
                    throw REX_FORMAT_EXCEPTION();
                }
            } else {
                //添加&运算符：当第一位是操作数、*、)且第二位为操作数或(时候添加&
                if (((pre_type == OPERATE_CHARACTER) || (pre_char == '*') || (pre_char == ')')) &&
                    ((next_type == OPERATE_CHARACTER) || (next_char == '('))) {
                    treated_expression.push_back('&');
                }
                treated_expression.push_back(next_char);

            }
            pre_char = next_char;
            pre_type = next_type;
        }


        //return treated_expression;
    }
    catch (const REX_INPUT_EXCEPTION &exception) {
        std::cerr << exception.what() << endl;
        exit(0);
    }
    catch (const exception &exception) {
        std::cerr << exception.what() << endl;
        exit(0);
    }


//    cout << "expression" << treated_expression << endl;
    return treated_expression;
}

//中缀表达式转成后缀表达式
string REX::REX_Infix2Postfix(string &pre_expression) {
    assert(pre_expression.size() > 0);

    string treated_expression;

    //利用栈转换
    try {
        stack<char> op;

        int len = pre_expression.length();

        int next_type;
        char next_char;

        for (int i = 0; i < len; ++i) {
            next_char = pre_expression[i];
            next_type = getchar_type(next_char);

            if (next_type == ILLEGAL_CHAR) throw REX_INPUT_EXCEPTION(next_char);

            // 遇到(入栈
            if (next_char == '(') { op.push(next_char); }

                // 遇到)将运算符出栈并输出，直到遇到(，将(出栈但不输出
            else if (next_char == ')') {
                // 匹配最近(
                while (!op.empty() && op.top() != '(') {
                    treated_expression.push_back(op.top());
                    op.pop();
                }
                // 没有(匹配抛出异常
                if (op.empty()) throw REX_FORMAT_EXCEPTION();

                // 弹出(
                op.pop();
            }
                // 遇到其他运算符：
                // 栈为空则将其压入栈
                // 栈不为空则弹出栈中优先级大于等于当前运算符的运算符并输出，并将当前运算符入栈
            else if (next_type == OPERATE_SYMBOL) {
                int next_priority = get_priority(next_char);
                // 栈不为空,弹出栈中优先级大于等于当前运算符的运算符并输出
                while (!op.empty() && get_priority(op.top()) >= next_priority) {
                    treated_expression.push_back(op.top());
                    op.pop();
                }
                // 当前运算符入栈
                op.push(next_char);
            }
                // 遇到操作数直接输出
            else if (next_type == OPERATE_CHARACTER) {
                treated_expression.push_back(next_char);
            }
        }

        //栈中运算符依次输出
        while (!op.empty()) {
            next_char = op.top();

            //缺少)匹配
            if (next_char == '(') throw REX_FORMAT_EXCEPTION();

            treated_expression.push_back(next_char);
            op.pop();
        }
    }
    catch (const REX_INPUT_EXCEPTION &exception) {
        std::cerr << exception.what() << endl;
        exit(0);
    }
    catch (const REX_FORMAT_EXCEPTION &exception) {
        std::cerr << exception.what() << endl;
        exit(0);
    }
    catch (const std::exception &exception) {
        std::cerr << exception.what() << endl;
        exit(0);
    }
    return treated_expression;
}

//#ifndef __MAINPROCESS__
//
//int main() {
//#ifndef __LOCAL
//    freopen("D:\\Compilers Principles\\src\\Lexical\\tokenin.txt", "r", stdin);
//    freopen("D:\\Compilers Principles\\src\\Lexical\\testout.txt", "w", stdout);
//#endif
//    string str;
//    while(cin>>str){
//        REX reg = REX(str);
//        cout << "postfix rex is :";
//        cout << reg.get_postfix() << endl;
//    }
//    fclose(stdin);//关闭文件
//    fclose(stdout);//关闭文件
//}
//
//#endif
