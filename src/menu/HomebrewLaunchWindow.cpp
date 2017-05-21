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
#include "HomebrewLoader.h"
#include "common/common.h"
#include "fs/DirList.h"
#include "fs/fs_utils.h"
#include "utils/HomebrewXML.h"
#include "Application.h"
#include "dynamic_libs/sys_functions.h"
#include "network/FileDownloader.h"
#include "utils/Zip.h"
#include "utils/HomebrewManager.h"
#include <algorithm>

HomebrewLaunchWindow::HomebrewLaunchWindow(homebrewButton & thisButton, HomebrewWindow * window)
    : GuiFrame(0, 0)
    , buttonClickSound(Resources::GetSound("button_click.mp3"))
    , backgroundImgData(Resources::GetImageData("launchMenuBox.png"))
    , backgroundImg(backgroundImgData)
    , getButtonImgData(Resources::GetImageData("GET_BUTTON.png"))
    , updateButtonImgData(Resources::GetImageData("UPDATE_BUTTON.png"))
    , deleteButtonImgData(Resources::GetImageData("DELETE_BUTTON.png"))
    , reinstallButtonImgData(Resources::GetImageData("REINSTALL_BUTTON.png"))
    , openButtonImgData(Resources::GetImageData("OPEN_BUTTON.png"))
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
    , openImg(openButtonImgData)
    , backImg(closeButtonImgData)
    , loadBtn(loadImg.getWidth(), loadImg.getHeight())
    , delBtn(loadImg.getWidth(), loadImg.getHeight())
    , updateBtn(updateImg.getWidth(), updateImg.getHeight())
    , reinstallBtn(reinstallImg.getWidth(), reinstallImg.getHeight())
    , openBtn(openImg.getWidth(), openImg.getHeight())
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
 
		openImg.setScale(scaleFactor);
        openBtn.setSize(scaleFactor * openImg.getWidth(), scaleFactor * openImg.getHeight());
        openBtn.setImage(&openImg);
        openBtn.setAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
        openBtn.setPosition(65, 110 + actionButtonYOff);
        openBtn.setTrigger(&touchTrigger);
        openBtn.setTrigger(&wpadTouchTrigger);
        openBtn.setEffectGrow();
        openBtn.setSoundClick(buttonClickSound);
        openBtn.clicked.connect(this, &HomebrewLaunchWindow::OnOpenButtonClick);
        append(&openBtn);
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
	Resources::RemoveImageData(openButtonImgData);
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
/*
void HomebrewLaunchWindow::OnFileLoadFinish(GuiElement *element, const std::string & filepath, int result)
{
    element->setState(GuiElement::STATE_DISABLED);
    element->setEffect(EFFECT_FADE, -10, 0);
    element->effectFinished.connect(this, &HomebrewLaunchWindow::OnCloseEffectFinish);
}

*/

void HomebrewLaunchWindow::OnDeleteButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger)
{
	//! Delete
	HomebrewManager * DeleteHomebrew = new HomebrewManager(selectedButton->shortname);
	DeleteHomebrew->Delete();
	delete DeleteHomebrew;
    
    OnBackButtonClick(button, controller, trigger); // Close the window

    //! refresh main directory
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
    ProgressWindow * progress = getProgressWindow(); 
	
    //! Get the homebrew window, which holds variables we need access to
    HomebrewWindow * homebrewWindowTarget = getHomebrewWindow();
    std::string appShortName = homebrewWindowTarget->appShortName;
	
	//! Install
	HomebrewManager * InstallHomebrew = new HomebrewManager(appShortName, progress);
	InstallHomebrew->Install();
	delete InstallHomebrew;
	
	
    homebrewWindowTarget->removeE(progress); // Remove the progress bar
    homebrewWindowTarget->OnLaunchBoxCloseClick(homebrewWindowTarget->launchWindowTarget); // Really hacky way to dismiss this window

    //! refresh main directory
    globalRefreshHomebrewApps();
    log_printf("asyncDownloadTargetedFiles: stop");
}

void HomebrewLaunchWindow::OnFileLoadFinish(GuiElement *element, const std::string & filepath, int result)
{
    element->setState(GuiElement::STATE_DISABLED);
    element->setEffect(EFFECT_FADE, -10, 0);
    element->effectFinished.connect(this, &HomebrewLaunchWindow::OnCloseEffectFinish);

    if(result > 0)
    {
        Application::instance()->quit(EXIT_SUCCESS);
    }
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
    openBtn.setState(GuiElement::STATE_DISABLED);
    // get progress window and homebrew window, add progress to view
    ProgressWindow * progress = getProgressWindow(); 
    HomebrewWindow * homebrewWindowTarget = getHomebrewWindow();
    homebrewWindowTarget->append(progress);
	
    homebrewWindow->backTabBtn.setState(GuiElement::STATE_DISABLED);
    homebrewWindow->randomTabBtn.setState(GuiElement::STATE_DISABLED);

    // store information about the desired file to homebrewWindowTarget, so that
    // the thread can access it
    homebrewWindowTarget->appShortName = selectedButton->shortname;
    homebrewWindowTarget->appBinary    = selectedButton->binary;
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

void HomebrewLaunchWindow::OnOpenButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger)
{
    log_printf("OnLoadButtonClick: Load button clicked");
    // disable the buttons
    delBtn.setState(GuiElement::STATE_DISABLED);
    loadBtn.setState(GuiElement::STATE_DISABLED);
    backBtn.setState(GuiElement::STATE_DISABLED);
    updateBtn.setState(GuiElement::STATE_DISABLED);
    reinstallBtn.setState(GuiElement::STATE_DISABLED);
    openBtn.setState(GuiElement::STATE_DISABLED);

    HomebrewLoader * loader = HomebrewLoader::loadToMemoryAsync(selectedButton->execPath);
    loader->setEffect(EFFECT_FADE, 15, 255);
    loader->asyncLoadFinished.connect(this, &HomebrewLaunchWindow::OnFileLoadFinish);
    append(loader);
}


    
