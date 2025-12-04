#include "firstalgo.h"
#include "secondwindow.h"
#include "ui_firstalgo.h"
//#include <iostream>
#include <QGridLayout>
#include <QWidget>
#include <QPushButton>
#include <QDebug>
#include <QRandomGenerator>
#include <QTimer>
#include <QMessageBox>
#include <QCloseEvent>
//class ThirdWindow




FirstAlgo::FirstAlgo(QWidget *parent, int rows_, int cols_)
    : QDialog(parent)
    , ui(new Ui::FirstAlgo)

{
    rows = rows_; //on copie la valeur du paramètre rows_ dans l'attribut rows -> donne la taille de la fenêtre précédente
    cols = cols_; //idem

    ui->setupUi(this);

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


    playGame();
}



void FirstAlgo::createCards( ){
    nbCards = rows*cols;
    nbPairs = nbCards/2;

    cards.clear(); //au cas où
    cardsValues.clear();
    labels.clear();

    cards.reserve(nbCards); //Allouer assez de places dans la grille pour toutes les cartes
    cardsValues.reserve(nbCards);

    //cardValues -> comparer les valeurs des cartes plus tard et labels -> noms affiché sur les cartes
    for(int v=1; v<nbPairs+1 ; v++){
        cardsValues.push_back(v);
        cardsValues.push_back(v); //On veut deux fois chaques valeurs pour que les cartes aillent par pairs


        labels.push_back(QString::number(v) + "A"); //Nom qui sera affiché sur les cartes
        labels.push_back(QString::number(v) + "B");
    }
}







void FirstAlgo::shuffleCards(){
    for(int i=0 ; i<nbPairs ; i++){
        int j = QRandomGenerator::global()->bounded(cardsValues.size());
        int b;
        QString c;
        b = cardsValues[i];
        cardsValues[i] = cardsValues[j];
        cardsValues[j] = b;

        c = labels[i];
        labels[i] = labels[j];
        labels[j] = c;
    }
}






//Enregistrer et tourner les cartes cliquées
void FirstAlgo::cardsRegister(int index){
    QString current = cards[index]->text();
    //Enregistrer la valeur de la 1ère carte
    if (firstValue == 0 && secondValue == 0 && current=="?"){
        cards[index]->setText(labels[index]);
        firstValue = cardsValues[index];
        firstValueIndex = index;
    }

    //Enregistrer la valeur de la seconde carte
    if (firstValue!=0 && secondValue == 0 && current=="?" && index != firstValueIndex){
        cards[index]->setText(labels[index]);
        secondValue = cardsValues[index];
        secondValueIndex = index;
        locked = true;
    }
}




void FirstAlgo::moveHistoric(){
    int indexMove1;
    int indexMove2;

    if(firstValueIndex <= secondValueIndex){
        indexMove1 = firstValueIndex;
        indexMove2 = secondValueIndex;
    }
    if(firstValueIndex > secondValueIndex){
        indexMove2 = firstValueIndex;
        indexMove1 = secondValueIndex;
    }


    std::string s1 = std::to_string(indexMove1);
    std::string s2 = std::to_string(indexMove2);


    int moveConcatenation = std::stoi(s1+s2);

    if(historic.contains(moveConcatenation)==false){
        historic.insert(moveConcatenation);
    }
    else{
        nbAttempt++;
    }

}



//Regarde si les deux cartes retournées sont les mêmes
void FirstAlgo::cardsComparaison(){

    if(secondValue != 0 && firstValue != 0){
        moveHistoric(); //Vérifier que ce coup n'a pas déjà été réalisé auparavent
        nbAttempt ++;
        attemptLabel->setText("Tentatives : " + QString::number(nbAttempt));

        if(firstValue==secondValue){
            firstValue = 0;
            secondValue=0;
            locked=false;
            pairFound++;
            pairLabel->setText("Paires trouvées : " + QString::number(pairFound));
        }

        //Cas d'échec
        else{
            cards[firstValueIndex]->setText("?");
            cards[secondValueIndex]->setText("?");
            firstValue = 0;
            secondValue = 0;
            locked = false;
        }

        //Cas de réussite

    }
}







void FirstAlgo::endCondition(){
    if(pairFound == nbPairs){ //Fin de jeu (on a trouvé toute les pairs)

        QMessageBox::information(this,
                                 "Vous avez gagné !",
                                 "Bravo, vous avez trouvé toutes les paires en " + QString::number(nbAttempt) + " coups !");
    }
}



void FirstAlgo::autoSolve(){


    while(pairFound < nbPairs){

        int coupsAleatoires = 0; //Pour chaque retour de boucle on réinitialise à 0

        //Partie 100 coups aléatoires
        while (coupsAleatoires < 100) {

            std::vector<int> hiddenCards;

            hiddenCards.reserve(nbCards);
            for (int i = 0; i < nbCards; ++i) {  //Toute les cartes avec "?" vont dans hiddenCards
                if (cards[i]->text() == "?") {
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

            cardsRegister(rdmHiddenCardA);
            cardsRegister(rdmHiddenCardB);
            cardsComparaison();


            coupsAleatoires++;
        }


         //Partie triche V2


        bool foundPair = false; //Pour chaque retour de boucle on réinitialise à false

        while(!foundPair){
            std::vector<int> hiddenCardsCheat;

            for (int i = 0; i < nbCards; ++i) {  //Toute les cartes avec "?" vont dans hiddenCards
                if (cards[i]->text() == "?") {
                    hiddenCardsCheat.push_back(i);
                }
            }

            if (hiddenCardsCheat.size() < 2) {
                break; // on ne peut plus jouer de coup aléatoire
            }

            int valueA = cardsValues[hiddenCardsCheat[0]]; //La valeur de la première carte du paquet
            int valueB = -1; //On va chercher à quel indice de hiddenCardsCheat valueB vaudra valueA
            int ind =0; //index pour boucler le while

            while(valueA != valueB){
                ind++;
                valueB = cardsValues[hiddenCardsCheat[ind]];
            }

            if(valueA == valueB){
                cardsRegister(hiddenCardsCheat[0]);
                cardsRegister(hiddenCardsCheat[ind]);
                cardsComparaison();
                foundPair=true;
            }
        }
    }
    endCondition();
}








void FirstAlgo::playGame(){
    createCards();
    shuffleCards();

    //Création de la grille
    for(int r = 0; r < rows; ++r){
        for(int c = 0; c < cols; ++c){
            QPushButton* card = new QPushButton("?", central);
            card->setFixedSize(120,150);

            grid->addWidget(card, r, c);
            cards.push_back(card);
        }
    }

        autoSolve(); // Algorithme "triche"
}




//Demande au joueur s'il souhaite sauvegarder lorsqu'il tente de quitter l'application en pleine partie
void FirstAlgo::closeEvent(QCloseEvent *event){
    QMessageBox msgBox;
    msgBox.setText("Voulez-vous sauvegarder la partie ?");

    QPushButton *saveButton = msgBox.addButton("Oui", QMessageBox::ActionRole);
    QPushButton *leaveButton = msgBox.addButton("Non", QMessageBox::ActionRole);

    msgBox.exec();

    if(msgBox.clickedButton()==saveButton){
        //GuiGui's Function
    }

    else if(msgBox.clickedButton()==leaveButton){
        hide();
    }
}








FirstAlgo::~FirstAlgo()
{
    delete ui;
}
