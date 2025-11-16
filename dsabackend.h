#ifndef DSABACKEND_H
#define DSABACKEND_H

#include <QString>
#include <QVector>
#include <QDateTime>
#include "espritdb.h"

// ================= PATIENT STRUCT (Qt version) =================
struct PatientData {
    int id;
    QString name;
    QString gender;
    QString birthDate;
    int visitCount;

    PatientData() : id(0), visitCount(0) {}
};

// ================= SESSION LINKED LIST NODE =================
struct SessionNode {
    int patientId;
    QString date;
    QString notes;
    SessionNode* next;

    SessionNode(int pid, QString d, QString n)
        : patientId(pid), date(d), notes(n), next(nullptr) {}
};

// ================= RECENTLY VISITED QUEUE NODE =================
struct RecentQueueNode {
    int patientID;
    QString patientName;
    RecentQueueNode* next;

    RecentQueueNode(int id, QString name)
        : patientID(id), patientName(name), next(nullptr) {}
};

// ================= MAIN BACKEND CLASS =================
class DSABackend
{
public:
    DSABackend(EspritDB* database);
    ~DSABackend();

    // ========== FEATURE 1: Search Patient (for Add Session) ==========
    QVector<PatientData> searchPatientByName(const QString& searchText);
    PatientData* searchPatientByID(int targetID);

    // ========== FEATURE 2: Add Session ==========
    bool addSession(int patientId, const QString& notes);

    // ========== FEATURE 3: Recently Visited Patients (Queue - FIFO) ==========
    QVector<PatientData> getRecentlyVisitedPatients();
    void markPatientAsVisited(int patientId, const QString& patientName);

    // ========== FEATURE 4: Frequently Visited Patients (Priority Queue/Sorting) ==========
    QVector<PatientData> getFrequentlyVisitedPatients(int topCount = 5);

    // ========== Display Session List (Linked List) ==========
    QVector<QString> getSessionListDisplay();

private:
    EspritDB* db;

    // DSA Data Structures (from your group's code)
    SessionNode* sessionListHead;           // Linked list for sessions
    RecentQueueNode* queueFront;           // Recently visited queue front
    RecentQueueNode* queueRear;            // Recently visited queue rear
    int recentVisitCount;
    const int MAX_RECENT_PATIENTS = 5;     // Queue size limit

    // Helper functions
    QVector<PatientData> getAllPatientsFromDB();
    void sortPatientsByVisitCount(QVector<PatientData>& patients);
    void sortPatientsByName(QVector<PatientData>& patients);

    // Queue operations (Circular Queue)
    void enqueueRecentVisit(int id, const QString& name);
    void cleanupQueue();
    void cleanupSessionList();
};

#endif // DSABACKEND_H
