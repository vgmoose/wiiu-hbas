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
#include "HomebrewWindow.h"
#include "common/common.h"
#include "Application.h"
#include "fs/DirList.h"
#include "fs/fs_utils.h"
#include "system/AsyncDeleter.h"
#include "utils/HomebrewXML.h"
#include "HomebrewLaunchWindow.h"
#include "network/FileDownloader.h"
#include "network/ConnectionTest.h"
#include <thread>
#include <sstream>


#define DEFAULT_WIILOAD_PORT        4299

#define MAX_BUTTONS_ON_PAGE     4
char * repoUrl = "http://wiiubru.com/appstore";
//char * repoUrl = "192.168.1.104:8000";
//char * repoUrl = "http://wiiubru.com/appstore/appstoretest";

ProgressWindow* progressWindow;
static HomebrewWindow* thisHomebrewWindow;

void HomebrewWindow::positionHomebrewButton(homebrewButton* button, int index)
{
    log_printf("going into this big function");
    const float cfImageScale = 0.8f;
    
    if (button->iconImgData)
    {
        button->iconImg = new GuiImage(button->iconImgData);
        button->iconImg->setAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
        button->iconImg->setPosition(60, 0);
        button->iconImg->setScale(cfImageScale);
    }

    if (button->nameLabel)
    {
        button->nameLabel->setAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
        button->nameLabel->setMaxWidth(350, GuiText::SCROLL_HORIZONTAL);
        button->nameLabel->setPosition(0, 70);
    }
    
    if (button->coderLabel)
    {
        button->coderLabel->setAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
        button->coderLabel->setMaxWidth(160, GuiText::SCROLL_HORIZONTAL);
        button->coderLabel->setPosition(300, 20);
        log_printf("did coder things");
    }
    
    if (button->versionLabel)
    {
        button->versionLabel->setAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
        button->versionLabel->setMaxWidth(160, GuiText::SCROLL_HORIZONTAL);
        button->versionLabel->setPosition(300, -15);
        log_printf("did version things");
    }
    
    if (button->descriptionLabel)
    {
        button->descriptionLabel->setAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
        button->descriptionLabel->setMaxWidth(350, GuiText::SCROLL_HORIZONTAL);
        button->descriptionLabel->setPosition(0, -60);
        log_printf("did description things");
    }

    if (button->image)
    {
        // set the right image for the status
        button->image->setScale(0.9);
        log_printf("did image scale things");
    }

    if (button->button)
    {
        button->button->setImage(button->image);
        log_printf("did another image things");
        button->button->setLabel(button->nameLabel, 0);
        button->button->setLabel(button->descriptionLabel, 1);
        button->button->setLabel(button->coderLabel, 2);
        button->button->setLabel(button->versionLabel, 3);
        button->button->setIcon(button->iconImg);
        button->button->setTrigger(&touchTrigger);
        button->button->setTrigger(&wpadTouchTrigger);
        button->button->setEffectGrow();
        log_printf("did button things");
    }
//    button->button->setPosition(0, 0);
    //        button->button->setSoundClick(buttonClickSound);
}

ProgressWindow* getProgressWindow()
{
    return progressWindow;
}

void updateProgress(void *arg, u32 done, u32 total)
{
    progressWindow->setProgress(100.0f* (((f32)done)/((f32)total)));
}

/**
This method filters the visible apps in in the current tab based on the display mode.

It goes throw the list of all apps (homebrewButtons) and puts only the relevant ones into
the curTabButtons list, which is what's actually rendered.
**/
void HomebrewWindow::filter()
{
    if (listingMode < 0)
        return;
    
    isFiltering = true;
    scrollOffY = -120;
    
    homebrewButtons.clear();
    
//    std::string output = "CURRENT: ";
//    for (u32 x=0; x<curTabButtons.size(); x++)
//    {
//        output += "["+std::string(curTabButtons[x]->shortname)+"] ";
//    }
        
//    std::random_shuffle(curTabButtons.begin(),curTabButtons.end());
    

    for (u32 x=0; x<curTabButtons.size(); x++)
    {
        // if it's 0, it's all apps
        if (listingMode > 0)
        {
            // do category sorting
            if (listingMode == 1 && curTabButtons[x]->category != "game")
                continue;
            if (listingMode == 2 && curTabButtons[x]->category != "emu")
                continue;
            if (listingMode == 3 && curTabButtons[x]->category != "tool")
                continue;
            if (listingMode == 4 && curTabButtons[x]->category != "loader")
                continue;
            if (listingMode == 5 && curTabButtons[x]->category != "poc")
                continue;
        }

        log_printf("%p", curTabButtons[x]->button);
        append(curTabButtons[x]->button);
        homebrewButtons.push_back(curTabButtons[x]);

//        log_printf("filter: added it");
        globalUpdatePosition = true;
    }
    
    isFiltering = false;
            
}

void HomebrewWindow::addAll(std::vector<homebrewButton*> buttons, int status)
{
    log_printf("Starting addall for status %d", status);
    GuiImageData* appButtonImages[4] = { localButtonImgData, updateButtonImgData, installedButtonImgData, getButtonImgData };

    // only add the specific status of button to the current tab
    for (unsigned int x=0; x<buttons.size(); x++)
    {
//        log_printf("Checking %d, named %s", x, buttons[x].shortname.c_str());
        if (buttons[x]->status == status)
        {
            log_printf("going in...");
            curTabButtons.push_back(buttons[x]);
            log_printf("added to current tab");
            
            buttons[x]->image = new GuiImage(appButtonImages[status]);
            log_printf("created new GuiImage");
            
            buttons[x]->button = new GuiButton(installedButtonImgData->getWidth(), installedButtonImgData->getHeight());
            log_printf("initialized button");

            buttons[x]->button->clicked.connect(this, &HomebrewWindow::OnHomebrewButtonClick);
            log_printf("attached click event");

                                    
            positionHomebrewButton(buttons[x], x);
            log_printf("positioned button");

        }
    }
}
    
/**
This method fetches the local apps from either /wiiu/games or /wiiu/apps
**/
void HomebrewWindow::loadLocalApps(int mode)
{
    clearScreen();
    
    localAppButtons.clear();
    homebrewButtons.clear();
    
    log_printf("loadLocalApps: begin");
    // get the 4 different types of app backgrounds
//    GuiImageData* appButtonImages[4] = { localButtonImgData, updateButtonImgData, installedButtonImgData, getButtonImgData };
    
    // log_printf("loadLocalApps: begin2");
    // get a list of directories
    DirList* dirList = new DirList("sd:/wiiu/apps", ".elf,.rpx", DirList::Files | DirList::CheckSubfolders);
    
    log_printf("loadLocalApps: directory is loaded, it has %d files", dirList->GetFilecount());
    
    // sort the dir list
    dirList->SortList();
    
    // reset all remote apps to their GET form for now
    for (unsigned int x=0; x<remoteAppButtons.size(); x++)
    {
        remoteAppButtons[x]->status = GET;
        
    }
    
    log_printf("loadLocalApps: directory is sorted");

    // load up local apps
    for(int i = 0; i < dirList->GetFilecount(); i++)
    {
        //! skip hidden linux and mac files
        if(dirList->GetFilename(i)[0] == '.' || dirList->GetFilename(i)[0] == '_')
            continue;

        std::string homebrewPath = dirList->GetFilepath(i);

        log_printf("Initial value: %s", homebrewPath.c_str());
        size_t slashPos = homebrewPath.rfind('/');
        if(slashPos != std::string::npos)
            homebrewPath.erase(slashPos);

        if(homebrewPath == "sd:/wiiu/apps") {
            // This cause (a lot of) problems
            log_printf("Skipping app with bad path to prevent errors");
            continue;
        }
        
        log_printf("Loaded up %s, going to check for a meta", homebrewPath.c_str());
        HomebrewXML metaXml;
        bool xmlReadSuccess = metaXml.LoadHomebrewXMLData((homebrewPath + "/meta.xml").c_str());
//        if (!xmlReadSuccess)
//            continue;
        std::string iconPath = "/icon.png";
        u8 * iconData = NULL;
        u32 iconDataSize = 0;
        
        log_printf("Continuing with it...");
        LoadFileToMem((homebrewPath + iconPath).c_str(), &iconData, &iconDataSize);
        
        log_printf("Loaded icon");

        int found = 0;
        if (!xmlReadSuccess) continue;
        
        std::string curAppString = homebrewPath.substr(14);
        if (curAppString == "") continue;
                
        // check if we already have this app in some as updated/installed
        for (unsigned int x=0; x<remoteAppButtons.size() && xmlReadSuccess; x++)
        {
            if (remoteAppButtons[x]->shortname == curAppString)
            {
                if (remoteAppButtons[x]->version != metaXml.GetVersion())
                    remoteAppButtons[x]->status = UPDATE;
                else 
                    remoteAppButtons[x]->status = INSTALLED;

                log_printf("decided it's %d", remoteAppButtons[x]->status);
                remoteAppButtons[x]->execPath = dirList->GetFilepath(i);
                remoteAppButtons[x]->dirPath = homebrewPath;

                if(iconData != NULL)
                {
                    remoteAppButtons[x]->iconImgData = new GuiImageData(iconData, iconDataSize);
                    free(iconData);
                    iconData = NULL;
                }

                found = 1;
                break;
            }
        }
        
        if (found) continue;
        
        scrollOffY = -120;
                
        log_printf("Making a localfile for this one");
        
        // make the homebrew list bigger if we're gonna process this
        int idx = homebrewButtons.size();
        homebrewButtons.resize(homebrewButtons.size() + 1);
        
        homebrewButtons[idx] = new homebrewButton();
                
        // file path
        homebrewButtons[idx]->execPath = dirList->GetFilepath(i);
        homebrewButtons[idx]->iconImgData = NULL;        
        homebrewButtons[idx]->typee = HBL;
        homebrewButtons[idx]->dirPath = homebrewPath;

//        // assume that the first part of homebrewPath is "sd:/wiiu/apps/"
        homebrewButtons[idx]->shortname = curAppString;
        homebrewButtons[idx]->status = LOCAL;
//        
        log_printf("Looking in %s for an icon", (homebrewPath + iconPath).c_str());
//
        if(iconData != NULL)
        {
            homebrewButtons[idx]->iconImgData = new GuiImageData(iconData, iconDataSize);
            free(iconData);
            iconData = NULL;
        }
      
        const char *cpName = xmlReadSuccess ? metaXml.GetName() : homebrewButtons[idx]->execPath.c_str();
//        const char *cpDescription = xmlReadSuccess ? metaXml.GetShortDescription() : "";
        if(strncmp(cpName, "sd:/wiiu/apps/", strlen("sd:/wiiu/apps/")) == 0)
            cpName += strlen("sd:/wiiu/apps/");
        
        homebrewButtons[idx]->nameLabel = new GuiText(cpName, 28, glm::vec4(0, 0, 0, 1));
        homebrewButtons[idx]->versionLabel = new GuiText(xmlReadSuccess? metaXml.GetVersion() : "", 28, glm::vec4(0, 0, 0, 1));
        homebrewButtons[idx]->coderLabel = new GuiText(xmlReadSuccess? metaXml.GetCoder() : "Unknown", 28, glm::vec4(0, 0, 0, 1));
        homebrewButtons[idx]->descriptionLabel = new GuiText(xmlReadSuccess? metaXml.GetShortDescription() : "", 28, glm::vec4(0, 0, 0, 1));
        log_printf("refreshHomebrewApps: added local button %d", idx);
        homebrewButtons[idx]->version = xmlReadSuccess? metaXml.GetVersion() : "999";
        
        localAppButtons.push_back(homebrewButtons[idx]);
        log_printf("totally done with and added that");
    }
    
    // add everything to the current tab (this defines the order)
    addAll(remoteAppButtons, UPDATE);
    addAll(remoteAppButtons, INSTALLED);
    addAll(localAppButtons, LOCAL);
    addAll(remoteAppButtons, GET);
    
    filter();
}

/**
This method fetches remote apps only and that's all nothing more
**/
void HomebrewWindow::refreshHomebrewApps()
{
    log_printf("refreshHomebrewApps: starting homebrew app refresh");
    // get the 4 different types of app backgrounds
    
    // clear arrays
    remoteAppButtons.clear();
    homebrewButtons.clear();
    curTabButtons.clear();
    
    std::string cache;
    
    if (!noIconMode)
    {
        // create the icon cache folder
        cache = "sd:/wiiu/apps/appstore/cache/";
        CreateSubfolder(cache.c_str());
    }
    
    int count = 0;
                        
    // download app list from the repo
    std::string targetUrl = std::string(repoUrl)+"/directory12.yaml";
    if (!gotDirectorySuccess)
    {
        log_printf("refreshHomebrewApps: Downloading remote %s", targetUrl.c_str());
        gotDirectorySuccess = FileDownloader::getFile(targetUrl, fileContents, &updateProgress);
//        removeE(progressWindow);
        log_printf("refreshHomebrewApps: Updated directory");
        
        std::istringstream f2(fileContents);
    
        // get the total number of apps (lines divided by 7)
        while (gotDirectorySuccess)
        {
            count ++;
            std::string dummy;
            if (!std::getline(f2, dummy)) break;
        }

        count /= 7;
    }
    
    std::istringstream f(fileContents);
    
    // totalLocalApps will represent how many apps aren't on the server
    totalLocalApps = homebrewButtons.size();
    
    u32 iterCount = 0;
    globalUpdatePosition = true;

    while (gotDirectorySuccess)
    {
        char numstr[30];
        sprintf(numstr, "Updating App info... (%d/%d)", iterCount, count);
        
        progressWindow->setTitle(numstr);
        updateProgress(0, iterCount, count);

        log_printf("starting yaml parsing");
        std::string shortname;

        // very poor yaml parsing, to be replaced with json in the future
        if (!std::getline(f, shortname)) break;
        shortname = shortname.substr(5);
        std::string name;    
        std::getline(f, name);
        name = name.substr(2);
        std::string author;    
        std::getline(f, author);
        author = author.substr(2);
        std::string desc;    
        std::getline(f, desc);
        desc = desc.substr(2);
        std::string binary;    
        std::getline(f, binary);
        binary = binary.substr(2);
        std::string version;
        std::getline(f, version);
        version = version.substr(2);
        std::string typee;
        std::getline(f, typee);
        typee = typee.substr(2);
        
        log_printf("done yaml parsing");

        int idx = homebrewButtons.size();
        homebrewButtons.resize(homebrewButtons.size() + 1);

        log_printf("resized homebrewButtons vector");
        
        homebrewButtons[idx] = new homebrewButton();
        // file path
        homebrewButtons[idx]->execPath = "";
        log_printf("set exec path");

        
        std::string homebrewPath = homebrewButtons[idx]->execPath;
        size_t slashPos = homebrewPath.rfind('/');
        if(slashPos != std::string::npos)
            homebrewPath.erase(slashPos);

        homebrewButtons[idx]->dirPath = homebrewPath;

        // since we got this app from the net, mark it as a GET
        homebrewButtons[idx]->status = GET;
        homebrewButtons[idx]->shortname = shortname;
        homebrewButtons[idx]->binary = binary;
        homebrewButtons[idx]->version = version;
        log_printf("set a bunch of attributes");
        
        // default to HBL type, set to RPX type if typee string matches
        homebrewButtons[idx]->category = typee;

        const char *cpName = name.c_str();
//        const char *cpDescription = desc.c_str();

        if(strncmp(cpName, "sd:/wiiu/apps/", strlen("sd:/wiiu/apps/")) == 0)
           cpName += strlen("sd:/wiiu/apps/");

        homebrewButtons[idx]->nameLabel = new GuiText(cpName, 28, glm::vec4(0, 0, 0, 1));
        homebrewButtons[idx]->versionLabel = new GuiText(version.c_str(), 28, glm::vec4(0, 0, 0, 1));
        homebrewButtons[idx]->coderLabel = new GuiText(author.c_str(), 28, glm::vec4(0, 0, 0, 1));
        homebrewButtons[idx]->descriptionLabel = new GuiText(desc.c_str(), 28, glm::vec4(0, 0, 0, 1));
        log_printf("created some more attributes");
        
        homebrewButtons[idx]->iconImgData = Resources::GetImageData("missing.png");

        
        // add this to the remote button array
        remoteAppButtons.push_back(homebrewButtons[idx]);
        
        
        log_printf("added %s to the remoteApps vector", shortname.c_str());
        iterCount ++;
        
        // download the icon for this remote app if we aren't ignoring icons
        if (!noIconMode)
        {
            int x = remoteAppButtons.size() - 1;

            // set the download app icon location
            const std::string dest = cache + remoteAppButtons[x]->shortname + ".png";

            // find the path on the server depending on our current tab
            std::string tabPath = "apps/" + remoteAppButtons[x]->shortname + "/icon.png";

            std::string targetIconUrl = std::string(repoUrl)+"/"+tabPath;
            bool imageDownloadSuccessful = false;
                                    
            // try to load an existing icon
            u8 * iconData = NULL;
            u32 iconDataSize = 0;
            
            // if invalidating cache, skip loading to memory (leaving iconData null)
            if (!invalidateCache)
                LoadFileToMem(dest.c_str(), &iconData, &iconDataSize);
            
            // check if the desired icon already exists in the cache, if not download it
            if (iconData == NULL)
                imageDownloadSuccessful = FileDownloader::getFile(targetIconUrl, dest);

            if (imageDownloadSuccessful || iconData != NULL)
            {
                u8 * iconData = NULL;
                u32 iconDataSize = 0;
                LoadFileToMem(dest.c_str(), &iconData, &iconDataSize);

                GuiImageData* iconImgData = new GuiImageData(iconData, iconDataSize);

                remoteAppButtons[x]->iconImgData = iconImgData;
            }
        }
        
    }
    
    removeE(progressWindow);

    initialLoadInProgress = false;
    globalUpdatePosition = true;
    log_printf("refreshHomebrewApps: done");
    
}

bool HomebrewWindow::checkLocalAppExists(std::string shortname)
{
    for (u32 x=0; x<localAppButtons.size(); x++)
    {
        if (localAppButtons[x]->shortname == shortname)
        {
            return true;
        }
    }
    
    return false;
}

HomebrewWindow::HomebrewWindow(int w, int h)
    : GuiFrame(w, h)
    , buttonClickSound(Resources::GetSound("button_click.mp3"))
    , installedButtonImgData(Resources::GetImageData("INSTALLED.png"))
    , getButtonImgData(Resources::GetImageData("GET.png"))
    , updateButtonImgData(Resources::GetImageData("UPDATE.png"))
    , localButtonImgData(Resources::GetImageData("LOCAL.png"))
    , hblVersionText("Credit: pwsincd and dimok, Music: (T-T)b     ", 32, glm::vec4(1.0f))
    , hblTabImgData(Resources::GetImageData("back_tab.png"))
    , rpxTabImgData(Resources::GetImageData("random_tab.png"))
    , hblTabImg(hblTabImgData)
    , rpxTabImg(rpxTabImgData)
    , backTabBtn(hblTabImg.getWidth(), hblTabImg.getHeight())
    , randomTabBtn(rpxTabImg.getWidth(), rpxTabImg.getHeight())
    , touchTrigger(GuiTrigger::CHANNEL_1, GuiTrigger::VPAD_TOUCH)
    , wpadTouchTrigger(GuiTrigger::CHANNEL_2 | GuiTrigger::CHANNEL_3 | GuiTrigger::CHANNEL_4 | GuiTrigger::CHANNEL_5, GuiTrigger::BUTTON_A)
{
//    tcpReceiver.serverReceiveStart.connect(this, &HomebrewWindow::OnTcpReceiveStart);
//    tcpReceiver.serverReceiveFinished.connect(this, &HomebrewWindow::OnTcpReceiveFinish);

    targetLeftPosition = 0;
    currentLeftPosition = 0;
    listOffset = 0;
    gotDirectorySuccess = false;
    screenLocked = false;

    listingMode = -1;
        
    char* localRepoUrl = "sd:/wiiu/apps/appstore/repository.txt";
        
     struct stat buffer;   
    if (stat (localRepoUrl, &buffer) == 0)
    {
        // load repo from repository.txt if it exists
        FILE* file = fopen(localRepoUrl, "r");

        // Get the file size
        long length = 0;
        fseek( file, 0, SEEK_END );
        length = ftell( file );
        fseek( file, 0, SEEK_SET );

        char buf[length+1];
        if ( fread( buf, length, 1, file ) == 1 ) {
            buf[length] = '\0';
            bufString = std::string(buf);
            repoUrl = (char*)bufString.c_str();
        }
    }
        
    log_printf(repoUrl);
    
    ConnectionTest * repoConnection = new ConnectionTest(std::string(repoUrl));
    internet_connection = repoConnection->Test();
        
    progressWindow = new ProgressWindow("Downloading app directory...");
    std::string qualifiedName = " " + std::string(repoUrl);

    if (internet_connection)
        hblRepoText = new GuiText(qualifiedName.c_str(), 32, glm::vec4(1.0f));
    else
        hblRepoText = new GuiText(" (!) OFFLINE", 32, glm::vec4(1, 1, 0, 1));
        
    hblVersionText.setAlignment(ALIGN_BOTTOM | ALIGN_RIGHT);
    hblRepoText->setAlignment(ALIGN_BOTTOM | ALIGN_LEFT);
    hblVersionText.setPosition(0, 50.0f);
    hblRepoText->setPosition(0, 50.0f);
    progressWindow->setPosition(0, 30.0f);
    append(&hblVersionText);
    append(hblRepoText);
        
//    header = new GuiText("Homebrew App Store", 64, glm::vec4(1, 1, 1, 1));
    header = new GuiImage(Resources::GetImageData("title.png"));
    header->setAlignment(ALIGN_TOP);
    header2 = new GuiText("Select a Category", 30, glm::vec4(1, 1, 1, 1));
    
        
    // for now, do this like this
    // in the future, a method should handle guibuttons like this
    backTabBtn.setImage(&hblTabImg);
    randomTabBtn.setImage(&rpxTabImg);
        
    backTabBtn.setScale(0.6);
    randomTabBtn.setScale(0.6);
        
    backTabBtn.setAlignment(ALIGN_LEFT);
    randomTabBtn.setAlignment(ALIGN_RIGHT);
        
    backTabBtn.setPosition(0, 0);
    randomTabBtn.setPosition(0, 0);
        
    backTabBtn.setEffectGrow();
    randomTabBtn.setEffectGrow();
        
    backTabBtn.setTrigger(&touchTrigger);
    randomTabBtn.setTrigger(&touchTrigger);
    
    backTabBtn.setTrigger(&wpadTouchTrigger);
    randomTabBtn.setTrigger(&wpadTouchTrigger);
        
    backTabBtn.setSoundClick(buttonClickSound);
    randomTabBtn.setSoundClick(buttonClickSound);
        
    backTabBtn.clicked.connect(this, &HomebrewWindow::OnHBLTabButtonClick);
    randomTabBtn.clicked.connect(this, &HomebrewWindow::OnRPXTabButtonClick);
    
    append(progressWindow);
        
//    refreshHomebrewApps();
}

HomebrewWindow::~HomebrewWindow()
{
    for(u32 i = 0; i < homebrewButtons.size(); ++i)
    {
        if (homebrewButtons[i]->image != 0)
            delete homebrewButtons[i]->image;
        if (homebrewButtons[i]->nameLabel != 0)
            delete homebrewButtons[i]->nameLabel;
        if (homebrewButtons[i]->descriptionLabel != 0)
            delete homebrewButtons[i]->descriptionLabel;
        if (homebrewButtons[i]->button != 0)
            delete homebrewButtons[i]->button;
        if (homebrewButtons[i]->iconImgData != 0)
            delete homebrewButtons[i]->iconImgData;
        if (homebrewButtons[i]->iconImg != 0)
            delete homebrewButtons[i]->iconImg;
    }

    Resources::RemoveSound(buttonClickSound);
    Resources::RemoveImageData(installedButtonImgData);
    Resources::RemoveImageData(getButtonImgData);
    Resources::RemoveImageData(updateButtonImgData);
    Resources::RemoveImageData(localButtonImgData);
    Resources::RemoveImageData(hblTabImgData);
    Resources::RemoveImageData(rpxTabImgData);
}

void HomebrewWindow::clearScreen()
{
    // remove any existing buttons
    for (u32 x=0; x<curTabButtons.size(); x++)
    {
//        log_printf("filter: about to remove button %d", x);
        removeE(curTabButtons[x]->button);  
    }
    curTabButtons.clear();
}

void HomebrewWindow::OnCategorySwitch(GuiButton *button, const GuiController *controller, GuiTrigger *trigger)
{
    
    // remove all category buttons
    for (unsigned int x=0; x<all_cats.size(); x++)
    {
        // detect which number category button was pressed
        if (all_cats[x] == button)
            listingMode = x;
                        
        // remove them
        removeE(all_cats[x]);
    }
    
    removeE(header);
    removeE(header2);
    
    append(&backTabBtn);

    if (internet_connection)
        append(&randomTabBtn);
    
    loadLocalApps(0);
    // change the category based on the click
//    filter();
}

void HomebrewWindow::appendCategoryButton(char* name, int x, int y)
{        
    int idx = all_cats.size();
    log_printf("CUR BUTTON: %d", idx);
    all_cats.resize(idx+1);
    
    GuiImage* img = new GuiImage(Resources::GetImageData(name));
        
    all_cats[idx] = new GuiButton(img->getWidth(), img->getHeight());
    all_cats[idx]->setImage(new GuiImage(Resources::GetImageData(name)));
    all_cats[idx]->setPosition(x, y);
    
    all_cats[idx]->setSoundClick(buttonClickSound);
    all_cats[idx]->setTrigger(&touchTrigger);
    all_cats[idx]->setTrigger(&wpadTouchTrigger);
    all_cats[idx]->setEffectGrow();
    all_cats[idx]->clicked.connect(this, &HomebrewWindow::OnCategorySwitch);
    

    append(all_cats[idx]);
}

void HomebrewWindow::displayCategories()
{
    clearScreen();
    all_cats.clear();
    
    header->setPosition(0, 0);
    append(header);
    
    header2->setPosition(0, -240);
    append(header2);
    
    const char* vals[6] = {"all_gray.png", "games.png", "emulators.png", "tools.png", "loaders.png", "concepts.png"};
    
    if (internet_connection) {
        appendCategoryButton((char*)vals[0], -250,  100);
        appendCategoryButton((char*)vals[1],    0,  100);
        appendCategoryButton((char*)vals[2],  250,  100);
        appendCategoryButton((char*)vals[3], -250, -100);
        appendCategoryButton((char*)vals[4],    0, -100);
        appendCategoryButton((char*)vals[5],  250, -100);
    } else {
        // When there's no internet the app is unable to detect categories so all the categories apart from "all apps" would be empty
        appendCategoryButton("all_gray.png",    0,    0);
    }
}


void HomebrewWindow::OnHBLTabButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger)
{
    if (screenLocked)
    {
        if (launchBox)
            // dismiss any active window
            OnLaunchBoxCloseClick(launchBox);
        return;
    }
    
    listingMode = -1;
    clearScreen();
    filter();
    
    removeE(&backTabBtn);

    if (internet_connection)
        removeE(&randomTabBtn);
    
    displayCategories();
    globalUpdatePosition = true;
}

void HomebrewWindow::OnRPXTabButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger)
{
    if (screenLocked)
    {
        if (launchBox)
            // dismiss any active window
            removeE(launchBox);
    }
    
    // click a random button within the displayed apps
    int r = rand() % homebrewButtons.size();
    OnHomebrewButtonClick(homebrewButtons[r]->button, controller, 0);
}

void HomebrewWindow::OnOpenEffectFinish(GuiElement *element)
{
    //! once the menu is open reset its state and allow it to be "clicked/hold"
    element->effectFinished.disconnect(this);
    element->clearState(GuiElement::STATE_DISABLED);
}

void HomebrewWindow::OnCloseEffectFinish(GuiElement *element)
{
    screenLocked = false;
    
    backTabBtn.clearState(GuiElement::STATE_DISABLED);
    randomTabBtn.clearState(GuiElement::STATE_DISABLED);
    
    //! remove element from draw list and push to delete queue
    removeE(element);
    AsyncDeleter::pushForDelete(element);

    for(u32 i = 0; i < homebrewButtons.size(); i++)
    {
        if (homebrewButtons[i]->button != 0)
            homebrewButtons[i]->button->clearState(GuiElement::STATE_DISABLED);
    }
}

void HomebrewWindow::OnLaunchBoxCloseClick(GuiElement *element)
{
    element->setState(GuiElement::STATE_DISABLED);
    element->setEffect(EFFECT_FADE, -10, 0);
    element->effectFinished.connect(this, &HomebrewWindow::OnCloseEffectFinish);
    launchBox = NULL;
}

void HomebrewWindow::OnHomebrewButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger)
{
    log_printf("clicked a homebrew button");
    if (getHasScrolled() || initialLoadInProgress) {
        return;
    }        
    
    thisHomebrewWindow = this;
        
    bool disableButtons = false;
//    return;

    for(u32 i = 0; i < homebrewButtons.size(); i++)
    {
        if(button == homebrewButtons[i]->button)
        {
            launchBox = new HomebrewLaunchWindow(*homebrewButtons[i], this);
            HomebrewLaunchWindow* launchBox2 = (HomebrewLaunchWindow*)launchBox;
            launchBox2->setEffect(EFFECT_FADE, 10, 255);
            launchBox2->setState(GuiElement::STATE_DISABLED);
            launchBox2->setPosition(0.0f, 30.0f);
            launchBox2->effectFinished.connect(this, &HomebrewWindow::OnOpenEffectFinish);
            launchBox2->backButtonClicked.connect(this, &HomebrewWindow::OnLaunchBoxCloseClick);
            log_printf("creating launchbox");
            append(launchBox);
            disableButtons = true;
            screenLocked = true;
            break;
        }
    }


    if(disableButtons)
    {
        for(u32 i = 0; i < homebrewButtons.size(); i++)
        {
            if (homebrewButtons[i]->button != 0)
                homebrewButtons[i]->button->setState(GuiElement::STATE_DISABLED);
        }
    }
}


void HomebrewWindow::draw(CVideo *pVideo)
{
    bool bUpdatePositions = false || globalUpdatePosition;
    
    if (scrollOffY != lastScrollOffY)
        bUpdatePositions = true;
        
    if(currentLeftPosition < targetLeftPosition)
    {
        currentLeftPosition += 35;

        if(currentLeftPosition > targetLeftPosition)
            currentLeftPosition = targetLeftPosition;

        bUpdatePositions = true;
    }
    else if(currentLeftPosition > targetLeftPosition)
    {
        currentLeftPosition -= 35;

        if(currentLeftPosition < targetLeftPosition)
            currentLeftPosition = targetLeftPosition;

        bUpdatePositions = true;
    }

    if(bUpdatePositions)
    {
        
        log_printf("draw: updating positions...");
//        bUpdatePositions = false;
        globalUpdatePosition = false;
        
        int imageHeight = 210;

        for(u32 i = 0; i < homebrewButtons.size(); i++)
        {
//            log_printf("draw: adding a button at pos %d", i);
            float fXOffset = ((i % 2)? 265 : -265);
            float fYOffset = scrollOffY + (imageHeight + 20.0f) * 1.5f - (imageHeight + 15) * ((i%2)? (int)((i-1)/2) : (int)(i/2));    
            if (homebrewButtons[i]->button != 0)
                homebrewButtons[i]->button->setPosition(currentLeftPosition + fXOffset, fYOffset);
//            log_printf("draw: added that button %d", i);
        }
        
//        if (listingMode == 1)
//        {
//            backTabBtn.setPosition(0, 85);
//            randomTabBtn.setPosition(-20, -85);
//        }
//        else if (listingMode == 2)
//        {
//            backTabBtn.setPosition(-20, 85);
//            randomTabBtn.setPosition(0, -85);
//        }
        lastScrollOffY = scrollOffY;
        
        log_printf("draw: done drawing");
    }
    
    

    GuiFrame::draw(pVideo);
    
    if (bUpdatePositions)
        log_printf("draw: done with literally everything now");

}


void refreshHomebrewAppIcons()
{
    
}

HomebrewWindow* getHomebrewWindow()
{
    return thisHomebrewWindow;
}
