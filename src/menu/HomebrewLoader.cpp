#include <algorithm>
#include <string>
#include <string.h>

#include "HomebrewLoader.h"
#include "fs/CFile.hpp"
#include "utils/logger.h"
#include "utils/StringTools.h"

HomebrewLoader * HomebrewLoader::loadToMemoryAsync(const std::string & file, unsigned char *address)
{
    HomebrewLoader * loader = new HomebrewLoader(file, address);
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
        return FILE_OPEN_FAILURE;

    u32 bytesRead = 0;
    u32 fileSize = file.size();

    progressWindow.setTitle(strfmt("Loading file %s", FullpathToFilename(filepath.c_str())));

    // Copy rpl in memory
    while(bytesRead < fileSize)
    {
        progressWindow.setProgress(100.0f * (f32)bytesRead / (f32)fileSize);

        u32 blockSize = 0x8000;
        if(blockSize > (fileSize - bytesRead))
            blockSize = fileSize - bytesRead;

        if((u32)(loadAddress + bytesRead + blockSize) > 0x01000000)
        {
            log_printf("File ist too big\n");
            return NOT_ENOUGH_MEMORY;
        }

        int ret = file.read(loadAddress + bytesRead, blockSize);
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
        log_printf("File loading not finished for file %s, finished %i of %i bytes\n", filepath.c_str(), bytesRead, fileSize);
        return FILE_READ_ERROR;
    }

    return fileSize;
}
