#include "addsessionwindow.h"
#include "dashboardwindow.h"
#include "backend.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QSpacerItem>


AddSessionWindow::AddSessionWindow(Backend* backendPtr, QWidget *parent)
    : QMainWindow(parent), backend(backendPtr), currentPatientID(-1)  // Initialize here
{
    setupUi();
}


void AddSessionWindow::setupUi()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    // Background (same as theme)
    central->setStyleSheet(R"(
        QWidget {
            background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1,
                        stop:0 #f3f7fc, stop:1 #d6e6f5);
        }
    )");

    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setAlignment(Qt::AlignCenter);

    // --- Main Card Container ---
    QFrame *formCard = new QFrame;
    formCard->setFixedWidth(550);
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

    // --- Title ---
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


    // --- Styles ---
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
    searchEdit->setPlaceholderText("Enter patient name or ID");
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
    QLabel *uploadLabel = new QLabel("Upload Session Recording");
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
    QLabel *notesLabel = new QLabel("Session Notes (Optional)");
    notesLabel->setStyleSheet(labelStyle);
    notesEdit = new QTextEdit;
    notesEdit->setPlaceholderText("Add any details about this session...");
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
    connect(uploadBtn, &QPushButton::clicked, this, &AddSessionWindow::onUploadRecordingClicked);
    connect(saveBtn, &QPushButton::clicked, this, &AddSessionWindow::onSaveSessionClicked);
    connect(cancelBtn, &QPushButton::clicked, this, &AddSessionWindow::onCancelClicked);

    // Window properties
    setWindowTitle("EspritCare - Add Session");
    resize(1000, 600);
}

// --- Slots ---

void AddSessionWindow::onSearchPatientClicked()
{
    QString searchText = searchEdit->text().trimmed();

    if (searchText.isEmpty()) {
        QMessageBox::warning(this, "Search Error", "Please enter a patient name or ID.");
        return;
    }

    // Search in backend (by ID or name)
    Patient* found = backend->searchPatient(searchText.toStdString());

    if (found) {
        currentPatientID = found->id;  // Store the patient ID

        // Display patient info
        patientResultLabel->setText(
            QString("✓ Patient Found: %1 (ID: %2)")
                .arg(QString::fromStdString(found->name))
                .arg(found->id)
            );
        patientResultLabel->setStyleSheet(R"(
            QLabel {
                color: #2b7de9;
                font-size: 10pt;
                font-weight: 500;
                margin-top: 5px;
                background: transparent;
                border: none;
            }
        )");

        // Show next session number (visit_count + 1)
        sessionNumberEdit->setText(QString::number(found->visit_count + 1));

    } else {
        currentPatientID = -1;
        patientResultLabel->setText("✗ Patient not found. Please check the name or ID.");
        patientResultLabel->setStyleSheet(R"(
            QLabel {
                color: #dc3545;
                font-size: 10pt;
                font-weight: 500;
                margin-top: 5px;
                background: transparent;
                border: none;
            }
        )");
        sessionNumberEdit->setText("—");
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

void AddSessionWindow::onSaveSessionClicked()
{
    // Validate patient is selected
    if (currentPatientID == -1) {
        QMessageBox::warning(this, "Error", "Please search and select a patient first.");
        return;
    }

    QString notes = notesEdit->toPlainText().trimmed();

    // Create session in backend
    Session newSession = backend->addSession(currentPatientID, notes.toStdString());

    // Get patient for recent visits update
    Patient* p = backend->getPatientByID(currentPatientID);
    if (p) {
        backend->addRecentVisit(p->id, p->name);
    }

    // Success message
    QMessageBox::information(this, "Success",
                             QString("Session #%1 saved successfully for Patient ID: %2")
                                 .arg(newSession.session_id)
                                 .arg(currentPatientID));

    // Return to dashboard with fresh data
    DashboardWindow *dashboard = new DashboardWindow(backend, nullptr);
    dashboard->setAttribute(Qt::WA_DeleteOnClose);
    dashboard->show();
    this->close();
}


void AddSessionWindow::onCancelClicked()
{
    DashboardWindow *dashboard = new DashboardWindow(backend, nullptr);
    dashboard->setAttribute(Qt::WA_DeleteOnClose);
    dashboard->show();
    this->close();
}

