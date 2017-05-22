#ifndef HOMEBREW_LOADER_H_
#define HOMEBREW_LOADER_H_

#include <vector>
#include <string>
#include <gctypes.h>

#include "ProgressWindow.h"
#include "system/CThread.h"
#include "gui/sigslot.h"

class HomebrewLoader : public GuiFrame, public CThread
{
public:
    enum eLoadResults
    {
        SUCCESS = 0,
        INVALID_INPUT = -1,
        FILE_OPEN_FAILURE = -2,
        FILE_READ_ERROR = -3,
        NOT_ENOUGH_MEMORY = -4,
    };


    static HomebrewLoader * loadToMemoryAsync(const std::string & filepath);
    sigslot::signal3<GuiElement *, const std::string &, int> asyncLoadFinished;
private:

    HomebrewLoader(const std::string & file)
        : GuiFrame(0, 0)
        , CThread(CThread::eAttributeAffCore0 | CThread::eAttributePinnedAff)
        , filepath(file)
        , progressWindow("Loading file...")
	{
	    append(&progressWindow);

	    width = progressWindow.getWidth();
	    height = progressWindow.getHeight();
	}
    void executeThread();

    int loadToMemory();

    static void loadCallback(CThread *thread, void *arg);

    const std::string filepath;
    ProgressWindow progressWindow;
};


#endif
