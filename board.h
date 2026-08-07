#ifndef BOARD_H
#define BOARD_H

class Board
{
public:

    struct Coordinate {
        int x;
        int y;
    };

    struct Move {
        Coordinate source;
        Coordinate destination;
    };

    enum class Color {NONE, WHITE, BLACK};

    enum class PieceType {
        EMPTY = 0,
        PAWN,
        ROOK,
        KNIGHT,
        BISHOP,
        QUEEN,
        KING
    };

    struct Piece{
        PieceType type=PieceType::EMPTY;
        Color color=Color::NONE;
    };
    Board::Coordinate FindKing(const Board::Color &color) const;
    Piece grid[8][8];
    void ResetBoard();
    void MovePiece(const Board::Move &move);
    void ChangeGrid(const Board::Coordinate &coord, Board::Piece &piece);

private:
    Coordinate enPassantTarget = {-1, -1};

public:
    bool whiteKingMoved = false;
    bool whiteRookA1Moved = false;
    bool whiteRookH1Moved = false;
    bool blackKingMoved = false;
    bool blackRookA8Moved = false;
    bool blackRookH8Moved = false;
    bool EnPassantActive=false;
    Coordinate GetEnPassantTarget() const { return enPassantTarget; }
    Piece GetPiece(Coordinate coord) const;
    void SetEnPassantTarget(Coordinate target) { enPassantTarget = target; }
    Board();
};

#endif