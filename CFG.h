//
// Created by Kiani on 12/10/2021.
//

#ifndef CFG_CFG_H
#define CFG_CFG_H
#include <iostream>
#include "vector"
#include "set"
#include <fstream>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;
class CFG {
private:
    set<string> V;
    set<string> T;
    vector<pair<string,vector<string>>> P;
    string S;
public:

    const set<string> &getV() const {
        return V;
    }

    const set<string> &getT() const {
        return T;
    }

    const vector<pair<string, vector<string>>> &getP() const {
        return P;
    }

    const string &getS() const {
        return S;
    }

    bool accepts(const string& input){
        vector<vector<vector<string>>> table;
        table.push_back(vector<vector<string>>());
        for(char c:input){
            table.back().push_back(vector<string>());
            for(pair<string,vector<string>> pair1: P){
                for(string s: pair1.second){
                    if(s[0]==c){ //zoeken naar terminals
                        table.back().back().push_back(pair1.first);
                    }
                }
            }
            if(table.back().back().empty()){
                table.back().back().push_back(""); //geen gevonden
            }
        }
        for(int i=1;i<input.size();i++){//niveau
            table.push_back(vector<vector<string>>());
            for(int j=0;j<input.size()-i;j++){//kolom
                table.back().push_back(vector<string>());
                set<string> temp;
                for(int r=0;r<i;r++){
                    for(const string& beneden:table[r][j]){
                        vector<string> diagonaal = table[i-r-1][j+1+r];
                        if(diagonaal[0].empty() || beneden.empty()){
//                            table.back().back().push_back(""); //geen entry gevonden
                            continue;
                        }
                        for(pair<string,vector<string>> pair1: P){
                            for(int q=0; q<pair1.second.size()-1;q++){
                                if(beneden ==pair1.second[q]){
                                    for(string diag:diagonaal){
                                        if(diag==pair1.second[q+1]){
                                            temp.insert(pair1.first);
//                                            table[i][j].push_back(pair1.first);
                                        }
                                    }
                                }
                            }

                        }

                    }
                }
            if(temp.empty()){
                table.back().back().push_back("");//geen entry gevonden
            }
            else{
                for(const string& s:temp){
                    table.back().back().push_back(s);
                }
            }
            }
        }
        for(int i=table.size()-1;i>=0;i--){
            for(const vector<string>& kolom:table[i]){
                cout<<"| {";
                for(const string& var:kolom){
                    if(var!=kolom.back()){
                        cout<<var<<", ";
                    }
                    else{
                        cout<<var<<"} \t";
                    }
                }
            }
            cout<<"|"<<endl;
        }
        for(const string& s: table.back()[0]){
            if(s=="S"){
                cout<<"true"<<endl;
                return true;
            }
        }
        cout<<"false"<<endl;
        return false;
    }
    void addProduction(const pair<string,vector<string>>& prod){
        P.push_back(prod);
    }
    void addVariable(const string& var){
        V.insert(var);
    }
    void addTerminal(const string& term){
        T.insert(term);
    }
    void addStart(const string& st){
        S=st;
    }
CFG(){
//    V.insert("S");
//    V.insert("BINDIGIT");
//    T.insert("1");
//    T.insert("0");
//    T.insert("b");
//    T.insert("a");
//    createProduction("BINDIGIT",vector<string>{"1"});
//    createProduction("BINDIGIT",vector<string>{"0"});
//    createProduction("S",vector<string>{""});
//    createProduction("S",vector<string>{"a", "S", "b","BINDIGIT"});
//    S="S";
}
CFG(const string& inputjson){
    ifstream input(inputjson);
    json j;
    input >>j;
    for(string v : j["Variables"]){
        V.insert(v);
    }
    for(string t : j["Terminals"]){
        T.insert(t);
    }
    for(auto p: j["Productions"]){
        pair<string,vector<string>> prod;
        prod.first=p["head"];
        if(p["body"].empty()){
            prod.second.emplace_back("");
        }
        for(string b: p["body"]){
            prod.second.push_back(b);
        }
        P.push_back(prod);
    }
    S=j["Start"];


}
void createProduction(const string& A, const vector<string>& alpha){ // A -> alpha
    P.push_back(pair<string,vector<string>>(A,alpha));
    return;
}
void print(){
    cout<<"V = {";
    for(set<string>::iterator it = V.begin(); it!= V.end(); it++){
        if(it==--V.end()){
            cout<<*it<<"}"<<endl;
        }
        else{
            cout<<*it<<", ";
        }
    }
    cout<<"T = {";
    for(set<string>::iterator it = T.begin(); it!= T.end(); it++){
        if(it==--T.end()){
            cout<<*it<<"}"<<endl;
        }
        else{
            cout<<*it<<", ";
        }
    }
    cout<<"P = {"<<endl;
    for(pair<string,vector<string>> pair1: P){
//        cout<< "\t"+pair1.first<<" -> `";
        cout<<pair1.first;
        cout<<"\t -> `";
            for(auto it=pair1.second.begin();it!=pair1.second.end();it++){
                if(it+1!=pair1.second.end()){
                    cout<<*it<<" ";
                }
                else{
                    cout<<*it<<"`"<<endl;
                }
            }
        }
    cout<<"}"<<endl;
    cout<<"S = "<<S;
}
    map<string,set<string>> firstSet(){
        map<string,set<string>> FIRST;
        for(const string& var : V){
            FIRST[var] = calcFirst(var);
        }
        return FIRST;
    }
    set<string> calcFirst(const string& var){
        set<string> FIRST;
        if(T.find(var)!= T.end()){
            FIRST.insert(var);
            return FIRST;
        }
        for(const auto& prod : P){
            if(prod.first == var){
                if(V.find(prod.second[0]) != V.end()){ //first char in prod rule is also var
                    if(prod.second[0] != prod.first){
                        set<string> temp = calcFirst(prod.second[0]);
                        FIRST.insert(temp.begin(), temp.end());
                    }
                }
                else{
                    FIRST.insert(prod.second[0]);
                }
            }
        }
        return FIRST;
    }
    map<string,set<string>> followsSet(){
        map<string,set<string>> FOLLOWS;
        for(const string& var : V){
            FOLLOWS[var]= calcFollows(var);
        }
        FOLLOWS[S+"'"] = set<string>{"EOF"};
        return FOLLOWS;
    }
    set<string> calcFollows(const string& var){
        set<string> FOLLOWS;
        if(var == S){
            for(const auto& t: T){
                FOLLOWS.insert(t);
            }
            return FOLLOWS;
        }
        for(const auto& prod: P){
            auto it = std::find(prod.second.begin(), prod.second.end(), var);
            if(it!= prod.second.end()){
                if(it+1!= prod.second.end()){
                    if(T.find(*(it+1)) != T.end()){
                        FOLLOWS.insert(*(it+1));
                    }
                    else{
                        set<string> temp = calcFirst(*(it+1));
                        FOLLOWS.insert(temp.begin(), temp.end());
                    }
                }
                else if(*it == prod.first){
                    continue;
                }
                else{
                    set<string> temp = calcFollows(prod.first);
                    FOLLOWS.insert(temp.begin(),temp.end());
                }
            }
        }
        return FOLLOWS;
    }
};



#endif //CFG_CFG_H
