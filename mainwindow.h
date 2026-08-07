#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QInputDialog>
#include <QLabel>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QApplication>
#include <QDebug>
#include "game.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::MainWindow *ui;
    Game Chess;
    QPushButton* squares[8][8];
    bool isPieceSelected=false;
    bool isBoardFlipped = false;
    Board::Coordinate FromCoord{-1,-1};
    Board::Coordinate ToCoord{-1,-1};
    Board::Coordinate lastFromCoord{-1, -1};
    Board::Coordinate lastToCoord{-1, -1};
    Board::Coordinate attackedKingCoord{-1, -1};
    void onSquareClicked(Board::Coordinate Coords);
    void updateBoardUI();
    void InitializeBoardUI();
    void ResetGame();
};
#endif // MAINWINDOW_H
