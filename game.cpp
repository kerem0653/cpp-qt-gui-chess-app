#include "board.h"
#include "ruleengine.h"
#include "game.h"

Game::Game() {
    board.ResetBoard();
    currentTurn=Board::Color::WHITE;
    positionHistory.push_back(GetBoardStateString());
}
Board::Piece Game::GetPiece(const Board::Coordinate &coord) const{
    return board.GetPiece(coord);
}

std::string Game::GetBoardStateString() {
    std::string state = "";
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            Board::Piece p = board.GetPiece({x, y});
            state += std::to_string(static_cast<int>(p.type)) + std::to_string(static_cast<int>(p.color));
        }
    }
    state += (currentTurn == Board::Color::WHITE ? "W" : "B");
    return state;
}

bool Game::MovePiece(const Board::Move &move) {
    if (!RuleEngine::IsValidMove(board, move, currentTurn)) {
        return false;
    }

    Board::Piece srcPiece = board.GetPiece(move.source);
    Board::Piece dstPiece = board.GetPiece(move.destination);
    bool isEnPassantCapture = false;

    if (srcPiece.type == Board::PieceType::PAWN && board.EnPassantActive) {
        Board::Coordinate epTarget = board.GetEnPassantTarget();
        if (move.destination.x == epTarget.x && move.destination.y == epTarget.y) {
            isEnPassantCapture = true;
        }
    }

    board.MovePiece(move);

    if (isEnPassantCapture) {
        Board::Coordinate epTarget = board.GetEnPassantTarget();
        int direction = (currentTurn == Board::Color::WHITE) ? 1 : -1;
        Board::Piece emptyPiece{Board::PieceType::EMPTY, Board::Color::NONE};
        board.ChangeGrid({epTarget.x, epTarget.y + direction}, emptyPiece);
    }

    if (srcPiece.type == Board::PieceType::PAWN) {
        if (move.destination.y == 0 || move.destination.y == 7) {
            emit pawnPromotionRequired(move.destination);
        }
    }

    bool nextEnPassantActive = false;
    Board::Coordinate nextEpTarget = {-1, -1};

    if (srcPiece.type == Board::PieceType::PAWN && abs(move.destination.y - move.source.y) == 2) {
        int forwardDir = (currentTurn == Board::Color::WHITE) ? -1 : 1;
        nextEpTarget = {move.source.x, move.source.y + forwardDir};
        nextEnPassantActive = true;
    }

    board.EnPassantActive = nextEnPassantActive;
    board.SetEnPassantTarget(nextEpTarget);

    if (srcPiece.type == Board::PieceType::PAWN || dstPiece.type != Board::PieceType::EMPTY || isEnPassantCapture) {
        HalfMoveCounter = 0;
    } else {
        HalfMoveCounter++;
    }

    if (HalfMoveCounter >= 100) {
        emit gameOverDraw();
        return true;
    }

    if (RuleEngine::IsInsufficientMaterial(board)) {
        emit gameOverDraw();
        return true;
    }

    std::string currentState = GetBoardStateString();
    positionHistory.push_back(currentState);

    int repetitionCount = 0;
    for (const auto& pos : positionHistory) {
        if (pos == currentState) repetitionCount++;
    }

    if (repetitionCount >= 3) {
        emit gameOverDraw();
        return true;
    }

    ChangeTurn();
    bool isKingUnderAttack = RuleEngine::IsKingUnderAttack(board, currentTurn);

    if (isKingUnderAttack) {
        emit KingUnderAttackSignal(board.FindKing(currentTurn));
    } else {
        emit KingUnderAttackSignal({-1, -1});
    }

    if (RuleEngine::HasAnyLegalMove(board, currentTurn)) {
        return true;
    }else if (isKingUnderAttack) {
        Board::Color winner = (currentTurn == Board::Color::WHITE) ? Board::Color::BLACK : Board::Color::WHITE;
        emit gameOverCheckmate(winner);
    } else {
        emit gameOverDraw();
    }

    return false;
}
void Game::ChangeTurn(){
    if(currentTurn==board.Color::WHITE){
        currentTurn=board.Color::BLACK;
    }else{
        currentTurn=board.Color::WHITE;
    }
    return;
}

Board::Color Game::GetCurrentTurn() const{
    return currentTurn;
}

void Game::PromotePawn(Board::Coordinate destination, Board::PieceType newType){
    Board::Piece piece={newType, currentTurn};
    board.ChangeGrid(destination, piece);
    return;
}
void Game::ResetBoard(){
    board.ResetBoard();
    currentTurn = Board::Color::WHITE;
    positionHistory.clear();
    positionHistory.push_back(GetBoardStateString());
    return;
}