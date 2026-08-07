#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QTimer>
#include "board.h"
// --- first y/col after x/row --- //

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Chess");
    connect(&Chess, &Game::gameOverCheckmate, this, [this](Board::Color winner) {
        updateBoardUI();
        QString winnerStr = (winner == Board::Color::WHITE) ? "White" : "Black";
        QMessageBox::information(this, "Game Over", "Checkmate! " + winnerStr + " wins!");
        ResetGame();
    });

    connect(&Chess, &Game::gameOverDraw, this, [this]() {
        updateBoardUI();
        QMessageBox::information(this, "Game Over", "It's a draw.");
        ResetGame();
    });


    connect(&Chess, &Game::pawnPromotionRequired, this, [this](Board::Coordinate destination) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Pawn Promotion");
        msgBox.setText("Choose a piece for promotion:");

        QPushButton *queenBtn = msgBox.addButton("Queen", QMessageBox::ActionRole);
        QPushButton *rookBtn = msgBox.addButton("Rook", QMessageBox::ActionRole);
        QPushButton *bishopBtn = msgBox.addButton("Bishop", QMessageBox::ActionRole);
        QPushButton *knightBtn = msgBox.addButton("Knight", QMessageBox::ActionRole);

        msgBox.exec();

        Board::PieceType newType = Board::PieceType::QUEEN;

        if (msgBox.clickedButton() == queenBtn) newType = Board::PieceType::QUEEN;
        else if (msgBox.clickedButton() == rookBtn) newType = Board::PieceType::ROOK;
        else if (msgBox.clickedButton() == bishopBtn) newType = Board::PieceType::BISHOP;
        else if (msgBox.clickedButton() == knightBtn) newType = Board::PieceType::KNIGHT;

        Chess.PromotePawn(destination, newType);
        updateBoardUI();
    });
    connect(&Chess, &Game::KingUnderAttackSignal, this, [this](Board::Coordinate kingCoord){
        attackedKingCoord=kingCoord;
        updateBoardUI();
    });
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Choose Side");
    msgBox.setText("Which color would you like to play?");


    QPushButton *whiteButton = msgBox.addButton("White", QMessageBox::AcceptRole);
    QPushButton *blackButton = msgBox.addButton("Black", QMessageBox::RejectRole);

    msgBox.exec();

    if (msgBox.clickedButton() == whiteButton) {
        isBoardFlipped = false;
    } else {
        isBoardFlipped = true;
    }
    InitializeBoardUI();
}

MainWindow::~MainWindow()
{

    delete ui;
}

void MainWindow::InitializeBoardUI() {
    ui->chessLayout->setContentsMargins(0, 0, 0, 0);
    ui->chessLayout->setHorizontalSpacing(0);
    ui->chessLayout->setVerticalSpacing(0);

    QStringList files = {"a", "b", "c", "d", "e", "f", "g", "h"};
    for (int col = 0; col < 8; ++col) {
        QLabel *bottomLabel = new QLabel(files[col], this);
        bottomLabel->setAlignment(Qt::AlignHCenter);
        bottomLabel->setStyleSheet("color: #888888; font-weight: bold; margin-top: 1px;");
        ui->chessLayout->addWidget(bottomLabel, 9, col + 1);
    }

    for (int row = 0; row < 8; ++row) {
        QString rowStr = QString::number(8 - row);
        QLabel *rightLabel = new QLabel(rowStr, this);
        rightLabel->setAlignment(Qt::AlignJustify);
        rightLabel->setStyleSheet("color: #888888; font-weight: bold; margin-left: 1px;");
        ui->chessLayout->addWidget(rightLabel, row + 1, 9);
    }

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            QPushButton *btn = new QPushButton(this);
            btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

            if((row + col) % 2 == 0) {
                btn->setStyleSheet("background-color: #f0d9b5; border: none;");
            } else {
                btn->setStyleSheet("background-color: #b58863; border: none;");
            }

            ui->chessLayout->addWidget(btn, row + 1, col + 1);
            squares[row][col] = btn;

            connect(btn, &QPushButton::pressed, this, [=]() {
                int boardRow = isBoardFlipped ? (7 - row) : row;
                int boardCol = isBoardFlipped ? (7 - col) : col;
                Board::Coordinate clickedCoord{boardCol, boardRow};
                onSquareClicked(clickedCoord);
            });
        }
    }

    this->setMaximumSize(800, 800);
    this->setMinimumSize(400, 400);
    updateBoardUI();
}

void MainWindow::onSquareClicked(Board::Coordinate ClickedCoords){
    if(!isPieceSelected){
        Board::Piece piece = Chess.GetPiece(ClickedCoords);
        if( piece.type != Board::PieceType::EMPTY &&
            piece.color != Board::Color::NONE &&
            piece.color == Chess.GetCurrentTurn() ){
            FromCoord = ClickedCoords;
            isPieceSelected = true;
            updateBoardUI();
        }
    } else {
        if (FromCoord.x == ClickedCoords.x && FromCoord.y == ClickedCoords.y) {
            isPieceSelected = false;
            updateBoardUI();
            return;
        }

        Board::Piece clickedPiece = Chess.GetPiece(ClickedCoords);
        Board::Piece selectedPiece = Chess.GetPiece(FromCoord);

        if (clickedPiece.color == selectedPiece.color) {
            FromCoord = ClickedCoords;
            updateBoardUI();
            return;
        }

        ToCoord = ClickedCoords;

        Board::Move move;
        move.source = FromCoord;
        move.destination = ToCoord;

        if(Chess.MovePiece(move)){
            isPieceSelected = false;
            lastFromCoord = FromCoord;
            lastToCoord = ToCoord;            
            updateBoardUI();
        }else{
            isPieceSelected = false;
            updateBoardUI();
        }
    }
    return;
}

void MainWindow::ResetGame() {
    QMessageBox sideBox(this);
    sideBox.setWindowTitle("Choose Side");
    sideBox.setText("Which color would you like to play?");
    QPushButton *whiteButton = sideBox.addButton("White", QMessageBox::AcceptRole);
    QPushButton *blackButton = sideBox.addButton("Black", QMessageBox::RejectRole);

    sideBox.exec();

    if (sideBox.clickedButton() == whiteButton) {
        isBoardFlipped = false;
    } else {
        isBoardFlipped = true;
    }
    FromCoord={-1,-1};
    ToCoord={-1,-1};
    lastFromCoord={-1, -1};
    lastToCoord={-1, -1};
    attackedKingCoord={-1, -1};
    isPieceSelected = false;
    Chess.ResetBoard();
    updateBoardUI();
}
void MainWindow::updateBoardUI() {
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {

            int boardRow = isBoardFlipped ? (7 - row) : row;
            int boardCol = isBoardFlipped ? (7 - col) : col;

            Board::Coordinate coords{boardCol, boardRow};
            QPushButton *btn = squares[row][col];
            Board::Piece piece = Chess.GetPiece(coords);

            bool isLight = (row + col) % 2 == 0;
            QString bgColor = isLight ? "#f0d9b5" : "#b58863";


            if (attackedKingCoord.x == boardCol && attackedKingCoord.y == boardRow) {
                bgColor = QString("qradialgradient(cx:0.5, cy:0.5, radius:0.45, fx:0.5, fy:0.5, "
                                  "stop:0 rgba(255, 0, 0, 230), "
                                  "stop:0.6 rgba(255, 60, 60, 180), "
                                  "stop:1 %1)").arg(isLight ? "#f0d9b5" : "#b58863");
            }
            if (isPieceSelected) {
                if (FromCoord.x == boardCol && FromCoord.y == boardRow) {
                    bgColor="#bcd238";
                }
            }

            if (lastFromCoord.x == boardCol && lastFromCoord.y == boardRow) {
                bgColor="#bcd238";
            }
            else if (lastToCoord.x == boardCol && lastToCoord.y == boardRow) {
                bgColor="#aaa23a";
            }
            if (piece.type == Board::PieceType::EMPTY || piece.color == Board::Color::NONE) {
                btn->setIcon(QIcon());
                btn->setStyleSheet(QString("QPushButton { background-color: %1; border: none; }").arg(bgColor));
            } else {
                QString typeChar = "";
                switch(piece.type) {
                case Board::PieceType::PAWN:   typeChar = "Pawn"; break;
                case Board::PieceType::ROOK:   typeChar = "Rook"; break;
                case Board::PieceType::KNIGHT: typeChar = "Knight"; break;
                case Board::PieceType::BISHOP: typeChar = "Bishop"; break;
                case Board::PieceType::QUEEN:  typeChar = "Queen"; break;
                case Board::PieceType::KING:   typeChar = "King"; break;
                default: break;
                }
                QString colorPrefix = (piece.color == Board::Color::WHITE) ? "w" : "b";
                QString iconPath = QString(":/") + colorPrefix + typeChar;

                int tileSize = qMin(btn->width(), btn->height());
                if (tileSize <= 0) tileSize = 65;

                btn->setIcon(QIcon(iconPath));
                btn->setIconSize(QSize(tileSize * 0.85, tileSize * 0.85));
                btn->setStyleSheet(QString("QPushButton { background-color: %1; border: none; background-position: center; background-repeat: no-repeat; }").arg(bgColor));
            }
        }
    }
}
void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

    int w = ui->centralwidget->width();
    int h = ui->centralwidget->height();

    int minDim = qMin(w, h);
    int tileSize = minDim / 8;

    if (tileSize <= 0) return;

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            if(squares[row][col]) {
                squares[row][col]->setFixedSize(tileSize, tileSize);

                if (!squares[row][col]->icon().isNull()) {
                    int iconSize = tileSize * 0.85;
                    squares[row][col]->setIconSize(QSize(iconSize, iconSize));
                }
            }
        }
    }
}