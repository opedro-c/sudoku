//
// Created by pedro_costa on 06/01/2022.
//

#ifndef SUDOKUGAME_SPLIT_H
#define SUDOKUGAME_SPLIT_H
#include <string>
using std::string;
#include <vector>
using std::vector;


namespace sdkg{
    /// Tokenize a string
    /*!
        * @param str The input string.
        * @param sep The delimiter symbol.
        * @return A list of tokenized strings.
        */
    // splits string into string vector
    vector<string> split(const string& str, const char sep=' ');
}

#endif //SUDOKUGAME_SPLIT_H
