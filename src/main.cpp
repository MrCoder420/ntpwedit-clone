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
        case IDC_BTN_BROWSE:
            {
                OPENFILENAME ofn;
                WCHAR szFile[MAX_PATH] = {0};
                ZeroMemory(&ofn, sizeof(ofn));
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = hwnd;
                ofn.lpstrFile = szFile;
                ofn.nMaxFile = sizeof(szFile);
                ofn.lpstrFilter = L"SAM Hive\0SAM\0All Files\0*.*\0";
                ofn.nFilterIndex = 1;
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
                if (GetOpenFileName(&ofn)) {
                    SetDlgItemText(hwnd, IDC_EDIT_PATH, szFile);
                }
            }
            return (INT_PTR)TRUE;

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

        case IDC_BTN_UNLOCK:
            {
                HWND hList = GetDlgItem(hwnd, IDC_LIST_USERS);
                int idx = ListView_GetNextItem(hList, -1, LVNI_SELECTED);
                if (idx != -1 && g_sam) {
                    const auto& users = g_sam->getUsers();
                    if (g_sam->unlockUser(users[idx].rid)) {
                        ListView_SetItemText(hList, idx, 2, (LPWSTR)L"Unlocked");
                        MessageBox(hwnd, L"Account unlocked successfully. Remember to click 'Save changes'.", L"Success", MB_OK);
                    } else MessageBox(hwnd, L"Failed to unlock account.", L"Error", MB_ICONERROR);
                } else MessageBox(hwnd, L"Please select a user first.", L"Warning", MB_ICONWARNING);
            }
            return (INT_PTR)TRUE;

        case IDC_BTN_CHANGE:
            {
                HWND hList = GetDlgItem(hwnd, IDC_LIST_USERS);
                int idx = ListView_GetNextItem(hList, -1, LVNI_SELECTED);
                if (idx != -1 && g_sam) {
                    if (MessageBox(hwnd, L"Are you sure you want to CLEAR the password for this user?", L"Confirm", MB_YESNO | MB_ICONQUESTION) == IDYES) {
                        if (g_sam->resetPassword(g_sam->getUsers()[idx].rid, L"")) {
                            ListView_SetItemText(hList, idx, 2, (LPWSTR)L"Password Cleared");
                            MessageBox(hwnd, L"Password cleared. Remember to click 'Save changes'.", L"Success", MB_OK);
                        } else MessageBox(hwnd, L"Failed to clear password.", L"Error", MB_ICONERROR);
                    }
                } else MessageBox(hwnd, L"Please select a user first.", L"Warning", MB_ICONWARNING);
            }
            return (INT_PTR)TRUE;

        case IDC_BTN_SAVE:
            if (g_hive && g_hive->save()) {
                MessageBox(hwnd, L"Changes saved to hive successfully.", L"Success", MB_OK);
            } else MessageBox(hwnd, L"Failed to save changes. Make sure the file is not locked.", L"Error", MB_ICONERROR);
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
