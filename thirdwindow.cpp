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
#include <QFile>
#include <QFileDialog>


ThirdWindow::ThirdWindow(QWidget *parent, int rows, int cols,int mode,const QString& filename)
    : QMainWindow(parent)
    , ui(new Ui::ThirdWindow)
    , engine(rows,cols)

{

    ui->setupUi(this);
    //qDebug() << "ThirdWindow rows =" << rows << "cols =" << cols;

    initializeGUI(); //

    if(mode == 0){ //mode de jeu classique avec création du jeu etc
        engine.createCards();
        engine.shuffleCards();
        playGame();
    }

    if(mode == 1){ //mode de jeu chargé avec cartes déjà créées
        loadGame(filename);  //  charge les données
    }

    setCentralWidget(central);

}

void ThirdWindow::initializeGUI(){ //copié collé de ce qui était mis dans le constructeur mais ça rend juste plus lisible en séparant


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
}


//Enregistrer et tourner les cartes cliquées
void ThirdWindow::cardsRegister(int index){
    QString current = cards[index]->text();

    //Enregistrer la valeur de la 1ère carte
    if (firstValue == 0 && secondValue == 0 && current=="?"){
        cards[index]->setText(engine.getLabel(index));
        firstValue = engine.getCardValue(index);
        firstValueIndex = index;
    }

    //Enregistrer la valeur de la seconde carte
    else if (firstValue!=0 && secondValue == 0 && current=="?" && index != firstValueIndex){
        cards[index]->setText(engine.getLabel(index));
        secondValue = engine.getCardValue(index);
        secondValueIndex = index;
        locked = true;


        //mécanique une fois que les deux cartes sont retournées (d'où le dans else if)

        bool isPair = engine.playMove(firstValueIndex, secondValueIndex);
        updateLabels();

        if(!isPair){ //Cas d'échec
            //Mettre les cartes en rouge
            cards[firstValueIndex]->setStyleSheet("background-color: rgb(150, 50, 0); color: white; font-weight: normal;");
            cards[secondValueIndex]->setStyleSheet("background-color: rgb(150, 50, 0); color: white; font-weight: normal;");


            QTimer::singleShot(1250, this, [this]() { //Le QTimer permet de voir l'affichage de la deuxième carte après t ms
                cards[firstValueIndex]->setText("?");
                cards[secondValueIndex]->setText("?");

                cards[firstValueIndex]->setStyleSheet(""); //redonner le style d'origine aux cartes
                cards[secondValueIndex]->setStyleSheet("");

                firstValue = 0;
                secondValue = 0;
                locked = false;
            });
        }

        else{ //cas de victoire
            cards[firstValueIndex]->setStyleSheet("background-color: rgb(30, 140, 0); color: white; font-weight: bold;"); //colorier la carte en vert si la paire est bonne
            cards[secondValueIndex]->setStyleSheet("background-color: rgb(30, 140, 0); color: white; font-weight: bold;");

            firstValue = 0;
            secondValue=0;
            locked=false;
            //plus besoin de pairFound++ et actualisation de l'affichage (déjà dans playMove)
        }

        endCondition(); // vérification de fin de partie
    }
}

void ThirdWindow::updateLabels() { //avant c'était dans playMove, mais mtn c'est du graphique donc cest ici
    attemptLabel->setText("Tentatives : " + QString::number(engine.getAttempts()));
    pairLabel->setText("Paires trouvées : " + QString::number(engine.getPairsFound()));
}



void ThirdWindow::endCondition(){
    qDebug() << ">>> endCondition appelée, pairFound:" << engine.getPairsFound() << "nbPairs:" << engine.getNbPairs();

    if(engine.getPairsFound() == engine.getNbPairs()){
        qDebug() << ">>> Condition vérifiée, partie terminée";

        QMessageBox msgBox;
        msgBox.setInformativeText("Vous avez gagné !");
        msgBox.setText("Bravo, vous avez trouvé toutes les paires en " +
                       QString::number(engine.getAttempts()) + " coups !");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);

        QAbstractButton *saveButton   = msgBox.button(QMessageBox::Save);
        QAbstractButton *cancelButton = msgBox.button(QMessageBox::Cancel);

        saveButton->setText("Sauvegarder");
        cancelButton->setText("Quitter");

        QPushButton *retryButton = msgBox.addButton("Rejouer", QMessageBox::ActionRole);

        msgBox.setDefaultButton(QMessageBox::Cancel);

        qDebug() << ">>> Affichage du QMessageBox";
        msgBox.exec();
        qDebug() << ">>> QMessageBox fermé";

        qDebug() << ">>> Bouton cliqué:" << msgBox.clickedButton()->text();

        if (msgBox.clickedButton() == saveButton){
            qDebug() << ">>> Bouton SAUVEGARDER cliqué";
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







void ThirdWindow::playGame(){

    int rows = engine.getRows();
    int cols = engine.getCols();

    for(int r=0 ; r<rows ; r++){
        for(int c=0; c<cols ; c++){

            QPushButton* card = new QPushButton("?", central); //création d'un pushButton avec central pour parent et "?" en txt
            card->setFixedSize(120,150);

            grid->addWidget(card, r ,c); //Ajouter card au widget à l'emplacement r ,c
            cards.push_back(card);

            int index = r*cols+c; //2D vers 1D

            //retourner les cartes deux par deux et retournement lors de non réussite
            connect(card, &QPushButton::clicked, this, [this,index](){

                if (locked==true){ //Pour ne pas pouvoir retourner + de deux cartes
                    return;
                }


                QString current = cards[index]->text(); //Pour ne pas retourner une carte déjà tourné
                if (current!="?"){
                    return;
                }

                cardsRegister(index); // cardsRegister fait aussi le boulot de feu cardsComparaison
            });
        }
    }

}




//Demande au joueur s'il souhaite sauvegarder lorsqu'il tente de quitter l'application en pleine partie
void ThirdWindow::closeEvent(QCloseEvent *event){
    qDebug()<< "Entrée dans le closeEvent";
    if(engine.getPairsFound()==engine.getNbPairs() || engine.isSaved == true){ // lorsque j'ai fini la partie et que je clique sur la croix, le jeu se ferme et ne demande pas une autre sauvegarde
        event->accept();
        return;
    }
    QMessageBox msgBox;
    msgBox.setText("Voulez-vous sauvegarder la partie ?");

    QPushButton *saveButton = msgBox.addButton("Oui", QMessageBox::ActionRole);
    QPushButton *leaveButton = msgBox.addButton("Non", QMessageBox::ActionRole);
    QPushButton *retryButton = msgBox.addButton("Recommencer", QMessageBox::ActionRole);

    msgBox.exec();

    if(msgBox.clickedButton()==saveButton){
        QString filename = QFileDialog::getSaveFileName(this,"Sauvegarder la partie","","Text files (*.txt) ;; All files (*)",nullptr,QFileDialog::DontUseNativeDialog);
        // Text files : suggestion par défault. All Files : deuxième filtre si l'utilisateur veut mettre une extension particulière.

        if(!filename.isEmpty()) { //vérification d'un nom valide
            engine.saveGame(filename);
            event -> accept(); //fermer la fenêtre
        }
        else{event->ignore();}
    }

    else if(msgBox.clickedButton()==leaveButton){
        event -> accept(); //accepter la fermeture de la fenêtre
    }

    else if (msgBox.clickedButton() == retryButton){
        qDebug() << ">>> Bouton REJOUER cliqué";
        close();
        secondwindow = new SecondWindow(nullptr); //nullptr pour ne pas avoir this en parent pour ne pas être lié si l'un est fermé
        secondwindow->show();
    }
}


void ThirdWindow::displayFoundCards(){
    qDebug() << "=== Affichage des cartes trouvées ===";

    // Set pour stocker les cartes trouvées (évite les doublons)
    QSet<int> foundCards;                                                                                    //peut être utile de l'avoir en attrivut peut être
    const std::vector<std::pair<int,int>>& hist = engine.getHistory();

    // Parcourir l'historique pour trouver les paires réussies
    for(const auto& move : hist){
        int idx1 = move.first;
        int idx2 = move.second;

        // Si c'est une paire (mêmes valeurs)
        if(engine.getCardValue(idx1) == engine.getCardValue(idx2)){
            foundCards.insert(idx1);
            foundCards.insert(idx2);
            qDebug() << "Paire:" << idx1 << "et" << idx2 << "→" << engine.getLabel(idx1);
            cards[idx1]->setStyleSheet("background-color: rgb(30, 140, 0); color: white; font-weight: normal;");
            cards[idx2]->setStyleSheet("background-color: rgb(30, 140, 0); color: white; font-weight: normal;");
        }
    }

    // Afficher toutes les cartes trouvées
    for(int index : foundCards){
        cards[index]->setText(engine.getLabel(index));      // Afficher le label
        cards[index]->setEnabled(false);           // Désactiver le bouton
        qDebug() << "Carte" << index << "affichée:" << engine.getLabel(index);
    }

    qDebug() << "Total de cartes affichées:" << foundCards.size();
    qDebug() << "=== Fin displayFoundCards ===";
}

void ThirdWindow::loadGame(const QString& filename){
    const auto start = std::chrono::steady_clock::now();
    engine.loadFromFile(filename);

    playGame(); //création des boutons
    engine.replayLoadedGame();
    displayFoundCards();
    updateLabels();

    const auto end = std::chrono::steady_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(); //permet la conv en ms affichable

    if (engine.getNbPairs() == engine.getPairsFound()){
        QMessageBox::information(this,"Chargement", QString("Chargement effectué en %1ms.\nLa partie est terminée, %2 coups ont été joués (pénalités comptées)").arg(duration).arg(engine.getAttempts()));
    }
    else{
        QMessageBox::information(this,"Chargement", QString("Chargement effectué en %1ms.\nLa partie n'est pas terminée :\n- %2 coups ont été joués.\n- %3 paires ont été trouvées.\nBonne chance pour la fin de partie !" ).arg(duration).arg(engine.getAttempts()).arg(engine.getPairsFound()));
    }

}

ThirdWindow::~ThirdWindow(){
    delete ui;
}
