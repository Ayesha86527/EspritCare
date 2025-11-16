#include "addsessionwindow.h"
#include "dashboardwindow.h"
#include "espritdb.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QListWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>

AddSessionWindow::AddSessionWindow(QWidget *parent)
    : QMainWindow(parent)
    , selectedPatientId(-1)
{
    // Initialize database
    db = new EspritDB(this);
    if (!db->openDatabase("espritcare.db")) {
        qDebug() << "Failed to open database!";
    }
    db->createTables();

    // Initialize backend
    backend = new DSABackend(db);

    setupUi();
}

AddSessionWindow::~AddSessionWindow()
{
    delete backend;
}

void AddSessionWindow::setupUi()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    central->setStyleSheet(R"(
        QWidget {
            background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1,
                        stop:0 #f3f7fc, stop:1 #d6e6f5);
        }
    )");

    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setAlignment(Qt::AlignCenter);

    QFrame *formCard = new QFrame;
    formCard->setFixedWidth(600);
    formCard->setStyleSheet(R"(
        QFrame {
            background-color: #ffffff;
            border: 1px solid #cfd9e6;
            border-radius: 12px;
        }
    )");

    QVBoxLayout *cardLayout = new QVBoxLayout(formCard);
    cardLayout->setContentsMargins(40, 45, 40, 45);
    cardLayout->setSpacing(18);

    QLabel *titleLabel = new QLabel("Add New Session");
    titleLabel->setStyleSheet(R"(
        font-size: 24px;
        font-weight: bold;
        color: #1f2f45;
        background: transparent;
        border: none;
        margin-bottom: 5px;
    )");
    titleLabel->setAlignment(Qt::AlignLeft);
    cardLayout->addWidget(titleLabel);

    QString inputStyle = R"(
        QLineEdit, QTextEdit {
            padding: 10px 12px;
            border: 1px solid #cfd9e6;
            border-radius: 6px;
            font-size: 11pt;
            color: #1f2f45;
            background-color: #ffffff;
        }
        QLineEdit:focus, QTextEdit:focus {
            border: 1px solid #2b7de9;
            background-color: #f8fafc;
        }
    )";

    QString labelStyle = R"(
        font-size: 11pt;
        color: #1f2f45;
        font-weight: 500;
        background: transparent;
        border: none;
        margin-bottom: 6px;
    )";

    // --- Search Section ---
    QLabel *searchLabel = new QLabel("Search Patient");
    searchLabel->setStyleSheet(labelStyle);
    cardLayout->addWidget(searchLabel);

    QHBoxLayout *searchLayout = new QHBoxLayout;
    searchEdit = new QLineEdit;
    searchEdit->setPlaceholderText("Enter patient name");
    searchEdit->setStyleSheet(inputStyle);

    searchBtn = new QPushButton("Search");
    searchBtn->setFixedWidth(100);
    searchBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #2b7de9;
            color: white;
            border: none;
            border-radius: 6px;
            font-size: 10.5pt;
            font-weight: 500;
            padding: 8px;
        }
        QPushButton:hover {
            background-color: #256dd1;
        }
    )");

    searchLayout->addWidget(searchEdit);
    searchLayout->addWidget(searchBtn);
    cardLayout->addLayout(searchLayout);

    // --- Search Results List (NEW) ---
    searchResultsList = new QListWidget;
    searchResultsList->setMaximumHeight(120);
    searchResultsList->setStyleSheet(R"(
        QListWidget {
            background: #f8fafc;
            border: 1px solid #cfd9e6;
            border-radius: 6px;
            font-size: 10pt;
            color: #1f2f45;
        }
        QListWidget::item {
            padding: 6px;
        }
        QListWidget::item:selected {
            background: #2b7de9;
            color: white;
        }
    )");
    searchResultsList->hide(); // Hidden until search
    cardLayout->addWidget(searchResultsList);

    patientResultLabel = new QLabel("No patient selected.");
    patientResultLabel->setStyleSheet(R"(
        QLabel {
            color: #6b7c8c;
            font-size: 10pt;
            margin-top: 5px;
            background: transparent;
            border: none;
        }
    )");
    cardLayout->addWidget(patientResultLabel);

    // --- Session Info ---
    QLabel *sessionLabel = new QLabel("Session Number");
    sessionLabel->setStyleSheet(labelStyle);
    sessionNumberEdit = new QLineEdit;
    sessionNumberEdit->setReadOnly(true);
    sessionNumberEdit->setStyleSheet(inputStyle);
    sessionNumberEdit->setText("—");
    cardLayout->addWidget(sessionLabel);
    cardLayout->addWidget(sessionNumberEdit);

    // --- Upload Recording ---
    QLabel *uploadLabel = new QLabel("Upload Session Recording (Optional)");
    uploadLabel->setStyleSheet(labelStyle);
    QPushButton *uploadBtn = new QPushButton("Choose File");
    uploadBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #2b7de9;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 8px 16px;
            font-size: 10.5pt;
            font-weight: 500;
        }
        QPushButton:hover {
            background-color: #256dd1;
        }
    )");

    recordingFileLabel = new QLabel("No file selected.");
    recordingFileLabel->setStyleSheet(R"(
        QLabel {
            color: #6b7c8c;
            font-size: 10pt;
            margin-top: 4px;
            background: transparent;
            border: none;
        }
    )");

    cardLayout->addWidget(uploadLabel);
    cardLayout->addWidget(uploadBtn);
    cardLayout->addWidget(recordingFileLabel);

    // --- Notes Section ---
    QLabel *notesLabel = new QLabel("Session Notes");
    notesLabel->setStyleSheet(labelStyle);
    notesEdit = new QTextEdit;
    notesEdit->setPlaceholderText("Add session notes, diagnosis, treatment plan...");
    notesEdit->setMaximumHeight(100);
    notesEdit->setStyleSheet(inputStyle);
    cardLayout->addWidget(notesLabel);
    cardLayout->addWidget(notesEdit);

    // --- Buttons ---
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->setSpacing(12);

    QPushButton *cancelBtn = new QPushButton("Cancel");
    cancelBtn->setFixedHeight(42);
    cancelBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #e8ecef;
            color: #1f2f45;
            border: none;
            border-radius: 6px;
            padding: 0 24px;
            font-size: 11pt;
            font-weight: 500;
        }
        QPushButton:hover {
            background-color: #d6dce2;
        }
    )");

    QPushButton *saveBtn = new QPushButton("Save Session");
    saveBtn->setFixedHeight(42);
    saveBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #2b7de9;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 0 24px;
            font-size: 11pt;
            font-weight: 500;
        }
        QPushButton:hover {
            background-color: #256dd1;
        }
    )");

    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelBtn);
    buttonLayout->addWidget(saveBtn);
    cardLayout->addLayout(buttonLayout);

    mainLayout->addWidget(formCard);

    // --- Connections ---
    connect(searchBtn, &QPushButton::clicked, this, &AddSessionWindow::onSearchPatientClicked);
    connect(searchResultsList, &QListWidget::itemClicked, this, &AddSessionWindow::onPatientSelected);
    connect(uploadBtn, &QPushButton::clicked, this, &AddSessionWindow::onUploadRecordingClicked);
    connect(saveBtn, &QPushButton::clicked, this, &AddSessionWindow::onSaveSessionClicked);
    connect(cancelBtn, &QPushButton::clicked, this, &AddSessionWindow::onCancelClicked);

    setWindowTitle("EspritCare - Add Session");
    resize(1000, 600);
}

// ========== SEARCH PATIENT (DSA BACKEND) ==========
void AddSessionWindow::onSearchPatientClicked()
{
    QString searchText = searchEdit->text().trimmed();

    if (searchText.isEmpty()) {
        QMessageBox::warning(this, "Search Error", "Please enter a patient name.");
        return;
    }

    // Use DSA backend to search
    QVector<PatientData> results = backend->searchPatientByName(searchText);

    searchResultsList->clear();

    if (results.isEmpty()) {
        searchResultsList->addItem("No patients found");
        searchResultsList->show();
        patientResultLabel->setText("No patient selected.");
        selectedPatientId = -1;
    } else {
        searchResultsList->show();
        for (const auto& patient : results) {
            QString displayText = QString("%1 (ID: %2) - %3")
            .arg(patient.name)
                .arg(patient.id)
                .arg(patient.gender);

            QListWidgetItem* item = new QListWidgetItem(displayText);
            item->setData(Qt::UserRole, patient.id); // Store patient ID in item
            searchResultsList->addItem(item);
        }
    }
}

// ========== PATIENT SELECTED FROM LIST ==========
void AddSessionWindow::onPatientSelected()
{
    QListWidgetItem* selectedItem = searchResultsList->currentItem();
    if (!selectedItem) return;

    selectedPatientId = selectedItem->data(Qt::UserRole).toInt();

    if (selectedPatientId > 0) {
        PatientData* patient = backend->searchPatientByID(selectedPatientId);

        if (patient) {
            patientResultLabel->setText(QString("✓ Selected: %1 (ID: %2)")
                                            .arg(patient->name)
                                            .arg(patient->id));

            // Auto-calculate session number
            sessionNumberEdit->setText(QString::number(patient->visitCount + 1));

            // Mark as recently visited
            backend->markPatientAsVisited(patient->id, patient->name);

            delete patient; // Clean up
        }

        searchResultsList->hide();
    }
}

void AddSessionWindow::onUploadRecordingClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Select Session Recording", "",
                                                    "Audio/Video Files (*.mp3 *.wav *.mp4 *.mkv);;All Files (*)");
    if (!filePath.isEmpty()) {
        selectedFilePath = filePath;
        recordingFileLabel->setText("Selected: " + QFileInfo(filePath).fileName());
    }
}

// ========== SAVE SESSION (DSA BACKEND + DB) ==========
void AddSessionWindow::onSaveSessionClicked()
{
    if (selectedPatientId <= 0) {
        QMessageBox::warning(this, "Error", "Please search and select a patient first.");
        return;
    }

    QString notes = notesEdit->toPlainText().trimmed();

    if (notes.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please add session notes.");
        return;
    }

    // Use DSA backend to add session (saves to DB + Linked List)
    if (backend->addSession(selectedPatientId, notes)) {
        QMessageBox::information(this, "Success", "Session saved successfully!");

        // Clear form
        searchEdit->clear();
        searchResultsList->clear();
        searchResultsList->hide();
        patientResultLabel->setText("No patient selected.");
        sessionNumberEdit->setText("—");
        notesEdit->clear();
        recordingFileLabel->setText("No file selected.");
        selectedPatientId = -1;
    } else {
        QMessageBox::critical(this, "Error", "Failed to save session!");
    }
}

void AddSessionWindow::onCancelClicked()
{
    DashboardWindow *dashboard = new DashboardWindow();
    dashboard->show();
    this->close();
}
