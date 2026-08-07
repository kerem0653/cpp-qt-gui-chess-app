#ifndef GAME_H
#define GAME_H
#include "board.h"
#include "ruleengine.h"
#include <QObject>
#include <cstdlib>
#include <vector>
#include <string>

class Game : public QObject
{
    Q_OBJECT
private:
    int HalfMoveCounter=0;
    RuleEngine validator;
    Board board;
    Board::Color currentTurn;
    void ChangeTurn();
    std::vector<std::string> positionHistory;
    std::string GetBoardStateString();

public:
    Game();
    Board::Piece GetPiece(const Board::Coordinate &coord) const;
    Board::Color GetCurrentTurn() const;
    Board::Color GetPlayerColor();
    bool MovePiece(const Board::Move &move);
    void PromotePawn(Board::Coordinate destination, Board::PieceType newType);
    void ResetBoard();

signals:
    void KingUnderAttackSignal(Board::Coordinate kingCoord);
    void gameOverCheckmate(Board::Color winner);
    void gameOverStalemate();
    void gameOverDraw();
    void pawnPromotionRequired(Board::Coordinate destination);
    void gameOverFiftyMove();
};

#endif