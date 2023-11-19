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

#include <gui/GuiFrame.h>
#include <gui/GuiButton.h>
#include <gui/GuiText.h>
#include <gui/GuiImage.h>
#include <gui/GuiTrigger.h>
#include <gui/GuiSound.h>
#include <gui/GuiTextureData.h>
#include <gui/GuiController.h>

#include "menu/ProgressWindow.h"

#include "../libs/get/src/Get.hpp"

#define LOCAL 0
#define UPDATE 1
#define INSTALLED 2
#define GET 3

#define HBL 1
#define RPX 2

extern ProgressWindow * progressWindow;

typedef struct
{
    // for legacy apps uninstallation
    std::string execPath;
    int status;
    std::string shortname;
    std::string binary;
    std::string category; // string version
    std::string version;
    std::string dirPath;

    // properties to be displayed/drawn
    GuiImage *image;
    GuiButton *button;
    GuiText *nameLabel;
    GuiText *versionLabel;
    GuiText *coderLabel;
    GuiText *descriptionLabel;
    GuiTextureData *iconImgData;
    GuiImage *iconImg;
    
    // the actual package this button represents
    Package* package;
} homebrewButton;

extern void updateProgress(void *arg, uint32_t done, uint32_t total);
extern ProgressWindow* getProgressWindow();

class HomebrewWindow : public GuiFrame, public sigslot::has_slots<>
{
public:
    HomebrewWindow(int w, int h);
    virtual ~HomebrewWindow();
    static void do_download(void *thread, void *arg);
    void draw(Renderer *pVideo);
    float scrollOffY = 0;
    float lastScrollOffY = 0;
    // all homebrew buttons
    std::vector<homebrewButton*> homebrewButtons;
    GuiFrame * launchBox;
    
    Get* get = NULL;
    
    GuiSound *buttonClickSound;
    GuiTextureData * installedButtonImgData;
    GuiTextureData * getButtonImgData;
    GuiTextureData * updateButtonImgData;
    GuiTextureData * localButtonImgData;
    
    GuiText hblVersionText;
    GuiText * hblRepoText;
        
    GuiTextureData *hblTabImgData;
    GuiTextureData *rpxTabImgData;
    GuiImage hblTabImg;
    GuiImage rpxTabImg;

    
    GuiButton backTabBtn;
    GuiButton randomTabBtn;
    
    GuiImage* header;
    GuiText* header2;
    std::vector<homebrewButton*> localAppButtons;    // will be refreshed a lot
    std::vector<homebrewButton*> remoteAppButtons;   // will refreshed once
    
    std::vector<GuiButton*> all_cats;
    
    bool noIconMode = true;
    bool invalidateCache = false;
    bool isFiltering = false;
    
    // separate vectors for the current tab (some combo off local and remote)
    std::vector<homebrewButton*> curTabButtons;

    bool screenLocked;
    std::string fileContents;
    bool checkLocalAppExists(std::string shortname);
    int totalLocalApps;
    int listingMode = -1; // 1 is hbl, 2 is rpx
    bool gotDirectorySuccess;
    bool initialLoadInProgress = true;
    bool globalUpdatePosition = false;
    std::vector<std::string> cachedIcons;
    void clearScreen();
    void displayCategories();
    void loadLocalApps(int mode);
    void positionHomebrewButton(homebrewButton*, int);
    void refreshHomebrewApps();
    void addAll(std::vector<homebrewButton*>, int);
    void refreshLocalApps();
    void refreshHomebrewAppIcons();
    void appendCategoryButton(char* name, int x, int y);
    void filter();
    void fetchThisIcon(int x, std::string targetIconUrl);
    std::string appShortName;
    std::string appBinary;
    
    void OnLaunchBoxCloseClick(GuiElement *element);
    
    // this is wrong, should pass args instead of doing this
//    GuiButton *buttonTarget;
//    const GuiController *controllerTarget;
//    GuiTrigger *triggerTarget;
    GuiFrame * launchWindowTarget;
    std::string bufString;
    void OnHomebrewButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);

    bool internet_connection = true;
    
private:
    void OnOpenEffectFinish(GuiElement *element);
    void OnCloseEffectFinish(GuiElement *element);
    
    void OnHBLTabButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);
    void OnRPXTabButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);
    
    void OnCategorySwitch(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);


    void OnCloseTcpReceiverFinish(GuiElement *element);
    void OnTcpReceiveStart(GuiElement *element, uint32_t ip);
    void OnTcpReceiveFinish(GuiElement *element, uint32_t ip, int result);
    



    GuiTrigger touchTrigger;
    GuiTrigger wpadTouchTrigger;
    int listOffset;
    int currentLeftPosition;
    int targetLeftPosition;

};
extern HomebrewWindow* getHomebrewWindow();


#endif //_HOMEBREW_WINDOW_H_
