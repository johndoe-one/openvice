#ifndef _LIBRW_LOADERIMG_HPP_
#define _LIBRW_LOADERIMG_HPP_

#include <cstddef>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <cassert>
#include <cctype>
#include <cstring>
#include <algorithm>
#include <iostream>

//#include <rw/filesystem.hpp>

/// \brief Points to one file within the archive
class LoaderIMGFile {
public:
    uint32_t offset;
    uint32_t size;
    char name[24];
};

/**
    \class LoaderIMG
    \brief Parses the structure of GTA .IMG archives and loads the files in it
           Warning: loadToMemory() is thread-unsafe, refer to its description.
*/
class LoaderIMG {
public:
    /// Multiple versions of .IMG files
    enum Version {
        GTAIIIVC,  ///< GTA III and GTA VC archives -- only this one is
                   ///implemented
                   GTASA,
                   GTAIV
    };

    /// Construct
    LoaderIMG() = default;
    LoaderIMG(const LoaderIMG&) = delete;
    LoaderIMG(LoaderIMG&&) noexcept = default;

    /// Load the structure of the archive
    /// Omit the extension in filename so both .dir and .img are loaded when
    /// appropriate
    int load(std::string filepathDIR, std::string filepathIMG);

    /// Load a file from the archive to memory and pass a pointer to it
    /// Warning: Returns nullptr if by any reason it can't load the file
    //
    /// Warning: CURRENTLY NOT THREADSAFE!
    //           This method access/modifies m_archive_stream unconditionally,
    //           be aware of that.
    std::unique_ptr<char[]> loadToMemory(const std::string& assetname);

    /// Writes the contents of assetname to filename
    bool saveAsset(const std::string& assetname, const std::string& filename);

    /// Get the information of an asset in the examining archive
    bool findAssetInfo(const std::string& assetname, LoaderIMGFile& out);

    /// Get the information of an asset by its index
    const LoaderIMGFile& getAssetInfoByIndex(size_t index) const {
        return m_assets[index];
    }

    /// Returns the number of asset files in the archive
    std::size_t getAssetCount() const {
        return m_assets.size();
    }

    Version getVersion() const {
        return m_version;
    }

private:
    Version m_version = GTAIIIVC;  ///< Version of this IMG archive
    std::string m_archive;  ///< Path to the archive being used (no extension)
    std::ifstream m_archive_stream; ///< File stream for archive
    std::vector<LoaderIMGFile> m_assets; ///< Asset info of the archive
    
};

#endif  // LoaderIMG_h__