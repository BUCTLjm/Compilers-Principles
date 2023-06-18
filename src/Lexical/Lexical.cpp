//
// Created by OMEN on 2023/4/17.
//
//Main_Process.cpp
#ifndef __Lexical__
#define __Lexical__

#include<bits/stdc++.h>
#include "Rex.cpp"
#include "NFA.cpp"
#include "DFA.cpp"
#include "miniDFA.h"
#include "miniDFA.cpp"

using namespace std;

const string digit = "(0|1|2|3|4|5|6|7|8|9)";
const string pos_digit = "(1|2|3|4|5|6|7|8|9)";
const string symbol = "+|-|\\*|=";
const string low_word = "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z)";
//const string up_word = "(A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)";
const string identifier = "(_|" + low_word + ")(_|" + digit + "|" + low_word + ")*";
const string integer = pos_digit + digit + "*";
const string decimal = integer + "." + digit + "*";
const string KeyWord = "(main)|(int)|(float)|(if)|(else)|(for)|(while)|(return)|(void)";
const string separator = "\\(|\\)|{|}|;";
#ifndef __lexDFA__
vector<miniDFA> lexical = vector<miniDFA>();
miniDFA symbolDFA(symbol);
miniDFA idDFA(identifier);
miniDFA integerDFA(integer);
miniDFA decimalDFA(decimal);
miniDFA keywordDFA(KeyWord);
miniDFA seperatorDFA(separator);
#endif


string getName(int index) {
    if (index == 0) return "KeyWord";
    if (index == 1) return "separator";
    if (index == 2) return "symbol";
    if (index == 3) return "identifier";
    if (index == 4) return "integer";
    if (index == 5) return "decimal";

    return "ERROR!!!!";
}

int main() {

//#ifdef __LOCAL
//    freopen("D:\\CodeEdit\\C++\\Compilers Principles\\tokenin.txt", "r", stdin);
//    freopen("D:\\CodeEdit\\C++\\Compilers Principles\\tokenout.txt", "w", stdout);
//#endif
    ifstream token_in_file("D:\\Compilers Principles\\src\\Lexical\\tokenin.txt");
    ofstream token_out_file("D:\\Compilers Principles\\src\\Lexical\\tokenout.txt");


    lexical.push_back(KeyWord);
    lexical.push_back(seperatorDFA);
    lexical.push_back(symbolDFA);
    lexical.push_back(idDFA);
    lexical.push_back(integerDFA);
    lexical.push_back(decimalDFA);



//    string code = "";
    vector<string> code_list;
    if (token_in_file.is_open()) {
        string str;
        while (token_in_file >> str) {
            code_list.push_back(str);
        }
        token_in_file.close();
    }

    if (token_out_file.is_open()) {
        for (auto lex: code_list) {
//            cout << lex << "     ";
            for (int i = 0; i < lexical.size(); i++) {
                if (lexical[i].match(lex)) {
                    token_out_file << "( " << lex << ", " << getName(i) << " )\n";
                    break;
                }
            }
        }
        token_out_file.close();
    }

}

#endif
