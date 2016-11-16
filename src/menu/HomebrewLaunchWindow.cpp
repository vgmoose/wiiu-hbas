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
#include "HomebrewLaunchWindow.h"
#include "common/common.h"
#include "fs/DirList.h"
#include "fs/fs_utils.h"
#include "utils/HomebrewXML.h"
#include "Application.h"
#include "dynamic_libs/sys_functions.h"
#include "network/FileDownloader.h"
#include <algorithm>

std::string ReplaceAll2(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    log_printf("Here's the formatted string: %s", str.c_str());
    return str;
}

HomebrewLaunchWindow::HomebrewLaunchWindow(homebrewButton & thisButton, HomebrewWindow * window)
    : GuiFrame(0, 0)
    , buttonClickSound(Resources::GetSound("button_click.mp3"))
    , backgroundImgData(Resources::GetImageData("launchMenuBox.png"))
    , backgroundImg(backgroundImgData)
    , getButtonImgData(Resources::GetImageData("GET_BUTTON.png"))
    , updateButtonImgData(Resources::GetImageData("UPDATE_BUTTON.png"))
    , deleteButtonImgData(Resources::GetImageData("DELETE_BUTTON.png"))
    , reinstallButtonImgData(Resources::GetImageData("REINSTALL_BUTTON.png"))
    , closeButtonImgData(Resources::GetImageData("CLOSE.png"))
    , iconImage(thisButton.iconImgData)
    , titleText((char*)NULL, 42, glm::vec4(0,0,0, 1))
    , versionValueText((char*)NULL, 32, glm::vec4(0,0,0, 1))
    , authorValueText((char*)NULL, 32, glm::vec4(0,0,0, 1))
    , descriptionText((char*)NULL, 28, glm::vec4(0,0,0, 1))
    , loadImg(getButtonImgData)
    , delImg(deleteButtonImgData)
    , updateImg(updateButtonImgData)
    , reinstallImg(reinstallButtonImgData)
    , backImg(closeButtonImgData)
    , loadBtn(loadImg.getWidth(), loadImg.getHeight())
    , delBtn(loadImg.getWidth(), loadImg.getHeight())
    , updateBtn(updateImg.getWidth(), updateImg.getHeight())
    , reinstallBtn(reinstallImg.getWidth(), reinstallImg.getHeight())
    , backBtn(backImg.getWidth(), backImg.getHeight())
    , touchTrigger(GuiTrigger::CHANNEL_1, GuiTrigger::VPAD_TOUCH)
    , wpadTouchTrigger(GuiTrigger::CHANNEL_2 | GuiTrigger::CHANNEL_3 | GuiTrigger::CHANNEL_4 | GuiTrigger::CHANNEL_5, GuiTrigger::BUTTON_A)
    , selectedButton(&thisButton)
    , homebrewWindow(window)
{
    width = backgroundImg.getWidth();
    height = backgroundImg.getHeight();
    backgroundImg.setPosition(0, -30);
    append(&backgroundImg);

    std::string launchPath = selectedButton->execPath;
    std::string homebrewPath = launchPath;
    size_t slashPos = homebrewPath.rfind('/');
    if(slashPos != std::string::npos)
        homebrewPath.erase(slashPos);

    HomebrewXML metaXml;
    bool xmlReadSuccess = metaXml.LoadHomebrewXMLData((homebrewPath + "/meta.xml").c_str());
        
    std::string tabPath = "apps";  
                        
    // if GET or UDPATE, fetch xml from server
    if (selectedButton->status == GET || selectedButton->status == UPDATE)
    {
        std::string xmlFetchData;
        FileDownloader::getFile(std::string(repoUrl)+"/"+tabPath+"/"+selectedButton->shortname+"/meta.xml", xmlFetchData);
        xmlReadSuccess = metaXml.LoadHomebrewXMLFromString(xmlFetchData.c_str());
        log_printf("Tried to download %s", (std::string(repoUrl)+"/"+tabPath+"/"+selectedButton->shortname+"/meta.xml").c_str());
    }

    int xOffset = 500;
    int yOffset = height * 0.5f - 75.0f - 50;

    const char *cpName = xmlReadSuccess ? metaXml.GetName() : launchPath.c_str();
    if(strncmp(cpName, "sd:/wiiu/apps/", strlen("sd:/wiiu/apps/")) == 0)
       cpName += strlen("sd:/wiiu/apps/");

    titleText.setText(cpName);
    titleText.setAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
    titleText.setPosition(0, yOffset + 10);
    titleText.setMaxWidth(width - 100, GuiText::DOTTED);
    append(&titleText);

    float scaleFactor = 1.4f;
    iconImage.setAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
    iconImage.setPosition(80, yOffset - 30 - iconImage.getHeight() * 0.5f * scaleFactor);
    iconImage.setScale(scaleFactor);
    append(&iconImage);
        
    

    yOffset -= 50;

    versionValueText.setTextf("%s", xmlReadSuccess ? metaXml.GetVersion() : launchPath.c_str());
    versionValueText.setAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
    versionValueText.setPosition(width - xOffset - 35, yOffset);
    versionValueText.setMaxWidth(xOffset - 50, GuiText::DOTTED);
    append(&versionValueText);
    yOffset -= 30;

    authorValueText.setTextf("%s", metaXml.GetCoder());
    authorValueText.setAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
    authorValueText.setPosition(width - xOffset - 35, yOffset);
    authorValueText.setMaxWidth(xOffset - 50, GuiText::DOTTED);
    append(&authorValueText);
    yOffset -= 50;

    descriptionText.setText(metaXml.GetLongDescription());
    descriptionText.setAlignment(ALIGN_LEFT | ALIGN_TOP);
    descriptionText.setPosition(100, -370);
    descriptionText.setMaxWidth(width - 200, GuiText::WRAP);
    append(&descriptionText);
        
    int actionButtonYOff = -30;
        
    log_printf("Creating the button with click events");

    if (thisButton.status == GET)
    {
        scaleFactor = 1.0f;
        loadImg.setScale(scaleFactor);
        loadBtn.setSize(scaleFactor * loadImg.getWidth(), scaleFactor * loadImg.getHeight());
        loadBtn.setImage(&loadImg);
//        loadBtn.setLabel(&loadBtnLabel);
        loadBtn.setAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
        loadBtn.setPosition(300, 160 + actionButtonYOff);
        loadBtn.setTrigger(&touchTrigger);
        loadBtn.setTrigger(&wpadTouchTrigger);
        loadBtn.setEffectGrow();
        loadBtn.setSoundClick(buttonClickSound);
        loadBtn.clicked.connect(this, &HomebrewLaunchWindow::OnLoadButtonClick);
        append(&loadBtn);
    }
        
    if (thisButton.status == UPDATE)
    {
        scaleFactor = 1.0f;
        updateImg.setScale(scaleFactor);
        updateBtn.setSize(scaleFactor * updateImg.getWidth(), scaleFactor * updateImg.getHeight());
        updateBtn.setImage(&updateImg);
        updateBtn.setAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
        updateBtn.setPosition(300, 200 + actionButtonYOff);
        updateBtn.setTrigger(&touchTrigger);
        updateBtn.setTrigger(&wpadTouchTrigger);
        updateBtn.setEffectGrow();
        updateBtn.setSoundClick(buttonClickSound);
        updateBtn.clicked.connect(this, &HomebrewLaunchWindow::OnLoadButtonClick);
        append(&updateBtn);
    }
        
    if (thisButton.status == INSTALLED)
    {
        scaleFactor = 1.0f;
        reinstallImg.setScale(scaleFactor);
        reinstallBtn.setSize(scaleFactor * reinstallImg.getWidth(), scaleFactor * reinstallImg.getHeight());
        reinstallBtn.setImage(&reinstallImg);
        reinstallBtn.setAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
        reinstallBtn.setPosition(300, 200 + actionButtonYOff);
        reinstallBtn.setTrigger(&touchTrigger);
        reinstallBtn.setTrigger(&wpadTouchTrigger);
        reinstallBtn.setEffectGrow();
        reinstallBtn.setSoundClick(buttonClickSound);
        reinstallBtn.clicked.connect(this, &HomebrewLaunchWindow::OnLoadButtonClick);
        append(&reinstallBtn);
    }
        
    if (thisButton.status != GET)
    {
		scaleFactor = 1.0f;
        delImg.setScale(scaleFactor);
        delBtn.setSize(scaleFactor * loadImg.getWidth(), scaleFactor * delImg.getHeight());
        delBtn.setImage(&delImg);
//        delBtn.setLabel(&delBtnLabel);
        delBtn.setAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
        delBtn.setPosition(300, 110 + actionButtonYOff);
        delBtn.setTrigger(&touchTrigger);
        delBtn.setTrigger(&wpadTouchTrigger);
        delBtn.setEffectGrow();
        delBtn.setSoundClick(buttonClickSound);
        delBtn.clicked.connect(this, &HomebrewLaunchWindow::OnDeleteButtonClick);
        append(&delBtn);
    }

    backImg.setScale(scaleFactor);
    backBtn.setSize(scaleFactor * backImg.getWidth(), scaleFactor * backImg.getHeight());
    backBtn.setImage(&backImg);
//    backBtn.setLabel(&backBtnLabel);
    backBtn.setAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
    backBtn.setPosition(-465, 275);
    backBtn.setTrigger(&touchTrigger);
    backBtn.setTrigger(&wpadTouchTrigger);
    backBtn.setEffectGrow();
    backBtn.setSoundClick(buttonClickSound);
    backBtn.clicked.connect(this, &HomebrewLaunchWindow::OnBackButtonClick);
    append(&backBtn);
}

HomebrewLaunchWindow::~HomebrewLaunchWindow()
{
    Resources::RemoveSound(buttonClickSound);
    Resources::RemoveImageData(backgroundImgData);
    Resources::RemoveImageData(getButtonImgData);
    Resources::RemoveImageData(updateButtonImgData);
    Resources::RemoveImageData(deleteButtonImgData);
    Resources::RemoveImageData(reinstallButtonImgData);
    Resources::RemoveImageData(closeButtonImgData);
}

void HomebrewLaunchWindow::OnOpenEffectFinish(GuiElement *element)
{
    //! once the menu is open reset its state and allow it to be "clicked/hold"
    element->effectFinished.disconnect(this);
    element->clearState(GuiElement::STATE_DISABLED);
}

void HomebrewLaunchWindow::OnCloseEffectFinish(GuiElement *element)
{
    //! remove element from draw list and push to delete queue
    removeE(element);
    AsyncDeleter::pushForDelete(element);

    backBtn.clearState(GuiElement::STATE_DISABLED);
    loadBtn.clearState(GuiElement::STATE_DISABLED);
}

void HomebrewLaunchWindow::OnFileLoadFinish(GuiElement *element, const std::string & filepath, int result)
{
    element->setState(GuiElement::STATE_DISABLED);
    element->setEffect(EFFECT_FADE, -10, 0);
    element->effectFinished.connect(this, &HomebrewLaunchWindow::OnCloseEffectFinish);
}

void HomebrewLaunchWindow::OnDeleteButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger)
{
    std::string removePath = selectedButton->dirPath;
    
    // if the remove path is the whole directory, stop!
    if (!removePath.compare(std::string("sd:/wiiu/apps")) || !removePath.compare(std::string("sd:/wiiu/apps/")))
        return;
    else
    {
        // remove the files in the directory
        DirList dirList(removePath, 0, DirList::Files | DirList::CheckSubfolders);
        for (int x=0; x<dirList.GetFilecount(); x++)
            remove(dirList.GetFilepath(x));
        
        // now remove the directory
        rmdir(removePath.c_str());
    }
    
    // close the window
    OnBackButtonClick(button, controller, trigger);
    
    // refresh main directory (crashes at the moment)
    globalRefreshHomebrewApps();

}

/**
This method is invoked after green is pressed, and is in a separate thread
so that the animation of the progress bar can be controlled.

The main issue with tis function is it needs some variables passed in order
to fetch them. Currently this is done by attaching variables to the singleton
instance of HomebrewWindow, which was set up at start.
**/
static void asyncDownloadTargetedFiles(CThread* thread, void* args)
{
    log_printf("asyncDownloadTargetedFiles: start");

    // Set the progress bar to 0%
    ProgressWindow * progress = getProgressWindow(); 
    progress->setProgress(0);
    
    // get the homebrew window, which holds variables we need access to
    HomebrewWindow * homebrewWindowTarget = getHomebrewWindow();
    
    // convert the repo url into a std::string
    std::string mRepoUrl = std::string(repoUrl);
    
    // three previously stored variables that are used belong to know which files to download
    std::string sdPathTarget = homebrewWindowTarget->sdPathTarget;
    std::string binaryTarget = homebrewWindowTarget->binaryTarget;
    std::string pathTarget = homebrewWindowTarget->pathTarget;
    
    std::string iconPath = "/icon.png";
    std::string codePath = "/" + binaryTarget;
    
    log_printf("asyncDownloadTargetedFiles: variables: repoUrl: %s, sdPath: %s, binary: %s, path: %s, icon: %s, code: %s", mRepoUrl.c_str(), sdPathTarget.c_str(), binaryTarget.c_str(), pathTarget.c_str(), iconPath.c_str(), codePath.c_str());
    
    // download the elf to sd card, and update the progres bar description
    progress->setTitle("Downloading " + sdPathTarget+codePath + "...");
    FileDownloader::getFile(mRepoUrl+pathTarget+codePath, sdPathTarget+codePath, &updateProgress);
    log_printf("asyncDownloadTargetedFiles: downloaded %s", binaryTarget.c_str());
    
    // download meta.xml to sd card, and update the progres bar description
    progress->setTitle("Downloading " + sdPathTarget+"/meta.xml...");
    FileDownloader::getFile(mRepoUrl+pathTarget+"/meta.xml", sdPathTarget+"/meta.xml", &updateProgress);
log_printf("asyncDownloadTargetedFiles: downloaded meta.xml");
    
    // download the app image icon for this app. (If the icon download is interrupted,
    // HBL may crash when it tries to read it
    log_printf("Gonna get %s and put it in %s", (mRepoUrl+pathTarget+iconPath).c_str(), (sdPathTarget+iconPath).c_str());
    progress->setTitle("Downloading " + sdPathTarget+iconPath+"...");
    FileDownloader::getFile(mRepoUrl+pathTarget+iconPath, sdPathTarget+iconPath, &updateProgress);
    log_printf("asyncDownloadTargetedFiles: downloaded the icon");
    
    // remove the progress bar
    homebrewWindowTarget->removeE(progress);
    
    // really hacky way to dismiss this window
    homebrewWindowTarget->OnLaunchBoxCloseClick(homebrewWindowTarget->launchWindowTarget);

    // refresh main directory (crashes at the moment)
    globalRefreshHomebrewApps();
    log_printf("asyncDownloadTargetedFiles: stop");
}

/**
This method is called when the green GET button is pushed. It is on the main thread, and
spawns another thread to handle the downloading in the background.
**/
void HomebrewLaunchWindow::OnLoadButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger)
{
    log_printf("OnLoadButtonClick: Load button clicked");
    // disable the buttons
    delBtn.setState(GuiElement::STATE_DISABLED);
    loadBtn.setState(GuiElement::STATE_DISABLED);
    backBtn.setState(GuiElement::STATE_DISABLED);
    updateBtn.setState(GuiElement::STATE_DISABLED);
    reinstallBtn.setState(GuiElement::STATE_DISABLED);
    
    // find the path on the server depending on our current tab
    std::string tabPath = "apps";

    // setup the paths based on the selected button
    std::string path = "/"+tabPath+"/"+selectedButton->shortname;
    std::string sdPath = ReplaceAll2("sd:/wiiu"+path, "%20", " ");
        
    // create a new directory on sd
    CreateSubfolder(sdPath.c_str());

    // get progress window and homebrew window, add progress to view
    ProgressWindow * progress = getProgressWindow(); 
    HomebrewWindow * homebrewWindowTarget = getHomebrewWindow();
    homebrewWindowTarget->append(progress);

    // store information about the desired files to homebrewWindowTarget, so that
    // the thread can access it
    homebrewWindowTarget->sdPathTarget = sdPath;
    homebrewWindowTarget->pathTarget = path;
    homebrewWindowTarget->binaryTarget = selectedButton->binary;
    
    // targets to store to dismiss the window later
//    homebrewWindowTarget->controllerTarget = controller;
//    homebrewWindowTarget->buttonTarget = button;
//    homebrewWindowTarget->triggerTarget = trigger;
    homebrewWindowTarget->launchWindowTarget = this;
    
    log_printf("OnLoadButtonClick: starting downloading thread");
    // Create a new thread to do the downloading it, so the prgress bar can be updated
    CThread * pThread = CThread::create(asyncDownloadTargetedFiles, NULL, CThread::eAttributeAffCore0 | CThread::eAttributePinnedAff, 10);
    pThread->resumeThread();
    
}
