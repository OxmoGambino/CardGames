#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "secondwindow.h"
#include "thirdwindow.h"
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString getFilename();


private slots:
    void on_startButton_clicked();
    void on_exitButton_clicked();
    void on_actionQuitter_triggered();
    void on_actionLoadGame_triggered();
    void on_actionGameRules_triggered();


private:
    QString filename;
    Ui::MainWindow *ui;
    SecondWindow *secondwindow;
    ThirdWindow *thirdwindow;

};
#endif // MAINWINDOW_H

