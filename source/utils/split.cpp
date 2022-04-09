//
// Created by pedro_costa on 05/01/2022.
//
#include <string>
using std::string;
#include <vector>
using std::vector;


namespace sdkg {

    vector<string> split(const string& str, const char sep=' ') {
        vector<string> word_vector{};
        string word;
        for (size_t i = 0; i < str.size(); i++) {
            // add word to word vector when current char is equals to separator
            if (*str.substr(i, 1).c_str() == sep) {
                word_vector.push_back(word);
                word.clear();
            } else {
                // concatenate current char with string
                // if it is different from separator char
                word += str.substr(i, 1);
            }
        }
        word_vector.push_back(word);
        return word_vector;
    }
}
