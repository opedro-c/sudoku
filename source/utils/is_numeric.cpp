//
// Created by pedro_costa on 06/01/2022.
//

#include "is_numeric.h"


bool is_numeric(const string &str) {
    if (str.size()) {
        for (size_t i{0}; i < str.size(); i++) {
            if (!isdigit(str.at(i))) return false;
        }
        return true;
    }
    return false;
}
