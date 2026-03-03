#ifndef SAM_H
#define SAM_H

#include "ntreg.h"
#include <vector>
#include <string>

#pragma pack(push, 1)

// Fixed part of the user data
struct user_F {
    char unknown1[8];
    FILETIME last_login;
    char unknown2[8];
    FILETIME last_pw_change;
    FILETIME expiry;
    FILETIME lock_time;
    DWORD rid;
    DWORD unknown3;
    WORD acb_bits;
    WORD unknown4;
    WORD failed_count;
    WORD login_count;
    char unknown5[12];
};

// Offset table for V value
struct v_offset {
    DWORD start;
    DWORD len;
};

struct user_V {
    DWORD unknown1[11];
    v_offset username;
    v_offset full_name;
    v_offset comment;
    char unknown2[12];
    v_offset lm_hash;
    v_offset nt_hash;
    // ... data follows
};

#pragma pack(pop)

// Account Control Bits (ACB)
#define ACB_DISABLED   0x0001
#define ACB_PWNOTREQ   0x0002
#define ACB_AUTOLOCK   0x0010
#define ACB_PWNOEXP    0x0200

struct UserEntry {
    std::string username;
    DWORD rid;
    DWORD nk_offset;  // Offset to the user's NK record
};

class SAMManager {
public:
    SAMManager(Hive* hive);
    
    bool loadUsers();
    const std::vector<UserEntry>& getUsers() const { return users; }

    bool unlockAccount(DWORD rid);
    bool changePassword(DWORD rid, const std::string& newPassword);

private:
    Hive* hive;
    std::vector<UserEntry> users;

    DWORD findUserNK(DWORD rid);
};

#endif // SAM_H
