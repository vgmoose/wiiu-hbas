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
#ifndef _HOMEBREW_WINDOW_H_
#define _HOMEBREW_WINDOW_H_

#include "gui/Gui.h"
#include "gui/GuiFrame.h"
#include "TcpReceiver.h"

class HomebrewWindow : public GuiFrame, public sigslot::has_slots<>
{
public:
    HomebrewWindow(int w, int h);
    virtual ~HomebrewWindow();

    void draw(CVideo *pVideo);

private:
    void OnOpenEffectFinish(GuiElement *element);
    void OnCloseEffectFinish(GuiElement *element);
    void OnLaunchBoxCloseClick(GuiElement *element);
    void OnHomebrewButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);
    void OnLeftArrowClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);
    void OnRightArrowClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);

    void OnCloseTcpReceiverFinish(GuiElement *element);
    void OnTcpReceiveStart(GuiElement *element, u32 ip);
    void OnTcpReceiveFinish(GuiElement *element, u32 ip, int result);

    GuiSound *buttonClickSound;
    GuiImageData * homebrewButtonImgData;

    GuiImageData* arrowRightImageData;
    GuiImageData* arrowLeftImageData;
    GuiImage arrowRightImage;
    GuiImage arrowLeftImage;
    GuiButton arrowRightButton;
    GuiButton arrowLeftButton;
    GuiText hblVersionText;

    typedef struct
    {
        std::string execPath;
        GuiImage *image;
        GuiButton *button;
        GuiText *nameLabel;
        GuiText *descriptionLabel;
        GuiImageData *iconImgData;
        GuiImage *iconImg;
    } homebrewButton;

    std::vector<homebrewButton> homebrewButtons;
    GuiTrigger touchTrigger;
    GuiTrigger wpadTouchTrigger;
    GuiTrigger buttonLTrigger;
    GuiTrigger buttonRTrigger;
    int listOffset;
    int currentLeftPosition;
    int targetLeftPosition;

    TcpReceiver tcpReceiver;
};

#endif //_HOMEBREW_WINDOW_H_
