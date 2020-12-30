#ifndef IMGArchive_H
#define IMGArchive_H

// https://gtamods.com/wiki/IMG_archive#Libraries

#include <iostream>
#include <string>
#include <vector>

typedef unsigned char		                uchar;
typedef unsigned int		                uint;
typedef unsigned short		                ushort;
typedef unsigned long long	                uint64;

struct DirEntry
{
    uint					offset;
    ushort					fSize;
    ushort					fSize2;
    char					fileName[24];
};

struct IMGArchiveFile
{
    DirEntry* fileEntry;
    uint64					actualFileOffset;
    uint64					actualFileSize;
    uchar* fileByteBuffer;
};

class IMGArchive
{
public:
    IMGArchive(std::string archiveFilePath);
    ~IMGArchive();

    uint					getFileCount();
    IMGArchiveFile* getFileByID(uint id);
    IMGArchiveFile* getFileByName(std::string fileName);
    std::vector<DirEntry>	                getArchiveDirEntries();
private:
    void					openArchive(std::string archiveFilePath);
    FILE* imgArchiveFile_;
    std::string				archiveFilePath_;
    std::vector<DirEntry>	                archiveFileEntries_;
};
#endif // IMGArchive_H