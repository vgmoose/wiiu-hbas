/****************************************************************************
 * Copyright (C) 2015 Dimok
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
#include "MainWindow.h"
#include "dynamic_libs/os_functions.h"
#include "dynamic_libs/socket_functions.h"
#include "Application.h"
#include "utils/StringTools.h"
#include "utils/logger.h"

#include "dynamic_libs/vpad_functions.h"

int movedALittleBit = 0;
int scrolledSoFar = 0;

bool doIntroScroll;

HomebrewWindow * homebrewWindow;

MainWindow::MainWindow(int w, int h)
    : width(w)
    , height(h)
    , bgImageColor(w, h, (GX2Color){ 0, 0, 0, 0 })
    , backgroundImg2Data(Resources::GetImageData("bg.png"))
    , backgroundImg2(backgroundImg2Data)
    , splashScreenImgData(Resources::GetImageData("splash.png"))
    , splashScreen(splashScreenImgData)
{
//    bgImageColor.setImageColor((GX2Color){ 40, 40, 40, 255 }, 0);
//    bgImageColor.setImageColor((GX2Color){ 40, 40, 40, 255 }, 1);
//    bgImageColor.setImageColor((GX2Color){ 40, 40, 40, 255 }, 2);
//    bgImageColor.setImageColor((GX2Color){ 40, 40, 40, 255 }, 3);
//    append(&bgImageColor);
        
    append(&backgroundImg2);
        
    disableSplashScreenNextUpdate = false;
//    splashScreenImgData = new GuiImageData(Resources::GetImageData("splash.png");
//    splashScreen = new GuiImage(splashScreenImgData);
		                        
    append(&splashScreen);
    showingSplashScreen = true;
		
//	srand(OSGetTime());

    for(int i = 0; i < 4; i++)
    {
        std::string filename = strfmt("player%i_point.png", i+1);
        pointerImgData[i] = Resources::GetImageData(filename.c_str());
        pointerImg[i] = new GuiImage(pointerImgData[i]);
        pointerImg[i]->setScale(1.5f);
        pointerValid[i] = false;
    }
        
    homebrewWindow = new HomebrewWindow(w, h);
        
    log_printf("MainWindow done loading");
    CThread * pThread = CThread::create(asyncRefreshHomebrewApps, NULL, CThread::eAttributeAffCore1 | CThread::eAttributePinnedAff, 10);
    pThread->resumeThread();
}

void asyncRefreshHomebrewAppIcons(CThread* thread, void* args)
{
    log_printf("NEW THREAD START: Icon async refresh");
//    homebrewWindow->populateIconCache();
    log_printf("EXISTING THREAD END: Icon async refresh");
}

void asyncRefreshHomebrewApps(CThread* thread, void* args)
{
    log_printf("NEW THREAD START: Async refresh homebrew apps");
    homebrewWindow->refreshHomebrewApps();
	
	// display another loading message
	// toDO: temporary until loadLocalApps is sped up
	GuiText* loadingMsg = new GuiText("One Moment Please...", 40, glm::vec4(1, 1, 1, 1));
	homebrewWindow->append(loadingMsg);
	homebrewWindow->loadLocalApps(0);
	homebrewWindow->removeE(loadingMsg);
	homebrewWindow->displayCategories();
	
//    CThread * pThread = CThread::create(asyncRefreshHomebrewAppIcons, NULL, CThread::eAttributeAffCore1 | CThread::eAttributePinnedAff, 10);
//    pThread->resumeThread();
    log_printf("EXISTING THREAD END: Async refresh homebrew apps");
}

void globalRefreshHomebrewApps()
{
    //homebrewWindow->refreshHomebrewApps();
    homebrewWindow->loadLocalApps(0);
}



MainWindow::~MainWindow()
{
	if (homebrewWindow)
		removeE(homebrewWindow);
//    removeE(&bgImageColor);
    if (showingSplashScreen)
        removeE(&splashScreen);

//    while(!tvElements.empty())
//    {
//        delete tvElements[0];
//        removeE(tvElements[0]);
//    }
//    while(!drcElements.empty())
//    {
//        delete drcElements[0];
//        removeE(drcElements[0]);
//    }
//    for(int i = 0; i < 4; i++)
//    {
//        delete pointerImg[i];
//        Resources::RemoveImageData(pointerImgData[i]);
//    }
}

void MainWindow::updateEffects()
{
    //! dont read behind the initial elements in case one was added
    u32 tvSize = tvElements.size();
    u32 drcSize = drcElements.size();

    for(u32 i = 0; (i < drcSize) && (i < drcElements.size()); ++i)
    {
        drcElements[i]->updateEffects();
    }

    //! only update TV elements that are not updated yet because they are on DRC
    for(u32 i = 0; (i < tvSize) && (i < tvElements.size()); ++i)
    {
        u32 n;
        for(n = 0; (n < drcSize) && (n < drcElements.size()); n++)
        {
            if(tvElements[i] == drcElements[n])
                break;
        }
        if(n == drcElements.size())
        {
            tvElements[i]->updateEffects();
        }
    }
}

void MainWindow::update(GuiController *controller)
{
    //! dont read behind the initial elements in case one was added
    //u32 tvSize = tvElements.size();
    
        if(controller->chanIdx >= 1 && controller->chanIdx <= 4 && controller->data.validPointer)
    {
        int wpadIdx = controller->chanIdx - 1;
        f32 posX = controller->data.x;
        f32 posY = controller->data.y;
        pointerImg[wpadIdx]->setPosition(posX, posY);
        pointerImg[wpadIdx]->setAngle(controller->data.pointerAngle);
        pointerValid[wpadIdx] = true;
    }
	
	// minus button toggles joysticks
	if (controller->data.buttons_r & VPAD_BUTTON_MINUS)
		joysticksDisabled = !joysticksDisabled;
	
//	// get a random app
//	if (controller->data.buttons_r & VPAD_BUTTON_PLUS)
//	{
//		srand(OSGetTime());
//		int r = rand() % homebrewWindow->curTabButtons.size();
//		homebrewWindow->OnHomebrewButtonClick(homebrewWindow->curTabButtons[r]->button, controller, 0);
//	}
//    
	
	// L button forces a cache refresh
	if (showingSplashScreen && controller->data.buttons_h & VPAD_BUTTON_L)
		homebrewWindow->invalidateCache = disableSplashScreenNextUpdate = true;

	// R button completely ignores icons
	if (showingSplashScreen && controller->data.buttons_h & VPAD_BUTTON_R)
		homebrewWindow->noIconMode = disableSplashScreenNextUpdate = true;
    
    if (showingSplashScreen && controller->data.touched)
        disableSplashScreenNextUpdate = true;
    
    else if ((disableSplashScreenNextUpdate ||
        (controller->data.buttons_h & VPAD_BUTTON_A)) &&
        showingSplashScreen)
    {
        removeE(&splashScreen);
        showingSplashScreen = false;
        append(homebrewWindow);

        // perform a synchronous refresh
//        globalRefreshHomebrewApps();
        
        log_printf("update made");
        

        return;
    }
    
    // below code from Space Game https://github.com/vgmoose/space/blob/hbl_elf/src/space.c
    		
	// Handle analog stick movements
	Vec2D left =  controller->data.lstick;
	Vec2D right = controller->data.rstick;

	// get the differences
	float ydif = -20*left.y + -20*right.y;
	
	if (joysticksDisabled)
		ydif = 0;
        
	// Handle D-pad movements as well
	ydif = (ydif >  1 || controller->data.buttons_h &	VPAD_BUTTON_DOWN)?    20 : ydif;
	ydif = (ydif < -1 || controller->data.buttons_h &  VPAD_BUTTON_UP)? -20: ydif;
    
//    // do auto scrolling on first load to the top
//    if (!showingSplashScreen && doIntroScroll)
//        ydif -= 20;
    
    if (ydif != 0) {
        movedALittleBit = 10;
        scrollMenu(ydif);
    }
    
    if (controller->data.touched) {
        if (lastTouchX2 == -1)
        {
            lastTouchX2 = controller->data.y;
        }
        else
        {
            scrollMenu(controller->data.y - lastTouchX2);
            lastTouchX2 = controller->data.y;
        }
    } else {
        // on touchup, reset everything
        lastTouchX2 = -1;
        if (movedALittleBit > 0)
            movedALittleBit --;
        scrolledSoFar = 0;
    }

    if(controller->chan & GuiTrigger::CHANNEL_1)
    {
        u32 drcSize = drcElements.size();

        for(u32 i = 0; (i < drcSize) && (i < drcElements.size()); ++i)
        {
            drcElements[i]->update(controller);
        }
    }
    else
    {
        u32 tvSize = tvElements.size();

        for(u32 i = 0; (i < tvSize) && (i < tvElements.size()); ++i)
        {
            tvElements[i]->update(controller);
        }
    }

//    //! only update TV elements that are not updated yet because they are on DRC
//    for(u32 i = 0; (i < tvSize) && (i < tvElements.size()); ++i)
//    {
//        u32 n;
//        for(n = 0; (n < drcSize) && (n < drcElements.size()); n++)
//        {
//            if(tvElements[i] == drcElements[n])
//                break;
//        }
//        if(n == drcElements.size())
//        {
//            tvElements[i]->update(controller);
//        }
//    }
}

void scrollMenu(float scrol)
{
    scrolledSoFar += abs(scrol);

    if (scrolledSoFar < 70 && !movedALittleBit) // must move by at least 70
    {
        return;
    }
    
    movedALittleBit = 10; // we scrolled

    homebrewWindow->lastScrollOffY = homebrewWindow->scrollOffY;
    if (homebrewWindow->scrollOffY + scrol < -120 || homebrewWindow->scrollOffY + scrol > homebrewWindow->homebrewButtons.size()*130)
    {
//        doIntroScroll = false;
        return;
    }
    homebrewWindow->scrollOffY += scrol;
}

void MainWindow::drawDrc(CVideo *video)
{
    for(u32 i = 0; i < drcElements.size(); ++i)
    {
        drcElements[i]->draw(video);
    }

    for(int i = 0; i < 4; i++)
    {
        if(pointerValid[i])
        {
            pointerImg[i]->setAlpha(0.5f);
            pointerImg[i]->draw(video);
            pointerImg[i]->setAlpha(1.0f);
        }
    }
}

void MainWindow::drawTv(CVideo *video)
{
    for(u32 i = 0; i < tvElements.size(); ++i)
    {
        tvElements[i]->draw(video);
    }

    for(int i = 0; i < 4; i++)
    {
        if(pointerValid[i])
        {
            pointerImg[i]->draw(video);
            pointerValid[i] = false;
        }
    }
}


int getHasScrolled()
{
    return movedALittleBit;
}