#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <vector>
#include <QString>
#include <unordered_set>
#include <utility>


class gameEngine
{
public:
    bool isSaved = false; //booléen qui a vocation a être modif par les autres méthodes des autres classes , d'où le public

    gameEngine(int rows_, int cols_);

    //Init du jeu
    void createCards();
    void shuffleCards();

    bool playMove(int idx1, int indx2);
    bool hasBeenPlayed(int idx1, int idx2);
    bool loadFromFile(const QString& filename);
    bool saveGame(const QString& filename); //nouvelle méthode pour sauvegarder
    bool replayLoadedGame();

    //getters au cas où
    int getRows() const { return rows; }
    int getCols() const { return cols; }
    int getNbCards() const { return nbCards; }
    int getNbPairs() const { return nbPairs; }
    int getAttempts() const { return nbAttempt; }
    int getPairsFound() const { return pairFound; }

    //getter de valeur isolée
    int getCardValue(int index) const { return cardsValues[index]; }
    QString getLabel(int index) const { return labels[index]; }


    //getter par ref pour le passage général
    const std::vector<int>& getCardsValues() const { return cardsValues; } //passage par référence pour alléger la gestin en mémoire
    const std::vector<QString>& getLabels() const { return labels; }
    const std::vector<std::pair<int,int>>& getHistory() const { return hist; }

private :
    int rows = 0;
    int cols = 0;
    int nbCards = 0;
    int nbPairs = 0;
    int nbAttempt = 0;
    int pairFound = 0;



    std::vector<int> cardsValues;
    std::vector<QString> labels; // Plateau de jeu, utilisé en front pour afficher
    std::vector<std::pair<int, int>> hist; // Historique complet des coups joués (non normalisé, avec doublons)
    std::unordered_set<int> playedMoves; // Historique des coups normalisés sans doublons pour détection de coups déjà joués

};

#endif // GAMEENGINE_H
