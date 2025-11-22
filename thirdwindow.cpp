#include "thirdwindow.h"
#include "ui_thirdwindow.h"
// #include <iostream>
#include <QGridLayout>
#include <QWidget>
#include <QPushButton>
#include <QDebug>
#include <QRandomGenerator>
#include <QTimer>
#include <QMessageBox>
#include <QCloseEvent>



ThirdWindow::ThirdWindow(QWidget *parent, int rows_, int cols_)
    : QMainWindow(parent)
    , ui(new Ui::ThirdWindow)

{
    rows = rows_; //on copie la valeur du paramètre rows_ dans l'attribut rows -> donne la taille de la fenêtre précédente
    cols = cols_; //idem

    //qDebug() << "ThirdWindow rows =" << rows << "cols =" << cols;

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

    central->setLayout(grid); //attacher la grille au widget
    setCentralWidget(central);

    grid = new QGridLayout();
    mainLayout->addLayout(grid);

    central->setLayout(mainLayout);
    setCentralWidget(central);

    playGame();

    setCentralWidget(central);

}





void ThirdWindow::createCards(){
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







void ThirdWindow::shuffleCards(){
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
void ThirdWindow::cardsRegister(int index){
    QString current = cards[index]->text();
    //Enregistrer la valeur de la 1ère carte
    if (firstIndex == 0 && secondIndex == 0 && current=="?"){
        cards[index]->setText(labels[index]);
        firstIndex = cardsValues[index];
        firstIndexIndex = index;
    }

    //Enregistrer la valeur de la seconde carte
    if (firstIndex!=0 && secondIndex == 0 && current=="?" && index != firstIndexIndex){
        cards[index]->setText(labels[index]);
        secondIndex = cardsValues[index];
        secondIndexIndex = index;
        locked = true;
    }
}




void ThirdWindow::moveHistoric(){
    int indexMove1;
    int indexMove2;

    if(firstIndexIndex <= secondIndexIndex){
        indexMove1 = firstIndexIndex;
        indexMove2 = secondIndexIndex;
    }
    if(firstIndexIndex > secondIndexIndex){
        indexMove2 = firstIndexIndex;
        indexMove1 = secondIndexIndex;
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




void ThirdWindow::cardsComparaison(){

    if(secondIndex != 0 && firstIndex != 0){
        moveHistoric(); //Vérifier que ce coup n'a pas déjà été réalisé auparavent
        nbAttempt ++;
        attemptLabel->setText("Tentatives : " + QString::number(nbAttempt));

        if(firstIndex!=secondIndex){ //Cas d'échec
            QTimer::singleShot(1250, this, [this]() { //Le QTimer permet de voir l'affichage de la deuxième carte après t ms
                cards[firstIndexIndex]->setText("?");
                cards[secondIndexIndex]->setText("?");
                firstIndex = 0;
                secondIndex = 0;
                locked = false;
            });
        }

        if(firstIndex==secondIndex){ //Cas de réussite
            firstIndex = 0;
            secondIndex=0;
            locked=false;
            pairFound++;
            pairLabel->setText("Paires trouvées : " + QString::number(pairFound));
        }
    }
}









void ThirdWindow::endCondition(){
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








void ThirdWindow::playGame(){
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
            connect(card, &QPushButton::clicked, this, [this,index](){
                //QString current = cards[index]->text();

                if (locked==true){    //Pour ne pas pouvoir retourner + de deux cartes
                    return;
                }


                QString current = cards[index]->text(); //Pour ne pas retourner une carte déjà tourné
                if (current!="?"){
                    return;
                }

                cardsRegister(index);
                cardsComparaison();
                endCondition();
            });
        }
    }

}




//Demande au joueur s'il souhaite sauvegarder lorsqu'il tente de quitter l'application en pleine partie
void ThirdWindow::closeEvent(QCloseEvent *event){
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








ThirdWindow::~ThirdWindow() {
    delete ui;
}
