Internship Report On

“NTPWEdit Clone - Windows Password Management Tool”

In the partial fulfillment of the requirement for Third Year
Computer Engineering

Submitted by
Kakad Saurabh Madhukar             Roll No: 32
Rohan Sachin Pagar                  Roll No: 32

Under the Guidance of
Prof. P. P. Kakade

DEPARTMENT OF COMPUTER ENGINEERING

NGSPM’S
BRAHMA VALLEY COLLEGE OF ENGINEERING AND RESEARCH INSTITUTE NASHIK-422213

SAVITRIBAI PHULE PUNE UNIVERSITY, PUNE 2025-2026

INDEX

Sr. No. | Content | Page No.
--- | --- | ---
1 | INTRODUCTION | 3
1.1 | Project Overview | 3
1.2 | Problem Statement | 3
1.3 | Objectives of the Project | 4
1.4 | Scope of the Project | 4
2 | SYSTEM FLOW AND ARCHITECTURE | 5
2.1 | Overall System Flow | 5
2.2 | System Architecture | 6
3 | REGISTRY ENGINE MODULE | 7
3.1 | Hive Parsing Interface | 7
3.2 | Working of Registry Parsing | 8
4 | MAIN INTERFACE MODULE | 9
4.1 | Introduction | 9
4.2 | User Interface Layout | 9
4.3 | Automatic SAM Detection | 10
4.4 | User Account Display | 10
5 | SAM MANAGEMENT MODULE | 11
5.1 | Introduction | 11
5.2 | User Enumeration Logic | 11
5.3 | RID Mapping | 12
6 | BUILD AND EXECUTION MODULE | 13
6.1 | Introduction | 13
6.2 | Build System (MinGW) | 13
6.3 | Execution Environment | 14
7 | ABOUT MODULE | 15
8 | RESULTS AND DISCUSSION | 16
8.1 | Introduction | 16
8.2 | Functional Results | 16
8.3 | System Performance Analysis | 17
8.4 | Discussion | 17
9 | CONCLUSION | 18
10 | FUTURE SCOPE | 19
10.1 | Introduction | 19
10.2 | Possible Enhancements | 19

---

CHAPTER 1
INTRODUCTION
1.1 Project Overview
In the field of system administration and forensic security, the ability to manage user accounts directly from system registry files is crucial. Often, administrators lose access to local accounts due to forgotten passwords or system lockouts. Traditional methods of password recovery can be complex or time-consuming.

The project “NTPWEdit Clone” was developed to provide a lightweight, dependency-free C++ application that can directly open and modify Windows SAM (Security Accounts Manager) registry hives. Unlike standard registry editors that require a running OS to access the registry via APIs, this tool parses the binary hive format directly, allowing for offline account management.

The system is developed using C++ and the Win32 API, ensuring a small footprint and high performance. It focuses on providing a simple interface for detecting, loading, and viewing user account information directly from the SAM hive.

1.2 Problem Statement
Many system administrators face the following challenges:
*   Forgotten local administrator passwords.
*   System lockouts where the OS is unbootable.
*   The need for offline account auditing.
*   Complexity of existing password reset tools.
*   Lack of a simple, open-source tool for manual SAM exploration.

Without a direct SAM editor:
*   Users may lose access to critical data.
*   Password recovery requires specialized, often paid, software.
*   Understanding the registry hive structure is difficult for developers.
*   Security auditing of local accounts becomes a manual, tedious task.

Therefore, there is a need for a system that allows users to digitally record, analyze, and manage SAM hives in a standalone environment.

1.3 Objectives of the Project
The main objectives of the NTPWEdit Clone system are:
1. To design and develop a C++ based registry hive parser.
2. To allow users to load SAM hives directly from the Windows file system.
3. To implement an automatic SAM detection feature for the local system.
4. To enumerate all user accounts, including their Relative Identifiers (RIDs).
5. To provide a clean Win32-based UI for interacting with hive data.
6. To implement a modular build system using MinGW and Batch scripts.

1.4 Scope of the Project
The scope of the system includes:
*   Offline SAM hive parsing.
*   User account enumeration and RID mapping.
*   Automatic detection of the system SAM path.
*   Low-level registry record (nk, vk, lf) processing.
*   Basic UI for account visualization.

The system currently runs on Windows and demonstrates the complete engine for parsing registry hives.

Future improvements may include:
*   Support for password resetting (NT hash modification).
*   Account unlocking (ACB bit modification).
*   Integration with other registry hives (SYSTEM, SOFTWARE).
*   WinPE-ready deployment packages.

---

CHAPTER 2
SYSTEM FLOW AND ARCHITECTURE
2.1 Overall System Flow
The overall working of the system follows a structured sequence. First, the user launches the NTPWEdit Clone executable. The main dialog interface is displayed. The system automatically detects the local SAM path (C:\Windows\System32\config\SAM) and populates the path field.

The user clicks the "(re)Open" button. The system opens the hive file in binary mode. The Registry Engine parses the "regf" header and "hbin" blocks. The SAM Manager then traverses the internal directory tree (\SAM\Domains\Account\Users) to find user records. If successful, the user list is populated in the main UI, showing the Username and RID for each account.

2.2 System Architecture
The NTPWEdit Clone system follows a modular architecture divided into four main layers:
1.  **User Interface Layer**: Handles the Win32 Dialog, event processing, and list view management.
2.  **SAM Management Layer**: Responsible for interpreting the SAM-specific structure within the registry.
3.  **Registry Engine Layer**: Provides low-level access to registry records, parsing headers, and navigating subkeys.
4.  **Data Access Layer**: Handles raw binary file I/O for hive files.

This modular approach ensures that the registry parsing logic can be reused for other hives, while keeping the UI code clean and responsive.

---

CHAPTER 3
REGISTRY ENGINE MODULE
3.1 Hive Parsing Interface
The Registry Engine is the core of the system. It is implemented in `ntreg.cpp` and `ntreg.h`. It defines the internal structures of a Windows Registry Hive, such as:
*   **regf_header**: The main file header.
*   **hbin_header**: Hive bin blocks containing data.
*   **nk_record**: Named Key records representing registry keys.
*   **vk_record**: Value Key records representing registry values.
*   **lx_record**: Subkey list records for navigation.

3.2 Working of Registry Parsing
Validation: The system verifies the "regf" signature and calculates the checksum of the header.
Navigation: The `findSubkey` and `enumerateSubkeys` functions allow the system to walk the registry tree by matching key names against `nk_record` entries.
Data Retrieval: The `getValueData` function extracts raw data from `vk_record` structures, handling both inline and external data offsets.

---

CHAPTER 4
MAIN INTERFACE MODULE
4.1 Introduction
The Main Interface module is the primary interaction point for the user. Built using the native Win32 API, it provides a fast and familiar Windows experience without the overhead of external frameworks.

4.2 User Interface Layout
The interface consists of:
*   **Path Selection Bar**: Displays the currently selected SAM hive path with a "Browse" option.
*   **User List View**: A multi-column list showing User Name, RID, and Account Status.
*   **Action Buttons**: Buttons for "Unlock", "Change Password", and "Save Changes".

4.3 Automatic SAM Detection
On startup, the system uses the `GetSystemDirectory` API to locate the Windows System32 folder and appends the standard configuration path to automatically find the local SAM hive.

4.4 User Account Display
Once the hive is opened, the UI dynamically populates the list view using the data retrieved by the SAM Manager. Each entry is mapped to its internal RID and record offset.

---

CHAPTER 5
SAM MANAGEMENT MODULE
5.1 Introduction
The SAM Management module (implemented in `sam.cpp`) interprets the logical structure of a SAM hive. Windows stores user accounts in a nested structure: `\SAM\Domains\Account\Users`.

5.2 User Enumeration Logic
The SAM Manager navigates to the "Names" subkey and enumerates all subkeys. Each subkey name corresponds to a username. The RID for each user is stored as the "Type" of the key.

5.3 RID Mapping
For each username found, the system performs a reverse lookup in the `\Users` key using the hex-formatted RID (e.g., `000001F4` for Administrator). This allows the tool to find the specific `nk_record` that contains the account's password hashes and control bits.

---

CHAPTER 6
BUILD AND EXECUTION MODULE
6.1 Introduction
Since the project is built for systems that may not have complex development environments (like recovery consoles), it uses a portable build system.

6.2 Build System (MinGW)
The project uses `g++` and `windres` from the MinGW toolkit. A custom `build.bat` script automates:
*   Resource compilation (icons, dialog templates).
*   Source code compilation with Unicode support.
*   Static linking of Windows libraries.

6.3 Execution Environment
The resulting executable is a standalone Win32 application. It is designed to be run from:
*   A live Windows system (running as Administrator).
*   Windows Recovery Environment (WinRE).
*   External bootable media (Windows PE).

---

CHAPTER 7
ABOUT MODULE
The About module provides documentation on the project's purpose. It highlights that the `ntpwedit_clone` is a security tool intended for password recovery and educational exploration of the Windows Registry format. It emphasizes the "Track, Analyze, Improve" methodology, adapted here as "Detect, Load, Modify."

---

CHAPTER 8
RESULTS AND DISCUSSION
8.1 Introduction
This chapter presents the functional validation of the NTPWEdit Clone. Testing was performed on various SAM hives to ensure compatibility and accuracy.

8.2 Functional Results
*   **Auto-Detection**: The system correctly identifies the local SAM path on every launch.
*   **Hive Parsing**: The engine successfully handles "regf" validation and "hbin" traversal.
*   **User Listing**: User accounts (Administrator, Guest, User) are correctly listed with their respective RIDs.
*   **UI Stability**: The Win32 dialog remains responsive even during large hive loading.

8.3 System Performance Analysis
The application exhibits near-instantaneous loading times for typical SAM hives (~few megabytes). Memory usage is minimal, as only the necessary hive blocks are parsed into memory.

8.4 Discussion
The project successfully achieves the primary goal of providing a standalone SAM viewer. The implementation of the custom registry parser demonstrates that Windows system files can be manipulated safely without relying on the internal registry APIs of a running OS.

---

CHAPTER 9
CONCLUSION
The NTPWEdit Clone project was successfully designed and implemented to provide a powerful yet simple tool for Windows account management. By focusing on direct binary parsing of registry hives, the project overcomes the limitations of standard Windows tools.

The modular architecture ensures that the system is scalable for future features like password hashing or account unlocking. Overall, the project fulfills the requirements of a Third Year Computer Engineering internship project, demonstrating proficiency in C++, system-level programming, and UI design.

---

CHAPTER 10
FUTURE SCOPE
10.1 Introduction
While the current version provides robust viewing capabilities, there is significant room for growth.

10.2 Possible Enhancements
1.  **NT Hash Manipulation**: Adding the ability to clear or reset user passwords by overwriting the V-record hashes.
2.  **Account Unlocking**: Modifying the Account Control Bits (ACB) to enable disabled or locked-out users.
3.  **GUI Improvements**: Adding hex-view capabilities specifically for registry records.
4.  **Cross-Hive Support**: Extending the parser to handle SYSTEM hives for decryption key extraction (SysKey).
