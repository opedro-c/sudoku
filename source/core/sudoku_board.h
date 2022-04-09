#ifndef SUDOKU_BOARD_H
#define SUDOKU_BOARD_H
#include <vector>
using std::vector;
#include <string>
using std::string;
#include "config.h"

/*!
 *  In this header file we have two classes: SBoard and SudokuPlayerBoard.
 *
 *  The former represents only the board data, as a matrix of short.
 *
 *  The latter represents the board and the actions necessary to support
 *  the player's moves and all the information associated with a
 *  Sudoku match. This involves encoding information regarding incorrect
 *  moves, invalid plays, correct play, original numbers, etc.
 *  These information is required to display the board with special colors
 *  representing all these different conditions.
 */


namespace sdkg {

    /// This class stores values for a 9x9 Sudoku board.
    class SBoard {
        private:
            short board[Config::SB_SIZE][Config::SB_SIZE]{};
        public:
            SBoard();
            void set_board( short b[Config::SB_SIZE][Config::SB_SIZE] );
            inline void set_loc( short line, short column, short num ) { this -> board[line][column] = num; }
            inline short at( short line, short column) const { return this -> board[line][column]; }
    };

    /*!
     *  This class represents a Sudoku board as the player's makes his or her moves during a match.
     *  The moves in the board data set is coded as follows:
     *
     *  + Digits in range [1,9]: These are the original values already filled in when the match started.
     *  + 0: An empty location that may receive a value provided by the player.
     *  + prefix `1` as in 1[1,9]: We add this prefix if the player's value is correct, i.e. corresponds
     *    to the correct value stored in the solution elsewhere.
     *  + prefix `2` as in 2[1,9]: We add this prefix if the player's value is incorrect, i.e. it is
     *    a valid play but it differs from the solution.
     *  + prefix `3` as in 3[1,9]: We add this prefix if the player makes an invalid move, i.e. a move
     *    that breaks the Sudoku's basic rules.
     *
     *  Note that the player's moves are always stored, when it is applied to a location that does
     *  not contain an original value.
     *  This is important so we can color the number accordingly when we display the board.
     */

    class SBoardManager {
        private:
            SBoard m_player_board;             //!< The Sudoku matrix where the user moves are stored.
            SBoard m_solution;                 //!< The Sudoku matrix with the solution.
            vector<SBoard> m_boards_read;      //!< Container with the valid boards read from input file
            short m_num_invalid_boards_read = 0;

        public:
            /// Possible types associated with a location on the board during a match.
            enum loc_type_e : short {
                EMPTY = 0,          //!< Empty cell.
                ORIGINAL,       //!< Original puzzle value, not mutable.
                CORRECT,        //!< Correct value.
                INCORRECT,      //!< Incorrect value.
                INVALID         //!< Invalid play.
            };

            /// Prefix we may add to a stored value to encode information.
            enum prefix_e : short {
                PRE_ORIGINAL = 0,
                PRE_CORRECT = 10,   //!< User has entered a correct value.
                PRE_INCORRECT = 20, //!< User has entered an incorrect value.
                PRE_INVALID = 30    //!< User has entered an invalid value.
            };

        private:
            // Verifies if board is a valid sudoku board
            static bool is_valid( const SBoard sb );

            // add sudoku board to boards read
            inline void add_board( const SBoard sb ) { m_boards_read.push_back(sb); }

            static short encode_value( prefix_e command_status, short value );

        public:
            //=== Regular methods.
            SBoardManager() = default;
            ~SBoardManager() = default;
            SBoardManager & operator=( const SBoardManager & ) = delete;
            SBoardManager( const SBoardManager & ) = delete;

            //=== Access methods.

            //=== Modifiers methods.


            // Reads input txt file and allocate boards data
            void read_input_file( const string & path_to_file );

            // Tells if number is on a valid range for sudoku, which is [1, 9]
            inline bool is_valid_sudoku_digit(const short &digit) {
                bool is_valid = digit <= Config::SUDOKU_BIGGEST_NUM and digit>= Config::SUDOKU_SMALLEST_NUM;
                if (not is_valid) throw std::invalid_argument("Sudoku digit must be in range [1, 9]!");
                else return is_valid;
            }

            // Verifies if placing a digit on a sudoku place is correct, invalid or incorrect
            loc_type_e get_placing_status(short line, short column, short digit);

            // Gets number of valid boards read
            inline size_t get_num_valid_boards() const { return m_boards_read.size(); }
            
            // Gets number of valid boards read
        	inline short get_num_invalid_boards_read() const { return this -> m_num_invalid_boards_read; }

            // Gets which digits are available to place on the player's board
            vector<short> get_digits_left_to_place() const;

            // Set player board hiding playable locations
            void set_player_board( const int &board_idx );

            void place_digit_on_board( prefix_e code, short line, short column, short digit );

            // Set solution board
            void set_solution_board( const int &board_idx );

            inline SBoard get_player_board() { return this -> m_player_board; }

            std::pair<loc_type_e, short> decode_player_board_loc(short line, short column ) const;

    };
}

#endif
