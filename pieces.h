#ifndef PIECES_H
#define PIECES_H

class Pieces
{
public:
    enum Piece {
        EMPTY = 0,
        WHITE_PAWN, WHITE_ROOK, WHITE_KNIGHT, WHITE_BISHOP, WHITE_QUEEN, WHITE_KING,
        BLACK_PAWN, BLACK_ROOK, BLACK_KNIGHT, BLACK_BISHOP, BLACK_QUEEN, BLACK_KING
    };

    Pieces();
};

#endif // PIECES_H
