//
// Created by Kiani on 29/03/2022.
//

#ifndef MB2_PARSETABLE_H
#define MB2_PARSETABLE_H
#include <utility>

#include "CFG.h"

class ParseTable {
public:
    explicit ParseTable(CFG grammar): grammar(std::move(grammar)){
    };
    void construct(){

    }
private:
    CFG grammar;
};


#endif //MB2_PARSETABLE_H
