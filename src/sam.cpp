#include "sam.h"
#include <sstream>
#include <iomanip>

SAMManager::SAMManager(Hive* h) : hive(h) {}

bool SAMManager::loadUsers() {
    users.clear();
    
    DWORD sam = hive->findSubkey(hive->getRootOffset(), "SAM");
    if (!sam) sam = hive->getRootOffset();
    
    DWORD domains = hive->findSubkey(sam, "Domains");
    if (!domains) return false;
    
    DWORD account = hive->findSubkey(domains, "Account");
    if (!account) return false;
    
    DWORD users_key = hive->findSubkey(account, "Users");
    if (!users_key) return false;
    
    DWORD names = hive->findSubkey(users_key, "Names");
    if (!names) return false;
    
    auto name_keys = hive->enumerateSubkeys(names);
    for (const auto& pair : name_keys) {
        UserEntry entry;
        entry.username = pair.first;
        
        DWORD type = 0;
        hive->getValueData(pair.second, "", &type);
        entry.rid = type;
        
        entry.nk_offset = findUserNK(entry.rid);
        users.push_back(entry);
    }
    
    return !users.empty();
}

bool SAMManager::unlockUser(DWORD rid) {
    DWORD user_nk = findUserNK(rid);
    if (!user_nk) return false;

    DWORD len = 0;
    void* f_data = hive->getValueData(user_nk, "F", nullptr, &len);
    if (!f_data || len < 0x40) return false;

    // ACB (Account Control Bits) is at offset 0x38 (56 bytes)
    unsigned short* acb = (unsigned short*)((char*)f_data + 0x38);
    *acb &= ~0x0001; // Clear USER_ACCOUNT_DISABLED bit

    // Find the offset of the value data in the hive
    // hive->getValueData returns a pointer to the data. 
    // We need to calculate the hive-relative offset to call writeBuffer.
    DWORD data_offset = (DWORD)((char*)f_data - (char*)hive->getOffset(0));
    
    return hive->writeBuffer(data_offset + 0x38, acb, 2);
}

bool SAMManager::resetPassword(DWORD rid, const std::wstring& newPassword) {
    DWORD user_nk = findUserNK(rid);
    if (!user_nk) return false;

    DWORD len = 0;
    void* v_data = hive->getValueData(user_nk, "V", nullptr, &len);
    if (!v_data || len < 44) return false;

    // Resetting to BLANK is easiest as we don't need to handle complex encryption offsets for now.
    // However, if newPassword is empty, we set lengths to 0.
    
    DWORD data_offset = (DWORD)((char*)v_data - (char*)hive->getOffset(0));
    DWORD zero = 0;

    // NT hash offset (0x44) and length (0x48)
    // LM hash offset (0x3C) and length (0x40)
    
    if (newPassword.empty()) {
        hive->writeBuffer(data_offset + 0x40, &zero, 4); // LM len = 0
        hive->writeBuffer(data_offset + 0x48, &zero, 4); // NT len = 0
        return true;
    }

    // For non-empty passwords, we'd need to handle encryption.
    // For now, let's implement 'Clear Password' as the primary 'Reset' feature.
    return false; 
}

DWORD SAMManager::findUserNK(DWORD rid) {
    DWORD sam = hive->findSubkey(hive->getRootOffset(), "SAM");
    if (!sam) sam = hive->getRootOffset();
    DWORD domains = hive->findSubkey(sam, "Domains");
    if (!domains) return 0;
    DWORD account = hive->findSubkey(domains, "Account");
    if (!account) return 0;
    DWORD users_key = hive->findSubkey(account, "Users");
    if (!users_key) return 0;
    char rid_hex[16];
    sprintf(rid_hex, "%08X", rid);
    return hive->findSubkey(users_key, rid_hex);
}
