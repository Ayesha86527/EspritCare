#pragma once
#include <QMainWindow>

class QLabel;
class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onGetStartedClicked();

private:
    void setupUi();

    QWidget *central;
    QLabel *titleLabel;
    QLabel *taglineLabel;
    QPushButton *getStartedBtn;
};



