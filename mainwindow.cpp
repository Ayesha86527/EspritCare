#include "mainwindow.h"
#include "dashboardwindow.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFont>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUi();
    resize(1000,600);
    setMinimumSize(800,500);
}

void MainWindow::setupUi()
{
    // Central widget
    central = new QWidget(this);
    setCentralWidget(central);

    // Perfectly smooth gradient background
    central->setStyleSheet(R"(
        QWidget {
            background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1,
                        stop:0 #f3f7fc, stop:1 #d6e6f5);
        }
    )");

    // Main layout
    QVBoxLayout *layout = new QVBoxLayout(central);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(16);

    // App name
    titleLabel = new QLabel("EspritCare", central);
    QFont titleFont("Segoe UI", 30, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(R"(
        QLabel {
            color: #1f2f45;
            background: transparent;
        }
    )");

    layout->addWidget(titleLabel);

    // Tagline
    taglineLabel = new QLabel("Clarity, care, and connection — for every mind.", central);
    QFont tagFont("Segoe UI", 12);
    taglineLabel->setFont(tagFont);
    taglineLabel->setAlignment(Qt::AlignCenter);
    taglineLabel->setWordWrap(true);
    taglineLabel->setStyleSheet(R"(
        QLabel {
            color: #4a5e72;
            background: transparent;
        }
    )");

    layout->addWidget(taglineLabel);

    // Spacing
    layout->addSpacing(25);

    // Get Started button
    getStartedBtn = new QPushButton("Get Started", central);
    getStartedBtn->setFixedSize(200, 45);
    getStartedBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #2b7de9;
            color: white;
            font-size: 13pt;
            font-weight: bold;
            border-radius: 8px;
            padding: 8px 16px;
            border: none;
        }
        QPushButton:hover {
            background-color: #358af0;
        }
        QPushButton:pressed {
            background-color: #266bcc;
        }
    )");

    layout->addWidget(getStartedBtn, 0, Qt::AlignHCenter);

    // Remove borders/margins that might cause misalignment
    layout->setContentsMargins(0, 0, 0, 0);

    connect(getStartedBtn, &QPushButton::clicked, this, &MainWindow::onGetStartedClicked);

    setWindowTitle("EspritCare");
}

void MainWindow::onGetStartedClicked()
{
    DashboardWindow* dashboard=new DashboardWindow();
    dashboard->show();
    this->close();
}
