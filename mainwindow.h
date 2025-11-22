#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "secondwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent=nullptr);
    ~MainWindow();


private slots:
    void on_startButton_clicked();
    void on_exitButton_clicked();
    void on_actionQuitter_triggered();


private:
    Ui::MainWindow *ui;
    SecondWindow *secondwindow;

};
#endif // MAINWINDOW_H

