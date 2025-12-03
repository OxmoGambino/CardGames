#include "secondalgo.h"
#include "secondwindow.h"
#include "ui_SecondAlgo.h"
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




SecondAlgo::SecondAlgo(QWidget *parent, int rows_, int cols_)
    : QDialog(parent)
    , ui(new Ui::SecondAlgo)

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


void SecondAlgo::createCards( ){
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







void SecondAlgo::shuffleCards(){
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
void SecondAlgo::cardsRegister(int index){
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




void SecondAlgo::moveHistoric(){
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
void SecondAlgo::cardsComparaison(){

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







void SecondAlgo::endCondition(){
    if(pairFound == nbPairs){ //Fin de jeu (on a trouvé toute les pairs)

        QMessageBox::information(this,
                                 "Vous avez gagné !",
                                 "Bravo, vous avez trouvé toutes les paires en " + QString::number(nbAttempt) + " coups !");
    }
}



 //Il manque un truc pour la rapidité, càd si on tourne un truc dont on a déjà la deuxième paire, on prend rdmValueB en conséquence

void SecondAlgo::autoSolve(){

    std::unordered_map<int, std::vector<int>> memory; //int -> valeur, & vecteur<int> -> liste des indices où est la valeur.
    std::vector<int> hiddenCards;
    std::vector<bool> alreadySeen(cards.size(), false); //chaque index est initialisé à false
    hiddenCards.reserve(cards.size());


    while(nbPairs > pairFound){

        bool playedKnownPair = false;

        hiddenCards.clear();
        for(int i=0 ; i<cards.size(); i++){
            if(cards[i]->text() == "?"){
                hiddenCards.push_back(i);
            }
        }

        //Partie 1 : Si on connaît déjà une paire
        for (auto &entry : memory) { //boucle pour parcourir un unordored_map
            //int value = entry.first;
            std::vector<int> &indices = entry.second;

            if (indices.size() >= 2) {
                int indA = indices[0];
                int indB = indices[1];
                if(cards[indA]->text() == "?" && cards[indB]->text() == "?"){
                    cardsRegister(indA);
                    cardsRegister(indB);
                    cardsComparaison();

                    playedKnownPair = true;
                    break;
                }
            }
        }

        if(playedKnownPair){ //On retourne dans la boucle for pour voir si on a pas d'autre pairs à tester
            continue;
        }


        //Partie 2 : si on ne connaît aucune paire
        int rdmValueA = QRandomGenerator::global()->bounded(hiddenCards.size());
        int indexA = hiddenCards[rdmValueA];

        //Au cas où toute les cartes ont déjà eté regardées.
        int count = 0;
        for(int i=0 ; i<nbCards ; i++){
            if(alreadySeen[i] == true){
                count++;
            }
            else{ //pour éviter trop de tour de boucle inutile
                break;
            }
        }

        while(alreadySeen[indexA] == true){ //Pour ne pas tirer une carte dont on connaît déjà le contenu
            if(count == nbCards){
                break;
            }
            rdmValueA = QRandomGenerator::global()->bounded(hiddenCards.size());
            indexA = hiddenCards[rdmValueA];
        }

        //Si on tourne une carte dont on connaît la paire ça les met ensemble directement
        int indexB;
        if(memory[cardsValues[indexA]].size() == 1 && indexA != memory[cardsValues[indexA]][0]){
            indexB = memory[cardsValues[indexA]][0];
        }

        else{
            int rdmValueB = QRandomGenerator::global()->bounded(hiddenCards.size());
            indexB = hiddenCards[rdmValueB];

            while(rdmValueA==rdmValueB || alreadySeen[indexB]==true){ //Ne pas tirer deux fois la même carte
                rdmValueB = QRandomGenerator::global()->bounded(hiddenCards.size());
                indexB = hiddenCards[rdmValueB];
            }
        }

        //Donc là on a deux valeurs rdmValueA et rdmValueB qui sont différentes et dont les pairs n'ont pas encore été trouvées.

        cardsRegister(indexA);
        cardsRegister(indexB);
        cardsComparaison();

        alreadySeen[indexA] = true;
        alreadySeen[indexB] = true;


        //Partie 3 : Remplissage de memory
        //Condition pour éviter d'ajouter deux fois le même indice pour la même valeur dans memory
        if(memory[cardsValues[indexA]].size() != 0){
            if(memory[cardsValues[indexA]].size() == 1){
                if(memory[cardsValues[indexA]][0] != indexA){
                    memory[cardsValues[indexA]].push_back(indexA);//Ajout de l'indice de la carte dans memory
                }
            }
            if(memory[cardsValues[indexA]].size() == 2){
                if(memory[cardsValues[indexA]][0] != indexA && memory[cardsValues[indexA]][1] != indexA){
                    memory[cardsValues[indexA]].push_back(indexA);//Ajout de l'indice de la carte dans memory
                }
            }
        }
        //Si y'a encore rien dans la liste d'indices
        else{
            memory[cardsValues[indexA]].push_back(indexA);
        }


        if(memory[cardsValues[indexB]].size() != 0){
            if(memory[cardsValues[indexB]].size() == 1){
                if(memory[cardsValues[indexB]][0] != indexB){
                    memory[cardsValues[indexB]].push_back(indexB);//Ajout de l'indice de la carte dans memory
                }
            }
            if(memory[cardsValues[indexB]].size() == 2){
                if(memory[cardsValues[indexB]][0] != indexB && memory[cardsValues[indexB]][1] != indexB){
                    memory[cardsValues[indexB]].push_back(indexB);//Ajout de l'indice de la carte dans memory
                }
            }
        }
        else{
            memory[cardsValues[indexB]].push_back(indexB);
        }

    }

    endCondition();
}






void SecondAlgo::playGame(){
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
void SecondAlgo::closeEvent(QCloseEvent *event){
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








SecondAlgo::~SecondAlgo()
{
    delete ui;
}
