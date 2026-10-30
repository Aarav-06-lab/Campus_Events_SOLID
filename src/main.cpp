#include "events.hpp"
#include <iostream>
#include <memory>
int main(int argc, char** argv) {
    try {
        bool fileMode = false, logMode = false;
        for (int i = 1; i < argc; ++i) {
            const std::string arg = argv[i];
            if (arg == "--file") fileMode = true;
            else if (arg == "--log") logMode = true;
            else { std::cerr << "Usage: events [--file] [--log]\n"; return 1; }
        }
        MemoryRepository memory;
        std::unique_ptr<FileRepository> disk;
        IRegistrationReader* reader = &memory;
        IRegistrationWriter* writer = &memory;
        if (fileMode) {
            disk = std::make_unique<FileRepository>("registrations.txt");
            reader = disk.get(); writer = disk.get();
        }
        ConsoleNotifier console(std::cout);
        LogNotifier log("notifications.log");
        INotifier& notifier = logMode ? static_cast<INotifier&>(log)
                                     : static_cast<INotifier&>(console);
        RegistrationService service(*writer, notifier);
        RegistrationReport report(*reader);
        std::string choice, student, event;
        while (true) {
            std::cout << "\n1 Register  2 View  0 Exit\nChoice: ";
            if (!std::getline(std::cin, choice) || choice == "0") break;
            if (choice == "2") report.print(std::cout);
            else if (choice == "1") {
                std::cout << "Student: ";
                if (!std::getline(std::cin, student)) break;
                std::cout << "Event: ";
                if (!std::getline(std::cin, event)) break;
                const auto result = service.registerFor(student, event);
                if (result == Result::Rejected)
                    std::cout << "Rejected: empty field or duplicate.\n";
                else if (result == Result::NotificationFailed)
                    std::cout << "Saved, but notification failed.\n";
                else std::cout << "Registration saved.\n";
            } else std::cout << "Invalid choice.\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n'; return 1;
    }
}
