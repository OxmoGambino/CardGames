#pragma once
#include <QMainWindow>
#include <QString>

class ThirdWindow;

QT_BEGIN_NAMESPACE
namespace Ui { class SecondWindow; }
QT_END_NAMESPACE

class SecondWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit SecondWindow(QWidget *parent = nullptr);
    ~SecondWindow();

private slots:
    void on_PlayButton_clicked();
    void on_actionQuitter_triggered();
    void on_HSlide_valueChanged(int value);
    void on_LSlide_valueChanged(int value);

private:
    Ui::SecondWindow *ui;
    ThirdWindow *thirdwindow = nullptr;
    int hValue = 0;
    int lValue = 0;
};
