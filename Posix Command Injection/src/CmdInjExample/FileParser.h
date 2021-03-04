#ifndef CMDINJEXAMPLE_FILEPARSER_H
#define CMDINJEXAMPLE_FILEPARSER_H

#include <iostream>
#include <cstdint>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <limits>

#define MAGIC 0xDEADBEEF

typedef struct PngFileInfo {
    uint32_t magic;
    char filePath[PATH_MAX];
} PngFileInfo_;

class FileParser {
public:
    explicit FileParser(const char* fname);
    bool Validate();
private:
    const char* fileName;
    void LoadFile(PngFileInfo** info);
    bool CheckPng(PngFileInfo** info);
    bool CheckMagic(PngFileInfo** info);
};

#endif //CMDINJEXAMPLE_FILEPARSER_H
