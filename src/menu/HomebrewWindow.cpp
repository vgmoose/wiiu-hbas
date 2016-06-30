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

void HomebrewWindow::positionHomebrewButton(homebrewButton* button, int index)
{
    const float cfImageScale = 0.8f;

    button->iconImg = new GuiImage(button->iconImgData);
    button->iconImg->setAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
    button->iconImg->setPosition(60, 0);
    button->iconImg->setScale(cfImageScale);
    
    button->nameLabel->setAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
    button->nameLabel->setMaxWidth(350, GuiText::SCROLL_HORIZONTAL);
    button->nameLabel->setPosition(0, 70);
    
    button->coderLabel->setAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
    button->coderLabel->setMaxWidth(170, GuiText::SCROLL_HORIZONTAL);
    button->coderLabel->setPosition(300, 20);
    
    button->versionLabel->setAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
    button->versionLabel->setMaxWidth(350, GuiText::SCROLL_HORIZONTAL);
    button->versionLabel->setPosition(300, -15);
    
    button->descriptionLabel->setAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
    button->descriptionLabel->setMaxWidth(350, GuiText::SCROLL_HORIZONTAL);
    button->descriptionLabel->setPosition(0, -60);

    // set the right image for the status
    button->image->setScale(0.9);

    button->button->setImage(button->image);
    button->button->setLabel(button->nameLabel, 0);
    button->button->setLabel(button->descriptionLabel, 1);
    button->button->setLabel(button->coderLabel, 2);
    button->button->setLabel(button->versionLabel, 3);
    button->button->setIcon(button->iconImg);
    button->button->setTrigger(&touchTrigger);
    button->button->setTrigger(&wpadTouchTrigger);
    button->button->setEffectGrow();
    //        button->button->setSoundClick(buttonClickSound);
    button->button->clicked.connect(this, &HomebrewWindow::OnHomebrewButtonClick);
}

int HomebrewWindow::checkIfUpdateOrInstalled(std::string name, std::string version, int totalLocalApps)
{
    for (int x=0; x<totalLocalApps; x++)
    {
        // if shortname matches
        if (!name.compare(homebrewButtons[x].shortname))
        {
            homebrewButtons[x].status = INSTALLED;
            if (version.compare(homebrewButtons[x].version))
            {
                // if version doesn't match
                homebrewButtons[x].status = UPDATE;
            }
            removeE(homebrewButtons[x].button);

            return x;
        }
    }
    return -1;
}

ProgressWindow* getProgressWindow()
{
    return progressWindow;
}

void updateProgress(void *arg, u32 done, u32 total)
{
    progressWindow->setProgress(100.0f* (((f32)done)/((f32)total)));
}

void HomebrewWindow::refreshHomebrewApps()
{
    GuiImageData* appButtonImages[4] = { localButtonImgData, updateButtonImgData, installedButtonImgData, getButtonImgData };

    DirList dirList("sd:/wiiu/apps", ".elf", DirList::Files | DirList::CheckSubfolders);

    for (int x=0; x<homebrewButtons.size(); x++)
    {
        removeE(homebrewButtons[x].button);  
    }
    
    homebrewButtons.clear();
    localAppButtons.clear();
    dirList.SortList();

    // load up local apps
    for(int i = 0; i < dirList.GetFilecount(); i++)
    {
        //! skip hidden linux and mac files
        if(dirList.GetFilename(i)[0] == '.' || dirList.GetFilename(i)[0] == '_')
            continue;
        
        int idx = homebrewButtons.size();
        homebrewButtons.resize(homebrewButtons.size() + 1);
                
        // file path
        homebrewButtons[idx].execPath = dirList.GetFilepath(i);
        homebrewButtons[idx].iconImgData = NULL;

        std::string homebrewPath = homebrewButtons[idx].execPath;
        size_t slashPos = homebrewPath.rfind('/');
        if(slashPos != std::string::npos)
            homebrewPath.erase(slashPos);

        u8 * iconData = NULL;
        u32 iconDataSize = 0;
        
        homebrewButtons[idx].dirPath = homebrewPath;
        
        // assume that the first part of homebrewPath is "sd:/wiiu/apps/"
        homebrewButtons[idx].shortname = homebrewPath.substr(14);
        
        // since we got this app from the sd card, mark it local for now.
        // if we see it later on the server, update its status appropriately to 
        // update or installed
        homebrewButtons[idx].status = LOCAL;

        LoadFileToMem((homebrewPath + "/icon.png").c_str(), &iconData, &iconDataSize);

        if(iconData != NULL)
        {
            homebrewButtons[idx].iconImgData = new GuiImageData(iconData, iconDataSize);
            free(iconData);
            iconData = NULL;
        }

        HomebrewXML metaXml;

        bool xmlReadSuccess = metaXml.LoadHomebrewXMLData((homebrewPath + "/meta.xml").c_str());
        
        const char *cpName = xmlReadSuccess ? metaXml.GetName() : homebrewButtons[idx].execPath.c_str();
        const char *cpDescription = xmlReadSuccess ? metaXml.GetShortDescription() : "";

        if(strncmp(cpName, "sd:/wiiu/apps/", strlen("sd:/wiiu/apps/")) == 0)
            cpName += strlen("sd:/wiiu/apps/");
        
        homebrewButtons[idx].nameLabel = new GuiText(cpName, 28, glm::vec4(0, 0, 0, 1));
        homebrewButtons[idx].versionLabel = new GuiText(metaXml.GetVersion(), 28, glm::vec4(0, 0, 0, 1));
        homebrewButtons[idx].coderLabel = new GuiText(metaXml.GetCoder(), 28, glm::vec4(0, 0, 0, 1));
        homebrewButtons[idx].descriptionLabel = new GuiText(metaXml.GetShortDescription(), 28, glm::vec4(0, 0, 0, 1));
        homebrewButtons[idx].button = new GuiButton(installedButtonImgData->getWidth(), installedButtonImgData->getHeight());
        homebrewButtons[idx].image = new GuiImage(appButtonImages[homebrewButtons[idx].status]);
        homebrewButtons[idx].version = metaXml.GetVersion();
        
        positionHomebrewButton(&homebrewButtons[idx], idx);
        
        scrollOffY = 0;

        append(homebrewButtons[idx].button);
        localAppButtons.push_back(homebrewButtons[idx]);
    }
            		
    // download app list from the repo
    std::string targetUrl = std::string(repoUrl)+"/directory.yaml";
    if (!gotDirectorySuccess)
    {
        gotDirectorySuccess = FileDownloader::getFile(targetUrl, fileContents, &updateProgress);
        removeE(progressWindow);
    }

    std::istringstream f(fileContents);
    
    totalLocalApps = homebrewButtons.size();
    int iterCount = -1;
    globalUpdatePosition = true;

    while (gotDirectorySuccess)
    {
        iterCount ++;
        
        std::string shortname;

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

        int idx = homebrewButtons.size();
        homebrewButtons.resize(homebrewButtons.size() + 1);

        // file path
        homebrewButtons[idx].execPath = "";
//        homebrewButtons[idx].iconImgData = NULL;

        std::string homebrewPath = homebrewButtons[idx].execPath;
        size_t slashPos = homebrewPath.rfind('/');
        if(slashPos != std::string::npos)
            homebrewPath.erase(slashPos);

        u8 * iconData = NULL;
        u32 iconDataSize = 0;

        homebrewButtons[idx].dirPath = homebrewPath;

        // since we got this app from the net, mark it as a GET
        homebrewButtons[idx].status = GET;
        homebrewButtons[idx].shortname = shortname;
        homebrewButtons[idx].binary = binary;
        homebrewButtons[idx].version = version;
        
        // update status if already a local app
        int addedIndex = checkIfUpdateOrInstalled(homebrewButtons[idx].shortname, homebrewButtons[idx].version, totalLocalApps);
        
        if (remoteAppButtons.size() <= iterCount)
        {
            // add this to the remote button array
            remoteAppButtons.push_back(homebrewButtons[idx]);
        }
        
        if (addedIndex >= 0)
        {
            homebrewButtons.pop_back();
            homebrewButtons[addedIndex].button = new GuiButton(installedButtonImgData->getWidth(), installedButtonImgData->getHeight());
            homebrewButtons[addedIndex].image = new GuiImage(appButtonImages[homebrewButtons[addedIndex].status]);
            append(homebrewButtons[addedIndex].button);
            positionHomebrewButton(&homebrewButtons[addedIndex], addedIndex);
            homebrewButtons[addedIndex].binary = binary;
            homebrewButtons[addedIndex].version = version;
            continue;
        }

        // download app icon
        std::string targetIcon;
//        std::string targetIconUrl = std::string(repoUrl)+"/apps/" + shortname + "/icon.png";
//        bool imageDownloadSuccessful = false;
        // try to load file from our memory cache
        if (cachedIcons.size() > iterCount)
            targetIcon = cachedIcons[iterCount];
        //FileDownloader::getFile(targetIconUrl, targetIcon);

        if (!targetIcon.empty())
            homebrewButtons[idx].iconImgData = new GuiImageData((u8*)targetIcon.c_str(), targetIcon.size());

        const char *cpName = name.c_str();
        const char *cpDescription = desc.c_str();

        if(strncmp(cpName, "sd:/wiiu/apps/", strlen("sd:/wiiu/apps/")) == 0)
           cpName += strlen("sd:/wiiu/apps/");

        homebrewButtons[idx].nameLabel = new GuiText(cpName, 28, glm::vec4(0, 0, 0, 1));
        homebrewButtons[idx].versionLabel = new GuiText(version.c_str(), 28, glm::vec4(0, 0, 0, 1));
        homebrewButtons[idx].coderLabel = new GuiText(author.c_str(), 28, glm::vec4(0, 0, 0, 1));
        homebrewButtons[idx].descriptionLabel = new GuiText(desc.c_str(), 28, glm::vec4(0, 0, 0, 1));
        homebrewButtons[idx].button = new GuiButton(installedButtonImgData->getWidth(), installedButtonImgData->getHeight());
        homebrewButtons[idx].image = new GuiImage(appButtonImages[homebrewButtons[idx].status]);

        positionHomebrewButton(&homebrewButtons[idx], idx);

        append(homebrewButtons[idx].button);
    }
    
    initialLoadInProgress = false;
    globalUpdatePosition = true;
}

void HomebrewWindow::findHomebrewIconAndSetImage(std::string shortname, std::string targetIcon)
{
    for (int x=0; x<homebrewButtons.size(); x++)
    {
        if (homebrewButtons[x].shortname == shortname)
        {
            homebrewButtons[x].iconImgData = new GuiImageData((u8*)targetIcon.c_str(), targetIcon.size());
            positionHomebrewButton(&homebrewButtons[x],  x);
            break;
//            removeE(homebrewButtons[x].button);
//            append(homebrewButtons[x].button);
        }
    }
}

bool HomebrewWindow::checkLocalAppExists(std::string shortname)
{
    for (int x=0; x<localAppButtons.size(); x++)
    {
        if (localAppButtons[x].shortname == shortname)
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
    cachedIcons.clear();
    
    for (int x=0; x<remoteAppButtons.size(); x++)
    {
        // if we already have this app
//        if (checkLocalAppExists(remoteAppButtons[x].shortname))
//        {
//            cachedIcons.push_back("");
//            continue;
//        }
        
        // download app icon
        std::string targetIcon;
        std::string targetIconUrl = std::string(repoUrl)+"/apps/" + remoteAppButtons[x].shortname + "/icon.png";
        bool imageDownloadSuccessful = false;
        
        FileDownloader::getFile(targetIconUrl, targetIcon);
        
        cachedIcons.push_back(targetIcon);
        
        findHomebrewIconAndSetImage(remoteAppButtons[x].shortname, targetIcon);
        
//        remoteAppButtons[x]->iconImgData = new GuiImageData((u8*)targetIcon.c_str(), targetIcon.size());
//        removeE(remoteAppButtons[x].button);
//        append(remoteAppButtons[x].button);
        
        // update icon for this iteration
        
    }
}

HomebrewWindow::HomebrewWindow(int w, int h)
    : GuiFrame(w, h)
    , buttonClickSound(Resources::GetSound("button_click.mp3"))
    , installedButtonImgData(Resources::GetImageData("INSTALLED.png"))
    , getButtonImgData(Resources::GetImageData("GET.png"))
    , updateButtonImgData(Resources::GetImageData("UPDATE.png"))
    , localButtonImgData(Resources::GetImageData("LOCAL.png"))
    , arrowRightImageData(Resources::GetImageData("rightArrow.png"))
    , arrowLeftImageData(Resources::GetImageData("leftArrow.png"))
    , arrowRightImage(arrowRightImageData)
    , arrowLeftImage(arrowLeftImageData)
    , arrowRightButton(arrowRightImage.getWidth(), arrowRightImage.getHeight())
    , arrowLeftButton(arrowLeftImage.getWidth(), arrowLeftImage.getHeight())
    , hblVersionText("Credit: pwsincd and dimok, Music: (T-T)b     ", 32, glm::vec4(1.0f))
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
        
//    refreshHomebrewApps();
}

HomebrewWindow::~HomebrewWindow()
{
    for(u32 i = 0; i < homebrewButtons.size(); ++i)
    {
        delete homebrewButtons[i].image;
        delete homebrewButtons[i].nameLabel;
        delete homebrewButtons[i].descriptionLabel;
        delete homebrewButtons[i].button;
        delete homebrewButtons[i].iconImgData;
        delete homebrewButtons[i].iconImg;
    }

    Resources::RemoveSound(buttonClickSound);
    Resources::RemoveImageData(installedButtonImgData);
    Resources::RemoveImageData(getButtonImgData);
    Resources::RemoveImageData(updateButtonImgData);
    Resources::RemoveImageData(localButtonImgData);
    Resources::RemoveImageData(arrowRightImageData);
    Resources::RemoveImageData(arrowLeftImageData);
}

void HomebrewWindow::OnOpenEffectFinish(GuiElement *element)
{
    //! once the menu is open reset its state and allow it to be "clicked/hold"
    element->effectFinished.disconnect(this);
    element->clearState(GuiElement::STATE_DISABLED);
}

void HomebrewWindow::OnCloseEffectFinish(GuiElement *element)
{
    //! remove element from draw list and push to delete queue
    removeE(element);
    AsyncDeleter::pushForDelete(element);

    for(u32 i = 0; i < homebrewButtons.size(); i++)
    {
        if (homebrewButtons[i].button != 0)
            homebrewButtons[i].button->clearState(GuiElement::STATE_DISABLED);
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
    if (getHasScrolled() || initialLoadInProgress) {
        return;
    }
    
    thisHomebrewWindow = this;
        
    bool disableButtons = false;
//    return;

    for(u32 i = 0; i < homebrewButtons.size(); i++)
    {
        if(button == homebrewButtons[i].button)
        {
            HomebrewLaunchWindow * launchBox = new HomebrewLaunchWindow(homebrewButtons[i], this);
            launchBox->setEffect(EFFECT_FADE, 10, 255);
            launchBox->setState(GuiElement::STATE_DISABLED);
            launchBox->setPosition(0.0f, 30.0f);
            launchBox->effectFinished.connect(this, &HomebrewWindow::OnOpenEffectFinish);
            launchBox->backButtonClicked.connect(this, &HomebrewWindow::OnLaunchBoxCloseClick);
            append(launchBox);
            disableButtons = true;
            break;
        }
    }


    if(disableButtons)
    {
        for(u32 i = 0; i < homebrewButtons.size(); i++)
        {
            if (homebrewButtons[i].button != 0)
                homebrewButtons[i].button->setState(GuiElement::STATE_DISABLED);
        }
    }
}

void HomebrewWindow::OnLeftArrowClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger)
{
    if(listOffset > 0)
    {
        listOffset--;
        targetLeftPosition = -listOffset * getWidth();

        if(listOffset == 0)
            removeE(&arrowLeftButton);
        append(&arrowRightButton);
    }
}

void HomebrewWindow::OnRightArrowClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger)
{
    if((listOffset * MAX_BUTTONS_ON_PAGE) < (int)homebrewButtons.size())
    {
        listOffset++;
        targetLeftPosition = -listOffset * getWidth();

        if(((listOffset + 1) * MAX_BUTTONS_ON_PAGE) >= (int)homebrewButtons.size())
            removeE(&arrowRightButton);

        append(&arrowLeftButton);
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
        bUpdatePositions = false;
        int imageHeight = 210;

        for(u32 i = 0; i < homebrewButtons.size(); i++)
        {
            float fXOffset = (i % 2)? 265 : -265;
            float fYOffset = scrollOffY + (imageHeight + 20.0f) * 1.5f - (imageHeight + 15) * ((i%2)? (int)((i-1)/2) : (int)(i/2));    
            if (homebrewButtons[i].button != 0)
                homebrewButtons[i].button->setPosition(currentLeftPosition + fXOffset, fYOffset);
        }
    }

    GuiFrame::draw(pVideo);

}

void refreshHomebrewAppIcons()
{
    
}

HomebrewWindow* getHomebrewWindow()
{
    return thisHomebrewWindow;
}