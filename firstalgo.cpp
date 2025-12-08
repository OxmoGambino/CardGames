#include "firstalgo.h"
#include "secondwindow.h"
#include "ui_firstalgo.h"
#include "gameengine.h"
#include <QGridLayout>
#include <QWidget>
#include <QPushButton>
#include <QDebug>
#include <QRandomGenerator>
#include <QTimer>
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include <QCloseEvent>


FirstAlgo::FirstAlgo(QWidget *parent, int rows_, int cols_)
    : QDialog(parent)
    , ui(new Ui::FirstAlgo)
    , engine(rows_,cols_)

{
    ui->setupUi(this);


    initializeGUI();
    const auto start = std::chrono::steady_clock::now();
    engine.createCards();
    engine.shuffleCards();

    playGame(); //affichage graphique
    autoSolve();
    const auto end = std::chrono::steady_clock::now();
    genDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    endCondition();

}

void FirstAlgo::updateDisplay() { //affichage des cartes
    attemptLabel->setText("Tentatives : " + QString::number(engine.getAttempts()));
    pairLabel->setText("Paires trouvées : " + QString::number(engine.getPairsFound()));

    // Afficher les cartes trouvées
    const auto& hist = engine.getHistory();
    for (const auto& move : hist) {
        int idx1 = move.first;
        int idx2 = move.second;

        if (engine.getCardValue(idx1) == engine.getCardValue(idx2)) {
            cards[idx1]->setText(engine.getLabel(idx1));
            cards[idx2]->setText(engine.getLabel(idx2));

            cards[idx1]->setStyleSheet("background-color: rgb(30, 140, 0); color: white; font-weight: normal;");
            cards[idx2]->setStyleSheet("background-color: rgb(30, 140, 0); color: white; font-weight: normal;");
        }
    }
}


void FirstAlgo::initializeGUI(){
    //Création de la grille avec cartes et les infos du nombre de tentatives et du nombre de pairs trouvées
    central = new QWidget(this); //widget qui contiendra la grille
    QVBoxLayout* mainLayout = new QVBoxLayout(central); //layout vertical
    QHBoxLayout* infoLayout = new QHBoxLayout(); //layout horizontal pour les labels


    attemptLabel = new QLabel("Tentatives : 0", this);
    pairLabel    = new QLabel("Paires trouvées : 0", this);

    attemptLabel->setAlignment(Qt::AlignCenter); //Pour centrer les labels (c'est + stylé)
    pairLabel->setAlignment(Qt::AlignCenter);

    infoLayout->addWidget(attemptLabel);
    infoLayout->addWidget(pairLabel); //ajout des labels dans la zone info

    mainLayout->addLayout(infoLayout); //labels en haut


    grid = new QGridLayout();
    mainLayout->addLayout(grid);

    setLayout(mainLayout);
    revealed.resize(engine.getNbCards(), false); //construction d'un vecteur qui indiquera les cartes revélées (init a false)
}



void FirstAlgo::endCondition(){
    qDebug() << ">>> endCondition appelée, pairFound:" << engine.getPairsFound() << "nbPairs:" << engine.getNbPairs();

    if(engine.getPairsFound() == engine.getNbPairs()){
        qDebug() << ">>> Condition vérifiée, partie terminée";

        QMessageBox msgBox;
        msgBox.setInformativeText("Toutes les paires en " +
                                  QString::number(engine.getAttempts()) + " coups !");
        msgBox.setText("Génération d'une partie aléatoire en " + QString::number(genDuration) +" ms");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);

        QPushButton *retryButton = msgBox.addButton("Rejouer", QMessageBox::ActionRole);


        qDebug() << ">>> Affichage du QMessageBox";
        msgBox.exec();
        qDebug() << ">>> QMessageBox fermé";

        QAbstractButton *saveButton   = msgBox.button(QMessageBox::Save);
        QAbstractButton *cancelButton = msgBox.button(QMessageBox::Cancel);

        saveButton->setText("Sauvegarder");
        cancelButton->setText("Quitter");

        qDebug() << ">>> Bouton cliqué:" << msgBox.clickedButton()->text();

        if (msgBox.clickedButton() == saveButton){
            qDebug() << ">>> Bouton SAUVEGARDER cliqué secondAlgo";
            qDebug() << engine.isSaved;
            if (engine.isSaved == true){ // permet de ne pas sauvegarder deux fois
                qDebug() << "partie déjà sauvegardée";
                close();
                return;
            }
            else{
                QString filename = QFileDialog::getSaveFileName(
                    this,
                    "Sauvegarder la partie",
                    "",
                    "Text files (*.txt);;All files (*)",
                    nullptr,
                    QFileDialog::DontUseNativeDialog
                    );

                qDebug() << ">>> QFileDialog fermé, filename:" << filename;

                if(!filename.isEmpty()) {
                    qDebug() << ">>> Appel de saveGame()";
                    engine.saveGame(filename);
                    qDebug() << engine.isSaved;
                    qDebug() << ">>> Retour de saveGame()";
                } else {
                    qDebug() << ">>> Filename vide, sauvegarde annulée";
                }
            }
        }
        else if (msgBox.clickedButton() == cancelButton){
            qDebug() << ">>> Bouton QUITTER cliqué";
            close();
        }
        else if (msgBox.clickedButton() == retryButton){
            qDebug() << ">>> Bouton REJOUER cliqué";
            close();
            secondwindow = new SecondWindow(nullptr); //nullptr pour ne pas avoir this en parent pour ne pas être lié si l'un est fermé
            secondwindow->show();
        }

        qDebug() << ">>> Fin de endCondition";
    } else {
        qDebug() << ">>> Condition NON vérifiée";
    }
}



void FirstAlgo::autoSolve(){


    while(engine.getPairsFound() < engine.getNbPairs()){

        int coupsAleatoires = 0; //Pour chaque retour de boucle on réinitialise à 0

        //Partie 100 coups aléatoires
        while (coupsAleatoires < 100 && engine.getPairsFound() < engine.getNbPairs()){ //la deuxième condition permet de de s'arrêter de jouer si on a trouvé toutes les paires

            std::vector<int> hiddenCards;

            hiddenCards.reserve(engine.getNbCards());
            for (int i = 0; i < engine.getNbCards(); ++i) {  //Toute les cartes avec "?" vont dans hiddenCards
                if (!revealed[i]) {
                    hiddenCards.push_back(i);
                }
            }

            if (hiddenCards.size() < 2) {
                break; // on ne peut plus jouer de coup aléatoire
            }

            int rdmValueA = QRandomGenerator::global()->bounded(hiddenCards.size()); //valeurs aléatoire faisant au max
            int rdmValueB = QRandomGenerator::global()->bounded(hiddenCards.size()); // la taille de hiddenCards

            while(rdmValueA == rdmValueB){
                rdmValueB = QRandomGenerator::global()->bounded(hiddenCards.size());
            }

            int rdmHiddenCardA = hiddenCards[rdmValueA];
            int rdmHiddenCardB = hiddenCards[rdmValueB];

            bool isPair = engine.playMove(rdmHiddenCardA, rdmHiddenCardB);//permet de jouer le coup
            if (isPair) {
                revealed[rdmHiddenCardA] = true;
                revealed[rdmHiddenCardB] = true;
            }
            coupsAleatoires++;
        }

        if (engine.getPairsFound()==engine.getNbPairs()){//condition d'arrêt si pas besoin de tricher
            break;
        }
         //Partie triche V2


        bool foundPair = false; //Pour chaque retour de boucle on réinitialise à false

        while(!foundPair){
            std::vector<int> hiddenCardsCheat;

            for (int i = 0; i < engine.getNbCards(); ++i) {  //Toute les cartes avec "?" vont dans hiddenCards
                if (!revealed[i]) {
                    hiddenCardsCheat.push_back(i);
                }
            }

            if (hiddenCardsCheat.size() < 2) {
                break; // on ne peut plus jouer de coup aléatoire
            }

            int valueA = engine.getCardValue(hiddenCardsCheat[0]); //La valeur de la première carte du paquet
            int valueB = -1; //On va chercher à quel indice de hiddenCardsCheat valueB vaudra valueA
            int ind =-1; //index pour boucler le while

            for (int i = 1; i < hiddenCardsCheat.size(); ++i) {
                if (engine.getCardValue(hiddenCardsCheat[i]) == valueA) {
                    ind = i;
                    valueB = engine.getCardValue(hiddenCardsCheat[ind]);
                    break;
                }
            }

            if (ind != -1) { // si on a trouvé une paire
                engine.playMove(hiddenCardsCheat[0], hiddenCardsCheat[ind]);// si la paire est une paire, on joue le coup
                revealed[hiddenCardsCheat[0]] = true; //on update les cartes revealed
                revealed[hiddenCardsCheat[ind]] = true;
                foundPair = true; //permet de sortir de la boucle de triche, sinon, triche iullimité pas bien
            }
        }
    }
    updateDisplay(); //appelé uniquement à la fin car pas besoin d'affichage avant la fin de l'algo
    //QMessageBox::information(this, "Algorithme terminé","Toutes les paires trouvées en " + QString::number(engine.getAttempts()) + " coups !");
}

void FirstAlgo::playGame(){
    int rows = engine.getRows();
    int cols = engine.getCols();

    //Création de la grille
    for(int r = 0; r < rows; ++r){
        for(int c = 0; c < cols; ++c){
            QPushButton* card = new QPushButton("?", central);
            card->setFixedSize(120,150);

            grid->addWidget(card, r, c);
            cards.push_back(card);
        }
    }
}


void FirstAlgo::closeEvent(QCloseEvent *event){
}


FirstAlgo::~FirstAlgo()
{
    qDebug() <<"Destruction FirstAlgo";
    delete ui;
}
