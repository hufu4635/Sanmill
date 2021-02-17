/*
  This file is part of Sanmill.
  Copyright (C) 2019-2021 The Sanmill developers (see AUTHORS file)

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

import 'types.dart';

Map<int, int> squareToIndex = {
  8: 17,
  9: 18,
  10: 25,
  11: 32,
  12: 31,
  13: 30,
  14: 23,
  15: 16,
  16: 10,
  17: 12,
  18: 26,
  19: 40,
  20: 38,
  21: 36,
  22: 22,
  23: 8,
  24: 3,
  25: 6,
  26: 27,
  27: 48,
  28: 45,
  29: 42,
  30: 21,
  31: 0
};

Map<int, int> indexToSquare = squareToIndex.map((k, v) => MapEntry(v, k));

int makeSquare(int file, int rank) {
  return (file << 3) + rank - 1;
}

enum GameResult { pending, win, lose, draw, none }

class Color {
  static const none = '*';
  static const black = '@';
  static const white = 'O';
  static const ban = 'X';
  static const nobody = '-';
  static const draw = '=';

  static String of(String piece) {
    if (black.contains(piece)) return black;
    if (white.contains(piece)) return white;
    if (ban.contains(piece)) return ban;
    return nobody;
  }

  static bool isSameColor(String p1, String p2) {
    return of(p1) == of(p2);
  }

  static String opponent(String color) {
    if (color == white) return black;
    if (color == black) return white;
    return color;
  }

  String operator -(String c) => opponent(c);
}

class Piece {
  static const noPiece = Color.none;
  static const blackStone = Color.black;
  static const whiteStone = Color.white;
  static const ban = Color.ban;

  static bool isEmpty(String c) => noPiece.contains(c);
  static bool isBlack(String c) => blackStone.contains(c);
  static bool isWhite(String c) => whiteStone.contains(c);
  static bool isBan(String c) => ban.contains(c);
}

class Move {
  static const invalidMove = -1;

  // Square
  int from = 0;
  int to = 0;

  // file & rank
  int fromFile = 0;
  int fromRank = 0;
  int toFile = 0;
  int toRank = 0;

  // Index
  int fromIndex = 0;
  int toIndex = 0;

  String removed = Piece.noPiece;

  // 'move' is the UCI engine's move-string
  String move;

  MoveType type;

  // Used to restore fen step conter when regreting
  String counterMarks;

  parse() {
    if (!legal(move)) {
      throw "Error: Invalid Move: $move";
    }

    if (move[0] == '-' && move.length == "-(1,2)".length) {
      type = MoveType.remove;
      from = fromFile = fromRank = fromIndex = invalidMove;
      toFile = int.parse(move[2]);
      toRank = int.parse(move[4]);
      //captured = Piece.noPiece;
    } else if (move.length == "(1,2)->(3,4)".length) {
      type = MoveType.move;
      fromFile = int.parse(move[1]);
      fromRank = int.parse(move[3]);
      from = makeSquare(fromFile, fromRank);
      fromIndex = squareToIndex[from];
      toFile = int.parse(move[8]);
      toRank = int.parse(move[10]);
      removed = Piece.noPiece;
    } else if (move.length == "(1,2)".length) {
      type = MoveType.place;
      from = fromFile = fromRank = fromIndex = invalidMove;
      toFile = int.parse(move[1]);
      toRank = int.parse(move[3]);
      removed = Piece.noPiece;
    } else if (move == "draw") {
      // TODO
      print("Computer request draw");
    } else {
      assert(false);
    }

    to = makeSquare(toFile, toRank);
    toIndex = squareToIndex[to];
  }

  Move(this.move) {
    parse();
  }

  /// Format:
  /// Place: (1,2)
  /// Remove: -(1,2)
  /// Move: (3,1)->(2,1)

  Move.set(String move) {
    this.move = move;
    parse();
  }

  static bool legal(String move) {
    if (move == "draw") {
      return true; // TODO
    }

    if (move == null || move.length > "(3,1)->(2,1)".length) return false;

    String range = "0123456789(,)->";

    if (!(move[0] == '(' || move[0] == '-')) {
      return false;
    }

    if (move[move.length - 1] != ')') {
      return false;
    }

    for (int i = 0; i < move.length; i++) {
      if (!range.contains(move[i])) return false;
    }

    if (move.length == "(3,1)->(2,1)".length) {
      if (move.substring(0, 4) == move.substring(7, 11)) {
        return false;
      }
    }

    return true;
  }
}