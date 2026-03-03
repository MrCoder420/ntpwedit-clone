#include "ntreg.h"
#include <fstream>
#include <iostream>

Hive::Hive() : is_dirty(false) {
    memset(&header, 0, sizeof(header));
}

Hive::~Hive() {
    close();
}

bool Hive::open(const std::string& filename) {
    filepath = filename;
    std::ifstream fs(filename, std::ios::binary | std::ios::ate);
    if (!fs.is_open()) return false;

    std::streamsize size = fs.tellg();
    if (size < REG_HEADER_SIZE) return false;

    fs.seekg(0, std::ios::beg);
    fs.read((char*)&header, sizeof(header));

    if (strncmp(header.id, "regf", 4) != 0) return false;

    data.resize((size_t)size - REG_HEADER_SIZE);
    fs.read(data.data(), data.size());
    
    return true;
}

bool Hive::save() {
    if (!is_dirty) return true;

    header.checksum = calculateChecksum();

    std::ofstream fs(filepath, std::ios::binary);
    if (!fs.is_open()) return false;

    fs.write((char*)&header, sizeof(header));
    fs.write(data.data(), data.size());
    
    is_dirty = false;
    return true;
}

void Hive::close() {
    if (is_dirty) save();
    data.clear();
    memset(&header, 0, sizeof(header));
}

void* Hive::getOffset(DWORD offset) {
    if (offset >= data.size()) return nullptr;
    return &data[offset + 4]; // Skip the 4-byte block size
}

bool Hive::writeBuffer(DWORD offset, void* buf, DWORD len) {
    if (offset + len > data.size()) return false;
    memcpy(&data[offset], buf, len);
    is_dirty = true;
    return true;
}

DWORD Hive::findSubkey(DWORD parent_nk_offset, const std::string& name) {
    auto subkeys = enumerateSubkeys(parent_nk_offset);
    for (const auto& pair : subkeys) {
        if (_stricmp(pair.first.c_str(), name.c_str()) == 0) return pair.second;
    }
    return 0;
}

std::vector<std::pair<std::string, DWORD>> Hive::enumerateSubkeys(DWORD parent_nk_offset) {
    std::vector<std::pair<std::string, DWORD>> result;
    nk_record* nk = (nk_record*)getOffset(parent_nk_offset);
    if (!nk || strncmp(nk->id, "nk", 2) != 0) return result;
    if (nk->subkey_count == 0) return result;

    lx_record* lx = (lx_record*)getOffset(nk->subkey_list_offset);
    if (!lx) return result;

    if (strncmp(lx->id, "lf", 2) == 0 || strncmp(lx->id, "lh", 2) == 0) {
        for (int i = 0; i < lx->count; i++) {
            nk_record* sub_nk = (nk_record*)getOffset(lx->entries[i].offset);
            if (sub_nk && strncmp(sub_nk->id, "nk", 2) == 0) {
                std::string sub_name(sub_nk->name, sub_nk->name_len);
                result.push_back({sub_name, lx->entries[i].offset});
            }
        }
    } else if (strncmp(lx->id, "ri", 2) == 0 || strncmp(lx->id, "li", 2) == 0) {
        // Indirect list
        for (int i = 0; i < lx->count; i++) {
            lx_record* sub_lx = (lx_record*)getOffset(lx->entries[i].offset);
            if (sub_lx && (strncmp(sub_lx->id, "lf", 2) == 0 || strncmp(sub_lx->id, "lh", 2) == 0)) {
                for (int j = 0; j < sub_lx->count; j++) {
                    nk_record* sub_nk = (nk_record*)getOffset(sub_lx->entries[j].offset);
                    if (sub_nk && strncmp(sub_nk->id, "nk", 2) == 0) {
                        std::string sub_name(sub_nk->name, sub_nk->name_len);
                        result.push_back({sub_name, sub_lx->entries[j].offset});
                    }
                }
            }
        }
    }
    return result;
}

void* Hive::getValueData(DWORD nk_offset, const std::string& value_name, DWORD* out_type, DWORD* out_len) {
    nk_record* nk = (nk_record*)getOffset(nk_offset);
    if (!nk || strncmp(nk->id, "nk", 2) != 0 || nk->value_count == 0) return nullptr;

    DWORD* val_offsets = (DWORD*)getOffset(nk->value_list_offset);
    if (!val_offsets) return nullptr;

    for (DWORD i = 0; i < nk->value_count; i++) {
        vk_record* vk = (vk_record*)getOffset(val_offsets[i]);
        if (!vk || strncmp(vk->id, "vk", 2) != 0) continue;

        std::string v_name;
        if (vk->name_len > 0) {
            v_name.assign(vk->name, vk->name_len);
        }

        if (v_name == value_name || (value_name.empty() && vk->name_len == 0)) {
            if (out_type) *out_type = vk->type;
            if (out_len) *out_len = vk->data_len;
            
            if (vk->data_len & 0x80000000) {
                // Inline data
                return &vk->data_offset;
            }
            return getOffset(vk->data_offset);
        }
    }
    return nullptr;
}

DWORD Hive::calculateChecksum() {
    DWORD sum = 0;
    DWORD* p = (DWORD*)&header;
    for (int i = 0; i < 127; i++) {
        sum ^= p[i];
    }
    return sum;
}
