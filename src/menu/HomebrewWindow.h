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

#define HBL 1
#define RPX 2

extern ProgressWindow * progressWindow;

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
        int typee;
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
    // all homebrew buttons
    std::vector<homebrewButton*> homebrewButtons;
    
    std::vector<homebrewButton*> localAppButtons;    // will be refreshed a lot
    std::vector<homebrewButton*> remoteAppButtons;   // will refreshed once
	
	bool noIconMode = false;
	bool invalidateCache = false;
    
    // separate vectors for the current tab (some combo off local and remote)
    std::vector<homebrewButton*> curTabButtons;

    bool screenLocked;
    std::string fileContents;
    bool checkLocalAppExists(std::string shortname);
    int totalLocalApps;
	int listingMode; // 1 is hbl, 2 is rpx
    bool gotDirectorySuccess;
    bool initialLoadInProgress = true;
    bool globalUpdatePosition = false;
    std::vector<std::string> cachedIcons;
    void loadLocalApps(int mode);
    void populateIconCache();
    void positionHomebrewButton(homebrewButton*, int);
    void refreshHomebrewApps();
    void addAll(std::vector<homebrewButton*>, int);
    void refreshLocalApps();
    void refreshHomebrewAppIcons();
    void filter();
    void fetchThisIcon(int x, std::string targetIconUrl);
    void findHomebrewIconAndSetImage(std::string shortname, std::string targetIcon);
    int checkIfUpdateOrInstalled(std::string name, std::string version, int totalLocalApps);
    std::string binaryTarget;
    std::string pathTarget;
    std::string sdPathTarget;
    
    void OnLaunchBoxCloseClick(GuiElement *element);
    
    // this is wrong, should pass args instead of doing this
//    GuiButton *buttonTarget;
//    const GuiController *controllerTarget;
//    GuiTrigger *triggerTarget;
    GuiFrame * launchWindowTarget;
    std::string bufString;
    
private:
    void OnOpenEffectFinish(GuiElement *element);
    void OnCloseEffectFinish(GuiElement *element);
    void OnHomebrewButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);
	
	void OnHBLTabButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);
	void OnRPXTabButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);


    void OnCloseTcpReceiverFinish(GuiElement *element);
    void OnTcpReceiveStart(GuiElement *element, u32 ip);
    void OnTcpReceiveFinish(GuiElement *element, u32 ip, int result);
    

    GuiSound *buttonClickSound;
    GuiImageData * installedButtonImgData;
    GuiImageData * getButtonImgData;
    GuiImageData * updateButtonImgData;
    GuiImageData * localButtonImgData;

    GuiText hblVersionText;
    GuiText * hblRepoText;
	
	GuiImageData *hblTabImgData;
    GuiImageData *rpxTabImgData;
	GuiImage hblTabImg;
    GuiImage rpxTabImg;
	GuiButton hblTabBtn;
    GuiButton rpxTabBtn;

    GuiTrigger touchTrigger;
    GuiTrigger wpadTouchTrigger;
    GuiTrigger buttonLTrigger;
    GuiTrigger buttonRTrigger;
    int listOffset;
    int currentLeftPosition;
    int targetLeftPosition;

};
extern HomebrewWindow* getHomebrewWindow();


#endif //_HOMEBREW_WINDOW_H_
