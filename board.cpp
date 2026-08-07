#include "board.h"

Board::Board() {
    ResetBoard();
}

void Board::ResetBoard(){
    using Piece = Board::Piece;

    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            grid[i][j] = {PieceType::EMPTY, Color::NONE};
        }
    }

    grid[0][0].type = PieceType::ROOK;
    grid[0][1].type = PieceType::KNIGHT;
    grid[0][2].type = PieceType::BISHOP;
    grid[0][3].type = PieceType::QUEEN;
    grid[0][4].type = PieceType::KING;
    grid[0][5].type = PieceType::BISHOP;
    grid[0][6].type = PieceType::KNIGHT;
    grid[0][7].type = PieceType::ROOK;

    for(int i=0; i<8; i++){
        grid[1][i]={PieceType::PAWN, Color::NONE};
    }

    for(int i = 0; i < 8; i++) {
        grid[7][i].type = grid[0][i].type;
        grid[6][i].type = grid[1][i].type;
    }

    for(int i = 0; i < 8; i++){
        grid[0][i].color = Color::BLACK;
        grid[1][i].color = Color::BLACK;

        grid[6][i].color = Color::WHITE;
        grid[7][i].color = Color::WHITE;
    }

    whiteKingMoved = false;
    whiteRookA1Moved = false;
    whiteRookH1Moved = false;
    blackKingMoved = false;
    blackRookA8Moved = false;
    blackRookH8Moved = false;

    return;
}
void Board::ChangeGrid(const Board::Coordinate &coord, Board::Piece &piece){
    grid[coord.y][coord.x]=piece;

    return;
}
void Board::MovePiece(const Board::Move &move){
    int srcX = move.source.x;
    int srcY = move.source.y;
    int destX = move.destination.x;
    int destY = move.destination.y;

    Piece movingPiece = grid[srcY][srcX];

    if (movingPiece.type == PieceType::KING) {
        if (movingPiece.color == Color::WHITE) {
            whiteKingMoved = true;
            if (srcX == 4 && srcY == 7 && destX == 6 && destY == 7) {
                grid[7][5] = grid[7][7];
                grid[7][7] = {PieceType::EMPTY, Color::NONE};
            }
            else if (srcX == 4 && srcY == 7 && destX == 2 && destY == 7) {
                grid[7][3] = grid[7][0];
                grid[7][0] = {PieceType::EMPTY, Color::NONE};
            }
        } else {
            blackKingMoved = true;
            if (srcX == 4 && srcY == 0 && destX == 6 && destY == 0) {
                grid[0][5] = grid[0][7];
                grid[0][7] = {PieceType::EMPTY, Color::NONE};
            }
            else if (srcX == 4 && srcY == 0 && destX == 2 && destY == 0) {
                grid[0][3] = grid[0][0];
                grid[0][0] = {PieceType::EMPTY, Color::NONE};
            }
        }
    }

    if (movingPiece.type == PieceType::ROOK) {
        if (srcX == 0 && srcY == 7) whiteRookA1Moved = true;
        if (srcX == 7 && srcY == 7) whiteRookH1Moved = true;
        if (srcX == 0 && srcY == 0) blackRookA8Moved = true;
        if (srcX == 7 && srcY == 0) blackRookH8Moved = true;
    }

    if (destX == 0 && destY == 7) whiteRookA1Moved = true;
    if (destX == 7 && destY == 7) whiteRookH1Moved = true;
    if (destX == 0 && destY == 0) blackRookA8Moved = true;
    if (destX == 7 && destY == 0) blackRookH8Moved = true;

    grid[destY][destX]=grid[srcY][srcX];
    grid[srcY][srcX] = {PieceType::EMPTY, Color::NONE};

    return;
}

Board::Piece Board::GetPiece(Coordinate coord) const {
    return grid[coord.y][coord.x];
}
Board::Coordinate Board::FindKing(const Board::Color &color) const{
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            Board::Coordinate coord{i,j};
            Board::Piece dummy=GetPiece(coord);
            Board::Piece target{Board::PieceType::KING, color};
            if(dummy.color==target.color && dummy.type==target.type){
                return coord;
            }
        }
    }
    return {-1,-1};
}