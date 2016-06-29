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
#include "menu/ProgressWindow.h"

#define LOCAL 0
#define UPDATE 1
#define INSTALLED 2
#define GET 3

static ProgressWindow * progressWindow;

typedef struct
    {
        std::string execPath;
        GuiImage *image;
        GuiButton *button;
        GuiText *nameLabel;
        GuiText *versionLabel;
        GuiText *coderLabel;
        GuiText *descriptionLabel;
        GuiImageData *iconImgData;
        GuiImage *iconImg;
        std::string shortname;
        int status;
        std::string dirPath;
        std::string binary;
        std::string version;
    } homebrewButton;

extern void updateProgress(void *arg, u32 done, u32 total);
extern ProgressWindow* getProgressWindow();
class HomebrewWindow : public GuiFrame, public sigslot::has_slots<>
{
public:
    HomebrewWindow(int w, int h);
    virtual ~HomebrewWindow();
	static void do_download(CThread *thread, void *arg);
    void draw(CVideo *pVideo);
    float scrollOffY = 0;
    float lastScrollOffY = 0;
    std::vector<homebrewButton> homebrewButtons;
    std::vector<homebrewButton> localAppButtons;    // will be refreshed a lot
    std::vector<homebrewButton> remoteAppButtons;   // will refreshed once
    std::string fileContents;
    int totalLocalApps;
    bool gotDirectorySuccess;
    std::vector<std::string> cachedIcons;
    void populateIconCache();
    void positionHomebrewButton(homebrewButton*, int);
    void refreshHomebrewApps();
    void refreshLocalApps();
    void refreshHomebrewAppIcons();
    void findHomebrewIconAndSetImage(std::string shortname, std::string targetIcon);
    int checkIfUpdateOrInstalled(std::string name, std::string version, int totalLocalApps);
    
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
    GuiImageData * installedButtonImgData;
    GuiImageData * getButtonImgData;
    GuiImageData * updateButtonImgData;
    GuiImageData * localButtonImgData;

    GuiImageData* arrowRightImageData;
    GuiImageData* arrowLeftImageData;
    GuiImage arrowRightImage;
    GuiImage arrowLeftImage;
    GuiButton arrowRightButton;
    GuiButton arrowLeftButton;
    GuiText hblVersionText;

    GuiTrigger touchTrigger;
    GuiTrigger wpadTouchTrigger;
    GuiTrigger buttonLTrigger;
    GuiTrigger buttonRTrigger;
    int listOffset;
    int currentLeftPosition;
    int targetLeftPosition;

};
static HomebrewWindow* thisHomebrewWindow;
extern HomebrewWindow* getHomebrewWindow();


#endif //_HOMEBREW_WINDOW_H_
