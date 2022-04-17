//
// Created by kiani on 05.04.22.
//

#ifndef MB2_PARSER_H
#define MB2_PARSER_H
#include "FSM.h"
#include "map"

class Parser {
public:
    explicit Parser(const CFG& grammar): fsm(FSM(grammar)){
        //create parseTable from fsm
        parseTable= fsm.createParseTable();
        fsm.toDot();
    }
    vector<string> readParse(const string& filename){
        ifstream tokenFile(filename);
        string line;
        vector<string> words;
        while(getline(tokenFile,line)){
            size_t pos = 0;
            while((pos = line.find(' ')) != string::npos){
                words.push_back(line.substr(0,pos));
                line.erase(0, pos + 1);
            }
            words.push_back(line);
        }
        tokenFile.close();
        return words;
    }
    void parse(const string& filename){
        //start in state 1 and read first input symbol
        vector<string> tokens = readParse(filename);
        int currentState = 1;
        vector<int> stateStack;
        vector<int> reductionStack;
        stateStack.push_back(currentState);
        vector<string> tokenValues;
        string token = tokens[0];
        bool restart = false;
        for(int i = 0; i<= tokens.size();){
//            string token = tokens[i];
            pair<int,string> currentKey(stateStack.back(), token);
            if(parseTable.find(currentKey) != parseTable.end()) {
                restart = false;
                pair<string, int> tableEntry = parseTable[currentKey];
                if (tableEntry.first == "s") { //shift
                    stateStack.push_back(tableEntry.second);
                    tokenValues.push_back(tokens[i+1]);
                    i+=2;
                    if(i<tokens.size()){
                        token = tokens[i];
                    }
                }
                else if (tableEntry.first == "r"){ //reduction
                    reductionStack.push_back(tableEntry.second);
                    pair<string,vector<string>> reduction = fsm.getGrammar().getP()[tableEntry.second];
                    stateStack.erase(stateStack.end()-reduction.second.size(), stateStack.end());
                    token = reduction.first;
                    currentKey= pair<int,string>(stateStack.back(), token);
                    //should check if entry is not empty
                    tableEntry = parseTable[currentKey];
                    stateStack.push_back(tableEntry.second);
                    token = tokens[i];
                }
                else if(tableEntry.first == "acc"){
//                    printDerivation(reductionStack);
                    for(const string& s: tokenValues){
                        cout<< s<<" ";
                    }
                    cout<<endl<<"accept!!"<<endl;

                    return;
                }
            }
            else if(restart){
                //restarted but still couldn't progress
                cout<< "error during parsing";
                return;
            }
            else { //if there is no entry in parse table, we reset parsing process (possible check for final state with EOF to avoid endless restart)
                restart = true;
                stateStack.clear();
                stateStack.push_back(1);
            }
        }
    }
    void printDerivation(vector<int> stateStack){
        string s;
        std::reverse(stateStack.begin(), stateStack.end());
        const auto variables = fsm.getGrammar().getV();
        const auto productions = fsm.getGrammar().getP();
        for(int nr: stateStack){
            if(s.empty()){
                for(const string& deelstr : productions[nr].second){
                    s.append(deelstr+" ");
                }
                s.pop_back();
            }
            else{
                string replacement;
                for(const string& deelstr : productions[nr].second){
                    replacement.append(deelstr+" ");
                }
                replacement.pop_back();
                size_t start_pos = s.rfind(productions[nr].first);
                s.replace(start_pos, productions[nr].first.length(), replacement);
            }
            cout<< s<<endl;
        }
    }
private:
    FSM fsm;
    map<pair<int,string>,pair<string,int>> parseTable;
};


#endif //MB2_PARSER_H
