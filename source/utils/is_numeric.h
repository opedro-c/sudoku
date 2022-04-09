//
// Created by pedro_costa on 06/01/2022.
//

#ifndef SUDOKUGAME_IS_NUMERIC_H
#define SUDOKUGAME_IS_NUMERIC_H
#include <string>
using std::string;

/*!
 * verifies if all characters from string are numbers
 * @param str   the input string
 * @return      a boolean value telling if string is numeric or not (false if string is empty)
 */
bool is_numeric(const string &str);

#endif //SUDOKUGAME_IS_NUMERIC_H
