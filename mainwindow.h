#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <vector>

using std::vector;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void init();

private slots:
    void handleButton();
    void changeToBoard();
    void changeToMenu();
    void changeToInstructions();

private:
    Ui::MainWindow *ui;

    bool firstGame = true;

    vector<vector<QPushButton*>> buttons;
    vector<vector<int>> status;
    vector<vector<QWidget*>> colorSquares;

    QLabel* turn;
    bool blueTurn;

    QLabel* score;
    int redScore;
    int blueScore;

    QPushButton* home;

    QVBoxLayout* menuLayout;
    QGridLayout* board;
    QVBoxLayout* instrucLayout;
    QStackedLayout* stackedLayout;

    void setupMenu();
    void setupBoard();
    void resetBoard();
    void setupInstruc();
    void updateScore();
    void onBoxMade(int, int);
};

#endif // MAINWINDOW_H
