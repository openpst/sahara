/**
* LICENSE PLACEHOLDER
*
* @file sahara.cpp
* @package OpenPST
* @brief sahara program entry point
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#include "gui/application.h"
#include "gui/sahara_window.h"

/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
    OpenPST::GUI::Application application(argc, argv, "sahara_");

    OpenPST::GUI::SaharaWindow window;

    window.show();

    return application.exec();
}