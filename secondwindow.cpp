#include "secondwindow.h"
#include "firstalgo.h"
#include "ui_secondwindow.h"
#include "thirdwindow.h"
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



SecondWindow::~SecondWindow() { delete ui; }



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
}


void SecondWindow::on_actionQuitter_triggered(){
    close(); }

void SecondWindow::on_HSlide_valueChanged(int value){
    hValue = value;
    ui->xLabel->setText(QString::number(value));
}


void SecondWindow::on_LSlide_valueChanged(int value){
    lValue = value;
    ui->yLabel->setText(QString::number(value));
}
