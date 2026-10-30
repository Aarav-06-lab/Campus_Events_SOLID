#pragma once
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

struct Registration {
    std::string student;
    std::string event;
};
// Both repositories accept nonempty names, reject duplicate pairs, and
// leave existing records unchanged when add returns false.
class IRegistrationReader {
public:
    virtual std::vector<Registration> all() const = 0;
    virtual ~IRegistrationReader() = default;
};
class IRegistrationWriter {
public:
    virtual bool add(const Registration& r) = 0;
    virtual ~IRegistrationWriter() = default;
};
inline bool duplicate(const std::vector<Registration>& rows,
                      const Registration& r) {
    return std::any_of(rows.begin(), rows.end(), [&](const auto& x) {
        return x.student == r.student && x.event == r.event;
    });
}
class MemoryRepository : public IRegistrationReader,
                         public IRegistrationWriter {
    std::vector<Registration> rows;
public:
    std::vector<Registration> all() const override { return rows; }
    bool add(const Registration& r) override {
        if (r.student.empty() || r.event.empty() || duplicate(rows, r))
            return false;
        rows.push_back(r);
        return true;
    }
};
class FileRepository : public IRegistrationReader,
                       public IRegistrationWriter {
    std::string path;
public:
    explicit FileRepository(std::string p) : path(std::move(p)) {
        std::ofstream file(path, std::ios::app);
        if (!file) throw std::runtime_error("Cannot open data file");
    }
    std::vector<Registration> all() const override {
        std::ifstream file(path);
        if (!file) throw std::runtime_error("Cannot read data file");
        std::vector<Registration> rows;
        Registration r;
        while (file >> std::quoted(r.student) >> std::quoted(r.event))
            rows.push_back(r);
        if (!file.eof()) throw std::runtime_error("Invalid data file");
        return rows;
    }
    bool add(const Registration& r) override {
        if (r.student.empty() || r.event.empty() || duplicate(all(), r))
            return false;
        std::ofstream file(path, std::ios::app);
        file << std::quoted(r.student) << ' '
             << std::quoted(r.event) << '\n';
        file.flush();
        if (!file) throw std::runtime_error("Cannot save registration");
        return true;
    }
};
class INotifier {
public:
    // Notification delivery is best effort; false does not undo storage.
    virtual bool send(const Registration& r) = 0;
    virtual ~INotifier() = default;
};
class ConsoleNotifier : public INotifier {
    std::ostream& out;
public:
    explicit ConsoleNotifier(std::ostream& o) : out(o) {}
    bool send(const Registration& r) override {
        out << "Confirmed: " << r.student << " -> " << r.event << '\n';
        return static_cast<bool>(out);
    }
};
// Demonstrated extension: a local notification log, not an email service.
class LogNotifier : public INotifier {
    std::string path;
public:
    explicit LogNotifier(std::string p) : path(std::move(p)) {}
    bool send(const Registration& r) override {
        std::ofstream file(path, std::ios::app);
        file << "Confirmed: " << r.student << " -> " << r.event << '\n';
        file.flush();
        return static_cast<bool>(file);
    }
};
enum class Result { Registered, Rejected, NotificationFailed };
class RegistrationService {
    IRegistrationWriter& writer;
    INotifier& notifier;
public:
    RegistrationService(IRegistrationWriter& w, INotifier& n)
        : writer(w), notifier(n) {}
    Result registerFor(const std::string& student,
                       const std::string& event) {
        Registration r{student, event};
        if (!writer.add(r)) return Result::Rejected;
        return notifier.send(r) ? Result::Registered
                               : Result::NotificationFailed;
    }
};
class RegistrationReport {
    const IRegistrationReader& reader;
public:
    explicit RegistrationReport(const IRegistrationReader& r) : reader(r) {}
    void print(std::ostream& out) const {
        const auto rows = reader.all();
        out << "Registrations: " << rows.size() << '\n';
        for (const auto& r : rows)
            out << r.student << " | " << r.event << '\n';
    }
};
