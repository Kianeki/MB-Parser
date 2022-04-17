#include <iostream>
#include "CFG.h"
#include "FSM.h"
#include "Parser.h"
int main() {
    CFG cfg("input5.json");
//    cfg.print();
//    FSM fsm(cfg);
//    fsm.toDot();
    Parser parser(cfg);
//    parser.readParse("input.txt");
    parser.parse("input2.txt");
//    auto first = cfg.firstSet();
//    auto follow = cfg.followsSet();
//    set<string> s{"auto","bus","aa"};
//    cout<<*s.begin() <<" ";
//    string s1 = "abs";
//    string s2 = "Abs";
//    if(s1 == s2){
//        cout<<"equal";
//    }
    return 0;
}
