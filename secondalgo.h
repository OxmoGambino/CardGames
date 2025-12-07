#pragma once
#include <QDialog>
#include <QGridLayout>
#include <QVector>
#include <QPushButton>
#include <QLabel>
#include "gameengine.h"

class SecondWindow;

class QGridLayout;
QT_BEGIN_NAMESPACE
namespace Ui { class SecondAlgo; }
QT_END_NAMESPACE

class SecondAlgo : public QDialog {
    Q_OBJECT

public:
    explicit SecondAlgo(QWidget *parent, int rows_ = 0, int cols_ = 0);
    ~SecondAlgo();

    void playGame();
    void endCondition();
    void autoSolve();
    void updateDisplay();
    void initializeGUI();



private:
    Ui::SecondAlgo *ui;

    gameEngine engine;
    int rows=0;
    int cols=0;

    QGridLayout* grid = nullptr;

    std::vector<QPushButton*> cards;


    int firstValue = 0;
    int firstValueIndex;
    int secondValue = 0;
    int secondValueIndex;
    long long genDuration;

    QLabel* attemptLabel;
    QLabel* pairLabel;
    bool locked=false;
    bool alreadyPlayed=false;

    QWidget* central;


    SecondWindow *secondwindow;
protected:
    void closeEvent(QCloseEvent *event) override;


};
