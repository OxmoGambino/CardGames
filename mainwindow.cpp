#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "QDebug"
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QChar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionQuitter, &QAction::triggered,
            this, &MainWindow::on_actionQuitter_triggered);

    ui->startButton->setText("Jouer");
    ui->exitButton->setText("Quitter");
    ui->TitleLabel->setFocusPolicy(Qt::NoFocus); //Pour enlever la bordure bleue sous le label
}

MainWindow::~MainWindow()
{
    qDebug() <<"Destruction MainWindow";
    delete ui;
}

void MainWindow::on_actionQuitter_triggered(){
    close();
}


void MainWindow::on_startButton_clicked(){
    close();
    secondwindow = new SecondWindow(nullptr);
    secondwindow->show();
}


void MainWindow::on_exitButton_clicked(){
    close();
}

QString MainWindow::getFilename(){
    return filename;
}

void MainWindow::on_actionLoadGame_triggered(){
    int rows, cols;

    filename = QFileDialog::getOpenFileName(nullptr,"Charger la Sauvegarde","","All files (*)",nullptr,QFileDialog::DontUseNativeDialog);
    qDebug() << getFilename();// récupère le chemin du fichier à charger
    // nécessaire d'avoir filename en attribut pour le récupérer dans thirdwindow avec une méthode de mainwindpw
    close();

    QFile file(filename);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::critical(nullptr,"Erreur","Impossible d'ouvrir la sauvegarde");
        close();
        return;
    }

    QTextStream in(&file);
    in >> rows >> cols;
    qDebug() << rows << cols;
    file.close(); //pas de fuite mémoire;

    thirdwindow = new ThirdWindow(nullptr,rows,cols,1,filename);
    thirdwindow -> show();


}

void MainWindow::on_actionGameRules_triggered(){
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



