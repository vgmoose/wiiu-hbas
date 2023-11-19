/****************************************************************************
 * Copyright (C) 2015-2018 VGMoose, rw-r-r_0644, Creepermario, Brienj
 * Copyright (C) 2015 Dimok, Maschell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#ifndef _APPLICATION_H
#define _APPLICATION_H

#include <gui/GuiButton.h>
#include <gui/input/SDLController.h>
#include "menu/MainWindow.h"

extern char* repoUrl;

class Application
{
public:
    static Application * instance() {
        if(!applicationInstance)
            applicationInstance = new Application();
        return applicationInstance;
    }
    static void destroyInstance() {
        if(applicationInstance) {
            delete applicationInstance;
            applicationInstance = NULL;
        }
    }

    MainWindow *getMainWindow(void) const {
        return mainWindow;
    }

    GuiSound *getBgMusic(void) const {
        return bgMusic;
    }

    int exec(void);
    void fadeOut(void);

    void quit(int code) {
        exitCode = code;
        exitApplication = true;
    }

private:
    Application();
    virtual ~Application();

    static Application *applicationInstance;
    static bool exitApplication;

    void executeThread(void);

    GuiSound *bgMusic;
    Renderer *video;
    MainWindow *mainWindow;
    SDLController *controller[5];
    int exitCode;
};

#endif //_APPLICATION_H
