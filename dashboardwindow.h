#pragma once
#include <QMainWindow>
#include "backend.h"

class QLabel;
class QPushButton;
class QListWidget;
class QTimer;

class DashboardWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit DashboardWindow(Backend* backend, QWidget* parent = nullptr);
    void refreshDashboard();

private slots:
    void updateDateTime();
    void onAddPatientClicked();
    void onAddSessionClicked();
    void onViewPatientsClicked();
    void onGoBackClicked();
private:
    void setupUi();
    QLabel *greetingLabel;
    QLabel *dateTimeLabel;
    QTimer *timer;
    QListWidget *recentList;
    QListWidget *frequentList;
    Backend* backend;

};

