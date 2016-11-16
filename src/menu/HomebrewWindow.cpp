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
#include <thread>
#include <sstream>


#define DEFAULT_WIILOAD_PORT        4299

#define MAX_BUTTONS_ON_PAGE     4
//char * repoUrl = "http://wiiubru.com/appstore";
char * repoUrl = "192.168.1.104:8000";
//char * repoUrl = "http://wiiubru.com/appstore/appstoretest";

ProgressWindow* progressWindow;
static HomebrewWindow* thisHomebrewWindow;

void HomebrewWindow::positionHomebrewButton(homebrewButton* button, int index)
{
    log_printf("going into this big function");
    const float cfImageScale = 0.8f;
    
    if (button->iconImg)
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
        button->coderLabel->setMaxWidth(170, GuiText::SCROLL_HORIZONTAL);
        button->coderLabel->setPosition(300, 20);
        log_printf("did coder things");
    }
    
    if (button->versionLabel)
    {
        button->versionLabel->setAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
        button->versionLabel->setMaxWidth(350, GuiText::SCROLL_HORIZONTAL);
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

int HomebrewWindow::checkIfUpdateOrInstalled(std::string name, std::string version, int totalLocalApps)
{
//    for (int x=0; x<totalLocalApps; x++)
//    {
//        // if shortname matches
//        if (!name.compare(homebrewButtons[x]->shortname.c_str()))
//        {
//            log_printf("DETECTED A LOCAL APP HERE");
//            homebrewButtons[x]->status = INSTALLED;
//            if (version.compare(homebrewButtons[x]->version))
//            {
//                // if version doesn't match
//                homebrewButtons[x]->status = UPDATE;
//            }
//
//            return x;
//        }
//    }
//    return -1;
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
    scrollOffY = -120;

//    
//    // empty the current tab
//    curTabButtons.clear();
    
//    for (u32 x=0; x<homebrewButtons.size(); x++)
//    {
//        if (homebrewButtons[x]->typee == listingMode)
//        {
//            curTabButtons.push_back(homebrewButtons[x]);
//        }
//    }
    
    homebrewButtons.clear();
    
    std::string output = "CURRENT: ";
    for (u32 x=0; x<curTabButtons.size(); x++)
    {
        output += "["+std::string(curTabButtons[x]->shortname)+"] ";
//        output << homebrewButtons[x].typee;
//        output << "/";
//        output << homebrewButtons[x].status;
//        output << "] ";
    }
    
    for (u32 x=0; x<curTabButtons.size(); x++)
    {
//        log_printf("filter: adding button %d, %s", x, curTabButtons[x].shortname.c_str());
        log_printf("%p", curTabButtons[x]->button);
        append(curTabButtons[x]->button);
        homebrewButtons.push_back(curTabButtons[x]);
//        log_printf("filter: added it");
    }
            
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
    
    // remove any existing buttons
    for (u32 x=0; x<curTabButtons.size(); x++)
    {
//        log_printf("filter: about to remove button %d", x);
        removeE(curTabButtons[x]->button);  
    }
    
    localAppButtons.clear();
    homebrewButtons.clear();
    curTabButtons.clear();
    
    log_printf("loadLocalApps: begin");
    // get the 4 different types of app backgrounds
//    GuiImageData* appButtonImages[4] = { localButtonImgData, updateButtonImgData, installedButtonImgData, getButtonImgData };
    
    log_printf("loadLocalApps: begin2");
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
        
        log_printf("Loaded up %s, going to check for a meta", homebrewPath.c_str());
        HomebrewXML metaXml;
        bool xmlReadSuccess = metaXml.LoadHomebrewXMLData((homebrewPath + "/meta.xml").c_str());
//        if (!xmlReadSuccess)
//            continue;
        std::string iconPath = "/icon.png";
        u8 * iconData = NULL;
        u32 iconDataSize = 0;
        
        log_printf("Continuing with it...");
        
        int found = 0;
        
        // check if we already have this app in some as updated/installed
        for (unsigned int x=0; x<remoteAppButtons.size() && xmlReadSuccess; x++)
        {
            if (remoteAppButtons[x]->shortname == homebrewPath.substr(14))
            {
                if (remoteAppButtons[x]->version != metaXml.GetVersion())
                    remoteAppButtons[x]->status = UPDATE;
                else 
                    remoteAppButtons[x]->status = INSTALLED;

                log_printf("decided it's %d", remoteAppButtons[x]->status);
                remoteAppButtons[x]->execPath = dirList->GetFilepath(i);
                remoteAppButtons[x]->dirPath = homebrewPath;
                LoadFileToMem((homebrewPath + iconPath).c_str(), &iconData, &iconDataSize);

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
        
        log_printf("Makinga localfile for this one");
        
        // make the homebrew list bigger if we're gonna process this
        int idx = homebrewButtons.size();
        homebrewButtons.resize(homebrewButtons.size() + 1);
                
        // file path
        homebrewButtons[idx]->execPath = dirList->GetFilepath(i);
        homebrewButtons[idx]->iconImgData = NULL;
        
        homebrewButtons[idx]->typee = HBL;
        
        homebrewButtons[idx]->dirPath = homebrewPath;
        
        // assume that the first part of homebrewPath is "sd:/wiiu/apps/"
        homebrewButtons[idx]->shortname =homebrewPath.substr(14);
        
        // since we got this app from the sd card, mark it local for now.
        // if we see it later on the server, update its status appropriately to 
        // update or installed
        homebrewButtons[idx]->status = LOCAL;
        
        log_printf("Looking in %s for an icon", (homebrewPath + iconPath).c_str());
        LoadFileToMem((homebrewPath + iconPath).c_str(), &iconData, &iconDataSize);

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
//        homebrewButtons[idx].button = new GuiButton(installedButtonImgData->getWidth(), installedButtonImgData->getHeight());
        log_printf("refreshHomebrewApps: added local button %d", idx);
//        homebrewButtons[idx].image = new GuiImage(appButtonImages[homebrewButtons[idx].status]);
        homebrewButtons[idx]->version = xmlReadSuccess? metaXml.GetVersion() : "999";
        
//        positionHomebrewButton(&homebrewButtons[idx], idx);
//        homebrewButtons[idx].button->clicked.connect(this, &HomebrewWindow::OnHomebrewButtonClick);
        
        scrollOffY = -120;

//        append(homebrewButtons[idx].button);
		
        localAppButtons.push_back(homebrewButtons[idx]);
        log_printf("totally done with and added that");
    }
    
    // add everything to the current tab
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
                		
    // download app list from the repo
    std::string targetUrl = std::string(repoUrl)+"/directory12.yaml";
    if (!gotDirectorySuccess)
    {
        log_printf("refreshHomebrewApps: Downloading remote %s", targetUrl.c_str());
        gotDirectorySuccess = FileDownloader::getFile(targetUrl, fileContents, &updateProgress);
        removeE(progressWindow);
        log_printf("refreshHomebrewApps: Updated directory");
    }

    std::istringstream f(fileContents);
    
    // totalLocalApps will represent how many apps aren't on the server
    totalLocalApps = homebrewButtons.size();
    
    u32 iterCount = 0;
    globalUpdatePosition = true;

    while (gotDirectorySuccess)
    {
        
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
        homebrewButtons[idx]->typee = HBL;

        const char *cpName = name.c_str();
//        const char *cpDescription = desc.c_str();

        if(strncmp(cpName, "sd:/wiiu/apps/", strlen("sd:/wiiu/apps/")) == 0)
           cpName += strlen("sd:/wiiu/apps/");

        homebrewButtons[idx]->nameLabel = new GuiText(cpName, 28, glm::vec4(0, 0, 0, 1));
        homebrewButtons[idx]->versionLabel = new GuiText(version.c_str(), 28, glm::vec4(0, 0, 0, 1));
        homebrewButtons[idx]->coderLabel = new GuiText(author.c_str(), 28, glm::vec4(0, 0, 0, 1));
        homebrewButtons[idx]->descriptionLabel = new GuiText(desc.c_str(), 28, glm::vec4(0, 0, 0, 1));
        log_printf("created some more attributes");
        
        // add this to the remote button array
        remoteAppButtons.push_back(homebrewButtons[idx]);
        log_printf("added %s to the remoteApps vector", shortname.c_str());

        iterCount ++;
    }

    initialLoadInProgress = false;
    globalUpdatePosition = true;
    log_printf("refreshHomebrewApps: done");
    
}

void HomebrewWindow::findHomebrewIconAndSetImage(std::string shortname, std::string targetIcon)
{
//    log_printf("findHomebrewIconAndSetImage: start");
//    for (u32 x=0; x<homebrewButtons.size(); x++)
//    {
//        log_printf("findHomebrewIconAndSetImage: checking element %d", x);
//        if (homebrewButtons[x].shortname == shortname)
//        {
//            if (targetIcon.compare("missing.png") == 0)
//                homebrewButtons[x].iconImgData = Resources::GetImageData("missing.png");
//            else
//                homebrewButtons[x].iconImgData = new GuiImageData((u8*)targetIcon.c_str(), targetIcon.size());
//            positionHomebrewButton(&homebrewButtons[x],  x);
//            break;
////            removeE(homebrewButtons[x].button);
////            append(homebrewButtons[x].button);
//        }
//    }
//    log_printf("findHomebrewIconAndSetImage: stop");
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

//void HomebrewWindow::fetchThisIcon(int x, std::string targetIconUrl)
//{
//        std::string targetIcon;        
//        FileDownloader::getFile(targetIconUrl, targetIcon);
//        cachedIcons.insert(cachedIcons.begin()+targetIcon);
//        
//        findHomebrewIconAndSetImage(remoteAppButtons[x].shortname, targetIcon);
//}

void HomebrewWindow::populateIconCache()
{
    log_printf("populateIconCache: start");
    cachedIcons.clear();
    
    for (u32 x=0; x<remoteAppButtons.size(); x++)
    {
        
        // download app icon
        std::string targetIcon;
        // find the path on the server depending on our current tab
        std::string tabPath = "apps/" + remoteAppButtons[x]->shortname + "/icon.png";
        
        std::string targetIconUrl = std::string(repoUrl)+"/"+tabPath;
        bool imageDownloadSuccessful = false;
        
        log_printf("populateIconCache: Downloading image for button %d", x);
        imageDownloadSuccessful = FileDownloader::getFile(targetIconUrl, targetIcon);
        
        if (!imageDownloadSuccessful)
            targetIcon = "missing.png";
        
        cachedIcons.push_back(targetIcon);
        
        findHomebrewIconAndSetImage(remoteAppButtons[x]->shortname, targetIcon);
        
        log_printf("populateIconCache: stop");
    }
}

HomebrewWindow::HomebrewWindow(int w, int h)
    : GuiFrame(w, h)
    , buttonClickSound(Resources::GetSound("button_click.mp3"))
    , installedButtonImgData(Resources::GetImageData("INSTALLED.png"))
    , getButtonImgData(Resources::GetImageData("GET.png"))
    , updateButtonImgData(Resources::GetImageData("UPDATE.png"))
    , localButtonImgData(Resources::GetImageData("LOCAL.png"))
	, hblVersionText("Credit: pwsincd and dimok, Music: (T-T)b     ", 32, glm::vec4(1.0f))
	, hblTabImgData(Resources::GetImageData("hbl_tab.png"))
	, rpxTabImgData(Resources::GetImageData("rpx_tab.png"))
	, hblTabImg(hblTabImgData)
	, rpxTabImg(rpxTabImgData)
	, hblTabBtn(hblTabImg.getWidth(), hblTabImg.getHeight())
	, rpxTabBtn(rpxTabImg.getWidth(), rpxTabImg.getHeight())
    , touchTrigger(GuiTrigger::CHANNEL_1, GuiTrigger::VPAD_TOUCH)
    , wpadTouchTrigger(GuiTrigger::CHANNEL_2 | GuiTrigger::CHANNEL_3 | GuiTrigger::CHANNEL_4 | GuiTrigger::CHANNEL_5, GuiTrigger::BUTTON_A)
    , buttonLTrigger(GuiTrigger::CHANNEL_ALL, GuiTrigger::BUTTON_L | GuiTrigger::BUTTON_LEFT, true)
    , buttonRTrigger(GuiTrigger::CHANNEL_ALL, GuiTrigger::BUTTON_R | GuiTrigger::BUTTON_RIGHT, true)
{
//    tcpReceiver.serverReceiveStart.connect(this, &HomebrewWindow::OnTcpReceiveStart);
//    tcpReceiver.serverReceiveFinished.connect(this, &HomebrewWindow::OnTcpReceiveFinish);

    targetLeftPosition = 0;
    currentLeftPosition = 0;
    listOffset = 0;
    gotDirectorySuccess = false;
    screenLocked = false;
	listingMode = HBL;
        
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
        
    progressWindow = new ProgressWindow("Downloading app directory...");
    std::string qualifiedName = " "+std::string(repoUrl);
        
    hblRepoText = new GuiText(qualifiedName.c_str(), 32, glm::vec4(1.0f));
        
    hblVersionText.setAlignment(ALIGN_BOTTOM | ALIGN_RIGHT);
    hblRepoText->setAlignment(ALIGN_BOTTOM | ALIGN_LEFT);
    hblVersionText.setPosition(0, 50.0f);
    hblRepoText->setPosition(0, 50.0f);
    progressWindow->setPosition(0, 30.0f);
    append(&hblVersionText);
    append(hblRepoText);

    append(progressWindow);
	hblTabBtn.setImage(&hblTabImg);
	rpxTabBtn.setImage(&rpxTabImg);
		
	hblTabBtn.setScale(0.6);
	rpxTabBtn.setScale(0.6);
		
	hblTabBtn.setAlignment(ALIGN_LEFT);
	rpxTabBtn.setAlignment(ALIGN_LEFT);
		
	hblTabBtn.setPosition(0, 85);
	rpxTabBtn.setPosition(-20, -85);
		
	hblTabBtn.setEffectGrow();
	rpxTabBtn.setEffectGrow();
		
    hblTabBtn.setTrigger(&touchTrigger);
//    hblTabBtn.setTrigger(&wpadTouchTrigger);
    hblTabBtn.setTrigger(&buttonLTrigger);
    hblTabBtn.setSoundClick(buttonClickSound);
				
    rpxTabBtn.setTrigger(&touchTrigger);
//    rpxTabBtn.setTrigger(&wpadTouchTrigger);
    rpxTabBtn.setTrigger(&buttonRTrigger);
    rpxTabBtn.setSoundClick(buttonClickSound);
		
	hblTabBtn.clicked.connect(this, &HomebrewWindow::OnHBLTabButtonClick);
    rpxTabBtn.clicked.connect(this, &HomebrewWindow::OnRPXTabButtonClick);

	append(&hblTabBtn);
	append(&rpxTabBtn);
        
//    refreshHomebrewApps();
}

HomebrewWindow::~HomebrewWindow()
{
    for(u32 i = 0; i < homebrewButtons.size(); ++i)
    {
        delete homebrewButtons[i]->image;
        delete homebrewButtons[i]->nameLabel;
        delete homebrewButtons[i]->descriptionLabel;
        delete homebrewButtons[i]->button;
        delete homebrewButtons[i]->iconImgData;
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


void HomebrewWindow::OnHBLTabButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger)
{
	if (listingMode == HBL || screenLocked) // already hbl mode
		return;
	
	listingMode = HBL;
    filter();
    globalUpdatePosition = true;
}

void HomebrewWindow::OnRPXTabButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger)
{
	if (listingMode == RPX || screenLocked) // already rpx mode
		return;
	
	listingMode = RPX;
    filter();
    globalUpdatePosition = true;
    log_printf("rpx: Done with moving rpx thing");
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
            HomebrewLaunchWindow * launchBox = new HomebrewLaunchWindow(*homebrewButtons[i], this);
            launchBox->setEffect(EFFECT_FADE, 10, 255);
            launchBox->setState(GuiElement::STATE_DISABLED);
            launchBox->setPosition(0.0f, 30.0f);
            launchBox->effectFinished.connect(this, &HomebrewWindow::OnOpenEffectFinish);
            launchBox->backButtonClicked.connect(this, &HomebrewWindow::OnLaunchBoxCloseClick);
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

        for(u32 i = 0; i < curTabButtons.size(); i++)
        {
//            log_printf("draw: adding a button at pos %d", i);
            float fXOffset = ((i % 2)? 265 : -265);
            float fYOffset = scrollOffY + (imageHeight + 20.0f) * 1.5f - (imageHeight + 15) * ((i%2)? (int)((i-1)/2) : (int)(i/2));    
            if (curTabButtons[i]->button != 0)
                curTabButtons[i]->button->setPosition(currentLeftPosition + fXOffset, fYOffset);
//            log_printf("draw: added that button %d", i);
        }
		
		if (listingMode == 1)
		{
			hblTabBtn.setPosition(0, 85);
			rpxTabBtn.setPosition(-20, -85);
		}
		else if (listingMode == 2)
		{
			hblTabBtn.setPosition(-20, 85);
			rpxTabBtn.setPosition(0, -85);
		}
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