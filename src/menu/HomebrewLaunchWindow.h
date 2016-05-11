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

class HomebrewLaunchWindow : public GuiFrame, public sigslot::has_slots<>
{
public:
    HomebrewLaunchWindow(const std::string & launchPath, GuiImageData * iconImgData);
    virtual ~HomebrewLaunchWindow();

    sigslot::signal1<GuiElement *> backButtonClicked;
private:
    void OnBackButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger)
    {
        backButtonClicked(this);
    }

    void OnLoadButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);

    void OnFileLoadFinish(GuiElement *element, const std::string & filepath, int result);
    void OnOpenEffectFinish(GuiElement *element);
    void OnCloseEffectFinish(GuiElement *element);

    GuiSound *buttonClickSound;
    GuiImageData * backgroundImgData;
    GuiImage backgroundImg;

    GuiImageData *buttonImgData;
    GuiImage iconImage;

    GuiText titleText;
    GuiText versionText;
    GuiText versionValueText;
    GuiText authorText;
    GuiText authorValueText;
    GuiText descriptionText;

    GuiText loadBtnLabel;
    GuiImage loadImg;
    GuiButton loadBtn;

    GuiText backBtnLabel;
    GuiImage backImg;
    GuiButton backBtn;

    GuiTrigger touchTrigger;
    GuiTrigger wpadTouchTrigger;

    const std::string homebrewLaunchPath;
};

#endif //_HOMEBREW_LAUNCHER_WINDOW_H_
