#include <algorithm>
#include <istream>
#include <fstream>
using std::ifstream;
using std::fstream;
#include <set>
using std::set;
#include <map>
using std::map;
#include "sudoku_board.h"
#include "sudoku_gm.h"
#include "config.h"
#include "../lib/contains.h"
#include "../utils/split.h"


namespace sdkg {
    SBoard::SBoard()
    {
        // Empty board.
        std::fill( &board[0][0], &board[Config::SB_SIZE][Config::SB_SIZE], 0 );
    }

    void SBoard::set_board( short b[Config::SB_SIZE][Config::SB_SIZE] ) {
        for (int i{0}; i < Config::SB_SIZE; i++) {
            for (int j{0}; j < Config::SB_SIZE; j++) {
                this -> board[i][j] = b[i][j];
            }
        }
    };

    bool SBoardManager::is_valid(const SBoard sb)
    {
        int line_sum = 0, col_sum = 0;
        set<short> col_appeared, lin_appeared;
        auto are_equal = [](const short &a, const short &b) { return a == b; };

        for (short lin{0}; lin < Config::SB_SIZE; lin++) {
            for (short col{0}; col < Config::SB_SIZE; col++) {
                // verifies if number is not on a valid range
                if (sb.at(lin, col) < 1 or sb.at(lin, col) > 9)
                    return false;
                // verifies if number is repeating on column
                if (contains(col_appeared.begin(), col_appeared.end(), sb.at(col, lin), are_equal))
                    return false;
                // verifies if number is repeating on lin
                if (contains(lin_appeared.begin(), lin_appeared.end(), sb.at(lin, col), are_equal))
                    return false;
                col_appeared.insert(sb.at(col, lin));
                lin_appeared.insert(sb.at(lin, col));
                col_sum += sb.at(col, lin);
                line_sum += sb.at(lin, col);
            }
            if (line_sum != 45 or col_sum != 45) return false;
            col_appeared.clear();
            lin_appeared.clear();
            line_sum = 0;
            col_sum = 0;
        }
        return true;
    }

    void SBoardManager::read_input_file(const string &path_to_file) {
        ifstream file{path_to_file, fstream::in};
        SBoard sb;
        vector<string> tokens;
        string input;
        auto is_empty_string = [](const string &str) { return str.empty(); };
        short
            b_original[Config::SB_SIZE][Config::SB_SIZE],
            b_abs[Config::SB_SIZE][Config::SB_SIZE], num,
            num_invalid_boards = 0;
        try {
            if (not file)
                throw std::runtime_error("File could not be opened!\n"); // verifies if file was opened successfully
            while (getline(file, input)) {
                if (not input.empty()) {
                    for (short i{0}; i < Config::SB_SIZE; i++) {
                        tokens = split(input);
                        // remove empty strings from vector of token strings
                        std::remove_if(tokens.begin(), tokens.end(), is_empty_string);
                        for (short j{0}; j < Config::SB_SIZE; j++) {
                            num = (short) std::stoi(tokens.at(j));
                            b_original[i][j] = num;
                            b_abs[i][j] = (num >= 0) ? num : num * - 1;
                        }
                        getline(file, input);
                    }
                    sb.set_board(b_abs);
                    if (is_valid(sb)) { sb.set_board(b_original); add_board(sb); }
                    else num_invalid_boards++;
                }
            }
            m_num_invalid_boards_read = num_invalid_boards;
        }
        catch (const std::exception &e) {
            std::cerr << Color::tcolor("\n>>> An error occurred while reading the file\n", Color::BRIGHT_RED);
            std::cerr << Color::tcolor(e.what(), Color::BRIGHT_RED);
            exit(1);
            
        }
        file.close();
    }

    void SBoardManager::set_player_board(const int &board_idx) {
        if (board_idx >= (short) (m_boards_read.size()) or board_idx < 0) {
            throw std::runtime_error("set_player_board -> Invalid board index: " + std::to_string(board_idx) + "\n");
        }
        SBoard board_chosen{ m_boards_read.at(board_idx) };
        for (short i{0}; i < Config::SB_SIZE; i++) {
            for (short j{0}; j < Config::SB_SIZE; j++) {
                if (board_chosen.at(i, j) > 0) {
                    m_player_board.set_loc(i, j, board_chosen.at(i, j));
                } else if (board_chosen.at(i, j) < 0){
                    m_player_board.set_loc(i, j, loc_type_e::EMPTY);
                }
            }
        }

    }

    std::pair<SBoardManager::loc_type_e, short> SBoardManager::decode_player_board_loc(short line, short column) const {
        loc_type_e code;
        short value;
        short player_board_loc = m_player_board.at(line, column);
        if (player_board_loc == 0) {
            code = loc_type_e::EMPTY;
            value = 0;
        } else if (player_board_loc >= 31) {
            code = loc_type_e::INVALID;
            value = (short) (player_board_loc - 30);
        } else if (player_board_loc >= 21) {
            code = loc_type_e::INCORRECT;
            value = (short) (player_board_loc - 20);
        } else if (player_board_loc >= 11) {
            code = loc_type_e::CORRECT;
            value = (short) (player_board_loc - 10);
        } else if (player_board_loc >= 1) {
            code = loc_type_e::ORIGINAL;
            value = player_board_loc;
        }
        return { code, value };
    }

    short SBoardManager::encode_value(SBoardManager::prefix_e command_status, short value) {
        return (short) (command_status + value);
    }

    void SBoardManager::set_solution_board(const int &board_idx) {
        if (board_idx >= (short) (m_boards_read.size()) or board_idx < 0) {
            throw std::invalid_argument("set_solution_board -> Invalid board index: " + std::to_string(board_idx) + "\n");
        }
        SBoard board_chosen{ m_boards_read.at(board_idx) };
        for (short i{0}; i < Config::SB_SIZE; i++) {
            for (short j{0}; j < Config::SB_SIZE; j++) {
                if (board_chosen.at(i, j) > 0) {
                    m_solution.set_loc(i, j, board_chosen.at(i, j));
                } else if (board_chosen.at(i, j) < 0){
                    m_solution.set_loc(i, j, (short) (board_chosen.at(i, j) * -1));
                }
            }
        }
    }

    vector<short> SBoardManager::get_digits_left_to_place() const {
        vector<short> digits_left_to_place;
        map<short, int> digits_found;
        short player_board_num;
        // map quantity of each digit on board
        for (short i{0}; i < Config::SB_SIZE; i++) {
            for (short j{0}; j < Config::SB_SIZE; j++) {
                player_board_num = decode_player_board_loc(i, j).second;
                if (player_board_num != 0) digits_found[player_board_num]++;    // ignore empty locations
            }
        }
        for (auto digit : digits_found) {
            if (digit.second < 9) digits_left_to_place.push_back(digit.first);
        }
        return digits_left_to_place;
    }

    void SBoardManager::place_digit_on_board(SBoardManager::prefix_e code, short line, short column, short digit) {
        m_player_board.set_loc(line, column, encode_value(code, digit));
    }

    SBoardManager::loc_type_e SBoardManager::get_placing_status(short line, short column, short digit) {
        vector<short> digits_left_to_place = get_digits_left_to_place();
        for (int i{0}; i < Config::SB_SIZE; i++) {
            if (
                    decode_player_board_loc(line, i).second == digit or
                    decode_player_board_loc(i, column).second == digit
            ) return loc_type_e::INVALID;
        }
        if (not contains(digits_left_to_place.begin(), digits_left_to_place.end(), digit, [](short a, short b) { return a==b; })) {
            return loc_type_e::INVALID;
        } else if (digit != m_solution.at(line, column)) {
            return loc_type_e::INCORRECT;
        } else {
            return loc_type_e::CORRECT;
        }
    }


}
