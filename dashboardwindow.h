#pragma once
#include <QMainWindow>

class QLabel;
class QPushButton;
class QListWidget;
class QTimer;
class DSABackend;  // NEW
class EspritDB;     // NEW

class DashboardWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit DashboardWindow(QWidget *parent = nullptr);
    ~DashboardWindow();

private slots:
    void updateDateTime();
    void onAddPatientClicked();
    void onAddSessionClicked();
    void onViewPatientsClicked();
    void onGoBackClicked();
    void loadDashboardData();  // NEW - Load recently/frequently visited

private:
    void setupUi();

    QLabel *greetingLabel;
    QLabel *dateTimeLabel;
    QTimer *timer;
    QListWidget *recentList;      // NEW
    QListWidget *frequentList;    // NEW

    // Backend integration
    EspritDB *db;                 // NEW
    DSABackend *backend;          // NEW
};
