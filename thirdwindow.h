#pragma once
#include <QMainWindow>
#include <QGridLayout>
#include <QVector>
#include <QPushButton>
#include <QLabel>
#include "secondwindow.h"
#include "gameengine.h"

class QGridLayout;

QT_BEGIN_NAMESPACE
namespace Ui { class ThirdWindow; }
QT_END_NAMESPACE

class ThirdWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit ThirdWindow(QWidget *parent = nullptr, int rows_ = 0, int cols_ = 0,int mode = 0, const QString& filename ="");
    ~ThirdWindow();

    void playGame();
    void cardsRegister(int index);
    void endCondition();


    void loadGame(const QString& filename);
    void replayLoadedGame();
    void displayFoundCards();


    void initializeGUI();
    void updateLabels();

private:
    Ui::ThirdWindow *ui;

    gameEngine engine; //inclut mtn toute la logique de jeu hors GUI


    int firstValue = 0;
    int firstValueIndex;
    int secondValue = 0;
    int secondValueIndex;
    bool locked=false;


    QGridLayout* grid = nullptr;
    std::vector<QPushButton*> cards; //vecteur de boutons

    QLabel* attemptLabel;
    QLabel* pairLabel;
    QWidget* central;


    SecondWindow *secondwindow;

protected:
    void closeEvent(QCloseEvent *event) override;

};
