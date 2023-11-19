#ifndef RECOURCES_H_
#define RECOURCES_H_

#ifdef PC
#define ASSET_ROOT "./assets/"
#else
// todo: replace with wuhb assets? or romfs/ramfs
#define ASSET_ROOT "/vol/external01/wiiu/apps/appstore/assets/"
#endif

#include <map>
#include <string>
#include <stdint.h>
#include <gui/GuiTextureData.h>
#include <gui/GuiSound.h>

class Resources {
    // this class is just a wrapper around file system methods now
public:
    static uint8_t * GetFile(const char * filename);
    static uint32_t GetFileSize(const char * filename);
    static GuiTextureData * GetImageData(const char * filename);
    static GuiSound * GetSound(const char * filename);
};

#endif
