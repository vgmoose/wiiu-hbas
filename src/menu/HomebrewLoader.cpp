#include <algorithm>
#include <string>
#include <string.h>

#include "HomebrewLoader.h"
#include "HomebrewMemory.h"
#include "fs/CFile.hpp"
#include "utils/logger.h"
#include "utils/StringTools.h"

HomebrewLoader * HomebrewLoader::loadToMemoryAsync(const std::string & file)
{
    HomebrewLoader * loader = new HomebrewLoader(file);
    loader->resumeThread();
    return loader;
}

void HomebrewLoader::executeThread()
{
    int result = loadToMemory();
    asyncLoadFinished(this, filepath, result);
}

int HomebrewLoader::loadToMemory()
{
    if(filepath.empty())
        return INVALID_INPUT;

    log_printf("Loading file %s\n", filepath.c_str());

    CFile file(filepath, CFile::ReadOnly);
    if(!file.isOpen())
    {
        progressWindow.setTitle(strfmt("Failed to open file %s", FullpathToFilename(filepath.c_str())));
        sleep(1);
        return FILE_OPEN_FAILURE;
    }

    u32 bytesRead = 0;
    u32 fileSize = file.size();

    progressWindow.setTitle(strfmt("Loading file %s", FullpathToFilename(filepath.c_str())));

    unsigned char *buffer = (unsigned char*) memalign(0x40, (fileSize + 0x3F) & ~0x3F);
    if(!buffer)
    {
        progressWindow.setTitle("Not enough memory");
        sleep(1);
        return NOT_ENOUGH_MEMORY;
    }

    // Copy rpl in memory
    while(bytesRead < fileSize)
    {
        progressWindow.setProgress(100.0f * (f32)bytesRead / (f32)fileSize);

        u32 blockSize = 0x8000;
        if(blockSize > (fileSize - bytesRead))
            blockSize = fileSize - bytesRead;

        int ret = file.read(buffer + bytesRead, blockSize);
        if(ret <= 0)
        {
            log_printf("Failure on reading file %s\n", filepath.c_str());
            break;
        }

        bytesRead += ret;
    }

    progressWindow.setProgress((f32)bytesRead / (f32)fileSize);

    if(bytesRead != fileSize)
    {
        free(buffer);
        log_printf("File loading not finished for file %s, finished %i of %i bytes\n", filepath.c_str(), bytesRead, fileSize);
        progressWindow.setTitle("File read failure");
        sleep(1);
        return FILE_READ_ERROR;
    }

    HomebrewInitMemory();

    int ret = HomebrewCopyMemory(buffer, bytesRead);

    free(buffer);

    if(ret < 0)
    {
        progressWindow.setTitle("Not enough memory");
        sleep(1);
        return NOT_ENOUGH_MEMORY;
    }

    return fileSize;
}
