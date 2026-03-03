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

DWORD SAMManager::findUserNK(DWORD rid) {
    DWORD sam = hive->findSubkey(hive->getRootOffset(), "SAM");
    if (!sam) sam = hive->getRootOffset();
    
    DWORD domains = hive->findSubkey(sam, "Domains");
    DWORD account = hive->findSubkey(domains, "Account");
    DWORD users_key = hive->findSubkey(account, "Users");
    
    if (!users_key) return 0;
    
    char rid_hex[16];
    sprintf(rid_hex, "%08X", rid);
    
    return hive->findSubkey(users_key, rid_hex);
}
