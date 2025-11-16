#ifndef VIEWPATIENTWINDOW_H
#define VIEWPATIENTWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QTableWidget>

class ViewPatientWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ViewPatientWindow(QWidget *parent = nullptr);

private slots:
    void onSearchClicked();
    void onBackClicked();

private:
    void setupUi();

    QLineEdit *searchEdit;
    QPushButton *searchBtn;
    QComboBox *sortCombo;
    QTableWidget *patientTable;
    QPushButton *backBtn;
};

#endif // VIEWPATIENTWINDOW_H
