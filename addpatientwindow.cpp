#include "addpatientwindow.h"
#include "dashboardwindow.h"
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDateEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QFrame>
#include <QMessageBox>
#include <QDate>

/*AddPatientWindow::AddPatientWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
}*/

AddPatientWindow::AddPatientWindow(Backend* backendPtr, QWidget *parent)
    : QMainWindow(parent), backend(backendPtr)  // assign pointer
{
    setupUi();
}

void AddPatientWindow::setupUi()
{
    // Central widget
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    // Match dashboard's gradient background
    central->setStyleSheet(R"(
        QWidget {
            background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1,
                        stop:0 #f3f7fc, stop:1 #d6e6f5);
        }
    )");

    // Main layout - centered with proper margins
    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Form card container - clean white card
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
    cardLayout->setSpacing(0);

    // Header section
    QLabel *titleLabel = new QLabel("Add New Patient");
    titleLabel->setStyleSheet(R"(
        font-size: 24px;
        font-weight: bold;
        color: #1f2f45;
        background: transparent;
        border: none;
        margin-bottom: 15px;

    )");
    titleLabel->setAlignment(Qt::AlignLeft);
    cardLayout->addWidget(titleLabel);


    // Divider line
    QFrame *line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("color: #cfd9e6; margin: 20px 0;");
    cardLayout->addWidget(line);

    // Form section with consistent spacing
    QVBoxLayout *formSection = new QVBoxLayout;
    formSection->setSpacing(18);

    // Common input style matching dashboard theme
    QString inputStyle = R"(
        QLineEdit, QComboBox, QSpinBox, QDateEdit {
            padding: 10px 12px;
            border: 1px solid #cfd9e6;
            border-radius: 6px;
            font-size: 11pt;
            background-color: #ffffff;
            color: #1f2f45;
        }
        QLineEdit:focus, QComboBox:focus, QSpinBox:focus, QDateEdit:focus {
            border: 1px solid #2b7de9;
            background-color: #f8fafc;
        }
        QTextEdit {
            padding: 10px 12px;
            border: 1px solid #cfd9e6;
            border-radius: 6px;
            font-size: 11pt;
            background-color: #ffffff;
            color: #1f2f45;
        }
        QTextEdit:focus {
            border: 1px solid #2b7de9;
            background-color: #f8fafc;
        }
    )";

    QString labelStyle = "font-size: 11pt; color: #1f2f45; font-weight: 500; background: transparent; border: none; margin-bottom: 8px;";

    // Name field
    QLabel *nameLabel = new QLabel("Patient Name");
    nameLabel->setStyleSheet(labelStyle);
    nameEdit = new QLineEdit;
    nameEdit->setPlaceholderText("Enter full name");
    nameEdit->setStyleSheet(inputStyle);
    formSection->addWidget(nameLabel);
    formSection->addWidget(nameEdit);

    // Gender and Age in a row (2 columns)
    QHBoxLayout *rowLayout1 = new QHBoxLayout;
    rowLayout1->setSpacing(15);

    // Gender column
    QVBoxLayout *genderCol = new QVBoxLayout;
    genderCol->setSpacing(8);
    QLabel *genderLabel = new QLabel("Gender");
    genderLabel->setStyleSheet(labelStyle);
    genderCombo = new QComboBox;
    genderCombo->addItems({"Select", "Male", "Female", "Other"});
    genderCombo->setStyleSheet(inputStyle);
    genderCol->addWidget(genderLabel);
    genderCol->addWidget(genderCombo);

    // Age column
    QVBoxLayout *ageCol = new QVBoxLayout;
    ageCol->setSpacing(8);
    QLabel *ageLabel = new QLabel("Age");
    ageLabel->setStyleSheet(labelStyle);
    ageSpin = new QSpinBox;
    ageSpin->setRange(1, 150);
    ageSpin->setValue(1);
    ageSpin->setStyleSheet(inputStyle);
    ageCol->addWidget(ageLabel);
    ageCol->addWidget(ageSpin);

    rowLayout1->addLayout(genderCol, 1);
    rowLayout1->addLayout(ageCol, 1);
    formSection->addLayout(rowLayout1);

    // Visit date field
    QLabel *dateLabel = new QLabel("Date of Visit");
    dateLabel->setStyleSheet(labelStyle);
    visitDateEdit = new QDateEdit;
    visitDateEdit->setCalendarPopup(true);
    visitDateEdit->setDate(QDate::currentDate());
    visitDateEdit->setDisplayFormat("dddd, MMMM d, yyyy");
    visitDateEdit->setStyleSheet(inputStyle);
    formSection->addWidget(dateLabel);
    formSection->addWidget(visitDateEdit);

    // Notes field
    QLabel *notesLabel = new QLabel("Additional Notes (Optional)");
    notesLabel->setStyleSheet(labelStyle);
    notesEdit = new QTextEdit;
    notesEdit->setPlaceholderText("Any additional information...");
    notesEdit->setMaximumHeight(90);
    notesEdit->setStyleSheet(inputStyle);
    formSection->addWidget(notesLabel);
    formSection->addWidget(notesEdit);

    cardLayout->addLayout(formSection);
    cardLayout->addSpacing(25);

    // Button section
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->setSpacing(12);

    // Cancel button - matching dashboard sidebar style
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

    // Add Patient button - matching dashboard blue theme
    QPushButton *addBtn = new QPushButton("Add Patient");
    addBtn->setFixedHeight(42);
    addBtn->setStyleSheet(R"(
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
    buttonLayout->addWidget(addBtn);

    cardLayout->addLayout(buttonLayout);

    // Add form card to main layout
    mainLayout->addWidget(formCard);

    // Connect signals
    connect(addBtn, &QPushButton::clicked, this, &AddPatientWindow::onAddPatientClicked);
    connect(cancelBtn, &QPushButton::clicked, this, &AddPatientWindow::onCancelClicked);

    // Window properties
    setWindowTitle("EspritCare - Add Patient");
    resize(1000, 600);
}

void AddPatientWindow::onAddPatientClicked()
{
    QString name = nameEdit->text().trimmed();
    QString gender = genderCombo->currentText();
    int age = ageSpin->value();
    QDate visitDate = visitDateEdit->date();
    QString notes = notesEdit->toPlainText().trimmed();

    // Validation
    if (name.isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Please enter the patient's name.");
        nameEdit->setFocus();
        return;
    }

    if (gender == "Select") {
        QMessageBox::warning(this, "Validation Error", "Please select a gender.");
        genderCombo->setFocus();
        return;
    }

    // Calculate approximate birth year from age
    int currentYear = QDate::currentDate().year();
    int birthYear = currentYear - age;
    QString birthDateStr = QString("%1-01-01").arg(birthYear);  // Approximate birth date

    // Add patient to backend
    Patient newPatient = backend->addPatient(
        name.toStdString(),
        gender.toStdString(),
        birthDateStr.toStdString()
        );

    // If notes provided, create a session for this visit
    if (!notes.isEmpty()) {
        backend->addSession(newPatient.id, notes.toStdString());
    }

    // Add to recent visits queue
    backend->addRecentVisit(newPatient.id, newPatient.name);

    // Success message
    QMessageBox::information(this, "Success",
                             QString("Patient '%1' added successfully!\nPatient ID: %2")
                                 .arg(QString::fromStdString(newPatient.name))
                                 .arg(newPatient.id));

    // Clear form
    nameEdit->clear();
    genderCombo->setCurrentIndex(0);
    ageSpin->setValue(1);
    visitDateEdit->setDate(QDate::currentDate());
    notesEdit->clear();
    nameEdit->setFocus();
}


void AddPatientWindow::onCancelClicked()
{
    DashboardWindow *dashboard = new DashboardWindow(backend, nullptr);
    dashboard->setAttribute(Qt::WA_DeleteOnClose);
    dashboard->show();
    this->close();
}
