#include "IMGArchive.h"

IMGArchive::IMGArchive(std::string archiveFilePath)
{
    imgArchiveFile_ = NULL;
    archiveFilePath_ = archiveFilePath;
    openArchive(archiveFilePath);
}

IMGArchive::~IMGArchive()
{
    archiveFileEntries_.clear();
}

void IMGArchive::openArchive(std::string archiveFilePath)
{
    fopen_s(&imgArchiveFile_, &archiveFilePath[0], "rb");
    if (imgArchiveFile_ != NULL)
    {
        char ver[4];
        fread(ver, 1, 4, imgArchiveFile_);
        if (ver[0] == 'V' && ver[3] == '2')
        {
            uint entryCount;
            fread(&entryCount, sizeof(uint), 1, imgArchiveFile_);
            for (int i = 0; i < entryCount; i++)
            {
                DirEntry newEntry;
                fread(&newEntry, 1, 32, imgArchiveFile_);
                archiveFileEntries_.push_back(newEntry);
            }
        }
        fclose(imgArchiveFile_);
    }
}

uint IMGArchive::getFileCount()
{
    return archiveFileEntries_.size();
}

std::vector<DirEntry> IMGArchive::getArchiveDirEntries()
{
    return archiveFileEntries_;
}

IMGArchiveFile* IMGArchive::getFileByID(uint id)
{
    if (archiveFileEntries_.size() <= id || id < 0)
    {
        return NULL;
    }
    else
    {
        imgArchiveFile_ = NULL;
        fopen_s(&imgArchiveFile_, &archiveFilePath_[0], "rb");
        if (imgArchiveFile_ != NULL)
        {
            IMGArchiveFile* newArchiveFile = new IMGArchiveFile;
            newArchiveFile->fileEntry = &archiveFileEntries_[id];
            newArchiveFile->actualFileOffset = archiveFileEntries_[id].offset * 2048;
            newArchiveFile->actualFileSize = archiveFileEntries_[id].fSize * 2048;
            newArchiveFile->fileByteBuffer = new uchar[newArchiveFile->actualFileSize];
            fseek(imgArchiveFile_, newArchiveFile->actualFileOffset, SEEK_SET);
            fread(newArchiveFile->fileByteBuffer, 1, newArchiveFile->actualFileSize, imgArchiveFile_);
            fclose(imgArchiveFile_);
            return newArchiveFile;
        }
        return NULL;
    }
}

IMGArchiveFile* IMGArchive::getFileByName(std::string fileName)
{
    for (int i = 0; i < archiveFileEntries_.size(); i++)
    {
        if ((std::string)archiveFileEntries_[i].fileName == fileName)
        {
            imgArchiveFile_ = NULL;
            fopen_s(&imgArchiveFile_, &archiveFilePath_[0], "rb");
            if (imgArchiveFile_ != NULL)
            {
                IMGArchiveFile* newArchiveFile = new IMGArchiveFile;
                newArchiveFile->fileEntry = &archiveFileEntries_[i];
                newArchiveFile->actualFileOffset = archiveFileEntries_[i].offset * 2048;
                newArchiveFile->actualFileSize = archiveFileEntries_[i].fSize * 2048;
                newArchiveFile->fileByteBuffer = new uchar[newArchiveFile->actualFileSize];
                fseek(imgArchiveFile_, newArchiveFile->actualFileOffset, SEEK_SET);
                fread(newArchiveFile->fileByteBuffer, 1, newArchiveFile->actualFileSize, imgArchiveFile_);
                fclose(imgArchiveFile_);
                return newArchiveFile;
            }
            return NULL;
        }
    }
    return NULL;
}