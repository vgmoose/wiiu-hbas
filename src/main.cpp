//#include <string>
#include "Application.h"
#include <dynamic_libs/os_functions.h>
#include <dynamic_libs/fs_functions.h>
#include <dynamic_libs/gx2_functions.h>
#include <dynamic_libs/sys_functions.h>
#include <dynamic_libs/vpad_functions.h>
#include <dynamic_libs/padscore_functions.h>
#include <dynamic_libs/socket_functions.h>
#include <dynamic_libs/ax_functions.h>
#include <fs/FSUtils.h>
#include <fs/sd_fat_devoptab.h>
#include <system/memory.h>
#include <utils/logger.h>
#include <utils/utils.h>
#include "common/common.h"
#include <dynamic_libs/curl_functions.h>
#include "custom/exception.h"

/* Entry point */
extern "C" int Menu_Main(void)
{
    //!*******************************************************************
    //!                   Initialize function pointers                   *
    //!*******************************************************************
    //! do OS (for acquire) and sockets first so we got logging
    InitOSFunctionPointers();
    InitSocketFunctionPointers();

    log_init();
    log_print("Starting launcher\n");

    InitFSFunctionPointers();
    InitGX2FunctionPointers();
    InitSysFunctionPointers();
    InitVPadFunctionPointers();
    InitPadScoreFunctionPointers();
    InitAXFunctionPointers();
    InitCurlFunctionPointers();
    InstallExceptionHandler();

    log_print("Function exports loaded\n");

    //!*******************************************************************
    //!                    Initialize heap memory                        *
    //!*******************************************************************
    log_print("Initialize memory management\n");
    memoryInitialize();

    //!*******************************************************************
    //!                        Initialize FS                             *
    //!*******************************************************************
    log_printf("Mount SD partition\n");
    mount_sd_fat("sd");

    //!*******************************************************************
    //!                    Enter main application                        *
    //!*******************************************************************
    log_printf("Start main application\n");
    Application::instance()->exec();
    log_printf("Main application stopped\n");

    Application::destroyInstance();

    log_printf("Unmount SD\n");
    unmount_sd_fat("sd");
    log_printf("Release memory\n");
    memoryRelease();
//    log_deinit();

    return 0;
}

