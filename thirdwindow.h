#pragma once
#include <QMainWindow>
#include <QGridLayout>
#include <QVector>
#include <QPushButton>
#include <QLabel>
#include "secondwindow.h"

class QGridLayout;

QT_BEGIN_NAMESPACE
namespace Ui { class ThirdWindow; }
QT_END_NAMESPACE

class ThirdWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit ThirdWindow(QWidget *parent, int rows_ = 0, int cols_ = 0);
    ~ThirdWindow();

    void createCards();
    void shuffleCards();
    void playGame();
    void cardsRegister(int index);
    void cardsComparaison();
    void endCondition();
    void moveHistoric();



private:
    Ui::ThirdWindow *ui;

    int rows=0;
    int cols=0;

    QGridLayout* grid = nullptr;

    QVector<QPushButton*> cards;
    QVector<int> cardsValues;
    QVector<QString> labels;

    int firstValue = 0;
    int firstValueIndex;
    int secondValue = 0;
    int secondValueIndex;
    int nbAttempt = 0;
    int pairFound =0;
    int nbPairs=0;

    QLabel* attemptLabel;
    QLabel* pairLabel;
    bool locked=false;
    bool alreadyPlayed=false;

    QWidget* central;

    std::unordered_set<int> historic;

    SecondWindow *secondwindow;

protected:
    void closeEvent(QCloseEvent *event) override;

};
