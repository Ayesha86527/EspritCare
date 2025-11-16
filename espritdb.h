#ifndef ESPRITDB_H
#define ESPRITDB_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVector>
#include <QString>

struct PatientRecord {
    int id;
    QString name;
    QString gender;
    QString birthDate;
    int visitCount;
};

struct SessionRecord {
    int sessionId;
    int patientId;
    QString sessionDate;
    QString sessionNotes;
};

class EspritDB : public QObject
{
    Q_OBJECT

public:
    explicit EspritDB(QObject* parent = nullptr);
    ~EspritDB();

    bool openDatabase(const QString& path);
    void closeDatabase();

    bool createTables();

    // Patient operations
    bool addPatient(const QString& name, const QString& gender, const QString& birthDate);
    QVector<PatientRecord> getAllPatients();

    // Session operations
    bool addSession(int patientId, const QString& notes);
    QVector<SessionRecord> getSessionsForPatient(int patientId);

private:
    QSqlDatabase db;

    QString currentDate() const;
};

#endif // ESPRITDB_H
