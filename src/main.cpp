#include <windows.h>
#include <commctrl.h>
#include "resource.h"
#include "ntreg.h"
#include "sam.h"

#pragma comment(lib, "comctl32.lib")

Hive* g_hive = nullptr;
SAMManager* g_sam = nullptr;

INT_PTR CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_INITDIALOG:
        // Set up list view columns
        {
            HWND hList = GetDlgItem(hwnd, IDC_LIST_USERS);
            ListView_SetExtendedListViewStyle(hList, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
            LVCOLUMN lvc;
            lvc.mask = LVCF_TEXT | LVCF_WIDTH;
            lvc.pszText = (LPWSTR)L"User Name";
            lvc.cx = 150;
            ListView_InsertColumn(hList, 0, &lvc);
            lvc.pszText = (LPWSTR)L"RID";
            lvc.cx = 60;
            ListView_InsertColumn(hList, 1, &lvc);
            lvc.pszText = (LPWSTR)L"Status";
            lvc.cx = 80;
            ListView_InsertColumn(hList, 2, &lvc);

            // Auto-detect SAM file
            WCHAR sysDir[MAX_PATH];
            if (GetSystemDirectory(sysDir, MAX_PATH)) {
                std::wstring samPath = sysDir;
                samPath += L"\\config\\SAM";
                SetDlgItemText(hwnd, IDC_EDIT_PATH, samPath.c_str());
                // Auto-open on startup
                PostMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_BTN_OPEN, BN_CLICKED), (LPARAM)GetDlgItem(hwnd, IDC_BTN_OPEN));
            }
        }
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_BTN_OPEN:
            {
                WCHAR path[MAX_PATH];
                GetDlgItemText(hwnd, IDC_EDIT_PATH, path, MAX_PATH);

                if (g_hive) delete g_hive;
                if (g_sam) delete g_sam;

                g_hive = new Hive();
                
                // Convert WCHAR to string for Hive::open (simplification for now)
                char aPath[MAX_PATH];
                WideCharToMultiByte(CP_ACP, 0, path, -1, aPath, MAX_PATH, NULL, NULL);

                if (g_hive->open(aPath)) {
                    g_sam = new SAMManager(g_hive);
                    if (g_sam->loadUsers()) {
                        HWND hList = GetDlgItem(hwnd, IDC_LIST_USERS);
                        ListView_DeleteAllItems(hList);
                        
                        const auto& users = g_sam->getUsers();
                        for (size_t i = 0; i < users.size(); ++i) {
                            LVITEM lvi = {0};
                            lvi.mask = LVIF_TEXT;
                            lvi.iItem = (int)i;
                            
                            // Username
                            WCHAR name[256];
                            MultiByteToWideChar(CP_ACP, 0, users[i].username.c_str(), -1, name, 256);
                            lvi.pszText = name;
                            ListView_InsertItem(hList, &lvi);
                            
                            // RID
                            WCHAR rid[16];
                            swprintf(rid, 16, L"0x%X", users[i].rid);
                            ListView_SetItemText(hList, (int)i, 1, rid);
                            
                            // Status (Simplified)
                            ListView_SetItemText(hList, (int)i, 2, (LPWSTR)L"Loaded");
                        }
                    } else {
                        MessageBox(hwnd, L"Failed to load users from SAM hive.", L"Error", MB_ICONERROR);
                    }
                } else {
                    MessageBox(hwnd, L"Failed to open SAM hive file. Make sure you have Administrator privileges.", L"Error", MB_ICONERROR);
                }
            }
            return (INT_PTR)TRUE;

        case IDC_BTN_EXIT:
            EndDialog(hwnd, IDOK);
            return (INT_PTR)TRUE;
        }
        break;

    case WM_CLOSE:
        EndDialog(hwnd, IDCANCEL);
        return (INT_PTR)TRUE;
    }
    return (INT_PTR)FALSE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    InitCommonControls();
    return (int)DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), NULL, DialogProc);
}
