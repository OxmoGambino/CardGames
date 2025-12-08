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
namespace Ui { class FirstAlgo; }
QT_END_NAMESPACE

class FirstAlgo : public QDialog {
    Q_OBJECT

public:
    explicit FirstAlgo(QWidget *parent, int rows_ = 0, int cols_ = 0);
    ~FirstAlgo();

    void updateDisplay();
    void initializeGUI();
    void autoSolve();
    void endCondition();
    void playGame();





private:
    Ui::FirstAlgo *ui;
    gameEngine engine;
    QGridLayout* grid = nullptr;

    std::vector<QPushButton*> cards;
    std::vector<bool> revealed;

    int firstValue = 0;
    int firstValueIndex;
    int secondValue = 0;
    int secondValueIndex;
    long long genDuration; //type renvoyé par std::chrono...

    QLabel* attemptLabel;
    QLabel* pairLabel;
    bool locked=false;
    bool alreadyPlayed=false;
    bool saveAsked=false;

    QWidget* central;

    SecondWindow *secondwindow;
protected:
    void closeEvent(QCloseEvent *event) override;


};
