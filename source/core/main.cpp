/**
 * @file main.cpp
 *
 * @description
 * This program implements an interactive Sudoku game.
 *
 * ===========================================================================
 * @license
 *
 * This file is part of SUDOKU GAME project.
 *
 * SUDOKU GAME is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SUDOKU GAME is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SUDOKU GAME.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) 2014-2020 Selan R. dos Santos, http://www.dimap.ufrn.br/~selan
 * ===========================================================================
 *
 * @author	Selan R dos Santos, <selan.rds@gmail.com>
 * @date	2020
 *
 * @remark On 2020-09-19 changed ...
 */

#include <cstdlib> // EXIT_SUCCESS

#include "sudoku_gm.h"

int main( int argc, char ** argv )
{
    sdkg::SudokuGame game;

    // Set up simulation.
    game.initialize( argc, argv );

    // The Game Loop (Architecture)
    while( not game.game_over() )
    {
        game.process_events();
        game.update();
        game.render();
    }


    return EXIT_SUCCESS;
}
