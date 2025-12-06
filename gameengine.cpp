#include "gameengine.h"
#include <QRandomGenerator>
#include <algorithm> //utilisé pour le temps
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>


gameEngine::gameEngine(int rows_, int cols_) {
    rows = rows_;
    cols = cols_;
    nbCards = rows_*cols_;
    nbPairs = nbCards/2;

    cardsValues.reserve(nbCards);
    labels.reserve(nbCards);
}


void gameEngine::createCards(){

//cardValues -> comparer les valeurs des cartes plus tard et labels -> noms affiché sur les cartes
    for(int v=1; v<nbPairs+1 ; v++){
        cardsValues.push_back(v);
        cardsValues.push_back(v); //On veut deux fois chaques valeurs pour que les cartes aillent par pairs


        labels.push_back(QString::number(v) + "A"); //Nom qui sera affiché sur les cartes
        labels.push_back(QString::number(v) + "B");
    }
}

void gameEngine::shuffleCards(){
    for(int i=0 ; i<nbPairs ; i++){
        int j = QRandomGenerator::global()->bounded(cardsValues.size());
        int b;
        QString c;

        b = cardsValues[i]; //mélange des valeurs de cartes (pour les tests)
        cardsValues[i] = cardsValues[j];
        cardsValues[j] = b;

        c = labels[i]; //mélange du visuel // à utiliser pour la sauvegarde
        labels[i] = labels[j];
        labels[j] = c;
    }
}

bool gameEngine::hasBeenPlayed(int idx1, int idx2){
    int mini = std::min(idx1, idx2);
    int maxi = std::max(idx1, idx2);

    QString conc_str = QString::number(mini) + QString::number(maxi); //concaténation en une string
    int conc = conc_str.toInt(); // conversion de la concaténation en une entier unique normalisé


    bool alreadyPlayed = (playedMoves.insert(conc)).second; //teste l'insert.
    //True : insersion faite, coup non déjà joué
    //False : insersion non faite, coup déjà joué

    return !alreadyPlayed; //renvoie l'inverse du booléen pour la lisibilité

}


bool gameEngine::playMove(int idx1, int idx2){
    if (idx1 < 0 || idx2 < 0 || idx1 >=rows*cols || idx2 >= rows*cols){
        qDebug() << "Indice erroné";
        return false;
    }

    if(idx1 == idx2){
        qDebug() << "Erreur, deux fois la même carte";
        return false;
    }


    hist.push_back(std::make_pair(idx1,idx2)); //ajout du coup non normalisé à l'historique complet

    if(hasBeenPlayed(idx1, idx2)){ // fait l'ajoue à l'histo normalisé et renvoie le booléen
        nbAttempt ++; // ajout d'une pénalité
        qDebug() <<"coup déjà joué, pénalité";
    }
    nbAttempt ++; //ajout d'un coup normal si pas déjà joué


    bool isPair = (cardsValues[idx1] == cardsValues[idx2]);

    if(isPair){
        pairFound++;
        qDebug() << "Paire trouvée" << pairFound <<"/" << nbPairs;
    }
    else {
        qDebug() << "Pas une paire";
    }

    return isPair;
}

bool gameEngine::loadFromFile(const QString& filename){
    QString lab; // stockage temp des labels après
    int indexs; //stockage temp des index des cartes
    QFile file(filename);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Imposible de charger le fichier de sauvegarde";
        return false;
    }

    QTextStream in(&file);
    qDebug() << filename << "le fichier a bien été lu et reconnu";

    //lectuire des infos de base
    in >> rows >> cols;
    nbCards = rows*cols;
    nbPairs = nbCards/2;

    //Remplissage des labels
    for(int i = 0; i<rows*cols; i++){
        in >> lab;
        labels.push_back(lab);
        qDebug() << labels[i];
    }

    //Remplissage des valeurs des cartes
    for(int i = 0; i<rows*cols;i++){
        in >> indexs;
        cardsValues.push_back(indexs);
        qDebug() << cardsValues[i];
    }
    //int i = 0; //utilisé pour débug
    while(!in.atEnd()){ //jusqu'au bout du fichier
        int idx1,idx2;
        in >> idx1 >> idx2; // lecture des indexs

        hist.push_back(std::make_pair(idx1,idx2));
        //qDebug() << hist[i].first << hist[i].second << i;
        //i++;
    }

    file.close(); //plus besoin du fichier pour replay
    return true;
}

bool gameEngine::replayLoadedGame(){
    nbAttempt = 0;
    pairFound = 0;

    std::vector<std::pair<int,int>> loadedHist = hist;
    hist.clear();
    //comme on rejoue les coups, on va à nouveau remplir hist. Donc c'est bien d'avoir un loadedhist pour les jouers et un hist attribut vide pour la partie.
    //Ca évite que hist soit corrompu avec des doublons si on veur resauvarder par dessus

    for(int i = 0; i < loadedHist.size(); i++){ // boucle qui rejoue les coups
        int idx1 = loadedHist[i].first; int idx2 = loadedHist[i].second;
        qDebug() << "rejeu du coup " << i << " : " << idx1 << "&" << idx2;
        playMove(idx1, idx2);// jeu avec les deux index enregistrés
    }

    qDebug() << "État final: Tentatives=" << nbAttempt << ", Paires=" << pairFound << "/" << nbPairs;
    qDebug() << "=== Fin replayLoadedGame ===";
    return true;
}

bool gameEngine::saveGame(const QString& filename){
    qDebug() << "=== Début saveGame ===";
    qDebug() << "Filename:" << filename;

    QFile file(filename);

    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug() << "ERREUR: Impossible d'ouvrir";
        return false;
    }

    qDebug() << "Fichier ouvert OK";

    QTextStream out(&file);

    out<<rows<<" "<<cols<<"\n";
    qDebug() << "Dimensions écrites:" << rows << cols;

    for(int i = 0;i < rows*cols; i++){ //écriture des labels visuels
        out << labels[i] << " ";
    }
    out<<"\n";
    qDebug() << "Grille écrite, taille labels:" << labels.size();

    for (int i = 0 ; i<rows*cols; i++) //écriture des index des cartes
    {
        out << cardsValues[i] << " ";
    }

    out<<"\n";

    qDebug() << "Grille écrite, taille indexs:" << cardsValues.size();

    for(int i = 0; i<hist.size(); i++){
        std::pair<int,int> move = hist[i];
        out << move.first << " " << move.second << "\n";
    }
    qDebug() << "Historique écrit, nombre de coups:" << hist.size();

    file.close();
    qDebug() << "=== Fin saveGame ===";

    return true;
}
