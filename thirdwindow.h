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
    explicit ThirdWindow(QWidget *parent = nullptr, int rows_ = 0, int cols_ = 0,int mode = 0, const QString& filename ="");
    ~ThirdWindow();

    void createCards();
    void shuffleCards();
    void playGame();
    void cardsRegister(int index);
    void cardsComparaison();
    void endCondition();
    bool hasBeenPlayed(int idx1, int idx2);


    void saveGame(const QString& filename); //nouvelle méthode pour sauvegarder
    void loadGame(const QString& filename);
    void replayLoadedGame();
    void displayFoundCards();
    bool playMove(int idx1, int idx2);

private:
    Ui::ThirdWindow *ui;

    int rows=0;
    int cols=0;
    int firstIndex = 0;
    int firstIndexIndex;
    int secondIndex = 0;
    int secondIndexIndex;
    int nbAttempt = 0;
    int pairFound =0;
    int nbPairs=0;


    QGridLayout* grid = nullptr;
    QVector<QPushButton*> cards; //vecteur de boutons
    QVector<int> cardsValues; // Plateau de jeu, utilisé en back pour jouer
    QVector<QString> labels; // Plateau de jeu, utilisé en front pour afficher

    QVector<std::pair<int, int>> hist; // Historique complet des coups joués (non normalisé, avec doublons)
    std::unordered_set<int> playedMoves; // Historique des coups normalisés sans doublons pour détection de coups déjà joués

    QLabel* attemptLabel;
    QLabel* pairLabel;
    QWidget* central;

    bool locked=false;
    bool alreadyPlayed=false;



    SecondWindow *secondwindow;

protected:
    void closeEvent(QCloseEvent *event) override;

};
