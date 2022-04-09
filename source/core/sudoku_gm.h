#ifndef SUDOKU_GAME_H
#define SUDOKU_GAME_H

/*!
 *  Sudoku game class header.
 */

#include <fstream>
#include <thread>
#include <memory>
#include <iostream>
using std::cout;
using std::endl;
#include <algorithm>
using std::fill_n;
using std::all_of;
#include <vector>
using std::vector;
#include <iterator>
using std::back_inserter;
#include <sstream>
using std::ostringstream;
#include <numeric>
using std::accumulate;
#include <stack>
using std::stack;

#include "../lib/messages.h"
#include "../lib/text_color.h"
#include "sudoku_board.h"

namespace sdkg {

    /// Game class representing a Life Game simulation manager.
    class SudokuGame
    {
        private:
            //=== Structs

            /// Internal game options
            struct Options {
                std::string input_filename; //!< Input cfg file.
                short total_checks;        //!< # of checks user has left.
            };

            /// Possible games states
            enum class game_state_e {
                STARTING,               //!< Beginning the game.
                //====================================================
                READING_MAIN_OPT,       //!< Reading main menu option.
                //----------------------------------------------------
                HELPING,                //!< Displaying game rules (main menu)
                QUITTING,               //!< Game is quiting either because user requested (main menu).
                //====================================================
                PLAYING_MODE,           //!< Reading user command (Play sub-menu).
                //----------------------------------------------------
                REQUESTING_NEW_GAME,        //!< User wants to start a new game.
                CONFIRMING_QUITTING_MATCH,  //!< User has already started a match. We need to confirm s/he wants to bail out.
                PLACING_PLAY,            //!< User wants to place a number on the board.
                REMOVING_PLAY,              //!< User wants to remove a number from the board.
                CHECKING_MOVES,             //!< User wants to check whether his previous moves are correct.
                UNDOING_PLAY,               //!< User wants to undo a previous move he did.
                // ====================================================
                FINISHED_PUZZLE         //!< User has completed a puzzle.
            };

            /// Options from the main menu.
            enum class main_menu_opt_e : uint {
                PLAY=0,        //!< Play option.
                NEW_GAME,      //!< New puzzle option.
                QUIT,          //!< Quit option.
                HELP,          //!< Print help option.
                N_OPTIONS,     //!< Total number of options.
                INVALID        //!< Invalid option.
            };

            /// Represents a command issued during a match (UNDO command).
            enum class Command {
                PLACE,  //!< Place command.
                REMOVE,   //!< Remove command.
                UNDO,     //!< Undo command
                CHECK,    //!< Check command
                EMPTY,    //!< No command entered
                INVALID   //!< Invalid command
            };

            /// Represents a user move.
            struct Play {
                Command command;
                short row;    //!< row selected by the user.
                short col;    //!< col selected by the user.
                short value;  //!< value to play selected by the user.
                /// Constructor.
                explicit Play( Command cmd=Command::EMPTY, short r=-1, short c=-1, short v=1 ) : command{cmd}, row{r}, col{c}, value{v}{/*empty*/}
            };


            //=== Data members

            SBoardManager sbm;
            Options m_opt;                          //!< Overall game Options to set up the game configuration.
            game_state_e m_game_state;              //!< Current game state.
            std::string m_curr_msg;                 //!< Current message to display on screen.
            Command m_curr_command;                 //!< Current user command on playing mode
            Play m_last_play;                       //!< Last user play.
            bool m_quitting_match;                  //!< Flag that indicates whether the user wants to end an ongoing game.
            bool m_game_is_over = false;                    //!< Flag that indicates if user chose to quit game;
            bool m_match_started = false;                   //!< Flag that indicates if match started (at least one play was exec)
            bool m_finished_match;
            int m_checks_left;                    //!< Current # of checks user can request.
            short m_curr_board_idx = 0;                 //!< Current player board index
            main_menu_opt_e m_curr_main_menu_opt;   //!< Current main menu option.
            stack< Play > undo_log;              //!< Log of commands to support undoing.

            void read_cli_options( int argc, char ** argv );

            void read_main_menu_opt();

            void read_confirm_quitting_match();

            void read_command();

            void change_to_new_game();

            bool is_finished() const;

            bool is_victory() const;

            void finish_game();

            void place_play();

            void remove_play();

            void undo_play();

            void display_welcome() const;

            void display_input_info() const;

            void print_number_from_player_board( short line, short column ) const;

            void display_player_board() const;

            void display_message() const;

            void display_main_menu_opt() const;

            void display_confirm_quitting_match() const;

            void display_ask_to_continue() const;

            void display_sudoku_help() const;

            void display_checks_left() const;

            void display_command_syntax() const;

            void display_ask_for_a_command() const;

            void display_digits_left_to_place() const;

            public:
                SudokuGame();
                ~SudokuGame() = default;

                static void usage() ;
                void initialize( int argc, char** argv );
                void update();
                void process_events();
                void render() const;
                bool game_over() const;

    }; // SudokuGame class.
}

#endif // SUDOKU_GAME_H
