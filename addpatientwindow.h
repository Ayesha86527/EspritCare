#pragma once
#include <QMainWindow>
#include "backend.h"

class QLineEdit;
class QComboBox;
class QSpinBox;
class QDateEdit;
class QTextEdit;
class QPushButton;

class AddPatientWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit AddPatientWindow(Backend* backend, QWidget *parent = nullptr);

private slots:
    void onAddPatientClicked();
    void onCancelClicked();

private:
    void setupUi();

    // Input fields
    QLineEdit *nameEdit;
    QComboBox *genderCombo;
    QSpinBox *ageSpin;
    QDateEdit *visitDateEdit;
    QTextEdit *notesEdit;
    Backend* backend;
};



