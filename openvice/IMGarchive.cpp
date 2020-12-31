#include "IMGarchive.h"

#include <cassert>
#include <cctype>
#include <cstring>
#include <algorithm>

//#include <rw/debug.hpp>

namespace {

    constexpr size_t kAssetRecordSize{ 2048 };

    void to_lowercase_inplace(char* name) {
        size_t len = std::strlen(name);

        std::transform(
            name, name + len, name,
            [](char ch) -> char { return std::tolower(ch); }
        );
    } // namespace

}

#include <iostream>

bool LoaderIMG::load(std::string filepathDIR, std::string filepathIMG) {
    assert(m_archive.empty());
    m_archive = filepathIMG;

    auto dirPath = filepathDIR;
    //dirPath.replace_extension(".dir");

    std::ifstream file(filepathDIR, std::ios::binary);
    if (!file.is_open()) {
        //RW_ERROR("Failed to open " + dirPath.string());
        return false;
    }

    file.seekg(0, std::ios::end);
    auto fileSize = file.tellg();
    file.seekg(0);

    std::size_t expectedCount = fileSize / sizeof(LoaderIMGFile);
    m_assets.resize(expectedCount);

    // copy to m_assets data from file
    file.read(reinterpret_cast<char*>(m_assets.data()), expectedCount * sizeof(LoaderIMGFile));

    if (file.fail() || file.gcount() != fileSize) {
        m_assets.resize(file.gcount() / sizeof(LoaderIMGFile));
        //exit(-1);
        //return -1;
        //RW_ERROR("Error reading records in IMG archive");
    }

    for (auto& asset : m_assets) {
        to_lowercase_inplace(asset.name);
        //std::cout << asset.name << std::endl;
    }

    //imgPath.replace_extension(".img");

    m_archive_stream.open(filepathIMG, std::ios::binary);
    if (!m_archive_stream.is_open()) {
        //RW_ERROR("Failed to open " << imgPath.string());
    }

    return true;
}


std::string convertToString(const char* a, int size)
{
    int i;
    std::string s = "";
    for (i = 0; i < size; i++) {
        s = s + a[i];
    }
    return s;
}

/// Get the information of a asset in the examining archive
bool LoaderIMG::findAssetInfo(const std::string& assetname,
    LoaderIMGFile& out) {

    /*for (int i = 0; i < m_assets.size(); i++)
    {
        if ((std::string)m_assets[i].name == assetname)
        {
            std::cout << "FINDED !!!!" << std::endl;

            printf("%s \n", m_assets[i].name);

            return true;
        }
        else {
            printf("%s \n", m_assets[i].name);
        }
    }

    return false;*/


    for (const auto& asset : m_assets) {
       
       /* int a_size = sizeof(asset.name) / sizeof(char);
        std::string s_a = convertToString(asset.name, a_size);
        std::cout << s_a << std::endl;*/

        if (assetname.compare(asset.name) == 0) {
            out = asset;
            return true;
        }
    }

    return false;
}

std::unique_ptr<char[]> LoaderIMG::loadToMemory(const std::string& assetname) {
    if (!m_archive_stream.is_open()) {
        return nullptr;
    }

    LoaderIMGFile assetInfo;
    bool found = findAssetInfo(assetname, assetInfo);

    if (!found) {
        //RW_ERROR("Asset '" << assetname << "' not found!");
        return nullptr;
    }

    std::streamsize asset_size = assetInfo.size * kAssetRecordSize;
    auto raw_data = std::make_unique<char[]>(asset_size);
    m_archive_stream.seekg(assetInfo.offset * kAssetRecordSize);
    m_archive_stream.read(raw_data.get(), asset_size);

    if (m_archive_stream.gcount() != asset_size) {
        //RW_ERROR("Error reading asset " << assetInfo.name);
    }

    return raw_data;
}

/// Writes the contents of assetname to filename
bool LoaderIMG::saveAsset(const std::string& assetname,
    const std::string& filename) {
    auto raw_data = loadToMemory(assetname);
    if (!raw_data) {
        return false;
    }

    LoaderIMGFile asset;
    if (!findAssetInfo(assetname, asset)) {
        return false;
    }

    std::ofstream dump_file(filename, std::ios::binary);
    if (!dump_file.is_open()) {
        return false;
    }

    dump_file.write(raw_data.get(), kAssetRecordSize * asset.size);
    //RW_MESSAGE("Saved " << assetname << " to disk with filename " << filename);

    return true;
}
