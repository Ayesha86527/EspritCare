#include "dashboardwindow.h"
#include "addsessionwindow.h"
#include "addpatientwindow.h"
#include "viewpatientwindow.h"
#include "mainwindow.h"
#include "DSABackend.h"     // NEW
#include "espritdb.h"       // NEW

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QDateTime>
#include <QTimer>
#include <QFrame>

DashboardWindow::DashboardWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Initialize database
    db = new EspritDB(this);
    if (!db->openDatabase("espritcare.db")) {
        qDebug() << "Failed to open database!";
    }
    db->createTables();

    // Initialize backend with DSA structures
    backend = new DSABackend(db);

    setupUi();
    loadDashboardData();  // Load recently/frequently visited patients
}

DashboardWindow::~DashboardWindow()
{
    delete backend;
}

void DashboardWindow::setupUi()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    central->setStyleSheet(R"(
        QWidget {
            background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1,
                        stop:0 #f3f7fc, stop:1 #d6e6f5);
        }
    )");

    // ---------- LEFT SIDE MENU ----------
    QFrame *sidePanel = new QFrame(central);
    sidePanel->setFixedWidth(220);
    sidePanel->setStyleSheet(R"(
        QFrame {
            background-color: #203040;
            border-top-right-radius: 16px;
            border-bottom-right-radius: 16px;
        }
        QPushButton {
            background-color: transparent;
            color: white;
            border: none;
            text-align: left;
            padding: 10px 20px;
            font-size: 11pt;
        }
        QPushButton:hover {
            background-color: #2b7de9;
            border-radius: 6px;
        }
    )");

    QVBoxLayout *sideLayout = new QVBoxLayout(sidePanel);
    sideLayout->setAlignment(Qt::AlignTop);
    sideLayout->setContentsMargins(10, 20, 10, 10);
    sideLayout->setSpacing(12);

    QLabel *menuTitle = new QLabel("Menu");
    menuTitle->setStyleSheet("color: white; font-weight: bold; font-size: 14pt;");
    sideLayout->addWidget(menuTitle);

    QPushButton *addPatientBtn = new QPushButton("➕ Add Patient");
    QPushButton *addSessionBtn = new QPushButton("🗒️  Add Session");
    QPushButton *viewPatientsBtn = new QPushButton("📋 View Patients");
    QPushButton *goBackBtn = new QPushButton("⬅️  Back to Home");

    sideLayout->addWidget(addPatientBtn);
    sideLayout->addWidget(addSessionBtn);
    sideLayout->addWidget(viewPatientsBtn);
    sideLayout->addStretch();
    sideLayout->addWidget(goBackBtn);

    // ---------- MAIN CONTENT AREA ----------
    QWidget *mainArea = new QWidget(central);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainArea);
    mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(18);

    greetingLabel = new QLabel("Hey there, Welcome Back!");
    greetingLabel->setStyleSheet(R"(
        QLabel {
            font-size: 26px;
            font-weight: bold;
            color: #1f2f45;
            background: transparent;
        }
    )");
    mainLayout->addWidget(greetingLabel, 0, Qt::AlignLeft);

    dateTimeLabel = new QLabel;
    dateTimeLabel->setStyleSheet(R"(
        QLabel {
            font-size: 11pt;
            color: #4a5e72;
            background: transparent;
        }
    )");
    mainLayout->addWidget(dateTimeLabel, 0, Qt::AlignLeft);

    QFrame *line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("color: #cfd9e6;");
    mainLayout->addWidget(line);

    // Layout for Recently & Frequently Visited Patients
    QHBoxLayout *patientLayout = new QHBoxLayout;
    patientLayout->setSpacing(25);

    // --- Recent Patients ---
    QWidget *recentWidget = new QWidget;
    QVBoxLayout *recentLayout = new QVBoxLayout(recentWidget);

    QLabel *recentLabel = new QLabel("🕒 Recently Visited Patients");
    recentLabel->setStyleSheet("font-size: 12pt; font-weight: bold; color: #1f2f45; background: transparent;");
    recentLayout->addWidget(recentLabel);

    recentList = new QListWidget;
    recentList->setStyleSheet(R"(
        QListWidget {
            background: #ffffff;
            border: 1px solid #cfd9e6;
            border-radius: 8px;
            font-size: 10.5pt;
            color: #1f2f45;
            padding: 8px;
        }
        QListWidget::item {
            padding: 8px;
            border-bottom: 1px solid #e8ecef;
        }
        QListWidget::item:hover {
            background: #f0f4f8;
        }
    )");
    recentLayout->addWidget(recentList);
    patientLayout->addWidget(recentWidget);

    // --- Frequent Patients ---
    QWidget *freqWidget = new QWidget;
    QVBoxLayout *freqLayout = new QVBoxLayout(freqWidget);

    QLabel *freqLabel = new QLabel("⭐ Frequently Visited Patients");
    freqLabel->setStyleSheet("font-size: 12pt; font-weight: bold; color: #1f2f45; background: transparent;");
    freqLayout->addWidget(freqLabel);

    frequentList = new QListWidget;
    frequentList->setStyleSheet(R"(
        QListWidget {
            background: #ffffff;
            border: 1px solid #cfd9e6;
            border-radius: 8px;
            font-size: 10.5pt;
            color: #1f2f45;
            padding: 8px;
        }
        QListWidget::item {
            padding: 8px;
            border-bottom: 1px solid #e8ecef;
        }
        QListWidget::item:hover {
            background: #f0f4f8;
        }
    )");
    freqLayout->addWidget(frequentList);
    patientLayout->addWidget(freqWidget);

    mainLayout->addLayout(patientLayout);

    // Combine side panel and main area
    QHBoxLayout *rootLayout = new QHBoxLayout(central);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->addWidget(sidePanel);
    rootLayout->addWidget(mainArea);

    // ---------- Connections ----------
    connect(addPatientBtn, &QPushButton::clicked, this, &DashboardWindow::onAddPatientClicked);
    connect(addSessionBtn, &QPushButton::clicked, this, &DashboardWindow::onAddSessionClicked);
    connect(viewPatientsBtn, &QPushButton::clicked, this, &DashboardWindow::onViewPatientsClicked);
    connect(goBackBtn, &QPushButton::clicked, this, &DashboardWindow::onGoBackClicked);

    // Date & Time Auto Update
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &DashboardWindow::updateDateTime);
    timer->start(1000);
    updateDateTime();

    setWindowTitle("EspritCare Dashboard");
    resize(1000, 600);
}

// ========== NEW: LOAD DASHBOARD DATA USING DSA BACKEND ==========
void DashboardWindow::loadDashboardData()
{
    // Load Recently Visited Patients (Queue - FIFO)
    recentList->clear();
    QVector<PatientData> recentPatients = backend->getRecentlyVisitedPatients();

    if (recentPatients.isEmpty()) {
        recentList->addItem("No recent visits yet");
    } else {
        for (const auto& patient : recentPatients) {
            QString displayText = QString("%1 (ID: %2) - %3 visits")
            .arg(patient.name)
                .arg(patient.id)
                .arg(patient.visitCount);
            recentList->addItem(displayText);
        }
    }

    // Load Frequently Visited Patients (Sorted by visit count)
    frequentList->clear();
    QVector<PatientData> frequentPatients = backend->getFrequentlyVisitedPatients(5);

    if (frequentPatients.isEmpty()) {
        frequentList->addItem("No patient data available");
    } else {
        for (const auto& patient : frequentPatients) {
            QString displayText = QString("%1 - %2 visits")
            .arg(patient.name)
                .arg(patient.visitCount);
            frequentList->addItem(displayText);
        }
    }
}

void DashboardWindow::updateDateTime()
{
    QString dateTime = QDateTime::currentDateTime().toString("dddd, MMMM d, yyyy  |  hh:mm:ss ap");
    dateTimeLabel->setText(dateTime);
}

void DashboardWindow::onAddPatientClicked()
{
    AddPatientWindow *addWindow = new AddPatientWindow();
    addWindow->show();
    this->close();
}

void DashboardWindow::onAddSessionClicked()
{
    AddSessionWindow *addSession = new AddSessionWindow();
    addSession->show();
    this->close();
}

void DashboardWindow::onViewPatientsClicked()
{
    auto *viewPatients = new ViewPatientWindow();
    viewPatients->show();
    this->close();
}

void DashboardWindow::onGoBackClicked()
{
    MainWindow *mainWin = new MainWindow();
    mainWin->show();
    this->close();
}
