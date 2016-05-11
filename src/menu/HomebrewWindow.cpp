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

#define DEFAULT_WIILOAD_PORT        4299

#define MAX_BUTTONS_ON_PAGE     4

HomebrewWindow::HomebrewWindow(int w, int h)
    : GuiFrame(w, h)
    , buttonClickSound(Resources::GetSound("button_click.mp3"))
    , homebrewButtonImgData(Resources::GetImageData("homebrewButton.png"))
    , arrowRightImageData(Resources::GetImageData("rightArrow.png"))
    , arrowLeftImageData(Resources::GetImageData("leftArrow.png"))
    , arrowRightImage(arrowRightImageData)
    , arrowLeftImage(arrowLeftImageData)
    , arrowRightButton(arrowRightImage.getWidth(), arrowRightImage.getHeight())
    , arrowLeftButton(arrowLeftImage.getWidth(), arrowLeftImage.getHeight())
    , hblVersionText("Homebrew Launcher " HBL_VERSION " by Dimok", 32, glm::vec4(1.0f))
    , touchTrigger(GuiTrigger::CHANNEL_1, GuiTrigger::VPAD_TOUCH)
    , wpadTouchTrigger(GuiTrigger::CHANNEL_2 | GuiTrigger::CHANNEL_3 | GuiTrigger::CHANNEL_4 | GuiTrigger::CHANNEL_5, GuiTrigger::BUTTON_A)
    , buttonLTrigger(GuiTrigger::CHANNEL_ALL, GuiTrigger::BUTTON_L | GuiTrigger::BUTTON_LEFT, true)
    , buttonRTrigger(GuiTrigger::CHANNEL_ALL, GuiTrigger::BUTTON_R | GuiTrigger::BUTTON_RIGHT, true)
    , tcpReceiver(DEFAULT_WIILOAD_PORT)
{
    tcpReceiver.serverReceiveStart.connect(this, &HomebrewWindow::OnTcpReceiveStart);
    tcpReceiver.serverReceiveFinished.connect(this, &HomebrewWindow::OnTcpReceiveFinish);

    targetLeftPosition = 0;
    currentLeftPosition = 0;
    listOffset = 0;

    DirList dirList("sd:/wiiu/apps", ".elf", DirList::Files | DirList::CheckSubfolders);

    dirList.SortList();

    for(int i = 0; i < dirList.GetFilecount(); i++)
    {
        //! skip our own application in the listing
        //!if(strcasecmp(dirList.GetFilename(i), "homebrew_launcher.elf") == 0)
        //!    continue;

        //! skip hidden linux and mac files
        if(dirList.GetFilename(i)[0] == '.' || dirList.GetFilename(i)[0] == '_')
            continue;

        int idx = homebrewButtons.size();
        homebrewButtons.resize(homebrewButtons.size() + 1);

        homebrewButtons[idx].execPath = dirList.GetFilepath(i);
        homebrewButtons[idx].image = new GuiImage(homebrewButtonImgData);
        homebrewButtons[idx].image->setScale(0.9f);
        homebrewButtons[idx].iconImgData = NULL;

        std::string homebrewPath = homebrewButtons[idx].execPath;
        size_t slashPos = homebrewPath.rfind('/');
        if(slashPos != std::string::npos)
            homebrewPath.erase(slashPos);

        u8 * iconData = NULL;
        u32 iconDataSize = 0;

        LoadFileToMem((homebrewPath + "/icon.png").c_str(), &iconData, &iconDataSize);

        if(iconData != NULL)
        {
            homebrewButtons[idx].iconImgData = new GuiImageData(iconData, iconDataSize);
            free(iconData);
            iconData = NULL;
        }

        const float cfImageScale = 1.0f;

        homebrewButtons[idx].iconImg = new GuiImage(homebrewButtons[idx].iconImgData);
        homebrewButtons[idx].iconImg->setAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
        homebrewButtons[idx].iconImg->setPosition(60, 0);
        homebrewButtons[idx].iconImg->setScale(cfImageScale);

        HomebrewXML metaXml;

        bool xmlReadSuccess = metaXml.LoadHomebrewXMLData((homebrewPath + "/meta.xml").c_str());

        const char *cpName = xmlReadSuccess ? metaXml.GetName() : homebrewButtons[idx].execPath.c_str();
        const char *cpDescription = xmlReadSuccess ? metaXml.GetShortDescription() : "";

        if(strncmp(cpName, "sd:/wiiu/apps/", strlen("sd:/wiiu/apps/")) == 0)
           cpName += strlen("sd:/wiiu/apps/");

        homebrewButtons[idx].nameLabel = new GuiText(cpName, 32, glm::vec4(1.0f));
        homebrewButtons[idx].nameLabel->setAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
        homebrewButtons[idx].nameLabel->setMaxWidth(350, GuiText::SCROLL_HORIZONTAL);
        homebrewButtons[idx].nameLabel->setPosition(256 + 80, 20);

        homebrewButtons[idx].descriptionLabel = new GuiText(cpDescription, 32, glm::vec4(1.0f));
        homebrewButtons[idx].descriptionLabel->setAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
        homebrewButtons[idx].descriptionLabel->setMaxWidth(350, GuiText::SCROLL_HORIZONTAL);
        homebrewButtons[idx].descriptionLabel->setPosition(256 + 80, -20);

        homebrewButtons[idx].button = new GuiButton(homebrewButtonImgData->getWidth(), homebrewButtonImgData->getHeight());

        homebrewButtons[idx].button->setImage(homebrewButtons[idx].image);
        homebrewButtons[idx].button->setLabel(homebrewButtons[idx].nameLabel, 0);
        homebrewButtons[idx].button->setLabel(homebrewButtons[idx].descriptionLabel, 1);
        homebrewButtons[idx].button->setIcon(homebrewButtons[idx].iconImg);
        float fXOffset = (i / MAX_BUTTONS_ON_PAGE) * width;
        float fYOffset = (homebrewButtons[idx].image->getHeight() + 20.0f) * 1.5f - (homebrewButtons[idx].image->getHeight() + 20) * (i % MAX_BUTTONS_ON_PAGE);
        homebrewButtons[idx].button->setPosition(currentLeftPosition + fXOffset, fYOffset);
        homebrewButtons[idx].button->setTrigger(&touchTrigger);
        homebrewButtons[idx].button->setTrigger(&wpadTouchTrigger);
        homebrewButtons[idx].button->setEffectGrow();
        homebrewButtons[idx].button->setSoundClick(buttonClickSound);
        homebrewButtons[idx].button->clicked.connect(this, &HomebrewWindow::OnHomebrewButtonClick);

        append(homebrewButtons[idx].button);
    }


    if((MAX_BUTTONS_ON_PAGE) < homebrewButtons.size())
    {
        arrowLeftButton.setImage(&arrowLeftImage);
        arrowLeftButton.setEffectGrow();
        arrowLeftButton.setPosition(40, 0);
        arrowLeftButton.setAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
        arrowLeftButton.setTrigger(&touchTrigger);
        arrowLeftButton.setTrigger(&wpadTouchTrigger);
        arrowLeftButton.setTrigger(&buttonLTrigger);
        arrowLeftButton.setSoundClick(buttonClickSound);
        arrowLeftButton.clicked.connect(this, &HomebrewWindow::OnLeftArrowClick);

        arrowRightButton.setImage(&arrowRightImage);
        arrowRightButton.setEffectGrow();
        arrowRightButton.setPosition(-40, 0);
        arrowRightButton.setAlignment(ALIGN_RIGHT | ALIGN_MIDDLE);
        arrowRightButton.setTrigger(&touchTrigger);
        arrowRightButton.setTrigger(&wpadTouchTrigger);
        arrowRightButton.setTrigger(&buttonRTrigger);
        arrowRightButton.setSoundClick(buttonClickSound);
        arrowRightButton.clicked.connect(this, &HomebrewWindow::OnRightArrowClick);
        append(&arrowRightButton);
    }

    hblVersionText.setAlignment(ALIGN_BOTTOM | ALIGN_RIGHT);
    hblVersionText.setPosition(-30, 30);
    append(&hblVersionText);
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
    Resources::RemoveImageData(homebrewButtonImgData);
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
    remove(element);
    AsyncDeleter::pushForDelete(element);

    for(u32 i = 0; i < homebrewButtons.size(); i++)
    {
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
    bool disableButtons = false;

    for(u32 i = 0; i < homebrewButtons.size(); i++)
    {
        if(button == homebrewButtons[i].button)
        {
            HomebrewLaunchWindow * launchBox = new HomebrewLaunchWindow(homebrewButtons[i].execPath, homebrewButtons[i].iconImgData);
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
            remove(&arrowLeftButton);
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
            remove(&arrowRightButton);

        append(&arrowLeftButton);
    }
}

void HomebrewWindow::draw(CVideo *pVideo)
{
    bool bUpdatePositions = false;

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

        for(u32 i = 0; i < homebrewButtons.size(); i++)
        {
            float fXOffset = (i / MAX_BUTTONS_ON_PAGE) * getWidth();
            float fYOffset = (homebrewButtons[i].image->getHeight() + 20.0f) * 1.5f - (homebrewButtons[i].image->getHeight() + 20) * (i % MAX_BUTTONS_ON_PAGE);
            homebrewButtons[i].button->setPosition(currentLeftPosition + fXOffset, fYOffset);
        }
    }

    GuiFrame::draw(pVideo);
}

void HomebrewWindow::OnCloseTcpReceiverFinish(GuiElement *element)
{
    //! remove element from draw list and push to delete queue
    remove(element);
    clearState(STATE_DISABLED);
}

void HomebrewWindow::OnTcpReceiveStart(GuiElement *element, u32 ip)
{
    setState(STATE_DISABLED);

    element->setEffect(EFFECT_FADE, 15, 255);
    element->effectFinished.connect(this, &HomebrewWindow::OnOpenEffectFinish);
    append(element);
}

void HomebrewWindow::OnTcpReceiveFinish(GuiElement *element, u32 ip, int result)
{
    element->setState(GuiElement::STATE_DISABLED);
    element->setEffect(EFFECT_FADE, -10, 0);
    element->effectFinished.connect(this, &HomebrewWindow::OnCloseTcpReceiverFinish);

    if(result > 0)
    {
        u32 ApplicationMemoryEnd;
        asm volatile("lis %0, __CODE_END@h; ori %0, %0, __CODE_END@l" : "=r" (ApplicationMemoryEnd));

        ELF_DATA_ADDR = ApplicationMemoryEnd;
        ELF_DATA_SIZE = result;
        Application::instance()->quit(EXIT_SUCCESS);
    }
}

