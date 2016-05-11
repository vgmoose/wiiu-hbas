#include <string.h>
#include "dynamic_libs/os_functions.h"
#include "dynamic_libs/sys_functions.h"
#include "common/common.h"
#include "utils/utils.h"
#include "main.h"

static volatile uint8_t ucRunOnce = 0;

int __entry_menu(int argc, char **argv)
{
    //! *******************************************************************
    //! *              Check if our application is started                *
    //! *******************************************************************
    if (OSGetTitleID != 0 &&
        OSGetTitleID() != 0x000500101004A200 && // mii maker eur
        OSGetTitleID() != 0x000500101004A100 && // mii maker usa
        OSGetTitleID() != 0x000500101004A000)   // mii maker jpn
        return EXIT_RELAUNCH_ON_LOAD;

    //! check if application needs a re-load
    if(ucRunOnce) {
        return EXIT_SUCCESS;
    }
    ucRunOnce = 1;

    //! *******************************************************************
    //! *                 Jump to our application                    *
    //! *******************************************************************
    return Menu_Main();
}
