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
#ifndef _HOMEBREW_LAUNCHER_WINDOW_H_
#define _HOMEBREW_LAUNCHER_WINDOW_H_

#include "gui/Gui.h"
#include "gui/GuiFrame.h"
#include "menu/HomebrewWindow.h"

//typedef void (GuiFrame::*fn)(GuiElement*); 
//static fn removeETarget;

class HomebrewLaunchWindow : public GuiFrame, public sigslot::has_slots<>
{
public:
    HomebrewLaunchWindow(homebrewButton & thisButton, HomebrewWindow* window);
    virtual ~HomebrewLaunchWindow();
	

    sigslot::signal1<GuiElement *> backButtonClicked;
        void OnBackButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger)
    {
        backButtonClicked(this);
    }

private:


    void OnLoadButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);
    void OnDeleteButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);

    void OnFileLoadFinish(GuiElement *element, const std::string & filepath, int result);
    void OnOpenEffectFinish(GuiElement *element);
    void OnCloseEffectFinish(GuiElement *element);

    GuiSound *buttonClickSound;
    GuiImageData * backgroundImgData;
    GuiImage backgroundImg;

    GuiImageData *delButtonImgData;
    GuiImageData *getButtonImgData;
    GuiImageData *updateButtonImgData;
    GuiImageData *deleteButtonImgData;
    GuiImageData *reinstallButtonImgData;
    GuiImageData *closeButtonImgData;
    GuiImage iconImage;

    GuiText titleText;
    GuiText versionText;
    GuiText versionValueText;
    GuiText authorText;
    GuiText authorValueText;
    GuiText descriptionText;

    GuiImage loadImg;
    GuiImage delImg;
    GuiImage updateImg;
    GuiImage reinstallImg;
    GuiImage backImg;
    
    GuiButton loadBtn;
    GuiButton delBtn;
    GuiButton updateBtn;
    GuiButton reinstallBtn;
    GuiButton backBtn;

    GuiTrigger touchTrigger;
    GuiTrigger wpadTouchTrigger;

    homebrewButton * selectedButton;
    HomebrewWindow * homebrewWindow;
};

//static HomebrewLaunchWindow* launchWindowTarget;


#endif //_HOMEBREW_LAUNCHER_WINDOW_H_
