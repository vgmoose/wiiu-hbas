
// #include <malloc.h>
#include <string.h>
#include "Resources.h"
#include "fs/FSUtils.h"
// #include <gui/GuiImageAsync.h>
#include <gui/GuiSound.h>

int loadFileAndSize(
    const char* filename,
    uint8_t** fileData
) {
    printf("Loading file %s\n", filename);
    if(FSUtils::CheckFile(filename) <= 0) {
        printf("Failed to open file %s\n", filename);
        return 0;
    }

    uint32_t fileSize = 0;
    
    FSUtils::LoadFileToMem(filename, fileData, &fileSize);
    printf("Size: %d\n", fileSize);

    return fileSize;
}

uint8_t * Resources::GetFile(const char * filename) {
    // open filename and return the file data
    uint8_t * fileData = NULL;
    loadFileAndSize(filename, &fileData);

    return fileData;
}

uint32_t Resources::GetFileSize(const char * filename) {
    // open filename and return the file data
    uint8_t * fileData = NULL;
    return loadFileAndSize(filename, &fileData);
}

GuiTextureData * Resources::GetImageData(const char * filename) {
    return new GuiTextureData(filename);
}

GuiSound * Resources::GetSound(const char * filename) {
    return new GuiSound(filename);
}
