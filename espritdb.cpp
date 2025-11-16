#include "espritdb.h"
#include <QDate>
#include <QDebug>

// ------------------------------
// Constructor / Destructor
// ------------------------------
EspritDB::EspritDB(QObject *parent)
    : QObject(parent)
{
}

EspritDB::~EspritDB()
{
    closeDatabase();
}

// ------------------------------
// DB Management
// ------------------------------
bool EspritDB::openDatabase(const QString& path)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);

    if (!db.open()) {
        qDebug() << "Failed to open DB:" << db.lastError().text();
        return false;
    }

    return true;
}

void EspritDB::closeDatabase()
{
    if (db.isOpen())
        db.close();
}

bool EspritDB::createTables()
{
    QSqlQuery q;

    QString patientsTable = R"(
        CREATE TABLE IF NOT EXISTS patients (
            patient_id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            gender TEXT,
            birth_date TEXT,
            visit_count INTEGER DEFAULT 0
        );
    )";

    QString sessionsTable = R"(
        CREATE TABLE IF NOT EXISTS sessions (
            session_id INTEGER PRIMARY KEY AUTOINCREMENT,
            patient_id INTEGER,
            session_date TEXT,
            session_notes TEXT,
            FOREIGN KEY(patient_id) REFERENCES patients(patient_id)
        );
    )";

    if (!q.exec(patientsTable)) {
        qDebug() << "Patients table error:" << q.lastError().text();
        return false;
    }

    if (!q.exec(sessionsTable)) {
        qDebug() << "Sessions table error:" << q.lastError().text();
        return false;
    }

    return true;
}

// ------------------------------
// Utility
// ------------------------------
QString EspritDB::currentDate() const
{
    return QDate::currentDate().toString("yyyy-MM-dd");
}

// ------------------------------
// PATIENT FUNCTIONS
// ------------------------------
bool EspritDB::addPatient(const QString& name, const QString& gender, const QString& birthDate)
{
    QSqlQuery q;
    q.prepare("INSERT INTO patients (name, gender, birth_date, visit_count) VALUES (?, ?, ?, 0)");

    q.addBindValue(name);
    q.addBindValue(gender);
    q.addBindValue(birthDate);

    if (!q.exec()) {
        qDebug() << "Add patient failed:" << q.lastError().text();
        return false;
    }

    return true;
}

QVector<PatientRecord> EspritDB::getAllPatients()
{
    QVector<PatientRecord> list;

    QSqlQuery q("SELECT patient_id, name, gender, birth_date, visit_count FROM patients");

    while (q.next()) {
        PatientRecord p;
        p.id = q.value(0).toInt();
        p.name = q.value(1).toString();
        p.gender = q.value(2).toString();
        p.birthDate = q.value(3).toString();
        p.visitCount = q.value(4).toInt();
        list.append(p);
    }

    return list;
}

// ------------------------------
// SESSION FUNCTIONS
// ------------------------------
bool EspritDB::addSession(int patientId, const QString& notes)
{
    QSqlQuery q;
    q.prepare("INSERT INTO sessions (patient_id, session_date, session_notes) VALUES (?, ?, ?)");

    q.addBindValue(patientId);
    q.addBindValue(currentDate());
    q.addBindValue(notes);

    if (!q.exec()) {
        qDebug() << "Add session failed:" << q.lastError().text();
        return false;
    }

    // Update visit count
    QSqlQuery update;
    update.prepare("UPDATE patients SET visit_count = visit_count + 1 WHERE patient_id = ?");
    update.addBindValue(patientId);
    update.exec();

    return true;
}

QVector<SessionRecord> EspritDB::getSessionsForPatient(int patientId)
{
    QVector<SessionRecord> list;

    QSqlQuery q;
    q.prepare("SELECT session_id, patient_id, session_date, session_notes "
              "FROM sessions WHERE patient_id = ? ORDER BY session_id DESC");
    q.addBindValue(patientId);
    q.exec();

    while (q.next()) {
        SessionRecord s;
        s.sessionId = q.value(0).toInt();
        s.patientId = q.value(1).toInt();
        s.sessionDate = q.value(2).toString();
        s.sessionNotes = q.value(3).toString();
        list.append(s);
    }

    return list;
}


