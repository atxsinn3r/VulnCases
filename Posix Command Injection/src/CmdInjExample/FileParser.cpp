#include "FileParser.h"

FileParser::FileParser(const char* fname) {
    fileName = fname;
}

bool FileParser::Validate() {
    PngFileInfo* info;
    LoadFile(&info);
    bool isValid = (CheckMagic(&info) && CheckPng(&info));
    munmap(info, sizeof(PngFileInfo));
    return isValid;
}

void FileParser::LoadFile(PngFileInfo** info) {
    int f = open(fileName, O_RDONLY);
    if (!f) {
        throw std::runtime_error("Failed to open file");
    }
    *info = (PngFileInfo*) mmap(nullptr, sizeof(PngFileInfo), PROT_READ, MAP_SHARED, f, 0);
    if (*info == MAP_FAILED) {
        throw std::runtime_error("Mapping failed");
    }
    close(f);
}

bool FileParser::CheckPng(PngFileInfo** info) {
    bool isPng = false;
    char* cmd = nullptr;
    FILE* pipe = nullptr;
    std::string output;
    char outBuf[512];

    char* filePath = strndup((*info)->filePath, sizeof(PngFileInfo::filePath)-1);
    size_t len = snprintf(nullptr, 0, "file %s", filePath);
    if (len == -1) {
        goto eof;
    }
    cmd = (char*) calloc(len+1, sizeof(char));
    snprintf(cmd, len+1, "file %s", filePath);
    pipe = popen(cmd, "r");
    if (!pipe) {
        goto eof;
    }
    while (!feof(pipe)) {
        if (fgets(outBuf, sizeof(outBuf), pipe) != nullptr) {
            output += outBuf;
        }
    }
    if (strstr(output.c_str(), "PNG")) {
        isPng = true;
    }

    eof:
    free(filePath);
    pclose(pipe);
    free(cmd);
    return isPng;
}

bool FileParser::CheckMagic(PngFileInfo** info) {
    uint32_t magic = (*info)->magic;
    return (magic == MAGIC);
}
