#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <windows.h>
#include "ntreg.h"
#include "sam.h"

void printUsage(const std::string& progName) {
    std::cout << "NTPWEdit Clone CLI - Windows Password Management Tool\n";
    std::cout << "Usage: " << progName << " [options]\n";
    std::cout << "Options:\n";
    std::cout << "  -p, --path <path>    Specify path to SAM hive file\n";
    std::cout << "  -h, --help           Display this help message\n";
}

std::string autoDetectSAM() {
    WCHAR sysDir[MAX_PATH];
    if (GetSystemDirectory(sysDir, MAX_PATH)) {
        std::wstring ws(sysDir);
        ws += L"\\config\\SAM";
        char buf[MAX_PATH];
        WideCharToMultiByte(CP_ACP, 0, ws.c_str(), -1, buf, MAX_PATH, NULL, NULL);
        return std::string(buf);
    }
    return "";
}

int main(int argc, char* argv[]) {
    std::string samPath = "";

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if ((arg == "-p" || arg == "--path") && i + 1 < argc) {
            samPath = argv[++i];
        }
    }

    if (samPath.empty()) {
        std::cout << "[*] No path provided, attempting auto-detection...\n";
        samPath = autoDetectSAM();
    }

    if (samPath.empty()) {
        std::cerr << "[!] Error: Could not detect SAM path automatically.\n";
        return 1;
    }

    std::cout << "[*] Target SAM: " << samPath << "\n";

    Hive hive;
    if (!hive.open(samPath)) {
        std::cerr << "[!] Error: Failed to open SAM hive. (Try running as Administrator)\n";
        return 1;
    }

    SAMManager sam(&hive);
    if (!sam.loadUsers()) {
        std::cerr << "[!] Error: Failed to load users from SAM.\n";
        return 1;
    }

    const auto& users = sam.getUsers();
    std::cout << "\n" << std::left << std::setw(30) << "Username" << " | " << "RID" << "\n";
    std::cout << "-------------------------------------------" << "\n";
    for (const auto& user : users) {
        std::cout << std::left << std::setw(30) << user.username << " | 0x" << std::hex << std::uppercase << user.rid << "\n";
    }

    std::cout << "\n[*] Successfully listed " << users.size() << " users.\n";
    std::cout << "[*] Interaction menu not yet implemented in this preview.\n";

    return 0;
}
