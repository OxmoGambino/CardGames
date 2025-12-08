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
#include <QFileDialog>



SecondAlgo::SecondAlgo(QWidget *parent, int rows_, int cols_)
    : QDialog(parent)
    , ui(new Ui::SecondAlgo)
    , engine(rows_,cols_)
    , saveAsked(false)
{

    ui->setupUi(this);

    initializeGUI();
    const auto start = std::chrono::steady_clock::now();

    engine.createCards();
    engine.shuffleCards();


    playGame();
    autoSolve();
    const auto end = std::chrono::steady_clock::now();
    genDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    endCondition();
}

void SecondAlgo::initializeGUI(){
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
}

void SecondAlgo::updateDisplay() { //affichage des cartes
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

/*void SecondAlgo::endCondition(){
    if(engine.getPairsFound() == engine.getNbPairs()){ //Fin de jeu (on a trouvé toute les pairs)

        QMessageBox::information(this,
                                 "Vous avez gagné !",
                                 "Bravo, vous avez trouvé toutes les paires en " + QString::number(engine.getAttempts()) + " coups !");
    }
}*/

void SecondAlgo::endCondition(){
    qDebug() << ">>> endCondition appelée, pairFound:" << engine.getPairsFound() << "nbPairs:" << engine.getNbPairs();

    if(engine.getPairsFound() == engine.getNbPairs()){
        qDebug() << ">>> Condition vérifiée, partie terminée";

        saveAsked=true;
        qDebug() << ">>> saveAsked passé à true";

        QMessageBox msgBox;
        msgBox.setInformativeText("Toutes les paires en " +
                                  QString::number(engine.getAttempts()) + " coups !");
        msgBox.setText("Génération d'une partie aléatoire en " + QString::number(genDuration) +" ms");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);


        //QPushButton *saveButton = msgBox.addButton("Sauvegarder", QMessageBox::AcceptRole);
        //QPushButton *cancelButton = msgBox.addButton("Quitter", QMessageBox::RejectRole);
        QPushButton *retryButton = msgBox.addButton("Rejouer", QMessageBox::ActionRole);

        //msgBox.setDefaultButton(saveButton);

        qDebug() << ">>> Affichage du QMessageBox";
        msgBox.exec();
        qDebug() << ">>> QMessageBox fermé";

        QAbstractButton *saveButton   = msgBox.button(QMessageBox::Save);
        QAbstractButton *cancelButton = msgBox.button(QMessageBox::Cancel);

        saveButton->setText("Sauvegarder");
        cancelButton->setText("Quitter");

        QAbstractButton *clicked = msgBox.clickedButton();
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



 //Il manque un truc pour la rapidité, càd si on tourne un truc dont on a déjà la deuxième paire, on prend rdmValueB en conséquence

void SecondAlgo::autoSolve(){

    std::unordered_map<int, std::vector<int>> memory; //int -> valeur, & vecteur<int> -> liste des indices où est la valeur.
    std::vector<int> hiddenCards;
    std::vector<bool> alreadySeen(cards.size(), false); //chaque index est initialisé à false
    hiddenCards.reserve(cards.size());
    std::vector<bool> revealed(cards.size(), false); //permet de remplacer les cards[i]->text() == "?"){ pcq ça buggait mais jsp pq
    revealed.reserve(cards.size());

    while(engine.getNbPairs() > engine.getPairsFound()){

        bool playedKnownPair = false;

        hiddenCards.clear();


        for(int i=0 ; i<engine.getNbCards(); i++){
            if(!revealed[i]){
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
                if(!revealed[indA] && !revealed[indB]){
                    engine.playMove(indA,indB);
                    revealed[indA] = true;
                    revealed[indB] = true;

                    playedKnownPair = true;
                    break;
                }
            }
        }

        if(playedKnownPair){ //On retourne dans la boucle for pour voir si on a pas d'autre pairs à tester
            continue;
        }


        //Partie 2 : si on ne connaît aucune paire
        if (hiddenCards.size()<2){ // plus de cartes à trouver, un tour de boucle en moins
            break;
        }

        int rdmValueA = QRandomGenerator::global()->bounded(hiddenCards.size());
        int indexA = hiddenCards[rdmValueA];

        //Au cas où toute les cartes ont déjà eté regardées.
        int count = 0;
        for(int i=0 ; i<engine.getNbCards() ; i++){
            if(alreadySeen[i] == true){
                count++;
            }
            else{ //pour éviter trop de tour de boucle inutile
                break;
            }
        }

        while(alreadySeen[indexA] == true){ //Pour ne pas tirer une carte dont on connaît déjà le contenu
            if(count == engine.getNbCards()){
                break;
            }
            rdmValueA = QRandomGenerator::global()->bounded(hiddenCards.size());
            indexA = hiddenCards[rdmValueA];
        }

        //Si on tourne une carte dont on connaît la paire ça les met ensemble directement
        int indexB;
        if(memory[engine.getCardValue(indexA)].size() == 1 && indexA != memory[engine.getCardValue(indexA)][0]){
            indexB = memory[engine.getCardValue(indexA)][0];
        }

        else{
            int rdmValueB = QRandomGenerator::global()->bounded(hiddenCards.size());
            indexB = hiddenCards[rdmValueB];

            while(indexA==indexB || alreadySeen[indexB]==true){ //Ne pas tirer deux fois la même carte
                rdmValueB = QRandomGenerator::global()->bounded(hiddenCards.size());
                indexB = hiddenCards[rdmValueB];
            }
        }



        //Donc là on a deux valeurs rdmValueA et rdmValueB qui sont différentes et dont les pairs n'ont pas encore été trouvées.

        bool isPair = engine.playMove(indexA,indexB);

        alreadySeen[indexA] = true;
        alreadySeen[indexB] = true;

        if (isPair){
            revealed[indexA] = true;
            revealed[indexB] = true;
        }


        //Partie 3 : Remplissage de memory
        //Condition pour éviter d'ajouter deux fois le même indice pour la même valeur dans memory
        if(memory[engine.getCardValue(indexA)].size() != 0){
            if(memory[engine.getCardValue(indexA)].size() == 1){
                if(memory[engine.getCardValue(indexA)][0] != indexA){
                    memory[engine.getCardValue(indexA)].push_back(indexA);//Ajout de l'indice de la carte dans memory
                }
            }
            if(memory[engine.getCardValue(indexA)].size() == 2){
                if(memory[engine.getCardValue(indexA)][0] != indexA && memory[engine.getCardValue(indexA)][1] != indexA){
                    memory[engine.getCardValue(indexA)].push_back(indexA);//Ajout de l'indice de la carte dans memory
                }
            }
        }
        //Si y'a encore rien dans la liste d'indices
        else{
            memory[engine.getCardValue(indexA)].push_back(indexA);
        }


        if(memory[engine.getCardValue(indexB)].size() != 0){
            if(memory[engine.getCardValue(indexB)].size() == 1){
                if(memory[engine.getCardValue(indexB)][0] != indexB){
                    memory[engine.getCardValue(indexB)].push_back(indexB);//Ajout de l'indice de la carte dans memory
                }
            }
            if(memory[engine.getCardValue(indexB)].size() == 2){
                if(memory[engine.getCardValue(indexB)][0] != indexB && memory[engine.getCardValue(indexB)][1] != indexB){
                    memory[engine.getCardValue(indexB)].push_back(indexB);//Ajout de l'indice de la carte dans memory
                }
            }
        }
        else{
            memory[engine.getCardValue(indexB)].push_back(indexB);
        }

    }
    updateDisplay();
}




void SecondAlgo::playGame(){

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


//Demande au joueur s'il souhaite sauvegarder lorsqu'il tente de quitter l'application en pleine partie
/*void SecondAlgo::closeEvent(QCloseEvent *event){
    if(engine.isSaved == true || saveAsked==true){
        event -> accept();
        return;
    }

    QMessageBox msgBox;
    msgBox.setText("Voulez-vous sauvegarder la partie ?");

    QPushButton *saveButton = msgBox.addButton("Oui", QMessageBox::ActionRole);
    QPushButton *leaveButton = msgBox.addButton("Non", QMessageBox::ActionRole);

    msgBox.exec();

    if(msgBox.clickedButton()==saveButton){
        QString filename = QFileDialog::getSaveFileName(this,"Sauvegarder la partie","","All files (*)",nullptr,QFileDialog::DontUseNativeDialog);

        if(!filename.isEmpty()) { //vérification d'un nom valide
            engine.saveGame(filename);
            event -> accept(); //fermer la fenêtre
        }
        else{event->ignore();}
    }

    else if(msgBox.clickedButton()==leaveButton){
        event->accept(); //fermer la feêntre

    }
}*/

void SecondAlgo::closeEvent(QCloseEvent *event){
}


SecondAlgo::~SecondAlgo()
{
    qDebug() <<"Destruction SecondtAlgo";
    delete ui;
}
