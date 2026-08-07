#ifndef CHESSBOARD_H
#define CHESSBOARD_H
#include "pieces.h"

struct Board{
    int grid[8][8]{{EMPTY}};

    Board()=default;

    void ResetBoard(int (&board)[8][8]){
        for(int i=0;i<8;i++){
            for(int j=0;j<8;j++){
                board[i][j]=EMPTY;
            }
        }
    }
};

class chessboard
{
protected:
    Board board;
public:
    chessboard();
};

#endif // CHESSBOARD_H
