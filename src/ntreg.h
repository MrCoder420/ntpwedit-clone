#ifndef NTREG_H
#define NTREG_H

#include <windows.h>
#include <vector>
#include <string>

#define REG_HEADER_SIZE 4096

#pragma pack(push, 1)

struct regf_header {
    char id[4];         // "regf"
    DWORD seq1;
    DWORD seq2;
    FILETIME mtime;
    DWORD ver_major;
    DWORD ver_minor;
    DWORD type;
    DWORD unknown1;
    DWORD root_key_offset;
    DWORD size;
    DWORD unknown2;
    char name[64];
    char unknown3[396];
    DWORD checksum;
};

struct hbin_header {
    char id[4];         // "hbin"
    DWORD offset_from_first;
    DWORD size;
    DWORD unknown1;
    DWORD unknown2;
    FILETIME mtime;
    DWORD unknown3;
};

struct nk_record {
    char id[2];         // "nk"
    WORD type;
    FILETIME mtime;
    DWORD unknown1;
    DWORD parent_offset;
    DWORD subkey_count;
    DWORD unknown2;
    DWORD subkey_list_offset;
    DWORD unknown3;
    DWORD value_count;
    DWORD value_list_offset;
    DWORD sk_offset;
    DWORD class_offset;
    WORD max_name_len;
    WORD max_class_len;
    WORD max_val_name_len;
    WORD max_val_data_len;
    DWORD unknown4;
    WORD name_len;
    WORD class_len;
    char name[1];       // Variable length
};

struct vk_record {
    char id[2];         // "vk"
    WORD name_len;
    DWORD data_len;
    DWORD data_offset;
    DWORD type;
    WORD flags;
    WORD unknown1;
    char name[1];       // Variable length
};

struct subkey_entry {
    DWORD offset;
    char hash[4];
};

struct lx_record {
    char id[2];         // "lf", "lh", "li", "ri"
    WORD count;
    subkey_entry entries[1];
};

#pragma pack(pop)

class Hive {
public:
    Hive();
    ~Hive();

    bool open(const std::string& filename);
    bool save();
    void close();

    DWORD getRootOffset() const { return header.root_key_offset; }
    
    // Low level access
    void* getOffset(DWORD offset);
    bool writeBuffer(DWORD offset, void* buf, DWORD len);

    // Traversal
    DWORD findSubkey(DWORD parent_nk_offset, const std::string& name);
    std::vector<std::pair<std::string, DWORD>> enumerateSubkeys(DWORD parent_nk_offset);
    void* getValueData(DWORD nk_offset, const std::string& value_name, DWORD* out_type = nullptr, DWORD* out_len = nullptr);

private:
    std::string filepath;
    regf_header header;
    std::vector<char> data;
    bool is_dirty;

    DWORD calculateChecksum();
};

#endif // NTREG_H
