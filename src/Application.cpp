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
#include "Application.h"
#include "common/common.h"
#include "resources/Resources.h"
#include <gui/system/SDLSystem.h>
#include <gui/input/SDLControllerMouse.h>
#include <gui/input/SDLControllerWiiUProContoller.h>

// #include <guiounds/SoundHandler.hpp"

Application *Application::applicationInstance = NULL;
bool Application::exitApplication = false;

Application::Application()
	// : CThread(CThread::eAttributeAffCore1 | CThread::eAttributePinnedAff, 0, 0x20000)
	: bgMusic(NULL)
	// , video(NULL)
    , mainWindow(NULL)
    , exitCode(EXIT_RELAUNCH_ON_LOAD)
{
    controller[0] = new SDLControllerMouse(GuiTrigger::CHANNEL_1);
    controller[1] = new SDLControllerWiiUProContoller(GuiTrigger::CHANNEL_2);
    controller[2] = new SDLControllerWiiUProContoller(GuiTrigger::CHANNEL_3);
    controller[3] = new SDLControllerWiiUProContoller(GuiTrigger::CHANNEL_4);
    controller[4] = new SDLControllerWiiUProContoller(GuiTrigger::CHANNEL_5);
        
    //! load resources
    // Resources::LoadFiles("sd:/wiiu/apps/appstore/resources");

    //! create bgMusic
    bgMusic = new GuiSound(Resources::GetFile(ASSET_ROOT "background.mp3"), Resources::GetFileSize(ASSET_ROOT "background.mp3"));
    bgMusic->SetLoop(true);
    bgMusic->Play();
    bgMusic->SetVolume(50);

	exitApplication = false;
}

Application::~Application()
{
    printf("Destroy music\n");

    delete bgMusic;

    printf("Destroy controller\n");

    for(int i = 0; i < 5; i++)
        delete controller[i];

    printf("Destroy async deleter\n");
	// AsyncDeleter::destroyInstance();

    printf("Clear resources\n");
    // Resources::Clear();

    printf("Stop sound handler\n");
	// SoundHandler::DestroyInstance();
}

int Application::exec()
{
    executeThread();
    //! start main GX2 thread
    // resumeThread();
    // //! now wait for thread to finish
	// shutdownThread();

	return exitCode;
}

void Application::fadeOut()
{
    // GuiImage fadeOut(video->getTvWidth(), video->getTvHeight(), (SDL_Color){ 0, 0, 0, 255 });

	// for(int i = 0; i < 255; i += 10)
    // {
    //     if(i > 255)
    //         i = 255;

    //     fadeOut.setAlpha(i / 255.0f);

    //     //! start rendering DRC
	//     video->prepareDrcRendering();
	//     mainWindow->drawDrc(video);

    //     GX2SetDepthOnlyControl(GX2_DISABLE, GX2_DISABLE, GX2_COMPARE_ALWAYS);
    //     fadeOut.draw(video);
    //     GX2SetDepthOnlyControl(GX2_ENABLE, GX2_ENABLE, GX2_COMPARE_LEQUAL);

	//     video->drcDrawDone();

    //     //! start rendering TV
	//     video->prepareTvRendering();

	//     mainWindow->drawTv(video);

    //     GX2SetDepthOnlyControl(GX2_DISABLE, GX2_DISABLE, GX2_COMPARE_ALWAYS);
    //     fadeOut.draw(video);
    //     GX2SetDepthOnlyControl(GX2_ENABLE, GX2_ENABLE, GX2_COMPARE_LEQUAL);

	//     video->tvDrawDone();

	//     //! as last point update the effects as it can drop elements
	//     mainWindow->updateEffects();

	//     video->waitForVSync();
    // }

    // //! one last cleared black screen
    // video->prepareDrcRendering();
    // video->drcDrawDone();
    // video->prepareTvRendering();
    // video->tvDrawDone();
    // video->waitForVSync();
    // video->tvEnable(false);
    // video->drcEnable(false);
}

#define sdlBlack (SDL_Color) { 0, 0, 0, 255 }

// TODO: actually measure TV on wiiu
int getTvWidth() {
    return 1280;
}

int getTvHeight() {
    return 720;
}

void Application::executeThread(void)
{
    printf("Initialize video\n");

    SDLSystem sdlSystem;
    // initElements();

    Renderer* mainRenderer = sdlSystem.getRenderer();
    video = new Renderer(mainRenderer->getRenderer(), SDL_PIXELFORMAT_RGBA8888);

    printf("Video size %i x %i\n", getTvWidth(), getTvHeight());

    //! setup default Font
    printf("Initialize main font system\n");
    GuiFont *fontSystem = new GuiFont(
        Resources::GetFile(ASSET_ROOT "font.ttf"),
        Resources::GetFileSize(ASSET_ROOT "font.ttf"),
        mainRenderer
    );
    GuiText::setPresetFont(fontSystem);
    GuiText::setPresets(28, sdlBlack);//, 0xFFFF, ALIGN_CENTER | ALIGN_MIDDLE); 

    printf("Initialize main window\n");

    mainWindow = new MainWindow(getTvWidth(), getTvHeight());

    printf("Entering main loop\n");

    //! main GX2 loop (60 Hz cycle with max priority on core 1)
    while(!exitApplication)
    {
        //! Read out inputs
        SDL_Event event;
        SDL_PollEvent(&event);

        // quit!!!
        if(event.type == SDL_QUIT) {
            exitApplication = true;
            break;
        }

        //! Read out inputs
        for(int i = 0; i < 5; i++)
        {
            if(controller[i] == NULL) continue;
            
            // if(controller[i]->update(&event, getTvWidth(), getTvHeight()))
            //     continue;
            
            if(controller[i]->data.buttons_d & GuiTrigger::BUTTON_HOME)
                exitApplication = true;
            
            //! update controller states
            mainWindow->update(controller[i]);
            
            //If the + button on the GamePad is pressed, switch to DPAD mode and vice versa.

// TODO: enable dpad mode again
//            if((i == 0) && (controller[i]->data.buttons_d & GuiTrigger::BUTTON_PLUS))
//            {
//                if(controller[i]->isDPadMode)
//                {
//                    delete controller[i];
//                    controller[i] = new VPadController(GuiTrigger::CHANNEL_1);
//                }
//                else
//                {
//                    delete controller[i];
//                    controller[i] = new DVPadController(GuiTrigger::CHANNEL_1);
//                }
//            }
//            
//            //If the + button on any other controller is pressed, switch to DPAD mode and vice versa.
//            else if(controller[i]->data.buttons_d & GuiTrigger::BUTTON_PLUS)
//            {
//                if(controller[i]->isDPadMode)
//                {
//                    delete controller[i];
//                    switch(i)
//                    {
//                        case 1:
//                            controller[i] = new WPadController(GuiTrigger::CHANNEL_2);
//                            break;
//                        case 2:
//                            controller[i] = new WPadController(GuiTrigger::CHANNEL_3);
//                            break;
//                        case 3:
//                            controller[i] = new WPadController(GuiTrigger::CHANNEL_4);
//                            break;
//                        case 4:
//                            controller[i] = new WPadController(GuiTrigger::CHANNEL_5);
//                            break;
//                    }
//                }
//                else
//                {
//                    delete controller[i];
//                    switch(i)
//                    {
//                        case 1:
//                            controller[i] = new DWPadController(GuiTrigger::CHANNEL_2);
//                            break;
//                        case 2:
//                            controller[i] = new DWPadController(GuiTrigger::CHANNEL_3);
//                            break;
//                        case 3:
//                            controller[i] = new DWPadController(GuiTrigger::CHANNEL_4);
//                            break;
//                        case 4:
//                            controller[i] = new DWPadController(GuiTrigger::CHANNEL_5);
//                            break;
//                    }
//                }
//            }
        }

        //! start rendering DRC
	    // video->prepareDrcRendering();
	    // mainWindow->drawDrc(video);
	    // video->drcDrawDone();

        // //! start rendering TV
	    // video->prepareTvRendering();
	    mainWindow->drawTv(video);
	    // video->tvDrawDone();

        //! enable screen after first frame render
	    // if(video->getFrameCount() == 0) {
        //     video->tvEnable(true);
        //     video->drcEnable(true);
	    // }

	    //! as last point update the effects as it can drop elements
	    mainWindow->updateEffects();

	    // video->waitForVSync();

        //! transfer elements to real delete list here after all processes are finished
        //! the elements are transfered to another list to delete the elements in a separate thread
        //! and avoid blocking the GUI thread
        // AsyncDeleter::triggerDeleteProcess();
	}

	fadeOut();

    delete mainWindow;
    delete fontSystem;
    delete video;
}
