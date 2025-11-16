#include "viewpatientwindow.h"
#include "dashboardwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QFrame>
#include <QMessageBox>

ViewPatientWindow::ViewPatientWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
}

void ViewPatientWindow::setupUi()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    // --- Background Gradient (Matches Theme) ---
    central->setStyleSheet(R"(
        QWidget {
            background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1,
                        stop:0 #f3f7fc, stop:1 #d6e6f5);
        }
    )");

    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    mainLayout->setSpacing(20);

    // --- Title ---
    QLabel *titleLabel = new QLabel("View Patients");
    titleLabel->setStyleSheet(R"(
        font-size: 24px;
        font-weight: bold;
        color: #1f2f45;
        background-color: rgba(255, 255, 255, 0.6);
        border-radius: 8px;
        padding: 10px 15px;
    )");
    mainLayout->addWidget(titleLabel);

    // --- Search and Sort Section ---
    QHBoxLayout *searchLayout = new QHBoxLayout;

    searchEdit = new QLineEdit;
    searchEdit->setPlaceholderText("Search by patient name or ID...");
    searchEdit->setFixedHeight(38);
    searchEdit->setStyleSheet(R"(
        QLineEdit {
            padding: 8px 12px;
            border: 1px solid #cfd9e6;
            border-radius: 6px;
            font-size: 11pt;
            color: #1f2f45;
            background-color: rgba(255, 255, 255, 0.8);
        }
        QLineEdit:focus {
            border: 1px solid #2b7de9;
            background-color: #f8fafc;
        }
    )");

    searchBtn = new QPushButton("Search");
    searchBtn->setFixedSize(100, 38);
    searchBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #2b7de9;
            color: white;
            border: none;
            border-radius: 6px;
            font-size: 10.5pt;
            font-weight: 500;
        }
        QPushButton:hover {
            background-color: #256dd1;
        }
    )");

    sortCombo = new QComboBox;
    sortCombo->addItem("Sort by Name (A–Z)");
    sortCombo->addItem("Sort by Most Recent Visit");
    sortCombo->setFixedHeight(38);
    sortCombo->setStyleSheet(R"(
        QComboBox {
            padding: 6px 10px;
            border: 1px solid #cfd9e6;
            border-radius: 6px;
            font-size: 10.5pt;
            background-color: rgba(255, 255, 255, 0.8);
        }
    )");

    searchLayout->addWidget(searchEdit);
    searchLayout->addWidget(searchBtn);
    searchLayout->addStretch();
    searchLayout->addWidget(sortCombo);

    mainLayout->addLayout(searchLayout);

    // --- Table Section (Empty for now) ---
    patientTable = new QTableWidget(0, 4);
    patientTable->setHorizontalHeaderLabels({"Patient ID", "Name", "Age", "Last Visit"});
    patientTable->horizontalHeader()->setStretchLastSection(true);
    patientTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    patientTable->setStyleSheet(R"(
        QHeaderView::section {
            background-color: rgba(255, 255, 255, 0.7);
            padding: 8px;
            font-weight: 600;
            color: #1f2f45;
            border: none;
        }
        QTableWidget {
            background-color: rgba(255, 255, 255, 0.85);
            border: 1px solid #cfd9e6;
            border-radius: 8px;
            gridline-color: #e1e8ef;
            font-size: 10.5pt;
            color: #1f2f45;
        }
        QTableWidget::item {
            padding: 6px;
        }
    )");
    mainLayout->addWidget(patientTable);

    // --- Back Button ---
    QHBoxLayout *bottomLayout = new QHBoxLayout;
    backBtn = new QPushButton("Back to Dashboard");
    backBtn->setFixedHeight(42);
    backBtn->setStyleSheet(R"(
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
    bottomLayout->addStretch();
    bottomLayout->addWidget(backBtn);
    mainLayout->addLayout(bottomLayout);

    // --- Connections ---
    connect(searchBtn, &QPushButton::clicked, this, &ViewPatientWindow::onSearchClicked);
    connect(backBtn, &QPushButton::clicked, this, &ViewPatientWindow::onBackClicked);

    // Window setup
    setWindowTitle("EspritCare - View Patients");
    resize(900, 600);
}

// --- Search Placeholder Function ---
void ViewPatientWindow::onSearchClicked()
{
    QString query = searchEdit->text().trimmed();
    if (query.isEmpty()) {
        QMessageBox::information(this, "Search", "Please enter a patient name or ID to search.");
    } else {
        QMessageBox::information(this, "Search", "Search feature coming soon!");
    }
}

// --- Back Button ---
void ViewPatientWindow::onBackClicked()
{
    auto *dashboard = new DashboardWindow();
    dashboard->show();
    this->close();
}

