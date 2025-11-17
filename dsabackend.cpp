#include "backend.h"
#include <ctime>
#include <algorithm>
#include <cctype>

Backend::Backend() {
    sessionHead = nullptr;
    sessionTail = nullptr;
}

std::string Backend::currentDate() const {
    time_t t = time(nullptr);
    tm* now = localtime(&t);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", now);
    return std::string(buffer);
}

// ================= PATIENT =================
Patient Backend::addPatient(const std::string& name, const std::string& gender, const std::string& birth_date) {
    Patient p;
    p.id = globalPatientID++;
    p.name = name;
    p.gender = gender;
    p.birth_date = birth_date;
    p.visit_count = 0;

    allPatients.push_back(p);
    return p;
}

Patient* Backend::getPatientByID(int id) {
    for (auto &p : allPatients) {
        if (p.id == id) return &p;
    }
    return nullptr;
}

// Search patient by ID (if numeric) or by name (partial match, case-insensitive)
Patient* Backend::searchPatient(const std::string& searchTerm) {
    if (searchTerm.empty()) return nullptr;

    // Try to search by ID first (if searchTerm is numeric)
    bool isNumeric = true;
    for (char c : searchTerm) {
        if (!isdigit(c)) {
            isNumeric = false;
            break;
        }
    }

    if (isNumeric) {
        int id = std::stoi(searchTerm);
        return getPatientByID(id);
    }

    // Search by name (case-insensitive partial match)
    std::string lowerSearch = searchTerm;
    std::transform(lowerSearch.begin(), lowerSearch.end(), lowerSearch.begin(), ::tolower);

    for (auto &p : allPatients) {
        std::string lowerName = p.name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

        if (lowerName.find(lowerSearch) != std::string::npos) {
            return &p;
        }
    }

    return nullptr;  // Not found
}

std::vector<Patient> Backend::getAllPatients() const {
    return allPatients;
}

std::vector<Patient> Backend::getFrequentlyVisited() const {
    std::vector<Patient> sortedPatients = allPatients;
    std::sort(sortedPatients.begin(), sortedPatients.end(), [](const Patient& a, const Patient& b){
        return a.visit_count > b.visit_count;
    });
    return sortedPatients;
}


// ================= SESSION =================

Session Backend::addSession(int patientID, const std::string& notes) {
    Session s;
    s.session_id = globalSessionID++;
    s.patientID = patientID;
    s.date = currentDate();
    s.notes = notes;

    allSessions.push_back(s);

    // === Insert into Linked List ===
    SessionNode* newNode = new SessionNode{ s, nullptr };

    if (!sessionHead) {
        sessionHead = sessionTail = newNode;
    } else {
        sessionTail->next = newNode;
        sessionTail = newNode;
    }

    // === Increment visit count ===
    Patient* p = getPatientByID(patientID);
    if (p) p->visit_count++;

    return s;
}

const std::vector<Session>& Backend::getAllSessions() const {
    return allSessions;
}

// ===== Return a vector by traversing the LINKED LIST =====
std::vector<Session> Backend::getAllSessionsLinkedList() const {
    std::vector<Session> result;
    SessionNode* curr = sessionHead;

    while (curr != nullptr) {
        result.push_back(curr->data);
        curr = curr->next;
    }

    return result;
}


// ================= RECENT VISITS =================
void Backend::addRecentVisit(int patientID, const std::string& name) {
    QueueNode node{patientID, name};

    recentVisits.push_back(node);
    if (recentVisits.size() > RECENT_VISIT_MAX) {
        recentVisits.erase(recentVisits.begin());
    }
}

const std::vector<QueueNode>& Backend::getRecentVisits() const {
    return recentVisits;
}
