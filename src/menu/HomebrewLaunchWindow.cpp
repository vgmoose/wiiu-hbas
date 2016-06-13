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

HomebrewLaunchWindow::HomebrewLaunchWindow(homebrewButton & thisButton)
    : GuiFrame(0, 0)
    , buttonClickSound(Resources::GetSound("button_click.mp3"))
    , backgroundImgData(Resources::GetImageData("launchMenuBox.png"))
    , backgroundImg(backgroundImgData)
    , buttonImgData(Resources::GetImageData("button.png"))
    , iconImage(thisButton.iconImgData)
    , titleText((char*)NULL, 42, glm::vec4(0,0,0, 1))
    , versionText("Version:", 32, glm::vec4(0,0,0, 1))
    , versionValueText((char*)NULL, 32, glm::vec4(0,0,0, 1))
    , authorText("Author:", 32, glm::vec4(0,0,0, 1))
    , authorValueText((char*)NULL, 32, glm::vec4(0,0,0, 1))
    , descriptionText((char*)NULL, 28, glm::vec4(0,0,0, 1))
    , loadBtnLabel("Download", 32, glm::vec4(1.0f))
    , delBtnLabel("Delete", 32, glm::vec4(1.0f))
    , loadImg(buttonImgData)
    , delImg(buttonImgData)
    , loadBtn(loadImg.getWidth(), loadImg.getHeight())
    , delBtn(loadImg.getWidth(), loadImg.getHeight())
    , backBtnLabel("Close", 32, glm::vec4(1.0f))
    , backImg(buttonImgData)
    , backBtn(backImg.getWidth(), backImg.getHeight())
    , touchTrigger(GuiTrigger::CHANNEL_1, GuiTrigger::VPAD_TOUCH)
    , wpadTouchTrigger(GuiTrigger::CHANNEL_2 | GuiTrigger::CHANNEL_3 | GuiTrigger::CHANNEL_4 | GuiTrigger::CHANNEL_5, GuiTrigger::BUTTON_A)
    , selectedButton(thisButton)
{
    width = backgroundImg.getWidth();
    height = backgroundImg.getHeight();
    append(&backgroundImg);

    std::string launchPath = selectedButton.execPath;
    std::string homebrewPath = launchPath;
    size_t slashPos = homebrewPath.rfind('/');
    if(slashPos != std::string::npos)
        homebrewPath.erase(slashPos);

    HomebrewXML metaXml;
    bool xmlReadSuccess = metaXml.LoadHomebrewXMLData((homebrewPath + "/meta.xml").c_str());

    int xOffset = 500;
    int yOffset = height * 0.5f - 75.0f;

    const char *cpName = xmlReadSuccess ? metaXml.GetName() : launchPath.c_str();
    if(strncmp(cpName, "sd:/wiiu/apps/", strlen("sd:/wiiu/apps/")) == 0)
       cpName += strlen("sd:/wiiu/apps/");

    titleText.setText(cpName);
    titleText.setAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
    titleText.setPosition(0, yOffset);
    titleText.setMaxWidth(width - 100, GuiText::DOTTED);
    append(&titleText);

    float scaleFactor = 1.0f;
    iconImage.setAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
    iconImage.setPosition(100, yOffset - 30 - iconImage.getHeight() * 0.5f * scaleFactor);
    iconImage.setScale(scaleFactor);
    append(&iconImage);

    yOffset -= 50;

    versionText.setAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
    versionText.setPosition(width - xOffset, yOffset);
    append(&versionText);

    versionValueText.setTextf("%s", xmlReadSuccess ? metaXml.GetVersion() : launchPath.c_str());
    versionValueText.setAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
    versionValueText.setPosition(width - xOffset + 100, yOffset);
    versionValueText.setMaxWidth(xOffset - 150, GuiText::DOTTED);
    append(&versionValueText);
    yOffset -= 30;

    authorText.setAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
    authorText.setPosition(width - xOffset, yOffset);
    append(&authorText);

    authorValueText.setTextf("%s", metaXml.GetCoder());
    authorValueText.setAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
    authorValueText.setPosition(width - xOffset + 100, yOffset);
    authorValueText.setMaxWidth(xOffset - 150, GuiText::DOTTED);
    append(&authorValueText);
    yOffset -= 50;

    descriptionText.setText(metaXml.GetLongDescription());
    descriptionText.setAlignment(ALIGN_LEFT | ALIGN_TOP);
    descriptionText.setPosition(100, -250);
    descriptionText.setMaxWidth(width - 200, GuiText::WRAP);
    append(&descriptionText);

    scaleFactor = 1.0f;
    loadImg.setScale(scaleFactor);
    loadBtn.setSize(scaleFactor * loadImg.getWidth(), scaleFactor * loadImg.getHeight());
    loadBtn.setImage(&loadImg);
    loadBtn.setLabel(&loadBtnLabel);
    loadBtn.setAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
    loadBtn.setPosition(-600, -310);
    loadBtn.setTrigger(&touchTrigger);
    loadBtn.setTrigger(&wpadTouchTrigger);
    loadBtn.setEffectGrow();
    loadBtn.setSoundClick(buttonClickSound);
    loadBtn.clicked.connect(this, &HomebrewLaunchWindow::OnLoadButtonClick);
    append(&loadBtn);
        
    delImg.setScale(scaleFactor);
    delBtn.setSize(scaleFactor * loadImg.getWidth(), scaleFactor * delImg.getHeight());
    delBtn.setImage(&delImg);
    delBtn.setLabel(&delBtnLabel);
    delBtn.setAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
    delBtn.setPosition(-200, -310);
    delBtn.setTrigger(&touchTrigger);
    delBtn.setTrigger(&wpadTouchTrigger);
    delBtn.setEffectGrow();
    delBtn.setSoundClick(buttonClickSound);
    delBtn.clicked.connect(this, &HomebrewLaunchWindow::OnDeleteButtonClick);
    append(&delBtn);

    backImg.setScale(scaleFactor);
    backBtn.setSize(scaleFactor * backImg.getWidth(), scaleFactor * backImg.getHeight());
    backBtn.setImage(&backImg);
    backBtn.setLabel(&backBtnLabel);
    backBtn.setAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
    backBtn.setPosition(200, -310);
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
    Resources::RemoveImageData(buttonImgData);
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

    if(result > 0)
    {
        u32 ApplicationMemoryEnd;
        asm volatile("lis %0, __CODE_END@h; ori %0, %0, __CODE_END@l" : "=r" (ApplicationMemoryEnd));

        ELF_DATA_ADDR = ApplicationMemoryEnd;
        ELF_DATA_SIZE = result;
        Application::instance()->quit(EXIT_SUCCESS);
    }
}

void HomebrewLaunchWindow::OnDeleteButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger)
{
    std::string removePath = selectedButton.dirPath;
    // if the remove path is the whole directory, stop!
    if (!removePath.compare(std::string("sd:/wiiu/apps")) || !removePath.compare(std::string("sd:/wiiu/apps/")))
        return;
    else
    {
        // remove the files in the directory and the directory
        DirList dirList(removePath, 0, DirList::Files | DirList::CheckSubfolders);
        for (int x=0; x<dirList.GetFilecount(); x++)
            remove(dirList.GetFilepath(x));
        rmdir(removePath.c_str());
    }
    
    // close the window
    OnBackButtonClick(button, controller, trigger);
}

void HomebrewLaunchWindow::OnLoadButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger)
{
//    backBtn.setState(GuiElement::STATE_DISABLED);
    delBtn.setState(GuiElement::STATE_DISABLED);
    loadBtn.setState(GuiElement::STATE_DISABLED);
    
    std::string path = "/apps/"+selectedButton.shortname;
    std::string sdPath = "sd:/wiiu"+path;
    CreateSubfolder(sdPath.c_str());
    std::string repoUrl = "http://192.168.1.104:8000";
    FileDownloader::getFile(repoUrl+path+"/"+selectedButton.binary, sdPath+"/"+selectedButton.binary, 0);
    FileDownloader::getFile(repoUrl+path+"/meta.xml", sdPath+"/meta.xml", 0);
    FileDownloader::getFile(repoUrl+path+"/icon.png", sdPath+"/icon.png", 0);

    // close the window
    OnBackButtonClick(button, controller, trigger);

//    struct SYSBrowserArgsIn args = {};
//    std::string url = "http://vgmoose.com";
//    args.url = url.c_str();
//    args.urlSize = url.size();
//    SYSSwitchToBrowser(&args);
    
    
//    u32 ApplicationMemoryEnd;
//    asm volatile("lis %0, __CODE_END@h; ori %0, %0, __CODE_END@l" : "=r" (ApplicationMemoryEnd));

//    HomebrewLoader * loader = HomebrewLoader::loadToMemoryAsync(homebrewLaunchPath, (unsigned char*)ApplicationMemoryEnd);
//    loader->setEffect(EFFECT_FADE, 15, 255);
//    loader->effectFinished.connect(this, &HomebrewLaunchWindow::OnOpenEffectFinish);
//    loader->asyncLoadFinished.connect(this, &HomebrewLaunchWindow::OnFileLoadFinish);
//    append(loader);
}
