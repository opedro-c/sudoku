#include <iterator>

#include "sudoku_gm.h"
#include "../lib/contains.h"
#include "../utils/split.h"
#include "../utils/is_numeric.h"


namespace sdkg {

    /// Lambda expression that transform the string to lowercase.
    auto STR_LOWERCASE = [](const char * t)->std::string{
        std::string str{ t };
        std::transform( str.begin(), str.end(), str.begin(), ::tolower );
        return str;
    };

    /// Default constructor
    SudokuGame::SudokuGame(){
        m_opt.total_checks = 3; // Default value.
        m_opt.input_filename = "../data/input.txt"; // Default value.
    }

    void SudokuGame::usage() {
        std::cout << "sudoku";

        std::cout << "Usage: sudoku [-c <num>] [--help] <input_puzzle_file>\n"
                  << "  Game options:\n"
                  << "    -c     <num> Number of checks per game. Default = 3.\n"
                  << "    --help       Print this help text.\n";
        std::cout << std::endl;

        exit( EXIT_SUCCESS );
    }

    void SudokuGame::process_events(){
        m_curr_msg.clear();
        if ( m_game_state == game_state_e::STARTING or
             m_game_state == game_state_e::HELPING  or
             m_game_state == game_state_e::CHECKING_MOVES) {
            // Reading a simple enter from user.
            std::string line;
            std::getline(std::cin, line);
        } else if ( m_game_state == game_state_e::READING_MAIN_OPT ) {
            read_main_menu_opt();
        } else if ( m_game_state == game_state_e::PLAYING_MODE ) {
            m_match_started = true;
            read_command();
        } else if ( m_game_state == game_state_e::REQUESTING_NEW_GAME ) {
            change_to_new_game();
        } else if ( m_game_state == game_state_e::CONFIRMING_QUITTING_MATCH ){
            read_confirm_quitting_match();
        } else if (m_game_state == game_state_e::QUITTING) {
            m_game_is_over = true;
        } else if ( m_game_state == game_state_e::PLACING_PLAY ) {
            place_play();
        } else if ( m_game_state == game_state_e::REMOVING_PLAY ) {
            remove_play();
        } else if (m_game_state == game_state_e::UNDOING_PLAY) {
            undo_play();
        } else if (m_game_state == game_state_e::FINISHED_PUZZLE) {
            finish_game();
        }
    }

    void SudokuGame::update(){
        if ( m_game_state == game_state_e::STARTING) {
            m_game_state = game_state_e::READING_MAIN_OPT;
        } else if (m_game_state == game_state_e::READING_MAIN_OPT) {
            if (m_curr_main_menu_opt == main_menu_opt_e::PLAY) {
                m_game_state = game_state_e::PLAYING_MODE;
            } else if (m_curr_main_menu_opt == main_menu_opt_e::NEW_GAME) {
                if (m_match_started) {
                    m_curr_msg = "Are you sure you want to leave this match?";
                    m_game_state = game_state_e::CONFIRMING_QUITTING_MATCH;
                } else {
                    m_game_state = game_state_e::REQUESTING_NEW_GAME;
                }
            } else if (m_curr_main_menu_opt == main_menu_opt_e::QUIT) {
                if ( m_match_started ) {
                    m_curr_msg = "Are you sure you want to leave this match?";
                    m_game_state = game_state_e::CONFIRMING_QUITTING_MATCH;
                } else {
                    m_game_state = game_state_e::QUITTING;
                }
            } else if (m_curr_main_menu_opt == main_menu_opt_e::HELP) {
                m_game_state = game_state_e::HELPING;
            } else if (m_curr_main_menu_opt == main_menu_opt_e::INVALID) {
                m_game_state = game_state_e::READING_MAIN_OPT;
                m_curr_msg = "Invalid option!";
            }
        } else if (m_game_state == game_state_e::REQUESTING_NEW_GAME) {
            m_game_state = game_state_e::READING_MAIN_OPT;
        } else if (m_game_state == game_state_e::CONFIRMING_QUITTING_MATCH) {
            if (m_curr_main_menu_opt == main_menu_opt_e::NEW_GAME and m_quitting_match) {
                m_game_state = game_state_e::REQUESTING_NEW_GAME;
                m_curr_msg = "New game set, good luck!";
            } else if (m_curr_main_menu_opt == main_menu_opt_e::QUIT and m_quitting_match) {
                m_curr_msg = "Bye bye! Come back soon!";
                m_game_state = game_state_e::QUITTING;
            } else {
                m_game_state = game_state_e::READING_MAIN_OPT;
            }
        } else if (m_game_state == game_state_e::HELPING or m_game_state == game_state_e::FINISHED_PUZZLE) {
            m_game_state = game_state_e::READING_MAIN_OPT;
        } else if (m_game_state == game_state_e::QUITTING) {
            m_curr_msg = "Bye bye! Come back soon!";
        } else if (m_game_state == game_state_e::PLAYING_MODE) {
            if (m_curr_command == Command::PLACE) {
                m_game_state = game_state_e::PLACING_PLAY;
            } else if (m_curr_command == Command::REMOVE) {
                m_game_state = game_state_e::REMOVING_PLAY;
            } else if (m_curr_command == Command::UNDO) {
                m_game_state = game_state_e::UNDOING_PLAY;
            } else if (m_curr_command == Command::CHECK) {
                m_game_state = game_state_e::CHECKING_MOVES;
                m_curr_msg = "Checking mode active!";
            } else if (m_curr_command == Command::EMPTY) {
                m_game_state = game_state_e::READING_MAIN_OPT;
            } else if (m_curr_command == Command::INVALID) {
                m_game_state = game_state_e::PLAYING_MODE;
            }
        } else if (
                m_game_state == game_state_e::PLACING_PLAY or
                m_game_state == game_state_e::REMOVING_PLAY or
                m_game_state == game_state_e::UNDOING_PLAY or
                m_game_state == game_state_e::CHECKING_MOVES
                ) {
            if (m_finished_match) {
                m_game_state = game_state_e::FINISHED_PUZZLE;
            } else {
                m_game_state = game_state_e::PLAYING_MODE;
            }
        }
    }

    void SudokuGame::render() const {
        if ( m_game_state == game_state_e::READING_MAIN_OPT) {
            display_player_board();
            display_message();
            display_main_menu_opt();
        } else if (m_game_state == game_state_e::CONFIRMING_QUITTING_MATCH) {
            display_player_board();
            display_message();
            display_confirm_quitting_match();
        } else if (m_game_state == game_state_e::HELPING) {
            display_sudoku_help();
            display_ask_to_continue();
        } else if (m_game_state == game_state_e::QUITTING) {
            display_player_board();
            display_message();
        } else if (m_game_state == game_state_e::PLAYING_MODE) {
            display_player_board();
            display_message();
            display_checks_left();
            display_digits_left_to_place();
            display_command_syntax();
            display_ask_for_a_command();
        } else if (
                m_game_state == game_state_e::FINISHED_PUZZLE or
                m_game_state == game_state_e::CHECKING_MOVES
                ) {
            display_player_board();
            display_message();
            display_ask_to_continue();
        }
    }

    void SudokuGame::read_cli_options(int argc, char **argv) {
	    for (int i{1}; i < argc; i++) {
			if (string{argv[i]} == "-c") {
				if (is_numeric(argv[++i])) {
				    m_opt.total_checks = (short) std::stoi(argv[i]);
				} else {
				    string msg = ">>> Invalid total checks value! Assuming default value of 3 checks\n\n";
				    cout << Color::tcolor(msg, Color::YELLOW);
				    m_opt.total_checks = 3;
				}
			} else if (string{argv[i]} == "-h" or string{argv[i]} == "--help") {
				usage();
			} else {
				m_opt.input_filename = argv[i];
			}
	    }
        
    }

    void SudokuGame::display_welcome() const {
        for(int i{0};i<55;i++) { std::cout << "="; } std::cout << std::endl;
        std::cout << "\t" << "Welcome to the Sudoku Game, v1.0" << std::endl;
        std::cout << "\t" << "Copyright (C) 2021, Pedro Costa Aragão" << std::endl;
        for(int i{0};i<55;i++) { std::cout << "="; } std::cout << std::endl;
        std::cout << std::endl;

        
		string msg = ">>> Preparing to read input file \"" + m_opt.input_filename + "\"...\n\n";
		msg = Color::tcolor(msg, Color::BRIGHT_GREEN);
		std::cout << msg;
    }
    
    void SudokuGame::display_input_info() const {
    	string msg = ">>> Number of checks per game: " + std::to_string(m_opt.total_checks) + "\n" +
    				 ">>> Total of valid sudoku boards read: " + std::to_string(sbm.get_num_valid_boards()) + "\n";
    	std::cout << Color::tcolor(msg, Color::BRIGHT_GREEN);
    	
    	if (sbm.get_num_invalid_boards_read()) { 
			msg = ">>> " + std::to_string(sbm.get_num_invalid_boards_read()) + " boards from input file didn't match sudoku rules\n\n";
			std::cout << Color::tcolor(msg, Color::YELLOW);
    	
    	}
        display_ask_to_continue();
    }

    void SudokuGame::initialize(int argc, char **argv) {
        read_cli_options(argc, argv);
        m_checks_left = m_opt.total_checks;
        display_welcome();
        sbm.read_input_file(m_opt.input_filename);
        display_input_info();
        sbm.set_player_board(m_curr_board_idx);
        sbm.set_solution_board(m_curr_board_idx);
        m_game_state = game_state_e::STARTING;
    }

    void SudokuGame::print_number_from_player_board(short line, short column) const {
        std::pair<SBoardManager::loc_type_e, short> loc = sbm.decode_player_board_loc(line, column);
        string num = std::to_string(loc.second);
        Color::value_t color = Color::BRIGHT_CYAN;

        if (m_game_state == game_state_e::FINISHED_PUZZLE or m_game_state == game_state_e::CHECKING_MOVES) {
            if (loc.first == SBoardManager::INCORRECT) {
               color = Color::BRIGHT_RED;
            } else if (loc.first == SBoardManager::CORRECT) {
                color = Color::BRIGHT_GREEN;
            }
        }

        if (loc.first == SBoardManager::ORIGINAL) {
            color = Color::BRIGHT_WHITE;
        } else if (loc.first == SBoardManager::EMPTY) {
            num = " ";
        } else if (loc.first == SBoardManager::INVALID) {
            color = Color::BRIGHT_RED;
        }

        cout << Color::tcolor(num, color);
    }

    void SudokuGame::display_player_board() const {
        short lin = 0, col = 0;

        cout << Color::tcolor("|--------[MAIN SCREEN]--------|\n", Color::BRIGHT_BLUE);
        cout << "     ";

        int i{1};
        while (i < 10) {
            if (i == m_last_play.col) cout << Color::tcolor("V", Color::BRIGHT_RED);
            else cout << " ";
            cout << " ";
            if (i % 3 == 0) cout << "  ";
            i++;
        }
        cout << "\n";
        cout << "     1 2 3   4 5 6   7 8 9\n";

        for (int c{0}; c < 3; c++) {
            // print horizontal line
            cout << "   ";  // horizontal line indentation
            for (int i{0}; i <= Config::SB_CHAR_WIDTH; i++) {
                if ((c == 1 or c == 2) and (i == 0 or i == 24)) cout << "|";
                else if (i % 8 == 0) cout << "+";
                else cout << "-";
            }
            cout << "\n";

            // print numbers divided by pipes
            for (int k{0}; k < 3; k++) {
                if (lin + 1 == m_last_play.row) cout << Color::tcolor(">", Color::BRIGHT_RED);
                else cout << " ";
                cout << lin + 1 << " ";     // print line number
                for (int d{0}; d <= Config::SB_CHAR_WIDTH; d++) {
                    if (d % 8 == 0) cout << "|";
                    else if (d % 2 == 0) {
                        print_number_from_player_board(lin, col);
                        col++;
                    }
                    else if (d % 2 != 0) cout << " ";
                    if (d == Config::SB_CHAR_WIDTH) {
                        cout << "\n";
                        lin++;
                        col = 0;
                    }
                }
            }
        }
        cout << "   ";
        for (int i{0}; i <= 24; i++) {
            if (i % 8 == 0) cout << "+";
            else cout << "-";
        }
        cout << "\n";
    }

    void SudokuGame::display_message() const {
        cout << Color::tcolor("MSG: [ ", Color::BRIGHT_YELLOW);
        cout << Color::tcolor(m_curr_msg, Color::BRIGHT_YELLOW);
        cout << Color::tcolor(" ]\n", Color::BRIGHT_YELLOW);
    }

    void SudokuGame::display_main_menu_opt() const {
        cout << "1-Play  2-New Game  3-Quit  4-Help" << endl;
        cout << "Select option [1, 4] > ";
    }

    bool SudokuGame::game_over() const {
        return m_game_is_over;
    }

    void SudokuGame::read_main_menu_opt() {
        short opt_num;
        string line;
        try {
            std::getline(std::cin, line);
            if (line.empty()) {
                m_curr_main_menu_opt = main_menu_opt_e::HELP;
                return;
            }
            opt_num = (short) std::stoi(line);
            if (opt_num == 1){
                m_curr_main_menu_opt = main_menu_opt_e::PLAY;
            } else if (opt_num == 2) {
                m_curr_main_menu_opt = main_menu_opt_e::NEW_GAME;
            } else if (opt_num == 3) {
                m_curr_main_menu_opt = main_menu_opt_e::QUIT;
            } else if (opt_num == 4) {
                m_curr_main_menu_opt = main_menu_opt_e::HELP;
            } else {
                m_curr_main_menu_opt = main_menu_opt_e::INVALID;
            }
        } catch (const std::exception &e) {
            m_curr_main_menu_opt = main_menu_opt_e::INVALID;
        }
    }

    void SudokuGame::read_confirm_quitting_match() {
        string confirm;
        try {
            getline(std::cin >> std::ws, confirm);
            if (confirm == "y" or confirm == "Y") {
                m_quitting_match = true;
                m_match_started = false;
            } else {
                m_quitting_match = false;
            }
        } catch (const std::exception &e) {
            m_quitting_match = false;
        }

    }

    void SudokuGame::change_to_new_game() {
        size_t new_game_idx = m_curr_board_idx + 1;
        short first_game_idx = 0;
        if ( new_game_idx > sbm.get_num_valid_boards() - 1 ) {
            m_curr_board_idx = first_game_idx;
        } else {
            m_curr_board_idx++;
        }
        sbm.set_player_board(m_curr_board_idx);
        sbm.set_solution_board(m_curr_board_idx);
        m_last_play = Play();
        m_checks_left = m_opt.total_checks;
        m_match_started = false;
        m_finished_match = false;
        undo_log = stack<Play>();
        m_curr_msg = "New game set, good luck!";
    }

    void SudokuGame::display_confirm_quitting_match() const {
        cout << "Select an option [ y / N ] > ";
    }

    void SudokuGame::display_ask_to_continue() const {
        std::cout << "Press < enter > to continue > ";
    }

    void SudokuGame::display_sudoku_help() const {
        string msg =
                string{"--------------------------------------------------------------------------------\n"} +
                string{" The goal of Sudoku is to fill a 9x9 grid with numbers so that each row, \n"} +
                string{" column and 3x3 section (nonet) contain all of the digits between 1 and 9.\n"} +
                string{" The Sudoku rules are:\n"} +
                string{" 1. Each row, column, and nonet can contain each number (typically 1 to 9)\n exactly once.\n"} +
                string{" 2. The sum of all numbers in any nonet, row, or column must be equal to 45.\n"} +
                string{"--------------------------------------------------------------------------------\n"};
        cout << Color::tcolor(msg, Color::BRIGHT_GREEN);
    }

    void SudokuGame::display_checks_left() const {
        cout << Color::tcolor("Checks left: " + std::to_string(m_checks_left) + "\n", Color::BRIGHT_YELLOW);
    }

    void SudokuGame::display_command_syntax() const {
        string msg =
                string{"Commands syntax:\n"} +
                string{"  'enter' (without typing anything)  -> go to previous menu.\n"} +
                string{"  'p' <row> <col> <number> + 'enter' -> place <number> on board at location (<row>, <col>).\n"} +
                string{"  'r' <row> <col> + 'enter'          -> remover <number> on board at location (<row>, <col>).\n"} +
                string{"  'c' + 'enter'                      -> check wich moves made are correct.\n"} +
                string{"  'u' + 'enter'                      -> undo last play.\n"} +
                string{"  <row>, <col>, <number> must be in range [1, 9].\n"};
        cout << Color::tcolor(msg, Color::BRIGHT_GREEN);
    }

    void SudokuGame::display_ask_for_a_command() const {
        cout << Color::tcolor("Enter a command > ", Color::BRIGHT_YELLOW);
    }

    void SudokuGame::read_command() {
        string command;
        vector<string> tokens;
        try {
            getline(std::cin, command);
            tokens = split(command);
            std::remove_if(tokens.begin(), tokens.end(), [](string &str) { return str.empty(); });
            if (tokens.at(0).empty()) {
                m_curr_command = Command::EMPTY;
            } else if (tokens.at(0) == "u") {
                m_curr_command = Command::UNDO;
            } else if (tokens.at(0) == "c") {
                if (m_checks_left > 0) {
                    m_curr_command = Command::CHECK;
                    m_checks_left--;
                }
                else {
                    m_curr_msg = "You ran out of checks!";
                    m_curr_command = Command::INVALID;
                }
            } else if (tokens.at(0) == "p" or tokens.at(0) == "r") {
                short r, c, v;
                bool locs_okay;
                m_curr_command = (tokens.at(0) == "p") ? Command::PLACE : Command::REMOVE;
                r = (short) (std::stoi(tokens.at(1)));
                c = (short) (std::stoi(tokens.at(2)));
                if (m_curr_command == Command::PLACE) v = (short) std::stoi(tokens.at(3));
                else v = 1;
                locs_okay =
                        sbm.is_valid_sudoku_digit(r) and
                        sbm.is_valid_sudoku_digit(c) and
                        sbm.is_valid_sudoku_digit(v);
                if (locs_okay) m_last_play = Play(m_curr_command, r, c, v);
            } else {
                m_curr_command = Command::INVALID;
                m_curr_msg = "Invalid command!";
            }
        } catch (const std::exception &e) {
                m_curr_command = Command::INVALID;
                m_curr_msg = e.what();
        }
    }

    void SudokuGame::display_digits_left_to_place() const {
        vector<short> digits_left_to_place = sbm.get_digits_left_to_place();
        cout << Color::tcolor("Digits left: [ ", Color::BRIGHT_YELLOW);
        for (int i{1}; i <= 9; i++) {
            if (contains(digits_left_to_place.begin(), digits_left_to_place.end(), i, [](short a, short b) {return a == b;})) {
                cout << Color::tcolor(std::to_string(i) + " ", Color::BRIGHT_YELLOW);
            }
        }
        cout << Color::tcolor("]\n", Color::BRIGHT_YELLOW);
    }

    void SudokuGame::place_play() {
        auto p_row = (short) (m_last_play.row - 1);
        auto p_col = (short) (m_last_play.col - 1);
        SBoardManager::prefix_e code = SBoardManager::PRE_INCORRECT;
        std::pair<SBoardManager::loc_type_e, short> loc_desired_to_play = sbm.decode_player_board_loc(p_row , p_col);
        if (loc_desired_to_play.first == SBoardManager::loc_type_e::ORIGINAL) {
            m_curr_msg = "Is not possible to place a number on a original sudoku value!";
        } else {
            SBoardManager::loc_type_e placing_status = sbm.get_placing_status(p_row, p_col, m_last_play.value);
            if (placing_status == SBoardManager::loc_type_e::CORRECT) {
                code = SBoardManager::prefix_e::PRE_CORRECT;
            } else if (placing_status == SBoardManager::loc_type_e::INVALID) {
                m_curr_msg = "Hey, you just made a invalid move! But it's up to you keep like this or not...";
                code = SBoardManager::prefix_e::PRE_INVALID;
            } else if (placing_status == SBoardManager::loc_type_e::INCORRECT) {
                code = SBoardManager::prefix_e::PRE_INCORRECT;
            }
            sbm.place_digit_on_board(code, p_row, p_col, m_last_play.value);
            if (m_game_state != game_state_e::UNDOING_PLAY) undo_log.push(m_last_play);
            if (is_finished()) {
                if (is_victory()) {
                    m_curr_msg = "Congratulations you won!";
                } else {
                    m_curr_msg = "Well, you lost... :(";
                }
                m_finished_match = true;
            }
        }
    }

    void SudokuGame::remove_play() {
        auto p_row = (short) (m_last_play.row - 1);
        auto p_col = (short) (m_last_play.col - 1);
        std::pair<SBoardManager::loc_type_e, short> loc_desired_to_play = sbm.decode_player_board_loc(p_row , p_col);
        if (loc_desired_to_play.first == SBoardManager::loc_type_e::ORIGINAL) {
            m_curr_msg = "Is not possible to remove a original sudoku value!";
        } else if (loc_desired_to_play.first == SBoardManager::loc_type_e::EMPTY) {
            m_curr_msg = "Location is already empty!";
        } else {
            m_last_play.value = loc_desired_to_play.second;
            m_last_play.command = Command::REMOVE;
            sbm.place_digit_on_board(SBoardManager::prefix_e::PRE_ORIGINAL, p_row, p_col, 0);
            if (m_game_state != game_state_e::UNDOING_PLAY) undo_log.push(m_last_play);
        }
    }

    void SudokuGame::undo_play() {
        if (not undo_log.empty()) {
            m_last_play.row = undo_log.top().row;
            m_last_play.col = undo_log.top().col;
            m_last_play.value = undo_log.top().value;
            if (undo_log.top().command == Command::PLACE) {
                m_last_play.command = Command::REMOVE;
                remove_play();
            } else if (undo_log.top().command == Command::REMOVE) {
                m_last_play.command = Command::PLACE;
                place_play();
            }
            undo_log.pop();
        } else {
            m_curr_msg = "Nothing to undo!";
        }
    }

    bool SudokuGame::is_finished() const {
        for (short i{0}; i < Config::SB_SIZE; i++) {
            for (short j{0}; j < Config::SB_SIZE; j++) {
                if (sbm.decode_player_board_loc(i, j).first == SBoardManager::EMPTY) { return false; }
            }
        }
        return true;
    }

    bool SudokuGame::is_victory() const {
        for (short i{0}; i < Config::SB_SIZE; i++) {
            for (short j{0}; j < Config::SB_SIZE; j++) {
                SBoardManager::loc_type_e loc_code = sbm.decode_player_board_loc(i, j).first;
                if (loc_code == SBoardManager::INVALID or loc_code == SBoardManager::INCORRECT) { return false; }
            }
        }
        return true;
    }

    void SudokuGame::finish_game() {
        change_to_new_game();
        // Reading a simple enter from user.
        std::string line;
        std::getline(std::cin, line);
    }
}
