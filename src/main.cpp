//#include <string>
#include "Application.h"
#include "fs/FSUtils.h"
#include "utils/utils.h"
#include "common/common.h"

int main(int argc, char **argv)
{

    printf("Start main application\n");
    Application::instance()->exec();
    printf("Main application stopped\n");
    Application::destroyInstance();

    return 0;
}

