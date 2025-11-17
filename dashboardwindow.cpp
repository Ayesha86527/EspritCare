#include "dashboardwindow.h"
#include "addsessionwindow.h"
#include "addpatientwindow.h"
#include "viewpatientwindow.h"
#include "mainwindow.h"  // for going back to home
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QDateTime>
#include <QTimer>
#include <QFrame>
#include "backend.h"

DashboardWindow::DashboardWindow(Backend* backendPtr, QWidget* parent)
    : QMainWindow(parent), backend(backendPtr)
{
    setupUi();
    refreshDashboard();  // Load data when dashboard opens
}

// Refresh dashboard - populate recent and frequent patient lists
void DashboardWindow::refreshDashboard()
{
    // Clear existing items
    recentList->clear();
    frequentList->clear();

    // === RECENTLY VISITED PATIENTS ===
    const std::vector<QueueNode>& recentVisits = backend->getRecentVisits();

    if (recentVisits.empty()) {
        recentList->addItem("No recent visits yet.");
    } else {
        // Display in reverse order (most recent first)
        for (auto it = recentVisits.rbegin(); it != recentVisits.rend(); ++it) {
            QString item = QString("ID %1 - %2")
            .arg(it->patientID)
                .arg(QString::fromStdString(it->patientName));
            recentList->addItem(item);
        }
    }

    // === FREQUENTLY VISITED PATIENTS ===
    std::vector<Patient> frequentPatients = backend->getFrequentlyVisited();

    if (frequentPatients.empty()) {
        frequentList->addItem("No patients yet.");
    } else {
        // Show top 10 most frequent visitors
        int count = 0;
        for (const auto& p : frequentPatients) {
            if (count >= 10) break;  // Limit to top 10
            if (p.visit_count == 0) continue;  // Skip patients with no visits

            QString item = QString("%1. %2 (%3 visits)")
                               .arg(count + 1)
                               .arg(QString::fromStdString(p.name))
                               .arg(p.visit_count);
            frequentList->addItem(item);
            count++;
        }

        if (count == 0) {
            frequentList->addItem("No frequent visitors yet.");
        }
    }
}

void DashboardWindow::setupUi()
{
    // Central widget for the entire window
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    // Set a soft, smooth gradient background (same tone as Main Window)
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

    // Sidebar buttons
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

    // Greeting Title
    greetingLabel = new QLabel("Hey there, Welcome Back!");
    greetingLabel->setStyleSheet(R"(
        QLabel {
            font-size: 26px;
            font-weight: bold;
            color: #1f2f45;
            background: transparent;  /* makes it blend with gradient */
        }
    )");
    mainLayout->addWidget(greetingLabel, 0, Qt::AlignLeft);

    // Date and Time label
    dateTimeLabel = new QLabel;
    dateTimeLabel->setStyleSheet(R"(
        QLabel {
            font-size: 11pt;
            color: #4a5e72;
            background: transparent;  /* blends perfectly */
        }
    )");
    mainLayout->addWidget(dateTimeLabel, 0, Qt::AlignLeft);

    // Line separator
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
        }
    )");
    freqLayout->addWidget(frequentList);
    patientLayout->addWidget(freqWidget);

    mainLayout->addLayout(patientLayout);

    // Combine side panel and main area into root layout
    QHBoxLayout *rootLayout = new QHBoxLayout(central);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->addWidget(sidePanel);
    rootLayout->addWidget(mainArea);

    // ---------- Connections ----------
    connect(addPatientBtn, &QPushButton::clicked, this, &DashboardWindow::onAddPatientClicked);
    QPushButton *addPatientButton = new QPushButton("Add Patient");
    connect(addSessionBtn, &QPushButton::clicked, this, &DashboardWindow::onAddSessionClicked);
    connect(viewPatientsBtn, &QPushButton::clicked, this, &DashboardWindow::onViewPatientsClicked);
    connect(goBackBtn, &QPushButton::clicked, this, &DashboardWindow::onGoBackClicked);

    // ---------- Date & Time Auto Update ----------
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &DashboardWindow::updateDateTime);
    timer->start(1000);
    updateDateTime();

    // Window Properties
    setWindowTitle("EspritCare Dashboard");
    resize(1000, 600);
}


// Update date & time every second
void DashboardWindow::updateDateTime()
{
    QString dateTime = QDateTime::currentDateTime().toString("dddd, MMMM d, yyyy  |  hh:mm:ss ap");
    dateTimeLabel->setText(dateTime);
}

// Placeholder button actions

void DashboardWindow::onAddPatientClicked()
{
    AddPatientWindow *addWindow = new AddPatientWindow(backend, nullptr);  // No parent
    addWindow->setAttribute(Qt::WA_DeleteOnClose);
    addWindow->show();
    this->hide();
}

void DashboardWindow::onAddSessionClicked()
{
    AddSessionWindow *addSession = new AddSessionWindow(backend, nullptr);  // No parent
    addSession->setAttribute(Qt::WA_DeleteOnClose);
    addSession->show();
    this->hide();
}


void DashboardWindow::onViewPatientsClicked()
{
    ViewPatientWindow *viewPatients = new ViewPatientWindow(backend, nullptr);  // No parent
    viewPatients->setAttribute(Qt::WA_DeleteOnClose);
    viewPatients->show();
    this->hide();
}


// Go back to the welcome screen (MainWindow)
void DashboardWindow::onGoBackClicked()
{
    MainWindow *mainWin = new MainWindow(backend, nullptr);  // No parent
    mainWin->setAttribute(Qt::WA_DeleteOnClose);
    mainWin->show();
    this->close();
}

