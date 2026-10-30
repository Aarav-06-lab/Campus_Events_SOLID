#include "events.hpp"
#include <cassert>
#include <cstdio>
#include <iostream>
#include <sstream>
class FakeNotifier : public INotifier {
public:
    int calls = 0;
    bool success = true;
    bool send(const Registration&) override { ++calls; return success; }
};
void repositoryContract(IRegistrationReader& r, IRegistrationWriter& w) {
    assert(r.all().empty());
    assert(!w.add({"", "Coding"}));
    assert(!w.add({"Aarav", ""}));
    assert(w.add({"Aarav", "Coding"}));
    assert(!w.add({"Aarav", "Coding"}));
    assert(w.add({"Aarav", "Music"}));
    assert(r.all().size() == 2);
    assert(r.all()[0].event == "Coding");
}
int main() {
    MemoryRepository memory;
    repositoryContract(memory, memory);
    std::remove("test-data.txt");
    FileRepository file("test-data.txt");
    repositoryContract(file, file);
    FileRepository reopened("test-data.txt");
    assert(reopened.all().size() == 2);
    MemoryRepository store;
    FakeNotifier fake;
    RegistrationService service(store, fake);
    assert(service.registerFor("A", "E") == Result::Registered);
    assert(fake.calls == 1);
    assert(service.registerFor("A", "E") == Result::Rejected);
    assert(fake.calls == 1);
    fake.success = false;
    assert(service.registerFor("B", "E") == Result::NotificationFailed);
    assert(store.all().size() == 2);
    std::ostringstream output;
    ConsoleNotifier console(output);
    assert(console.send({"A", "E"}));
    assert(output.str() == "Confirmed: A -> E\n");
    std::remove("test-notify.log");
    LogNotifier log("test-notify.log");
    assert(log.send({"A", "E"}));
    std::ifstream input("test-notify.log");
    std::string line;
    std::getline(input, line);
    assert(line == "Confirmed: A -> E");
    LogNotifier bad("missing-directory/no.log");
    assert(!bad.send({"A", "E"}));
    std::ostringstream report;
    RegistrationReport(store).print(report);
    assert(report.str().find("Registrations: 2") != std::string::npos);
    std::remove("test-data.txt");
    std::remove("test-notify.log");
    std::cout << "PASS: repository contracts, persistence, service,\n"
                 "notification substitution, failure handling and report.\n";
}
