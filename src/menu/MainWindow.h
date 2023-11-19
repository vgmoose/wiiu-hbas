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
#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

#include <vector>
#include <queue>
#include "HomebrewWindow.h"

extern HomebrewWindow * homebrewWindow;
extern int movedALittleBit;
extern int scrolledSoFar;

extern void scrollMenu(float scrol);
extern void asyncRefreshHomebrewAppIcons(void* thread, void* args);
extern void asyncRefreshHomebrewApps(void* thread, void* args);
extern void globalRefreshHomebrewApps();


class MainWindow : public sigslot::has_slots<>
{
public:
    MainWindow(int w, int h);
    virtual ~MainWindow();
    int width, height;

    GuiImage bgImageColor;
    
    bool showingSplashScreen = true;
    bool disableSplashScreenNextUpdate;
    GuiTextureData * backgroundImg2Data;
    GuiImage backgroundImg2;
	bool joysticksDisabled = false;

        
    GuiTextureData * splashScreenImgData;
    GuiImage splashScreen;
    
    // CThread * initialLoadThread;

    void appendTv(GuiElement *e)
    {
        if(!e)
            return;

        removeTv(e);
        tvElements.push_back(e);
    }
    void appendDrc(GuiElement *e)
    {
        if(!e)
            return;

        removeDrc(e);
        drcElements.push_back(e);
    }

    void append(GuiElement *e)
    {
        appendTv(e);
        appendDrc(e);
    }

    void insertTv(uint32_t pos, GuiElement *e)
    {
        if(!e)
            return;

        removeTv(e);
        tvElements.insert(tvElements.begin() + pos, e);
    }
    void insertDrc(uint32_t pos, GuiElement *e)
    {
        if(!e)
            return;

        removeDrc(e);
        drcElements.insert(drcElements.begin() + pos, e);
    }

    void insert(uint32_t pos, GuiElement *e)
    {
        insertTv(pos, e);
        insertDrc(pos, e);
    }

    void removeTv(GuiElement *e)
    {
        for(uint32_t i = 0; i < tvElements.size(); ++i)
        {
            if(e == tvElements[i])
            {
                tvElements.erase(tvElements.begin() + i);
                break;
            }
        }
    }
    void removeDrc(GuiElement *e)
    {
        for(uint32_t i = 0; i < drcElements.size(); ++i)
        {
            if(e == drcElements[i])
            {
                drcElements.erase(drcElements.begin() + i);
                break;
            }
        }
    }

    void removeE(GuiElement *e)
    {
        removeTv(e);
        removeDrc(e);
    }
    void removeAll()
    {
        tvElements.clear();
        drcElements.clear();
    }

    void drawDrc(Renderer *video);
    void drawTv(Renderer *video);
    void update(GuiController *controller);
    void updateEffects();
    float lastTouchX2 = 0;
private:
    std::vector<GuiElement *> drcElements;
    std::vector<GuiElement *> tvElements;


    GuiTextureData *pointerImgData[5];
    GuiImage *pointerImg[5];
    bool pointerValid[5];
};
extern int getHasScrolled();


#endif //_MAIN_WINDOW_H_
