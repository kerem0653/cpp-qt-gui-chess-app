#include "ruleengine.h"
#include <cmath>

RuleEngine::RuleEngine() {}

bool RuleEngine::IsValidMove(Board& board, const Board::Move &move, Board::Color currentTurn) {

    const Board::Piece srcPiece = board.GetPiece(move.source);
    const Board::Piece dstPiece = board.GetPiece(move.destination);

    if (srcPiece.type == Board::PieceType::EMPTY || dstPiece.color == srcPiece.color || srcPiece.color != currentTurn)
        return false;

    Board::Coordinate enPassantTarget = board.GetEnPassantTarget();

    bool isValid = false;
    switch (srcPiece.type) {
    case Board::PieceType::KNIGHT:
        isValid = IsValidKnightMove(move);
        break;
    case Board::PieceType::BISHOP:
        isValid = IsValidBishopMove(board, move);
        break;
    case Board::PieceType::ROOK:
        isValid = IsValidRookMove(board, move);
        break;
    case Board::PieceType::QUEEN:
        isValid = IsValidQueenMove(board, move);
        break;
    case Board::PieceType::KING:
        isValid = IsValidKingMove(board, move, srcPiece.color);
        break;
    case Board::PieceType::PAWN:
        isValid = IsValidPawnMove(board, move, srcPiece.color);
        break;
    default:
        isValid = false;
        break;
    }

    if (!isValid)
        return false;

    Board dummy_board = board;
    dummy_board.MovePiece(move);

    return !IsKingUnderAttack(dummy_board, currentTurn);
}

bool RuleEngine::IsValidKnightMove(const Board::Move &move) {
    int dx = abs(move.destination.x - move.source.x);
    int dy = abs(move.destination.y - move.source.y);
    return ((dx == 2 && dy == 1) || (dx == 1 && dy == 2));
}

bool RuleEngine::IsValidRookMove(const Board& board, const Board::Move &move) {
    int dx = abs(move.destination.x - move.source.x);
    int dy = abs(move.destination.y - move.source.y);
    return IsPathClear(board, move) ? (dx == 0 || dy == 0) : false;
}

bool RuleEngine::IsValidBishopMove(const Board& board, const Board::Move &move) {
    int dx = abs(move.destination.x - move.source.x);
    int dy = abs(move.destination.y - move.source.y);
    return IsPathClear(board, move) ? (dx == dy) : false;
}

bool RuleEngine::IsValidQueenMove(const Board& board, const Board::Move &move) {
    int dx = abs(move.destination.x - move.source.x);
    int dy = abs(move.destination.y - move.source.y);
    return IsPathClear(board, move) ? ((dx == dy) || (dx == 0 || dy == 0)) : false;
}

bool RuleEngine::IsValidKingMove(const Board& board, const Board::Move &move, Board::Color currentTurn) {
    int dx = abs(move.destination.x - move.source.x);
    int dy = abs(move.destination.y - move.source.y);

    if (dx == 2 && dy == 0) {
        return IsValidCastling(board, move, currentTurn);
    }

    if (!(dx <= 1 && dy <= 1) || (dx == 0 && dy == 0)) {
        return false;
    }

    Board dummy_board = board;
    dummy_board.MovePiece(move);

    return !IsKingUnderAttack(dummy_board, currentTurn);
}

bool RuleEngine::IsValidCastling(const Board& board, const Board::Move &move, Board::Color currentTurn) {
    int y = (currentTurn == Board::Color::WHITE) ? 7 : 0;
    if (move.source.x != 4 || move.source.y != y || move.destination.y != y) return false;

    if (move.destination.x == 6) {
        if (currentTurn == Board::Color::WHITE) {
            if (board.whiteKingMoved || board.whiteRookH1Moved) return false;
            if (board.GetPiece({5, 7}).type != Board::PieceType::EMPTY || board.GetPiece({6, 7}).type != Board::PieceType::EMPTY) return false;
        } else {
            if (board.blackKingMoved || board.blackRookH8Moved) return false;
            if (board.GetPiece({5, 0}).type != Board::PieceType::EMPTY || board.GetPiece({6, 0}).type != Board::PieceType::EMPTY) return false;
        }
        return !IsKingUnderAttack(const_cast<Board&>(board), currentTurn);
    }
    else if (move.destination.x == 2) {
        if (currentTurn == Board::Color::WHITE) {
            if (board.whiteKingMoved || board.whiteRookA1Moved) return false;
            if (board.GetPiece({1, 7}).type != Board::PieceType::EMPTY || board.GetPiece({2, 7}).type != Board::PieceType::EMPTY || board.GetPiece({3, 7}).type != Board::PieceType::EMPTY) return false;
        } else {
            if (board.blackKingMoved || board.blackRookA8Moved) return false;
            if (board.GetPiece({1, 0}).type != Board::PieceType::EMPTY || board.GetPiece({2, 0}).type != Board::PieceType::EMPTY || board.GetPiece({3, 0}).type != Board::PieceType::EMPTY) return false;
        }
        return !IsKingUnderAttack(const_cast<Board&>(board), currentTurn);
    }
    return false;
}

bool RuleEngine::IsInsufficientMaterial(const Board& board) {
    int knightCount = 0;
    int bishopCount = 0;
    int otherPieces = 0;

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            Board::Piece p = board.GetPiece({x, y});
            if (p.type == Board::PieceType::EMPTY || p.type == Board::PieceType::KING) continue;

            if (p.type == Board::PieceType::KNIGHT) knightCount++;
            else if (p.type == Board::PieceType::BISHOP) bishopCount++;
            else otherPieces++;
        }
    }

    if (otherPieces > 0) return false;

    if (knightCount == 0 && bishopCount == 0) return true;
    if ((knightCount == 1 && bishopCount == 0) || (bishopCount == 1 && knightCount == 0)) return true;

    return false;
}

bool RuleEngine::IsValidPawnMove(Board& board, const Board::Move &move, Board::Color pieceColor) {
    int dx = abs(move.destination.x - move.source.x);
    int dy = move.destination.y - move.source.y;
    int srcY = move.source.y;

    int forwardDirection = (pieceColor == Board::Color::WHITE) ? -1 : 1;
    int startingRow = (pieceColor == Board::Color::WHITE) ? 6 : 1;

    bool validMove = false;

    if (dx == 0 && dy == forwardDirection) {
        validMove = (board.GetPiece(move.destination).type == Board::PieceType::EMPTY);
    }
    else if (dx == 0 && dy == (forwardDirection * 2) && srcY == startingRow) {
        Board::Coordinate intermediateCoord = {move.source.x, move.source.y + forwardDirection};

        if (board.GetPiece(move.destination).type == Board::PieceType::EMPTY &&
            board.GetPiece(intermediateCoord).type == Board::PieceType::EMPTY) {
            validMove = true;
        }
    }
    else if (dx == 1 && dy == forwardDirection) {
        Board::Piece target = board.GetPiece(move.destination);

        if (target.type != Board::PieceType::EMPTY && target.color != pieceColor) {
            validMove = true;
        }
        else if (target.type == Board::PieceType::EMPTY && board.EnPassantActive) {
            Board::Coordinate epTarget = board.GetEnPassantTarget();
            if (move.destination.x == epTarget.x && move.destination.y == epTarget.y) {
                validMove = true;
            }
        }
    }

    return validMove;
}
bool RuleEngine::IsKingUnderAttack(Board& board, Board::Color KingColor) {
    Board::Coordinate KingCoord = board.FindKing(KingColor);
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            Board::Coordinate coords;
            coords.x = col;
            coords.y = row;
            Board::Piece piece = board.GetPiece(coords);

            if (piece.color != Board::Color::NONE && piece.color != KingColor) {
                Board::Move testMove = { coords, KingCoord };
                if (piece.type != Board::PieceType::KING && IsValidMoveForPiece(board, testMove, piece)) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool RuleEngine::IsValidMoveForPiece(Board& board, const Board::Move &move, const Board::Piece &piece) {
    switch (piece.type) {
    case Board::PieceType::KNIGHT:
        return IsValidKnightMove(move);
    case Board::PieceType::BISHOP:
        return IsValidBishopMove(board, move);
    case Board::PieceType::ROOK:
        return IsValidRookMove(board, move);
    case Board::PieceType::QUEEN:
        return IsValidQueenMove(board, move);
    case Board::PieceType::KING:
        return IsValidKingMove(board, move, piece.color);
    case Board::PieceType::PAWN:
        return IsValidPawnMove(board, move, piece.color);
    default:
        return false;
    }
}

bool RuleEngine::IsPathClear(const Board& board, const Board::Move &move) {
    int srcX = move.source.x;
    int srcY = move.source.y;
    int dstX = move.destination.x;
    int dstY = move.destination.y;

    int dx = dstX - srcX;
    int dy = dstY - srcY;

    int stepX = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
    int stepY = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);

    int currentX = srcX + stepX;
    int currentY = srcY + stepY;

    while (currentX != dstX || currentY != dstY) {
        Board::Coordinate coord = {currentX, currentY};
        if (board.GetPiece(coord).type != Board::PieceType::EMPTY) {
            return false;
        }
        currentX += stepX;
        currentY += stepY;
    }

    return true;
}
bool RuleEngine::HasAnyLegalMove(Board& board, Board::Color currentTurn) {
    bool HasLegalMove = false;
    for (int r1 = 0; r1 < 8 && !HasLegalMove; ++r1) {
        for (int c1 = 0; c1 < 8 && !HasLegalMove; ++c1) {
            Board::Coordinate src = {c1, r1};
            if (board.GetPiece(src).color == currentTurn) {
                for (int r2 = 0; r2 < 8 && !HasLegalMove; ++r2) {
                    for (int c2 = 0; c2 < 8 && !HasLegalMove; ++c2) {
                        Board::Coordinate dst = {c2, r2};
                        Board::Move testMove = {src, dst};

                        if (RuleEngine::IsValidMove(board, testMove, currentTurn)) {
                            HasLegalMove = true;
                        }
                    }
                }
            }
        }
    }
    return HasLegalMove;
}