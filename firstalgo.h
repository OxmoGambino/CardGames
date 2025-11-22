#pragma once
#include <QDialog>
#include <QGridLayout>
#include <QVector>
#include <QPushButton>
#include <QLabel>

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

    void createCards();
    void shuffleCards();
    void playGame();
    void cardsRegister(int index);
    void cardsComparaison();
    void endCondition();
    void moveHistoric();



private:
    Ui::FirstAlgo *ui;

    int rows=0;
    int cols=0;

    QGridLayout* grid = nullptr;

    QVector<QPushButton*> cards;
    QVector<int> cardsValues;
    QVector<QString> labels;

    int firstIndex = 0;
    int firstIndexIndex;
    int secondIndex = 0;
    int secondIndexIndex;
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


};
