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
#include "fs/FSUtils.h"
// #include <system/AsyncDeleter.h>
#include "HomebrewXML.h"
#include "HomebrewLaunchWindow.h"
#include "network/ConnectionTest.h"
#include "../resources/Resources.h"
#include <thread>
#include <sstream>

#define sdlBlack (SDL_Color) { 0, 0, 0, 255 }
#define sdlWhite (SDL_Color) { 255, 255, 255, 255 }
#define sdlRed (SDL_Color) { 255, 0, 0, 255 }

#define DEFAULT_WIILOAD_PORT        4299

char * repoUrl = "http://wiiubru.com/appstore";
//char * repoUrl = "192.168.1.104:8000";
//char * repoUrl = "http://wiiubru.com/appstore/appstoretest";

ProgressWindow* progressWindow;
static HomebrewWindow* thisHomebrewWindow;

void HomebrewWindow::positionHomebrewButton(homebrewButton* button, int index)
{
    printf("going into this big function");
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
        button->nameLabel->setMaxWidth(350);//, GuiText::SCROLL_HORIZONTAL);
        button->nameLabel->setPosition(0, 70);
    }
    
    if (button->coderLabel)
    {
        button->coderLabel->setAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
        button->coderLabel->setMaxWidth(160);//, GuiText::SCROLL_HORIZONTAL);
        button->coderLabel->setPosition(300, 20);
        printf("did coder things");
    }
    
    if (button->versionLabel)
    {
        button->versionLabel->setAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
        button->versionLabel->setMaxWidth(160);//, GuiText::SCROLL_HORIZONTAL);
        button->versionLabel->setPosition(300, -15);
        printf("did version things");
    }
    
    if (button->descriptionLabel)
    {
        button->descriptionLabel->setAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
        button->descriptionLabel->setMaxWidth(350);//, GuiTex t::SCROLL_HORIZONTAL);
        button->descriptionLabel->setPosition(0, -60);
        printf("did description things");
    }

    if (button->image)
    {
        // set the right image for the status
        button->image->setScale(0.9);
        printf("did image scale things");
    }

    if (button->button)
    {
        button->button->setImage(button->image);
        printf("did another image things");
        button->button->setLabel(button->nameLabel, 0);
        button->button->setLabel(button->descriptionLabel, 1);
        button->button->setLabel(button->coderLabel, 2);
        button->button->setLabel(button->versionLabel, 3);
        button->button->setIcon(button->iconImg);
        button->button->setTrigger(&touchTrigger);
        button->button->setTrigger(&wpadTouchTrigger);
        button->button->setEffectGrow();
        printf("did button things");
    }
//    button->button->setPosition(0, 0);
    //        button->button->setSoundClick(buttonClickSound);
}

ProgressWindow* getProgressWindow()
{
    return progressWindow;
}

void updateProgress(void *arg, uint32_t done, uint32_t total)
{
    progressWindow->setProgress(100.0f* (((float)done)/((float)total)));
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
//    for (uint32_t x=0; x<curTabButtons.size(); x++)
//    {
//        output += "["+std::string(curTabButtons[x]->shortname)+"] ";
//    }
        
//    std::random_shuffle(curTabButtons.begin(),curTabButtons.end());
    

    for (uint32_t x=0; x<curTabButtons.size(); x++)
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

        printf("%p", curTabButtons[x]->button);
        append(curTabButtons[x]->button);
        homebrewButtons.push_back(curTabButtons[x]);

//        printf("filter: added it");
        globalUpdatePosition = true;
    }
    
    isFiltering = false;
            
}

void HomebrewWindow::addAll(std::vector<homebrewButton*> buttons, int status)
{
    printf("Starting addall for status %d", status);
    GuiTextureData* appButtonImages[4] = { localButtonImgData, updateButtonImgData, installedButtonImgData, getButtonImgData };

    // only add the specific status of button to the current tab
    for (unsigned int x=0; x<buttons.size(); x++)
    {
//        printf("Checking %d, named %s", x, buttons[x].shortname.c_str());
        if (buttons[x]->status == status)
        {
            printf("going in...");
            curTabButtons.push_back(buttons[x]);
            printf("added to current tab");
            
            buttons[x]->image = new GuiImage(appButtonImages[status]);
            printf("created new GuiImage");
            
            buttons[x]->button = new GuiButton(installedButtonImgData->getWidth(), installedButtonImgData->getHeight());
            printf("initialized button");

            buttons[x]->button->clicked.connect(this, &HomebrewWindow::OnHomebrewButtonClick);
            printf("attached click event");

                                    
            positionHomebrewButton(buttons[x], x);
            printf("positioned button");

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
    
    printf("loadLocalApps: begin");
    // get the 4 different types of app backgrounds
//    GuiTextureData* appButtonImages[4] = { localButtonImgData, updateButtonImgData, installedButtonImgData, getButtonImgData };
    
    // printf("loadLocalApps: begin2");
    // get a list of directories
    DirList* dirList = new DirList("sd:/wiiu/apps", ".elf,.rpx", DirList::Files | DirList::CheckSubfolders);
    
    printf("loadLocalApps: directory is loaded, it has %d files", dirList->GetFilecount());
    
    // sort the dir list
    dirList->SortList();
    
    // reset all remote apps to their GET form for now
    for (unsigned int x=0; x<remoteAppButtons.size(); x++)
    {
        remoteAppButtons[x]->status = GET;
        
    }
    
    printf("loadLocalApps: directory is sorted");

    // load up local apps
    for(int i = 0; i < dirList->GetFilecount(); i++)
    {
        //! skip hidden linux and mac files
        if(dirList->GetFilename(i)[0] == '.' || dirList->GetFilename(i)[0] == '_')
            continue;

        std::string homebrewPath = dirList->GetFilepath(i);

        printf("Initial value: %s", homebrewPath.c_str());
        size_t slashPos = homebrewPath.rfind('/');
        if(slashPos != std::string::npos)
            homebrewPath.erase(slashPos);

        if(homebrewPath == "sd:/wiiu/apps") {
            // This cause (a lot of) problems
            printf("Skipping app with bad path to prevent errors");
            continue;
        }
        
        printf("Loaded up %s, going to check for a meta", homebrewPath.c_str());
        HomebrewXML metaXml;
        bool xmlReadSuccess = metaXml.LoadHomebrewXMLData((homebrewPath + "/meta.xml").c_str());
//        if (!xmlReadSuccess)
//            continue;
        std::string iconPath = "/icon.png";
        uint8_t * iconData = NULL;
        uint32_t iconDataSize = 0;
        
        printf("Continuing with it...");
        FSUtils::LoadFileToMem((homebrewPath + iconPath).c_str(), &iconData, &iconDataSize);
        
        printf("Loaded icon");

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

                printf("decided it's %d", remoteAppButtons[x]->status);
                remoteAppButtons[x]->execPath = dirList->GetFilepath(i);
                remoteAppButtons[x]->dirPath = homebrewPath;

                if(iconData != NULL)
                {
                    remoteAppButtons[x]->iconImgData = new GuiTextureData(iconData, iconDataSize);
                    free(iconData);
                    iconData = NULL;
                }

                found = 1;
                break;
            }
        }
        
        if (found) continue;
        
        scrollOffY = -120;
                
        printf("Making a localfile for this one");
        
        // make the homebrew list bigger if we're gonna process this
        int idx = homebrewButtons.size();
        homebrewButtons.resize(homebrewButtons.size() + 1);
        
        homebrewButtons[idx] = new homebrewButton();
                
        // file path
        homebrewButtons[idx]->execPath = dirList->GetFilepath(i);
        homebrewButtons[idx]->iconImgData = NULL;        
        homebrewButtons[idx]->dirPath = homebrewPath;

//        // assume that the first part of homebrewPath is "sd:/wiiu/apps/"
        homebrewButtons[idx]->shortname = curAppString;
        homebrewButtons[idx]->status = LOCAL;
//        
        printf("Looking in %s for an icon", (homebrewPath + iconPath).c_str());
//
        if(iconData != NULL)
        {
            homebrewButtons[idx]->iconImgData = new GuiTextureData(iconData, iconDataSize);
            free(iconData);
            iconData = NULL;
        }
      
        const char *cpName = xmlReadSuccess ? metaXml.GetName() : homebrewButtons[idx]->execPath.c_str();
//        const char *cpDescription = xmlReadSuccess ? metaXml.GetShortDescription() : "";
        if(strncmp(cpName, "sd:/wiiu/apps/", strlen("sd:/wiiu/apps/")) == 0)
            cpName += strlen("sd:/wiiu/apps/");
        
        homebrewButtons[idx]->nameLabel = new GuiText(cpName, 28, sdlBlack);
        homebrewButtons[idx]->versionLabel = new GuiText(xmlReadSuccess? metaXml.GetVersion() : "", 28, sdlBlack);
        homebrewButtons[idx]->coderLabel = new GuiText(xmlReadSuccess? metaXml.GetCoder() : "Unknown", 28, sdlBlack);
        homebrewButtons[idx]->descriptionLabel = new GuiText(xmlReadSuccess? metaXml.GetShortDescription() : "", 28, sdlBlack);
        printf("refreshHomebrewApps: added local button %d", idx);
        homebrewButtons[idx]->version = xmlReadSuccess? metaXml.GetVersion() : "999";
        
        localAppButtons.push_back(homebrewButtons[idx]);
        printf("totally done with and added that");
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
    printf("refreshHomebrewApps: starting homebrew app refresh");
    
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
        FSUtils::CreateSubfolder(cache.c_str());
    }
    
    // download app list from the repo
    // check if the Get object already exists, and make it if it doesn't
    if (this->get == NULL)
        this->get = new Get(".get", repoUrl);
    
    int count = this->get->packages.size();
    
    // totalLocalApps will represent how many apps aren't on the server
    totalLocalApps = homebrewButtons.size();
    
    globalUpdatePosition = true;

    for (int iterCount=0; iterCount<count; iterCount++)
    {
        char numstr[30];
        sprintf(numstr, "Updating App info... (%d/%d)", iterCount, count);
        
        progressWindow->setTitle(numstr);
        updateProgress(0, iterCount, count);
        
        // the info for the current package is stored within the package variable
        Package* package = this->get->packages[iterCount];
        
        printf("done yaml parsing");

        int idx = homebrewButtons.size();
        homebrewButtons.resize(homebrewButtons.size() + 1);

        printf("resized homebrewButtons vector");
        
        homebrewButtons[idx] = new homebrewButton();
        // file path

        // store the package pointer inside the homebrew button element
        homebrewButtons[idx]->package = package;
        
        // store other proeprteis (legacy, reference)
        homebrewButtons[idx]->status = package->status;
        homebrewButtons[idx]->shortname = package->pkg_name;
        homebrewButtons[idx]->category = package->category;
        homebrewButtons[idx]->version = package->version;
        printf("set a bunch of attributes");

        homebrewButtons[idx]->nameLabel = new GuiText(package->title.c_str(), 28, sdlBlack);
        homebrewButtons[idx]->versionLabel = new GuiText(package->version.c_str(), 28, sdlBlack);
        homebrewButtons[idx]->coderLabel = new GuiText(package->author.c_str(), 28, sdlBlack);
        homebrewButtons[idx]->descriptionLabel = new GuiText(package->short_desc.c_str(), 28, sdlBlack);
        printf("created some more attributes");
        
        homebrewButtons[idx]->iconImgData = Resources::GetImageData(ASSET_ROOT "missing.png");

        
        // add this to the remote button array
        remoteAppButtons.push_back(homebrewButtons[idx]);
        
        // printf("added %s to the remoteApps vector", shortname.c_str());
        
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
            uint8_t * iconData = NULL;
            uint32_t iconDataSize = 0;
            
            // if invalidating cache, skip loading to memory (leaving iconData null)
            if (!invalidateCache)
                FSUtils::LoadFileToMem(dest.c_str(), &iconData, &iconDataSize);
            
            // check if the desired icon already exists in the cache, if not download it
            // if (iconData == NULL)
            //     imageDownloadSuccessful = FileDownloader::getFile(targetIconUrl, dest);

            if (imageDownloadSuccessful || iconData != NULL)
            {
                uint8_t * iconData = NULL;
                uint32_t iconDataSize = 0;
                FSUtils::LoadFileToMem(dest.c_str(), &iconData, &iconDataSize);

                GuiTextureData* iconImgData = new GuiTextureData(iconData, iconDataSize);

                remoteAppButtons[x]->iconImgData = iconImgData;
            }
        }
        
    }
    
    this->remove(progressWindow);

    initialLoadInProgress = false;
    globalUpdatePosition = true;
    printf("refreshHomebrewApps: done");
    
}

bool HomebrewWindow::checkLocalAppExists(std::string shortname)
{
    for (uint32_t x=0; x<localAppButtons.size(); x++)
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
    , installedButtonImgData(Resources::GetImageData(ASSET_ROOT "INSTALLED.png"))
    , getButtonImgData(Resources::GetImageData(ASSET_ROOT "GET.png"))
    , updateButtonImgData(Resources::GetImageData(ASSET_ROOT "UPDATE.png"))
    , localButtonImgData(Resources::GetImageData(ASSET_ROOT "LOCAL.png"))
    , hblVersionText("Credit: pwsincd and dimok, Music: (T-T)b     ", 32, sdlWhite)
    , hblTabImgData(Resources::GetImageData(ASSET_ROOT "back_tab.png"))
    , rpxTabImgData(Resources::GetImageData(ASSET_ROOT "random_tab.png"))
    , hblTabImg(hblTabImgData)
    , rpxTabImg(rpxTabImgData)
    , backTabBtn(hblTabImg.getWidth(), hblTabImg.getHeight())
    , randomTabBtn(rpxTabImg.getWidth(), rpxTabImg.getHeight())
    , touchTrigger(GuiTrigger::CHANNEL_1, GuiTrigger::TOUCHED)
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
        
    printf(repoUrl);
    
    ConnectionTest * repoConnection = new ConnectionTest(std::string(repoUrl));
    internet_connection = repoConnection->Test();
        
    progressWindow = new ProgressWindow("Downloading app directory...");
    std::string qualifiedName = " " + std::string(repoUrl);

    if (internet_connection)
        hblRepoText = new GuiText(qualifiedName.c_str(), 32, sdlWhite);
    else
        hblRepoText = new GuiText(" (!) OFFLINE", 32, sdlRed);
        
    hblVersionText.setAlignment(ALIGN_BOTTOM | ALIGN_RIGHT);
    hblRepoText->setAlignment(ALIGN_BOTTOM | ALIGN_LEFT);
    hblVersionText.setPosition(0, 50.0f);
    hblRepoText->setPosition(0, 50.0f);
    progressWindow->setPosition(0, 30.0f);
    append(&hblVersionText);
    append(hblRepoText);
        
//    header = new GuiText("Homebrew App Store", 64, sdlWhite);
    header = new GuiImage(Resources::GetImageData(ASSET_ROOT "title.png"));
    header->setAlignment(ALIGN_TOP);
    header2 = new GuiText("Select a Category", 30, sdlWhite);
    
        
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
    for(uint32_t i = 0; i < homebrewButtons.size(); ++i)
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

    // Resources::RemoveSound(buttonClickSound);
    // Resources::RemoveImageData(installedButtonImgData);
    // Resources::RemoveImageData(getButtonImgData);
    // Resources::RemoveImageData(updateButtonImgData);
    // Resources::RemoveImageData(localButtonImgData);
    // Resources::RemoveImageData(hblTabImgData);
    // Resources::RemoveImageData(rpxTabImgData);
}

void HomebrewWindow::clearScreen()
{
    // remove any existing buttons
    for (uint32_t x=0; x<curTabButtons.size(); x++)
    {
//        printf("filter: about to remove button %d", x);
        this->remove(curTabButtons[x]->button);  
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
        this->remove(all_cats[x]);
    }
    
    this->remove(header);
    this->remove(header2);
    
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
    printf("CUR BUTTON: %d", idx);
    all_cats.resize(idx+1);
    
    std::string nameStr = std::string(ASSET_ROOT) + std::string(name);
    GuiImage* img = new GuiImage(Resources::GetImageData(nameStr.c_str()));
        
    all_cats[idx] = new GuiButton(img->getWidth(), img->getHeight());
    all_cats[idx]->setImage(new GuiImage(Resources::GetImageData(nameStr.c_str())));
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
    
    this->remove(&backTabBtn);

    if (internet_connection)
        this->remove(&randomTabBtn);
    
    displayCategories();
    globalUpdatePosition = true;
}

void HomebrewWindow::OnRPXTabButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger)
{
    if (screenLocked)
    {
        if (launchBox)
            // dismiss any active window
            this->remove(launchBox);
    }
    
    // click a random button within the displayed apps
    int r = rand() % homebrewButtons.size();
    OnHomebrewButtonClick(homebrewButtons[r]->button, controller, 0);
}

void HomebrewWindow::OnOpenEffectFinish(GuiElement *element)
{
    //! once the menu is open reset its state and allow it to be "clicked/hold"
    element->effectFinished.disconnect(this);
    element->clearState(GuiElement::STATE_DISABLED, -1);
}

void HomebrewWindow::OnCloseEffectFinish(GuiElement *element)
{
    screenLocked = false;
    
    backTabBtn.clearState(GuiElement::STATE_DISABLED, -1);
    randomTabBtn.clearState(GuiElement::STATE_DISABLED, -1);
    
    //! remove element from draw list and push to delete queue
    this->remove(element);
    // AsyncDeleter::pushForDelete(element);

    for(uint32_t i = 0; i < homebrewButtons.size(); i++)
    {
        if (homebrewButtons[i]->button != 0)
            homebrewButtons[i]->button->clearState(GuiElement::STATE_DISABLED, -1);
    }
}

void HomebrewWindow::OnLaunchBoxCloseClick(GuiElement *element)
{
    element->setState(GuiElement::STATE_DISABLED, -1);
    element->setEffect(EFFECT_FADE, -10, 0);
    element->effectFinished.connect(this, &HomebrewWindow::OnCloseEffectFinish);
    launchBox = NULL;
}

void HomebrewWindow::OnHomebrewButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger)
{
    printf("clicked a homebrew button");
    if (getHasScrolled() || initialLoadInProgress) {
        return;
    }        
    
    thisHomebrewWindow = this;
        
    bool disableButtons = false;
//    return;

    for(uint32_t i = 0; i < homebrewButtons.size(); i++)
    {
        if(button == homebrewButtons[i]->button)
        {
            launchBox = new HomebrewLaunchWindow(*homebrewButtons[i], this);
            HomebrewLaunchWindow* launchBox2 = (HomebrewLaunchWindow*)launchBox;
            launchBox2->setEffect(EFFECT_FADE, 10, 255);
            launchBox2->setState(GuiElement::STATE_DISABLED, -1);
            launchBox2->setPosition(0.0f, 30.0f);
            launchBox2->effectFinished.connect(this, &HomebrewWindow::OnOpenEffectFinish);
            launchBox2->backButtonClicked.connect(this, &HomebrewWindow::OnLaunchBoxCloseClick);
            printf("creating launchbox");
            append(launchBox);
            disableButtons = true;
            screenLocked = true;
            break;
        }
    }


    if(disableButtons)
    {
        for(uint32_t i = 0; i < homebrewButtons.size(); i++)
        {
            if (homebrewButtons[i]->button != 0)
                homebrewButtons[i]->button->setState(GuiElement::STATE_DISABLED, -1);
        }
    }
}


void HomebrewWindow::draw(Renderer *pVideo)
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
        
        printf("draw: updating positions...");
//        bUpdatePositions = false;
        globalUpdatePosition = false;
        
        int imageHeight = 210;

        for(uint32_t i = 0; i < homebrewButtons.size(); i++)
        {
//            printf("draw: adding a button at pos %d", i);
            float fXOffset = ((i % 2)? 265 : -265);
            float fYOffset = scrollOffY + (imageHeight + 20.0f) * 1.5f - (imageHeight + 15) * ((i%2)? (int)((i-1)/2) : (int)(i/2));    
            if (homebrewButtons[i]->button != 0)
                homebrewButtons[i]->button->setPosition(currentLeftPosition + fXOffset, fYOffset);
//            printf("draw: added that button %d", i);
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
        
        printf("draw: done drawing");
    }
    
    

    GuiFrame::draw(pVideo);
    
    if (bUpdatePositions)
        printf("draw: done with literally everything now");

}


void refreshHomebrewAppIcons()
{
    
}

HomebrewWindow* getHomebrewWindow()
{
    return thisHomebrewWindow;
}
