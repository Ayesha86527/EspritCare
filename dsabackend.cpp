#include "DSABackend.h"
#include <QDebug>
#include <algorithm>

// ================= CONSTRUCTOR / DESTRUCTOR =================
DSABackend::DSABackend(EspritDB* database)
    : db(database)
    , sessionListHead(nullptr)
    , queueFront(nullptr)
    , queueRear(nullptr)
    , recentVisitCount(0)
{
    qDebug() << "DSABackend initialized";
}

DSABackend::~DSABackend()
{
    cleanupQueue();
    cleanupSessionList();
}

void DSABackend::cleanupQueue()
{
    if (queueFront == nullptr) return;

    RecentQueueNode* current = queueFront;
    for (int i = 0; i < recentVisitCount; i++) {
        RecentQueueNode* next = current->next;
        delete current;
        current = next;
    }
    queueFront = queueRear = nullptr;
    recentVisitCount = 0;
}

void DSABackend::cleanupSessionList()
{
    SessionNode* current = sessionListHead;
    while (current != nullptr) {
        SessionNode* next = current->next;
        delete current;
        current = next;
    }
    sessionListHead = nullptr;
}

// ================= HELPER: GET ALL PATIENTS FROM DB =================
QVector<PatientData> DSABackend::getAllPatientsFromDB()
{
    QVector<PatientData> patients;
    QVector<PatientRecord> records = db->getAllPatients();

    for (const auto& rec : records) {
        PatientData p;
        p.id = rec.id;
        p.name = rec.name;
        p.gender = rec.gender;
        p.birthDate = rec.birthDate;
        p.visitCount = rec.visitCount;
        patients.append(p);
    }

    return patients;
}

// ================= SORTING ALGORITHMS (DSA) =================
// Bubble Sort by visit count (descending)
void DSABackend::sortPatientsByVisitCount(QVector<PatientData>& patients)
{
    int n = patients.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (patients[j].visitCount < patients[j + 1].visitCount) {
                // Swap
                PatientData temp = patients[j];
                patients[j] = patients[j + 1];
                patients[j + 1] = temp;
            }
        }
    }
}

// Bubble Sort by name (ascending)
void DSABackend::sortPatientsByName(QVector<PatientData>& patients)
{
    int n = patients.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (patients[j].name > patients[j + 1].name) {
                // Swap
                PatientData temp = patients[j];
                patients[j] = patients[j + 1];
                patients[j + 1] = temp;
            }
        }
    }
}

// ================= FEATURE 1: SEARCH PATIENT =================
QVector<PatientData> DSABackend::searchPatientByName(const QString& searchText)
{
    QVector<PatientData> results;
    QVector<PatientData> allPatients = getAllPatientsFromDB();

    // Sort patients by name for binary search
    sortPatientsByName(allPatients);

    // Linear search for partial matches (since we need "contains" not exact match)
    for (const auto& patient : allPatients) {
        if (patient.name.contains(searchText, Qt::CaseInsensitive)) {
            results.append(patient);
        }
    }

    qDebug() << "Search results:" << results.size() << "patients found";
    return results;
}

PatientData* DSABackend::searchPatientByID(int targetID)
{
    QVector<PatientData> allPatients = getAllPatientsFromDB();

    // Binary Search by ID (after sorting)
    std::sort(allPatients.begin(), allPatients.end(),
              [](const PatientData& a, const PatientData& b) {
                  return a.id < b.id;
              });

    int left = 0;
    int right = allPatients.size() - 1;

    while (left <= right) {
        int mid = (left + right) / 2;

        if (allPatients[mid].id == targetID) {
            // Return pointer to found patient (allocated on heap)
            PatientData* found = new PatientData();
            *found = allPatients[mid];
            return found;
        }

        if (allPatients[mid].id < targetID) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return nullptr; // Not found
}

// ================= FEATURE 2: ADD SESSION =================
bool DSABackend::addSession(int patientId, const QString& notes)
{
    // 1. Add session to database
    if (!db->addSession(patientId, notes)) {
        qDebug() << "Failed to add session to database";
        return false;
    }

    // 2. Add to linked list (in-memory DSA structure)
    QString currentDate = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    SessionNode* newNode = new SessionNode(patientId, currentDate, notes);

    if (sessionListHead == nullptr) {
        sessionListHead = newNode;
    } else {
        // Add to end of linked list
        SessionNode* temp = sessionListHead;
        while (temp->next != nullptr) {
            temp = temp->next;
        }
        temp->next = newNode;
    }

    qDebug() << "Session added successfully (DB + Linked List)";
    return true;
}

// ================= FEATURE 3: RECENTLY VISITED (QUEUE) =================
void DSABackend::enqueueRecentVisit(int id, const QString& name)
{
    RecentQueueNode* newNode = new RecentQueueNode(id, name);

    if (queueFront == nullptr) {
        // Empty queue - circular setup
        queueFront = queueRear = newNode;
        newNode->next = queueFront; // Point to itself
    } else {
        // Add to rear
        queueRear->next = newNode;
        newNode->next = queueFront; // Maintain circular
        queueRear = newNode;
    }

    recentVisitCount++;

    // If queue exceeds limit, remove oldest (front)
    if (recentVisitCount > MAX_RECENT_PATIENTS) {
        RecentQueueNode* temp = queueFront;
        queueFront = queueFront->next;
        queueRear->next = queueFront; // Maintain circular
        delete temp;
        recentVisitCount--;
    }
}

void DSABackend::markPatientAsVisited(int patientId, const QString& patientName)
{
    enqueueRecentVisit(patientId, patientName);
    qDebug() << "Marked as recently visited:" << patientName;
}

QVector<PatientData> DSABackend::getRecentlyVisitedPatients()
{
    QVector<PatientData> recentPatients;

    if (queueFront == nullptr) {
        qDebug() << "No recently visited patients";
        return recentPatients;
    }

    // Traverse circular queue
    RecentQueueNode* temp = queueFront;
    for (int i = 0; i < recentVisitCount; i++) {
        // Get full patient data from database
        PatientData* patient = searchPatientByID(temp->patientID);
        if (patient != nullptr) {
            recentPatients.append(*patient);
            delete patient; // Clean up
        }
        temp = temp->next;
    }

    qDebug() << "Recently visited patients:" << recentPatients.size();
    return recentPatients;
}

// ================= FEATURE 4: FREQUENTLY VISITED (SORTING) =================
QVector<PatientData> DSABackend::getFrequentlyVisitedPatients(int topCount)
{
    QVector<PatientData> allPatients = getAllPatientsFromDB();

    if (allPatients.isEmpty()) {
        qDebug() << "No patients in database";
        return QVector<PatientData>();
    }

    // Sort by visit count (descending) using bubble sort
    sortPatientsByVisitCount(allPatients);

    // Return top N patients
    QVector<PatientData> topPatients;
    int count = qMin(topCount, allPatients.size());

    for (int i = 0; i < count; i++) {
        topPatients.append(allPatients[i]);
    }

    qDebug() << "Frequently visited patients:" << topPatients.size();
    return topPatients;
}

// ================= DISPLAY SESSION LINKED LIST =================
QVector<QString> DSABackend::getSessionListDisplay()
{
    QVector<QString> sessions;

    if (sessionListHead == nullptr) {
        qDebug() << "No sessions in linked list";
        return sessions;
    }

    SessionNode* temp = sessionListHead;
    while (temp != nullptr) {
        QString sessionInfo = QString("Patient ID: %1 | Date: %2 | Notes: %3")
        .arg(temp->patientId)
            .arg(temp->date)
            .arg(temp->notes);
        sessions.append(sessionInfo);
        temp = temp->next;
    }

    return sessions;
}
