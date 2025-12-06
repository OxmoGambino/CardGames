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

    ui->startButton->setText("Play");
    ui->exitButton->setText("Exit");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuitter_triggered(){
    close();
}


void MainWindow::on_startButton_clicked(){
    hide();
    secondwindow = new SecondWindow(this);
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

    filename = QFileDialog::getOpenFileName(this,"Charger la Sauvegarde","","Text files (*.txt) ;; All files (*)",nullptr,QFileDialog::DontUseNativeDialog);
    qDebug() << getFilename();// récupère le chemin du fichier à charger
    // nécessaire d'avoir filename en attribut pour le récupérer dans thirdwindow avec une méthode de mainwindpw
    hide();

    QFile file(filename);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::critical(this,"Erreur","Impossible d'ouvrir la sauvegarde");
        return;
    }

    QTextStream in(&file);
    in >> rows >> cols;
    qDebug() << rows << cols;
    file.close(); //pas de fuite mémoire;

    thirdwindow = new ThirdWindow(this,rows,cols,1,filename);
    thirdwindow -> show();


}
