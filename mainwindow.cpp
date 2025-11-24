#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionQuitter, &QAction::triggered,
            this, &MainWindow::on_actionQuitter_triggered);

    ui->exitButton->setText("Exit");
    ui->startButton->setText("Play");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuitter_triggered(){
    close();
}


void MainWindow::on_startButton_clicked(){
    close();
    secondwindow = new SecondWindow(this);
    secondwindow->show();
}


void MainWindow::on_exitButton_clicked(){
    close();
}
