/**
 * This class implements the DPAD mode for the GamePad, allowing you to use the DPAD to move a virtual
 * on screen pointer, rather than using the touch screen. The program will not be able to detect
 * any DPAD/A button presses in this mode, as it may interfere with the user who is navigating the pointer.
 *
 * Created by CreeperMario in July 2017.
 */

#ifndef DPAD_CONTROLLER_H_
#define DPAD_CONTROLLER_H_

#include <gui/GuiController.h>

class DVPadController : public GuiController
{
public:
    
    //!Constructor
    DVPadController(int channel) : GuiController(channel)
    {
        memset(&vpad, 0, sizeof(PADData));
        memset(&data, 0, sizeof(PadData));
        memset(&lastData, 0, sizeof(PadData));
        
        data.validPointer = true;
//        isDPadMode = true;
//        showPointer = true;
    }
    
    //!Destructor
    virtual ~DVPadController() {}
    
    //Remove the DPAD buttons (by clearing their bits) so that they aren't used by the Gui processes.
    uint32_t fixButtons(uint32_t buttons)
    {
        buttons &= ~GuiTrigger::BUTTON_LEFT;
        buttons &= ~GuiTrigger::BUTTON_RIGHT;
        buttons &= ~GuiTrigger::BUTTON_UP;
        buttons &= ~GuiTrigger::BUTTON_DOWN;
        buttons &= ~GuiTrigger::BUTTON_A;
        return buttons;
    }
    
    bool update(int width, int height)
    {
        lastData = data;
        
        s32 vpadError = -1;
        VPADRead(0, &vpad, 1, &vpadError);
        
        if(vpadError == 0)
        {
            if(vpad.btns_h & GuiTrigger::BUTTON_LEFT)
            {
                if(data.x > -(width / 2)) data.x -= 10;
            }
            if(vpad.btns_h & GuiTrigger::BUTTON_RIGHT)
            {
                if(data.x < (width / 2)) data.x += 10;
            }
            if(vpad.btns_h & GuiTrigger::BUTTON_UP)
            {
                if(data.y < (height / 2)) data.y += 10;
            }
            if(vpad.btns_h & GuiTrigger::BUTTON_DOWN)
            {
                if(data.y > -(height / 2)) data.y -= 10;
            }
            
            if(vpad.btns_h & GuiTrigger::BUTTON_A)
                data.touched = true;
            else
                data.touched = false;
            
            data.buttons_r = fixButtons(vpad.btns_r);
            data.buttons_h = fixButtons(vpad.btns_h);
            data.buttons_d = fixButtons(vpad.btns_d);
//            data.lstick = vpad.lstick;
//            data.rstick = vpad.rstick;
            
            return true;
        }
        
        return false;
    }
    
private:
    VPADData vpad;
};

#endif
