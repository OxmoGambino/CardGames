#include "secondwindow.h"
#include "ui_secondwindow.h"
#include "mainwindow.h"
#include "thirdwindow.h"
#include "firstalgo.h"
#include "secondalgo.h"
#include <QMessageBox>

#include <QSlider>

SecondWindow::SecondWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SecondWindow)
{
    ui->setupUi(this);

    ui->HSlider->setRange(3, 9);
    ui->HSlider->setSingleStep(1);
    ui->LSlider->setRange(3, 9);
    ui->LSlider->setSingleStep(1);

    ui->HSlider->setValue(4); //Initialiser la valeur à 4x4 quand on lance le programme
    ui->LSlider->setValue(4);

    ui->yLabel->setText(QString::number(ui->HSlider->value()));
    ui->xLabel->setText(QString::number(ui->LSlider->value()));

    hValue = ui->HSlider->value(); //Sans ça, les valeurs du Slider sont considérés nuls tant qu'on ne les bouge pas
    lValue = ui->LSlider->value();

    connect(ui->HSlider, &QSlider::valueChanged, this, [this](int v){
        ui->yLabel->setText(QString::number(v));
        hValue = v; //on mémorise
    });

    connect(ui->LSlider, &QSlider::valueChanged, this, [this](int v){
        ui->xLabel->setText(QString::number(v));
        lValue = v; //on mémorise
    });
}



SecondWindow::~SecondWindow() {
    qDebug() <<"Destruction secondWindow";
    delete ui; }



void SecondWindow::on_PlayButton_clicked(){
    bool estPair = ((hValue * lValue) % 2 == 0);

    if (estPair){
        close();
        thirdwindow = new ThirdWindow(this, hValue, lValue);
        thirdwindow->show();
    }

    else{
        QMessageBox::warning(this,"Erreur\n", "Le produit H*L doit être pair afin de continuer");
    }
}

void SecondWindow::on_AlgoButton_clicked(){
    bool estPair = ((hValue * lValue) % 2 == 0);

    if (estPair){
        close();
        firstalgo = new FirstAlgo(this, hValue, lValue);
        firstalgo->show();
    }

    else{
        QMessageBox::warning(this,"Erreur\n", "Le produit H*L doit être pair afin de continuer");
    }
}


void SecondWindow::on_SecAlgoButton_clicked(){
    bool estPair = ((hValue * lValue) % 2 == 0);

    if (estPair){
        close();
        secondalgo = new SecondAlgo(this, hValue, lValue);
        secondalgo->show();
    }

    else{
        QMessageBox::warning(this,"Erreur\n", "Le produit H*L doit être pair afin de continuer");
    }
}


void SecondWindow::on_actionQuitter_triggered(){ close(); }

void SecondWindow::on_HSlide_valueChanged(int value){
    hValue = value;
    ui->xLabel->setText(QString::number(value));
}


void SecondWindow::on_LSlide_valueChanged(int value){
    lValue = value;
    ui->yLabel->setText(QString::number(value));
}


void SecondWindow::on_rulesButton_clicked(){
    QMessageBox::information(this,"Règles du jeu","But du jeu : \n"
    "Retrouver toutes les paires de cartes identiques.\n\n"
    "Règles :\n"
    "- Le jeu se joue sur une grille de cartes face cachée.\n"
    "- À chaque coup, deux cartes doivent être retournées.\n"
    "- Si les deux cartes ont la même valeur, la paire est trouvée et reste visible.\n"
    "- Sinon, elles sont à nouveau cachées.\n"
    "- Une paire est par exemple : 6A et 6B ou 1A et 1B\n\n"
    "Algorithmes automatiques :\n\n"
    "- Algorithme avec vision : joue d’abord 100 coups aléatoires, "
    "puis utilise la triche pour retourner une paire, "
    "ces deux étapes s'alternent jusqu'à ce que toute les paires soient découvertes.\n\n"
    "- Algorithme à l'aveugle : Termine le jeu en faisant le moins de coups possible sans voir les cartes, "
    "mémorise les cartes déjà vues pour jouer de plus en plus intelligemment.\n\n"
    "La partie se termine lorsque toutes les paires ont été trouvées.");
}


void SecondWindow::on_backButton_clicked(){
    close();
    mainwindow = new MainWindow(this);
    mainwindow->show();
}
