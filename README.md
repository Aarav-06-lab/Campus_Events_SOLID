# Campus Event Registration - SOLID C++ Case Study

A small C++17 console application for recording student registrations. It rejects empty names and duplicate student/event pairs, lists registrations, supports memory or text-file storage, and uses either console confirmation or a local notification log.

## Build and run
Requires a C++17 compiler; make is optional. No third-party libraries or internet required.

```sh
make
./events
./events --file
./events --file --log
make test
```

Without make (macOS/Linux):
```sh
c++ -std=c++17 -Wall -Wextra -pedantic -Iinclude src/main.cpp -o events
c++ -std=c++17 -Wall -Wextra -pedantic -Iinclude tests/tests.cpp -o tests_runner
./tests_runner
```
On macOS, install Apple Command Line Tools if no compiler is available. On Windows use a C++17 compiler and run the resulting .exe. Run commands from this project folder. Do not define NDEBUG when running these assertion-based tests.

## Example
Start ./events, enter 1, then Aarav, then Coding Workshop. Enter 2 to view it. Register the same pair again to see rejection. Enter 0 to exit. Names can contain spaces and matching is case-sensitive. Memory mode loses records after exit. --file saves quoted text to registrations.txt in the current directory. --log writes notifications.log instead of printing the confirmation.

## Extension demonstration
Baseline: ./events --file
Extension: ./events --file --log
Register a different student/event in each run. Inspect notifications.log after the second run. LogNotifier implements the existing INotifier interface; only startup selection changes, while RegistrationService stays unchanged. These are local confirmations, not real email/SMS.

## Structure
- include/events.hpp: data, narrow interfaces, concrete repositories/notifiers, service and report
- src/main.cpp: command-line selection and interactive menu
- tests/tests.cpp: common repository contract and service/notifier tests
- Makefile: build/test commands
- sample-output.txt: captured execution and test output

## SOLID mapping
SRP: storage, notification, registration coordination and reporting have separate classes.
OCP: LogNotifier extends notification behavior through INotifier.
LSP: both repositories obey the same duplicate/empty-input contract; shared tests verify this.
ISP: readers and writers have separate interfaces; reports require no write access.
DIP: RegistrationService depends on IRegistrationWriter and INotifier, supplied at startup.

## Tests
make test checks empty fields, duplicate rejection, distinct events, order, reopening file data, one notification after success, no notification after rejection, saved records after notification failure, console/log behavior, a failed log path and report output.

## Contributor and assistance
Student associated with this case study: Aarav Dangwal, 24BCE0711. Individual project; no partner listed.
This supplied implementation, tests and README were prepared with ChatGPT assistance. Before submission, the student must review the code and record their actual contribution here; do not claim unperformed work.

## Limitations
Single process only. No authentication, roles, event catalogue, capacity, cancellation, transactional file writes or concurrency control. Whitespace-only names are currently accepted. File corruption and partial writes need stronger handling. Notification failure does not roll back a saved registration. SOLID supports changing requirements; it does not establish high-load performance.

## Submission
Public repository: https://github.com/Aarav-06-lab/Campus_Events_SOLID

This repository contains Part B only. Keep Parts A and C and their reference PDF OUTSIDE the repository. Add the actual public URL to the individual report. Verify the link while signed out. Do not upload runtime data containing real student information.
