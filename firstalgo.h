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
    void printCards();
    void playGame();
    void cardsRegister(int index);
    void cardsComparaison();
    void endCondition();
    void moveHistoric();
    void autoSolve();



private:
    Ui::FirstAlgo *ui;

    int rows=0;
    int cols=0;

    QGridLayout* grid = nullptr;

    std::vector<QPushButton*> cards;
    std::vector<int> cardsValues;
    std::vector<QString> labels;

    int firstValue = 0;
    int firstValueIndex;
    int secondValue = 0;
    int secondValueIndex;
    int nbAttempt = 0;
    int pairFound =0;
    int nbPairs=0;
    int nbCards=0;

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
