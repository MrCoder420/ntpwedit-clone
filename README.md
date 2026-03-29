# NTPWEdit Clone

A high-performance C++ clone of the popular NTPWEdit tool for editing Windows SAM hives offline.

## New Features (v0.7)
- **Account Unlocking**: Re-enable disabled local accounts by clearing the ACB bit.
- **Password Reset**: Clear passwords for any local account (Reset to Blank).
- **Save Integrity**: Recalculates registry checksums to ensure system compatibility.
- **Native GUI**: Win32 dialog interface with full row selection and path browsing.
- **Portable**: Single binary with no external dependencies.

## Features

- **Direct Registry Manipulation**: Operates directly on the SAM registry hive without requiring the Registry Editor.
- **Automatic SAM Detection**: Automatically detects the system's SAM file path on startup.
- **Auto-Loading**: Attempts to load the user list immediately upon launch.
- **User Enumeration**: Lists all user accounts registered in the SAM hive, including their RIDs and status.
- **Unlock/Change Password**: Architectural support for unlocking accounts and changing passwords (skeleton implemented).

## Screenshots

*(Screenshots of the application UI can be added here)*

## Installation and Building

The project is designed to be built using the MinGW toolkit.

### Prerequisites

- **MinGW-w64**: Ensure you have a MinGW installation (like the one provided with CodeBlocks).
- **Windows OS**: This tool is specific to the Windows NT family.

### Build Instructions

To build the project, run the provided batch script:

```powershell
cmd /c build.bat
```

This will:
1. Compile the Windows resource file (`ntpwedit.rc`).
2. Compile and link all C++ source files (`main.cpp`, `ntreg.cpp`, `sam.cpp`, `crypto.cpp`).
3. Generate the final executable: `ntpwedit_clone.exe`.

## Usage

1. **Run as Administrator**: Right-click `ntpwedit_clone.exe` and select "Run as administrator".
2. **Path Selection**: The application will automatically attempt to find `C:\Windows\System32\config\SAM`. 
3. **Open Hive**: Click "(re)Open" if the auto-load fails or if you want to select a different hive file.
4. **Manage Users**: Select a user from the list to see available actions (Unlock, Change Password).

> [!IMPORTANT]
> **Live System Locks**: The Windows kernel locks the SAM file while the operating system is running. To edit the *live* system's SAM file, you must run this tool from a separate environment (like WinPE, a recovery console, or another OS instance).

## Technical Implementation

- **Language**: C++17
- **UI Framework**: Vanilla Windows API (Win32) for maximum portability and zero dependencies.
- **Registry Engine**: Custom-built parser for the Windows Registry Hive format (regf/hbin/nk/vk/lf/lh records).
- **Unicode Build**: Full support for Unicode characters in usernames and paths.

## Disclaimer

This tool is for educational and administrative purposes only. Use it responsibly and only on systems you own or have explicit permission to manage. Making incorrect changes to the SAM hive can render a Windows system unbootable. Use at your own risk.

## License

MIT License - feel free to use and modify for your own projects.
