#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "constants.h"
#include <QtWidgets>
#include <string>
#include <vector>
#include <algorithm>

using std::vector;
using std::string;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // Qt provides some default UI, disable them
    ui->mainToolBar->close();
    ui->menuBar->close();
    ui->statusBar->close();

    // Create and setup 3 pages and layouts
    QWidget* menu = new QWidget;
    menuLayout = new QVBoxLayout;
    setupMenu();
    menu->setLayout(menuLayout);

    QWidget* gamePage = new QWidget;
    board = new QGridLayout;
    setupBoard();
    gamePage->setLayout(board);

    QWidget* instructions = new QWidget;
    instrucLayout = new QVBoxLayout;
    setupInstruc();
    instructions->setLayout(instrucLayout);

    // Put layouts together in StackedLayout
    stackedLayout = new QStackedLayout;
    stackedLayout->addWidget(menu);
    stackedLayout->addWidget(gamePage);
    stackedLayout->addWidget(instructions);
    stackedLayout->setCurrentIndex(0);

    // Add layout to MainWindow
    QWidget* window = new QWidget;
    window->setLayout(stackedLayout);

    setCentralWidget(window);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setupMenu() {
    QLabel* title = new QLabel("Dots and Boxes");
    QFont titleFont("Arial", 16, QFont::Bold);
    title->setFont(titleFont);
    menuLayout->addWidget(title, 0, Qt::AlignCenter);

    QLabel* subtitle = new QLabel("Created by Hannah Daniel, Parth Savla, and Glenn Sun");
    QFont subtitleFont("Arial", 12);
    subtitle->setFont(subtitleFont);
    menuLayout->addWidget(subtitle, 0, Qt::AlignCenter);

    QPushButton* newGame = new QPushButton("New Game");
    menuLayout->addWidget(newGame);
    connect(newGame, SIGNAL(released()), this, SLOT(changeToBoard()));
}

void MainWindow::setupBoard() {
    // Allow [] access
    buttons.resize(7);
    for (int i = 0; i < 7; i++) {
        buttons[i].resize(7);
    }

    status.resize(7);
    for (int i = 0; i < 7; i++) {
        status[i].resize(7);
    }

    colorSquares.resize(3);
    for (int i = 0; i < 3; i++) {
        colorSquares[i].resize(3);
    }

    // Fill grid with horizontal and vertical buttons
    // Note that many spaces are left blank
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            bool horiz = i % 2 == 0 && j % 2 == 1;
            bool vert = i % 2 == 1 && j % 2 == 0;
            if (horiz) {
                buttons[i][j] = new QPushButton("");
                buttons[i][j]->setMinimumSize(60, 30);
                buttons[i][j]->setMaximumHeight(30);
            } else if (vert) {
                buttons[i][j] = new QPushButton("");
                buttons[i][j]->setMinimumSize(30, 60);
                buttons[i][j]->setMaximumWidth(30);
            }

            if (horiz || vert) {
                board->addWidget(buttons[i][j], i, j);
                connect(buttons[i][j], SIGNAL(released()), this, SLOT(handleButton()));
                buttons[i][j]->setStyleSheet("background-color: gray");
                buttons[i][j]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            }
            status[i][j] = BoardStatus::Blank;
        }
    }

    // Intialize score variables
    QFont font("Arial", 12, QFont::Bold);

    redScore = 0;
    blueScore = 0;
    score = new QLabel;
    score->setFont(font);

    blueTurn = true;
    turn = new QLabel;
    turn->setFont(font);

    updateScore();

    board->addWidget(score, 7, 0, 1, 7, Qt::AlignCenter);
    board->addWidget(turn, 8, 2, 1, 3, Qt::AlignCenter);

    // Navigation buttons
    QPushButton* info = new QPushButton("Rules");
    board->addWidget(info, 8, 0, 1, 2, Qt::AlignLeft);
    connect(info, SIGNAL(released()), this, SLOT(changeToInstructions()));

    home = new QPushButton("Home");
    board->addWidget(home, 8, 5, 1, 2, Qt::AlignRight);
    connect(home, SIGNAL(released()), this, SLOT(changeToMenu()));
    home->hide();
}

void MainWindow::setupInstruc() {
    string titleText = "Instructions";
    string contentText =
            "In Dots and Boxes, players take turns clicking on "
            "line segments, which turns the segment into the player's "
            "color. When all four sides of a square are colored "
            "(not necessarily the same color), the inside of the square "
            "is colored the same as the player who clicked the final "
            "side, scoring a point for that player. The objective of "
            "the game is to score as many points as possible.\n\n"
            "It's possible to score multiple points with one click. "
            "Try to come up with strategies to prevent your opponent "
            "from gaining points and to score many points for yourself! "
            "The game ends when there are no more segments left to click.";

    QLabel* title = new QLabel;
    QFont titleFont("Arial", 12, QFont::Bold);
    title->setText(QString::fromStdString(titleText));
    title->setFont(titleFont);

    QLabel* content = new QLabel;
    QFont contentFont("Arial", 12);
    content->setText(QString::fromStdString(contentText));
    content->setFont(contentFont);
    content->setWordWrap(true);
    content->setContentsMargins(50, 20, 50, 0);

    QPushButton* toBoard = new QPushButton("Back to Board");

    instrucLayout->addWidget(title, 0, Qt::AlignCenter);
    instrucLayout->addWidget(content);
    instrucLayout->addWidget(toBoard, 0, Qt::AlignCenter);

    connect(toBoard, SIGNAL(released()), this, SLOT(changeToBoard()));
}

void MainWindow::resetBoard() {
    // Reset score
    blueTurn = true;

    redScore = 0;
    blueScore = 0;

    updateScore();

    // Reset board buttons
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            bool horiz = i % 2 == 0 && j % 2 == 1;
            bool vert = i % 2 == 1 && j % 2 == 0;
            if (horiz || vert) {
                connect(buttons[i][j], SIGNAL(released()), this, SLOT(handleButton()));
                buttons[i][j]->setStyleSheet("background-color: gray");
            }
            status[i][j] = BoardStatus::Blank;
        }
    }

    // Delete colored squares
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            colorSquares[i][j]->hide(); // hacky way bc I don't know how to actually remove it
        }
    }

    home->hide();
}

void MainWindow::updateScore() {
    // Update score text
    string scoreText = "Blue: " + std::to_string(blueScore) +
                       " vs. Red: " + std::to_string(redScore);
    score->setText(QString::fromStdString(scoreText));
    score->update();

    // Update turn text (also win notification)
    string turnText;
    if (blueScore + redScore == 9) {
        if (blueScore > redScore) {
            turnText = "Blue won!";
        } else {
            turnText = "Red won!";
        }
        home->show();
    } else {
        if (blueTurn) {
            turnText = "Turn: Blue";
        } else {
            turnText = "Turn: Red";
        }
    }
    turn->setText(QString::fromStdString(turnText));
    turn->update();
}

void MainWindow::onBoxMade(int i, int j) {
    // Increment score
    if (blueTurn) {
        blueScore++;
    } else {
        redScore++;
    }

    // Add colored square
    colorSquares[(i-1)/2][(j-1)/2] = new QWidget;
    QPalette pal = palette();
    if (blueTurn) {
        pal.setColor(QPalette::Background, Qt::blue);
    } else {
        pal.setColor(QPalette::Background, Qt::red);
    }
    colorSquares[(i-1)/2][(j-1)/2]->setAutoFillBackground(true);
    colorSquares[(i-1)/2][(j-1)/2]->setPalette(pal);
    board->addWidget(colorSquares[(i-1)/2][(j-1)/2], i, j);
}

void MainWindow::handleButton() {
    // Change button color
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (blueTurn) {
        button->setStyleSheet("background-color: darkblue");
    } else {
        button->setStyleSheet("background-color: darkred");
    }
    button->update();

    // Find button in grid
    int i, j;
    for (i = 0; i < 7; i++) {
        auto p = std::find(buttons[i].begin(), buttons[i].end(), button);
        if (p != buttons[i].end()) {
            j = p - buttons[i].begin();
            break;
        }
    }

    // Update status
    if (blueTurn) {
        status[i][j] = BoardStatus::Blue;
    } else {
        status[i][j] = BoardStatus::Red;
    }

    // Detect points
    if (j % 2 == 0) {
        // Vertical button
        if (j != 0) {
            // Check left square
            if (status[i+1][j-1] != BoardStatus::Blank &&
                    status[i][j-2] != BoardStatus::Blank &&
                    status[i-1][j-1] != BoardStatus::Blank) {
                onBoxMade(i, j - 1);
            }
        }
        if (j != 6) {
            // Check right square
            if (status[i+1][j+1] != BoardStatus::Blank &&
                    status[i][j+2] != BoardStatus::Blank &&
                    status[i-1][j+1] != BoardStatus::Blank) {
                onBoxMade(i, j + 1);
            }
        }
    } else {
        // Horizontal button
        if (i != 0) {
            // Check top square
            if (status[i-1][j+1] != BoardStatus::Blank &&
                    status[i-2][j] != BoardStatus::Blank &&
                    status[i-1][j-1] != BoardStatus::Blank) {
                onBoxMade(i - 1, j);
            }
        }
        if (i != 6) {
            // Check bottom square
            if (status[i+1][j+1] != BoardStatus::Blank &&
                    status[i+2][j] != BoardStatus::Blank &&
                    status[i+1][j-1] != BoardStatus::Blank) {
                onBoxMade(i + 1, j);
            }
        }
    }

    blueTurn = !blueTurn;

    updateScore();

    disconnect(sender(), 0, this, 0);
}

void MainWindow::changeToMenu() {
    stackedLayout->setCurrentIndex(0);
}

void MainWindow::changeToBoard() {
    if (firstGame){
        stackedLayout->setCurrentIndex(1);
        firstGame = false;
    } else {
        resetBoard();
        stackedLayout->setCurrentIndex(1);
    }
}

void MainWindow::changeToInstructions() {
    stackedLayout->setCurrentIndex(2);
}
