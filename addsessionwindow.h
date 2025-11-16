#pragma once
#include <QMainWindow>
#include "DSABackend.h"     // NEW

class QLineEdit;
class QTextEdit;
class QPushButton;
class QLabel;
class QListWidget;         // NEW - to show search results
class EspritDB;

class AddSessionWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit AddSessionWindow(QWidget *parent = nullptr);
    ~AddSessionWindow();

private slots:
    void onSearchPatientClicked();
    void onUploadRecordingClicked();
    void onSaveSessionClicked();
    void onCancelClicked();
    void onPatientSelected();      // NEW - when user selects from search results

private:
    void setupUi();

    // Widgets
    QLineEdit *searchEdit;
    QPushButton *searchBtn;
    QListWidget *searchResultsList;  // NEW - display search results
    QLabel *patientResultLabel;
    QLineEdit *sessionNumberEdit;
    QLabel *recordingFileLabel;
    QTextEdit *notesEdit;

    QString selectedFilePath;

    // Backend integration
    EspritDB *db;                    // NEW
    DSABackend *backend;             // NEW
    int selectedPatientId;           // NEW - store selected patient ID
};
