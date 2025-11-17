#pragma once
#include <QMainWindow>
#include "backend.h"

class QLineEdit;
class QTextEdit;
class QPushButton;
class QLabel;

class AddSessionWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit AddSessionWindow(Backend* backendPtr, QWidget* parent = nullptr);

private slots:
    void onSearchPatientClicked();
    void onUploadRecordingClicked();
    void onSaveSessionClicked();
    void onCancelClicked();

private:
    void setupUi();

    // Widgets
    QLineEdit *searchEdit;
    QPushButton *searchBtn;
    QLabel *patientResultLabel;
    QLineEdit *sessionNumberEdit;
    QLabel *recordingFileLabel;
    QTextEdit *notesEdit;

    QString selectedFilePath;
    Backend* backend;
    int currentPatientID;
};



