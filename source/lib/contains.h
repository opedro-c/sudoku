//
// Created by pedro_costa on 05/01/2022.
//

#ifndef SUDOKUGAME_CONTAINS_H
#define SUDOKUGAME_CONTAINS_H

/*
*   @param it_begin     iterator to the container beginning
*   @param it_end       iterator to the container ending
*   @param element      item to be searched in container
*   @param eq           comparison function that must tell if elements are equal
*   @return             a boolean telling if container contains element
*/
template <typename T, typename Q, typename Func>
bool contains(T it_begin, T it_end, Q element, Func eq) {
    while (it_begin != it_end) {
        if (eq(*it_begin, element)) return true;
        it_begin++;
    }
    return false;
}

#endif //SUDOKUGAME_CONTAINS_H
