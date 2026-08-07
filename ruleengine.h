#ifndef RULEENGINE_H
#define RULEENGINE_H
#include "board.h"

class RuleEngine {
public:
    RuleEngine();

    static bool IsValidMove(Board& board, const Board::Move &move, Board::Color currentTurn);
    static bool IsKingUnderAttack(Board& board, Board::Color KingColor);
    static bool HasAnyLegalMove(Board& board, Board::Color currentTurn);
    static bool IsInsufficientMaterial(const Board& board);

private:
    static bool IsValidKnightMove(const Board::Move &move);
    static bool IsValidRookMove(const Board& board, const Board::Move &move);
    static bool IsValidBishopMove(const Board& board, const Board::Move &move);
    static bool IsValidQueenMove(const Board& board, const Board::Move &move);
    static bool IsValidKingMove(const Board& board, const Board::Move &move, Board::Color currentTurn);
    static bool IsValidPawnMove(Board& board, const Board::Move &move, Board::Color pieceColor);
    static bool IsValidMoveForPiece(Board& board, const Board::Move &move, const Board::Piece &piece);
    static bool IsPathClear(const Board& board, const Board::Move &move);
    static bool IsValidCastling(const Board& board, const Board::Move &move, Board::Color currentTurn);
};
#endif