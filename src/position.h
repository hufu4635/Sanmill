﻿/*
  Sanmill, a mill game playing engine derived from NineChess 1.5
  Copyright (C) 2015-2018 liuweilhy (NineChess author)
  Copyright (C) 2019-2020 Calcitem <calcitem@outlook.com>

  Sanmill is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Sanmill is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef POSITION_H
#define POSITION_H

#include <cassert>
#include <deque>
#include <memory> // For std::unique_ptr
#include <string>
#include <cstring>

#include "config.h"
#include "types.h"
#include "rule.h"
#include "search.h"

using namespace std;

class AIAlgorithm;
class Node;

extern string tips;

/// StateInfo struct stores information needed to restore a Position object to
/// its previous state when we retract a move. Whenever a move is made on the
/// board (by calling Position::do_move), a StateInfo object must be passed.

struct StateInfo
{

    // Copied when making a move
    int    rule50;
    int    pliesFromNull;

    // Not copied when making a move (will be recomputed anyhow)
    Key        key;
    Piece      capturedPiece;
    StateInfo *previous;
    int        repetition;
};

/// A list to keep track of the position states along the setup moves (from the
/// start position to the position just before the search starts). Needed by
/// 'draw by repetition' detection. Use a std::deque because pointers to
/// elements are not invalidated upon list resizing.
typedef std::unique_ptr<std::deque<StateInfo>> StateListPtr;

/// Position class stores information regarding the board representation as
/// pieces, side to move, hash keys, castling info, etc. Important methods are
/// do_move() and undo_move(), used by the search to update node info when
/// traversing the search tree.
class Thread;

class Position
{
public:
    Position();
    virtual ~Position();

    Position(const Position &) = delete;
    Position &operator=(const Position &) = delete;

    // FEN string input/output
    Position &set(const std::string &fenStr, StateInfo *si, Thread *th);
    Position &set(const std::string &code, Color c, StateInfo *si);
    const std::string fen() const;

    // Position representation
    Piece piece_on(Square s) const;
    bool empty(Square s) const;
    Color color_on(Square s);
    int getPiecesInHandCount(Color c) const;
    int getPiecesOnBoardCount(Color c) const;

    // Properties of moves
    bool select_piece(Square s);
    bool place_piece(Square s, bool updateCmdlist = false);
    bool remove_piece(Square s, bool updateCmdlist = false);
    bool move_piece(Square from, Square to);
    bool _selectPiece(File file, Rank rank);
    bool _placePiece(File file, Rank rank);
    bool _removePiece(File file, Rank rank);

    // Doing and undoing moves
    bool do_move(Move m);
    bool undo_move(Move m);
    bool undo_null_move();
    bool do_null_move();

    // Accessing hash keys
    Key key();
    void construct_key();
    Key revert_key(Square square);
    Key update_key(Square square);
    Key update_key_misc();
    Key next_primary_key(Move m);

    // Other properties of the position
    Color side_to_move() const;

    //////////////////////////////////////

    bool set_position(const struct Rule *rule);

    time_t get_elapsed_time(int us);
    time_t start_timeb() const;
    void set_start_time(int stimeb);

    Piece *get_board() const;
    Square current_square() const;
    int get_step() const;
    enum Phase get_phase() const;
    enum Action get_action() const;
    const string get_tips() const;
    const char *cmd_line() const;
    const vector<string> *cmd_list() const;

    int get_mobility_diff(bool includeFobidden);

    bool reset();
    bool start();
    bool giveup(Color loser);
    bool command(const char *cmd);
    int update();
    bool check_gameover_condition(int8_t cp = 0);
    void clean_banned();
    void set_side_to_move(Color c);
  
    void change_side_to_move();
    void set_tips();
    Color get_winner() const;

    void mirror(int32_t move_, Square square, bool cmdChange = true);
    void turn(int32_t move_, Square square, bool cmdChange = true);
    void rotate(int degrees, int32_t move_, Square square, bool cmdChange = true);
    void flip();

    void create_mill_table();
    int add_mills(Square square);
    int in_how_many_mills(Square square, Color c, Square squareSelected = SQ_0);
    bool is_all_in_mills(Color c);

    int surrounded_empty_squres_count(Square square, bool includeFobidden);
    void surrounded_pieces_count(Square square, int &nOurPieces, int &nTheirPieces, int &nBanned, int &nEmpty);
    bool is_all_surrounded();

    static void square_to_polar(Square square, File &file, Rank &rank);
    static Square polar_to_square(File file, Rank rank);

    static void print_board();

    int pieces_on_board_count();
    int pieces_in_hand_count();

    static char color_to_char(Color color);
    static std::string char_to_string(char ch);

    static bool is_star_square(Square square);

// private:
      // Initialization helpers (used while setting up a position)
    void set_state(StateInfo *si) const;

    // Data members
    int gamePly;
    Color sideToMove { NOCOLOR };
    Color them { NOCOLOR };
    Color winner;

    enum Phase phase {PHASE_NONE};
    enum Action action;

    int score[COLOR_NB] { 0 };
    int score_draw { 0 };

    Step currentStep;
    int moveStep;

    static const int onBoard[SQUARE_NB];

    // Relate to Rule
    static int millTable[SQUARE_NB][LD_NB][FILE_NB - 1];

    Square currentSquare;
    int nPlayed{ 0 };

    vector <string> cmdlist;
    char cmdline[64]{ '\0' };

    // Note: [0] is sum of Black and White
    int nPiecesInHand[COLOR_NB] { 0 };
    int nPiecesOnBoard[COLOR_NB] { 0 };
    int nPiecesNeedRemove { 0 };

    int tm { -1 };
    time_t startTime;
    time_t currentTime;
    time_t elapsedSeconds[COLOR_NB];

    Piece board[SQUARE_NB];

    Bitboard byTypeBB[PIECE_TYPE_NB];

    /*
        0x   00     00     00    00    00    00    00    00
           unused unused piece1 square1 piece2 square2 piece3 square3
    */

    uint64_t millList[4];
    int millListSize { 0 };

    /*
        0x   00    00
            square1  square2
        Placing：0x00??，?? is place location
        Moving：0x__??，__ is from，?? is to
        Removing：0xFF??，?? is neg

        31 ----- 24 ----- 25
        | \       |      / |
        |  23 -- 16 -- 17  |
        |  | \    |   / |  |
        |  |  15 08 09  |  |
        30-22-14    10-18-26
        |  |  13 12 11  |  |
        |  | /    |   \ |  |
        |  21 -- 20 -- 19  |
        | /       |     \  |
        29 ----- 28 ----- 27
    */
    Move move { MOVE_NONE };

    StateInfo st;
};

inline bool Position::empty(Square s) const
{
    return piece_on(s) == NO_PIECE;
}


inline Color Position::side_to_move() const
{
    return sideToMove;
}

inline Piece Position::piece_on(Square s) const
{
    assert(is_ok(s));
    return board[s];
}

inline char Position::color_to_char(Color color)
{
    return static_cast<char>('0' + color);
}

inline std::string Position::char_to_string(char ch)
{
    if (ch == '1') {
        return "1";
    } else {
        return "2";
    }
}

inline int Position::getPiecesInHandCount(Color c) const
{
    return nPiecesInHand[c];
}

inline int Position::getPiecesOnBoardCount(Color c) const
{
    return nPiecesOnBoard[c];
}

inline Piece *Position::get_board() const
{
    return (Piece *)board;
}

inline Square Position::current_square() const
{
    return currentSquare;
}

inline int Position::get_step() const
{
    return currentStep;
}

inline enum Phase Position::get_phase() const
{
    return phase;
}

inline enum Action Position::get_action() const
{
    return action;
}

inline const string Position::get_tips() const
{
    return tips;
}

inline const char *Position::cmd_line() const
{
    return cmdline;
}

inline const vector<string> *Position::cmd_list() const
{
    return &cmdlist;
}

inline time_t Position::start_timeb() const
{
    return startTime;
}

inline void Position::set_start_time(int stimeb)
{
    startTime = stimeb;
}

#endif /* POSITION_H */
