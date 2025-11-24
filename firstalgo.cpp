#include "firstalgo.h"
#include "secondwindow.h"
#include "ui_firstalgo.h"
#include <iostream>
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


    createCards();

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





FirstAlgo::~FirstAlgo()
{
    delete ui;
}
