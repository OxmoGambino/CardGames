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


ThirdWindow::ThirdWindow(QWidget *parent, int rows_, int cols_,int mode,const QString& filename)
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

    if(mode == 0){ //mode de jeu classique avec création du jeu etc
        createCards();
        shuffleCards();
        playGame();
    }

    if(mode == 1){ //mode de jeu chargé avec cartes déjà créées
        loadGame(filename);      // ← Charge les données

        // CALCULER nbPairs ICI (IMPORTANT !)
        nbPairs = (rows * cols) / 2;
        qDebug() << "nbPairs calculé:" << nbPairs;

        playGame();              // ← Créer les boutons D'ABORD
        replayLoadedGame();      // ← Rejouer APRÈS
        displayFoundCards();
    }

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

        b = cardsValues[i]; //mélange des valeurs de cartes (pour les tests)
        cardsValues[i] = cardsValues[j];
        cardsValues[j] = b;

        c = labels[i]; //mélange du visuel // à utiliser pour la sauvegarde
        labels[i] = labels[j];
        labels[j] = c;
    }
}



//Enregistrer et tourner les cartes cliquées
void ThirdWindow::cardsRegister(int index){
    QString current = cards[index]->text();

    //Enregistrer la valeur de la 1ère carte
    if (firstValue == 0 && secondValue == 0 && current=="?"){
        cards[index]->setText(labels[index]);
        firstValue = cardsValues[index];
        firstValueIndex = index;
    }

    //Enregistrer la valeur de la seconde carte
    else if (firstValue!=0 && secondValue == 0 && current=="?" && index != firstValueIndex){
        cards[index]->setText(labels[index]);
        secondValue = cardsValues[index];
        secondValueIndex = index;
        locked = true;


        //mécanique une fois que les deux cartes sont retournées (d'où le dans else if)

        bool isPair = playMove(firstValueIndex, secondValueIndex);

        if(!isPair){ //Cas d'échec
            QTimer::singleShot(1250, this, [this]() { //Le QTimer permet de voir l'affichage de la deuxième carte après t ms
                cards[firstValueIndex]->setText("?");
                cards[secondValueIndex]->setText("?");
                firstValue = 0;
                secondValue = 0;
                locked = false;
            });
        }

        else{ //cas de victoire
            firstValue = 0;
            secondValue=0;
            locked=false;
            //plus besoin de pairFound++ et actualisation de l'affichage (déjà dans playMove)
        }

        endCondition(); // vérification de fin de partie
    }
}


bool ThirdWindow::hasBeenPlayed(int idx1, int idx2){
    int mini = std::min(idx1, idx2);
    int maxi = std::max(idx1, idx2);

    QString conc_str = QString::number(mini) + QString::number(maxi); //concaténation en une string
    int conc = conc_str.toInt(); // conversion de la concaténation en une entier unique normalisé


    bool alreadyPlayed = (playedMoves.insert(conc)).second; //teste l'insert.
    //True : insersion faite, coup non déjà joué
    //False : insersion non faite, coup déjà joué

    return !alreadyPlayed; //renvoie l'inverse du booléen pour la lisibilité

}


void ThirdWindow::endCondition(){
    qDebug() << ">>> endCondition appelée, pairFound:" << pairFound << "nbPairs:" << nbPairs;

    if(pairFound == nbPairs){
        qDebug() << ">>> Condition vérifiée, partie terminée";

        QMessageBox msgBox;
        msgBox.setInformativeText("Vous avez gagné !");
        msgBox.setText("Bravo, vous avez trouvé toutes les paires en " +
                       QString::number(nbAttempt) + " coups !");
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
                saveGame(filename);
                qDebug() << ">>> Retour de saveGame()";
            } else {
                qDebug() << ">>> Filename vide, sauvegarde annulée";
            }
        }
        else if (msgBox.clickedButton() == cancelButton){
            qDebug() << ">>> Bouton QUITTER cliqué";
            hide();
        }
        else if (msgBox.clickedButton() == retryButton){
            qDebug() << ">>> Bouton REJOUER cliqué";
            hide();
            secondwindow = new SecondWindow(this);
            secondwindow->show();
        }

        qDebug() << ">>> Fin de endCondition";
    } else {
        qDebug() << ">>> Condition NON vérifiée";
    }
}







void ThirdWindow::playGame(){

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

                cardsRegister(index); // cardsRegister fait aussi le boulot de feu cardsComparaison
            });
        }
    }

}




//Demande au joueur s'il souhaite sauvegarder lorsqu'il tente de quitter l'application en pleine partie
void ThirdWindow::closeEvent(QCloseEvent *event){
    if(pairFound==nbPairs){ // lorsque j'ai fini la partie et que je clique sur la croix, le jeu se ferme et ne demande pas une autre sauvegarde
        event->accept();
        return;
    }
    QMessageBox msgBox;
    msgBox.setText("Voulez-vous sauvegarder la partie ?");

    QPushButton *saveButton = msgBox.addButton("Oui", QMessageBox::ActionRole);
    QPushButton *leaveButton = msgBox.addButton("Non", QMessageBox::ActionRole);

    msgBox.exec();

    if(msgBox.clickedButton()==saveButton){
        QString filename = QFileDialog::getSaveFileName(this,"Sauvegarder la partie","","Text files (*.txt) ;; All files (*)",nullptr,QFileDialog::DontUseNativeDialog);
        // Text files : suggestion par défault. All Files : deuxième filtre si l'utilisateur veut mettre une extension particulière.

        if(!filename.isEmpty()) { //vérification d'un nom valide
            saveGame(filename);
            event -> accept(); //fermer la fenêtre
        }
        else{event->ignore();}
    }

    else if(msgBox.clickedButton()==leaveButton){
        event -> accept(); //accepter la fermeture de la fenêtre
    }
}


void ThirdWindow::saveGame(const QString& filename){
    qDebug() << "=== Début saveGame ===";
    qDebug() << "Filename:" << filename;

    QFile file(filename);

    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug() << "ERREUR: Impossible d'ouvrir";
        QMessageBox::critical(this,"Erreur","Impossible de créer la Sauvegarde");
        return;
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

    QMessageBox::information(this, "Sauvegarde","Partie Sauvegardée avec succès \n");
}

void ThirdWindow::loadGame(const QString& filename){
    QString lab; // stockage temp des labels après
    int indexs; //stockage temp des index des cartes
    QFile file(filename);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::critical(this,"Erreur","Impossible d'ouvrir la sauvegarde");
        return;
    }

    //fichier ouvert, début du chargement
    const auto start  = std::chrono::steady_clock::now(); //init du compteur de temps, steady clock de stl très utilisé pour le calcul d'intervalle

    QTextStream in(&file);
    qDebug() << filename << "le fichier a bien été lu et reconnu";
    in.readLine(); //skip de la première ligne car déjà init;

    for(int i = 0; i<rows*cols; i++){
        in >> lab;
        labels.push_back(lab);
        qDebug() << labels[i];
    }

    for(int i = 0; i<rows*cols;i++){
        in >> indexs;
        cardsValues.push_back(indexs);
        qDebug() << cardsValues[i];
    }
    //int i = 0;
    while(!in.atEnd()){ //jusqu'au bout du fichier
        int idx1,idx2; //utilisé pour débug
        in >> idx1 >> idx2; // lecture des indexs

        hist.push_back(std::make_pair(idx1,idx2));
        //qDebug() << hist[i].first << hist[i].second << i;
        //i++;
    }

    file.close(); //plus besoin du fichier pour replay

    replayLoadedGame();


    //partie chargée, fin du compteur de temps
    const auto end = std::chrono::steady_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    QString msg = QString("Votre sauvegarde a été chargée en %1 ms").arg(duration); //.arg -> met la valeur de temps (type inconnu) en %1

    QMessageBox::information(this,"Chargement effectué",msg);
}

void ThirdWindow::replayLoadedGame(){
    nbAttempt = 0;
    pairFound = 0;
    playedMoves.clear(); //on sait jamais, pas qu'il y ait de conflits si on ferme pas le jeu entre temps

    QVector<std::pair<int,int>> loadedHist = hist;
    hist.clear(); // idem, pour les prochaines parties c'est moieux si hist est vide. (à vérifier s'il ne se vide pas déjà automatiquement)

    for(int i = 0; i < loadedHist.size(); i++){ // boucle qui rejoue les coups
        int idx1 = loadedHist[i].first; int idx2 = loadedHist[i].second;
        qDebug() << "rejeu du coup " << i << " : " << idx1 << "&" << idx2;
        playMove(idx1, idx2);// jeu avec les deux index enregistrés
    }

    qDebug() << "État final: Tentatives=" << nbAttempt << ", Paires=" << pairFound << "/" << nbPairs;
    qDebug() << "=== Fin replayLoadedGame ===";
}

void ThirdWindow::displayFoundCards(){
    qDebug() << "=== Affichage des cartes trouvées ===";

    // Set pour stocker les cartes trouvées (évite les doublons)
    QSet<int> foundCards;

    // Parcourir l'historique pour trouver les paires réussies
    for(const auto& move : hist){
        int idx1 = move.first;
        int idx2 = move.second;

        // Si c'est une paire (mêmes valeurs)
        if(cardsValues[idx1] == cardsValues[idx2]){
            foundCards.insert(idx1);
            foundCards.insert(idx2);
            qDebug() << "Paire:" << idx1 << "et" << idx2 << "→" << labels[idx1];
        }
    }

    // Afficher toutes les cartes trouvées
    for(int index : foundCards){
        cards[index]->setText(labels[index]);      // Afficher le label
        cards[index]->setEnabled(false);           // Désactiver le bouton
        qDebug() << "Carte" << index << "affichée:" << labels[index];
    }

    qDebug() << "Total de cartes affichées:" << foundCards.size();
    qDebug() << "=== Fin displayFoundCards ===";
}

bool ThirdWindow::playMove(int idx1, int idx2){
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

    //mise à jour de l'affichage
    attemptLabel->setText("Tentatives : " + QString::number(nbAttempt));
    pairLabel->setText("Paires trouvées : " + QString::number(pairFound));

    return isPair;
}


ThirdWindow::~ThirdWindow(){
    delete ui;
}
