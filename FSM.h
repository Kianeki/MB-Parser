//
// Created by Kiani on 29/03/2022.
//

#ifndef MB2_FSM_H
#define MB2_FSM_H
#include "iostream"
#include "set"
#include "CFG.h"
#include "list"
#include <fstream>
using namespace std;

//class Rule{
//private:
//    string LHS;
//    vector<string> RHS;
//    int marker;
//};
class State{
public:
    void addTransition(const string& symbol, State* toState){
        transitions.insert(pair<string,State*>(symbol,toState));
    }
    void addRule(const pair<int,pair<string,vector<string>>>& rule){
        closure.insert(closure.end(),rule);
    }
    pair<int,pair<string,vector<string>>> getStartRule() const{
//        cout<< (*closure.begin()).second.first;
        return *closure.begin();
    }
    set<pair<int,pair<string,vector<string>>>> getClosure() const{
        return closure;
    }

    const set<pair<string, State *>> &getTransitions() const {
        return transitions;
    }

    int getId() const {
        return id;
    }

    void setId(int id) {
        State::id = id;
    }

    void setFinal(bool final) {
        State::final = final;
    }

    void setClosure(const set<pair<int, pair<string, vector<string>>>> &closure) {
        State::closure = closure;
    }

    bool isFinal() const {
        return final;
    }

    void print(){
        cout<< "ID = "<<id<<endl;
        cout<< "Final = "<<final<<endl;
        for(const auto& p1 : closure){
            cout<< p1.second.first<<"->";
            for(int i=0; i<p1.second.second.size(); i++){
                if(i == p1.first){
                    cout<<".";
                }
                cout<<" "<<p1.second.second[i];
            }
            if(p1.first == p1.second.second.size()){
                cout<<".";
            }
            cout<<endl;

        }
        cout<< "transitions:" <<endl;
        for(const auto& transition: transitions){
            cout<< transition.first <<" goes to" << transition.second->getId()<<endl;
        }
        cout<<"----------------------"<<endl;

    }
private:
    int id=1;
    bool final = false;
    set<pair<string,State*>> transitions;
//    set<pair<string,vector<string>>> closure;
    set<pair<int,pair<string,vector<string>>>> closure;
//    bool accept = false;
};
class FSM {
private:
    CFG grammar;
    vector<State*> states;
    map<string,set<string>> followSet;
public:
    explicit FSM(CFG grammar): grammar(std::move(grammar)){
        followSet = FSM::grammar.followsSet();
        construct();
    };

    const CFG &getGrammar() const {
        return grammar;
    }

    void construct(){
        State* startState = new State();
        states.push_back(startState);
        pair<string,vector<string>> augRule(grammar.getS()+"'",vector<string>{grammar.getS()});
        startState->addRule(pair<int,pair<string,vector<string>>>(0,augRule));
//        calcClosure(startState);
        calcStates(startState);
        for(const auto& state: states){
            state->print();
        }
    };

    set<pair<int,pair<string,vector<string>>>> calcClosure(set<pair<int,pair<string,vector<string>>>>& itemSet){
        set<pair<int,pair<string,vector<string>>>> newItemSet1;
        bool allterm = true;
        for(const auto& rule: itemSet){
            for(const auto& prod: grammar.getP()){
                if(prod.first == rule.second.second[rule.first] /*&& prod.second[0] != rule.second.first*/){ //
                    pair<string,vector<string>> newProduction(prod.first,vector<string>{prod.second});
//                    auto newRule = pair<int,pair<string,vector<string>>>(0,newProduction);
                    if(itemSet.find(pair<int,pair<string,vector<string>>>(0, newProduction))==itemSet.end()){
                        newItemSet1.insert(pair<int,pair<string,vector<string>>>(0, newProduction));
                    }
//                    s->addRule(pair<int,pair<string,vector<string>>>(0,newProduction));
                }
                else{
                    allterm = false;
                }
            }
        }
        if(allterm){
            return {};
        }
        else{
            set<pair<int,pair<string,vector<string>>>> newItemSet2 = calcClosure(newItemSet1);
            itemSet.insert(newItemSet2.begin(), newItemSet2.end());
            return itemSet;
        }
    }

    void calcStates(State* s) {
        set<pair<int, pair<string, vector<string>>>> temp = s->getClosure();
        calcClosure(temp);
        s->setClosure(temp);
//        s->print();
        if (temp.size() == 1 && (*temp.begin()).first == (*temp.begin()).second.second.size()) { // is final
            s->setFinal(true);
            return;
        }

        for (const auto &variable: grammar.getV()) {
            set<pair<int, pair<string, vector<string>>>> itemSet;
            set<pair<int, pair<string, vector<string>>>> oldItemSet = s->getClosure();
            for (const auto &rule: oldItemSet) {
                if (variable == rule.second.second[rule.first]) {
                    pair<int, pair<string, vector<string>>> r(rule.first + 1,
                                                              pair<string, vector<string>>(rule.second.first,
                                                                                           rule.second.second));
                    itemSet.insert(r);

//                    bool duplicate = false;
//                    for (const auto &state: states) {
//                        set<pair<int, pair<string, vector<string>>>> closure = state->getClosure();
//                        if (state->getStartRule() == r && closure.size() == 1) {
//                            duplicate = true;
//                            s->addTransition(variable, state); //add transition to existing state
//                            break;
//                        }
////                        if (closure.find(r) != closure.end()) { //duplicate state check
////                            duplicate = true;
////                            s->addTransition(variable, state); //add transition to existing state
////                            break;
////                        }
//                    }
//                    if (!duplicate) {
//                        itemSet.insert(r);
//                    }
                }
            }
            if(!itemSet.empty()){
                bool duplicate = true;
                for(const auto& state: states){
                    duplicate = true;
                    const auto clos = state->getClosure();
                    for(const auto& rule : itemSet){
                        if(clos.find(rule)==clos.end()){
                            duplicate = false;
                        }
                    }
                    if(duplicate){
                        s->addTransition(variable, state); //add transition to existing state
                        itemSet.clear();
                        break;
                    }
                }
            }
            if (!itemSet.empty()) {
            State *newState = new State();
            states.push_back(newState);
            newState->setId(states.size());
            for (const auto &r: itemSet) {
                newState->addRule(r);
                s->addTransition(r.second.second[r.first - 1], newState);
            }
            calcStates(newState);
            }
        }
        // now terminals
        for(const auto& terminal : grammar.getT()){
            set<pair<int,pair<string,vector<string>>>> itemSet;
            set<pair<int,pair<string,vector<string>>>> oldItemSet = s->getClosure();
            for(const auto& rule : oldItemSet){
                if(terminal == rule.second.second[rule.first] ){
                    pair<int,pair<string,vector<string>>> r(rule.first+1,pair<string,vector<string>>(rule.second.first,rule.second.second));
                    itemSet.insert(r);
//                    bool duplicate = false;
//                    for(const auto& state : states){
//                        set<pair<int,pair<string,vector<string>>>> closure = state->getClosure();
//                        if(state->getStartRule() == r && closure.size() == 1){
//                            duplicate = true;
//                            s->addTransition(terminal, state); //add transition to existing state
//                            break;
//                        }
//                    }
//                    if(!duplicate){
//                        itemSet.insert(r);
//                    }
                }
            }
            if(!itemSet.empty()){
                bool duplicate = true;
                for(const auto& state: states){
                    duplicate = true;
                    const auto clos = state->getClosure();
                    for(const auto& rule : itemSet){
                        if(clos.find(rule)==clos.end()){
                            duplicate = false;
                        }
                    }
                    if(duplicate){
                        s->addTransition(terminal, state); //add transition to existing state
                        itemSet.clear();
                        break;
                    }
                }
            }
            if(!itemSet.empty()) {
                State *newState = new State();
                states.push_back(newState);
                newState->setId(states.size());
                for (const auto &rule : itemSet) {
                    newState->addRule(rule);
                    s->addTransition(rule.second.second[rule.first-1],newState);
                }
                calcStates(newState);
            }
        }
    }

//    void calcStates(State* s){
//        set<pair<int,pair<string,vector<string>>>> temp = s->getClosure();
//        calcClosure(temp);
//        s->setClosure(temp);
////        s->print();
//        if(temp.size()==1 && (*temp.begin()).first == (*temp.begin()).second.second.size()){ // is final
//            s->setFinal(true);
//            return;
//        }
//
//        for(const auto& variable : grammar.getV()){
//            set<pair<int,pair<string,vector<string>>>> itemSet;
//            set<pair<int,pair<string,vector<string>>>> oldItemSet = s->getClosure();
//            for(const auto& rule : oldItemSet) {
//                if (variable == rule.second.second[rule.first]) {
//                    pair<int, pair<string, vector<string>>> r(rule.first + 1,
//                                                              pair<string, vector<string>>(rule.second.first,
//                                                                                           rule.second.second));
//                    bool duplicate = false;
//                    for (const auto &state : states) {
//                        set<pair<int, pair<string, vector<string>>>> closure = state->getClosure();
//                        if(state->getStartRule() == r && closure.size() == 1){
//                            duplicate = true;
//                            s->addTransition(variable, state); //add transition to existing state
//                            break;
//                        }
////                        if (closure.find(r) != closure.end()) { //duplicate state check
////                            duplicate = true;
////                            s->addTransition(variable, state); //add transition to existing state
////                            break;
////                        }
//                    }
//                    if (!duplicate){
//                        itemSet.insert(r);
//                    }
//                }
//            }
//
//            if (!itemSet.empty()) {
//                State *newState = new State();
//                states.push_back(newState);
//                newState->setId(states.size());
//                for (const auto &r : itemSet) {
//                    newState->addRule(r);
//                    s->addTransition(r.second.second[r.first-1],newState);
//                }
//                //add transition
//                calcStates(newState);
//            }
//        }
//        for(const auto& terminal : grammar.getT()){
//            set<pair<int,pair<string,vector<string>>>> itemSet;
//            set<pair<int,pair<string,vector<string>>>> oldItemSet = s->getClosure();
//            for(const auto& rule : oldItemSet){
//                if(terminal == rule.second.second[rule.first] ){
//                    pair<int,pair<string,vector<string>>> r(rule.first+1,pair<string,vector<string>>(rule.second.first,rule.second.second));
//                    bool duplicate = false;
//                    for(const auto& state : states){
//                        set<pair<int,pair<string,vector<string>>>> closure = state->getClosure();
//                        if(state->getStartRule() == r && closure.size() == 1){
//                            duplicate = true;
//                            s->addTransition(terminal, state); //add transition to existing state
//                            break;
//                        }
////                        if(closure.find(r) != closure.end()){ //duplicate state check
////                            duplicate = true;
////                            // add transition to existing state
////                            s->addTransition(terminal, state);
////                            break;
////                        }
//                    }
//                    if(!duplicate){
//                        itemSet.insert(r);
//                    }
//                }
//            }
//            if(!itemSet.empty()) {
//                State *newState = new State();
//                states.push_back(newState);
//                newState->setId(states.size());
//                for (const auto &rule : itemSet) {
//                    newState->addRule(rule);
//                    s->addTransition(rule.second.second[rule.first-1],newState);
//                }
//                calcStates(newState);
//            }
//        }
//    }
    void toDot(){
        // Create and open a text file
        ofstream MyFile("graph.gv");

        // Write to the file
        MyFile << "digraph graphname {"<<endl;
        for(const auto& state: states){
            MyFile<<'\t'<<state->getId() <<"[label= \"";
            MyFile<<state->getId()<<" \\n ";
            for(const auto& rule: state->getClosure()){
                MyFile<<rule.second.first<< " -> ";
                for(int i = 0; i<rule.second.second.size();i++){
                    if(rule.first == i){
                        MyFile<<".";
                    }
                    MyFile<<rule.second.second[i]<<" ";
                    if(rule.first == i+1 && rule.second.second.size() == i+1){
                        MyFile<<".";
                    }
                }
                MyFile<<"\\n ";
            }
            MyFile<<"\"];"<<endl;
        }
        for(const auto& state:states){
            for(const auto& tran: state->getTransitions()){
                MyFile<<"\t"<<state->getId()<<" -> "<< tran.second->getId()<< " [ label = \""<<tran.first<<"\"];"<<endl;
            }
        }
        // Close the file
        MyFile<<"}";
        MyFile.close();
    }
    bool validReduction(const string& var, const string& term){ // checks if terminal is in Follow(var)
        if(followSet[var].find(term)!= followSet[var].end()){ //valid reduction
            return true;
        }
        return false;
    }
    map<pair<int,string>,pair<string,int>> createParseTable(){
        map<pair<int,string>,pair<string,int>> parseTable; //<current state,look ahead> -> < shift/reduce, next state>
        for(const auto& state: states){
            for(const auto& rule: state->getClosure()){
                if(rule.second.second.size() == rule.first){ // rule is a reduction
//                    cout<< "final rule found: "<< rule.second.first<<" -> "<<*(rule.second.second.end()-1)<<endl;
                    for(const string& a: grammar.getT()) {
                        if (a == "EOF" && state->getStartRule().second.second[0] == grammar.getS()) {
                            parseTable[pair<int, string>(state->getId(), a)] = pair<string, int>("acc", 0);
                        }
                        else{
                            if(validReduction(rule.second.first,a)){
                                for(int i =0; i<grammar.getP().size();i++){
                                    if(rule.second == grammar.getP()[i]){ //check which production rule is used (i-th production in gram)
                                        parseTable[pair<int,string>(state->getId(),a)] = pair<string,int> ("r",i);
                                    }
                                }
//                                *(std::find(grammar.getP().begin(), grammar.getP().end(), rule.second)).;
//                                parseTable[pair<int,string>(state->getId(),a)] = pair<string,int> ("r",0);
                            }
                        }
                    }
                }
            }
            for(const auto& transition : state->getTransitions()){
                if(parseTable[pair<int,string>(state->getId(),transition.first)].first.empty()){
                    parseTable[pair<int,string>(state->getId(),transition.first)] = pair<string,int>("s", transition.second->getId());
                }
                else{
                    cout<<"error shift"<<endl;
                }
            }
        }
        return parseTable;
    }
};


#endif //MB2_FSM_H
