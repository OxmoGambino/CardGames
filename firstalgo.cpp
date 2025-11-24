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
    int nbCards = rows*cols;
    nbPairs = nbCards/2;

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




void FirstAlgo::cardsComparaison(){

    if(secondValue != 0 && firstValue != 0){
        moveHistoric(); //Vérifier que ce coup n'a pas déjà été réalisé auparavent
        nbAttempt ++;
        attemptLabel->setText("Tentatives : " + QString::number(nbAttempt));

        if(firstValue!=secondValue){ //Cas d'échec
            QTimer::singleShot(1250, this, [this]() { //Le QTimer permet de voir l'affichage de la deuxième carte après t ms
                cards[firstValueIndex]->setText("?");
                cards[secondValueIndex]->setText("?");
                firstValue = 0;
                secondValue = 0;
                locked = false;
            });
        }

        if(firstValue==secondValue){ //Cas de réussite
            firstValue = 0;
            secondValue=0;
            locked=false;
            pairFound++;
            pairLabel->setText("Paires trouvées : " + QString::number(pairFound));
        }
    }
}






void FirstAlgo::endCondition(){
    if(pairFound == nbPairs){ //Fin de jeu (on a trouvé toute les pairs)
        QMessageBox msgBox;
        msgBox.setInformativeText("Vous avez gagné !");
        msgBox.setText("Bravo, vous avez trouvé toutes les paires en " + QString::number(nbAttempt) + " coups !");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);

        QAbstractButton *saveButton   = msgBox.button(QMessageBox::Save); //Pour renommer le bouton Save
        QAbstractButton *cancelButton = msgBox.button(QMessageBox::Cancel); //Renommer le boutton Cancel

        saveButton->setText("Sauvegarder");
        cancelButton->setText("Quitter");

        QPushButton *retryButton = msgBox.addButton("Rejouer", QMessageBox::ActionRole); //Boutton pour rejouer


        msgBox.setDefaultButton(QMessageBox::Cancel); //le bouton de base c'est quitter

        msgBox.exec();

        if (msgBox.clickedButton() == saveButton){
            //Guillaume's script
        }

        else if (msgBox.clickedButton()==cancelButton){
            hide();
        }

        else if (msgBox.clickedButton()==retryButton){
            hide();
            secondwindow = new SecondWindow(this); //On réouvre la page du choix de la taille du plateau
            secondwindow->show();
        }

    }
}








void FirstAlgo::playGame(){
    createCards();
    shuffleCards();


    for(int r=0 ; r<rows ; r++){
        for(int c=0; c<cols ; c++){

            QPushButton* card = new QPushButton("?", central); //création d'un pushButton avec central pour parent et  "?" en txt
            card->setFixedSize(120,150);

            grid->addWidget(card, r ,c); //Ajouter card au widget à l'emplacement r ,c
            cards.push_back(card);

            int index = r*cols+c; //2D vers 1D
            //cardsValues.push_back(index);

            //retourner les cartes deux par deux et retournement lors de non réussite
            //connect(card, &QPushButton::clicked, this, [this,index](){
                //QString current = cards[index]->text();

            /*if (locked==true){    //Pour ne pas pouvoir retourner + de deux cartes
                return;
            }*/


            /*QString current = cards[index]->text(); //Pour ne pas retourner une carte déjà tourné
            if (current!="?"){
                return;
            }*/

            while(pairFound != nbCards/2){
                int rdmValue = QRandomGenerator::global()->bounded(cardsValues.size()/2); //Index
                //QString current = cards[rdmValue]->text();

                for(int indexRdmValue=0 ; indexRdmValue<rows ; indexRdmValue++){
                    if (cardsValues[indexRdmValue]==rdmValue){ // && current == "?"){
                        cardsRegister(indexRdmValue);
                    }
                }
                cardsComparaison();

            }
            endCondition();
        //});
        }
    }

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

















/*void FirstAlgo::playGame(){
    while(pairFound != nbCards/2){
        int rdmValue = QRandomGenerator::global()->bounded(cardsValues.size()/2); //Index
        QString current = cards[rdmValue]->text();

        for(int i=0 ; i<rows ; i++){
            if (cardsValues[i]==rdmValue && current == "?"){
                cards[i]->setText(labels[i]);
            }

        }
    }
}*/






FirstAlgo::~FirstAlgo()
{
    delete ui;
}
