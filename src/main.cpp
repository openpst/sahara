/**
*
* (c) Gassan Idriss <ghassani@gmail.com>
* 
* This file is part of sahara.
* 
* sahara is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* sahara is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with sahara. If not, see <http://www.gnu.org/licenses/>.
*
* @file sahara.cpp
* @package openpst/sahara
* @brief sahara program entry point
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#include <QGuiApplication>

#include "application.h"
#include "sahara_window.h"

/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    OpenPST::GUI::Application application(argc, argv, "sahara_");

    OpenPST::GUI::SaharaWindow window;

    window.show();

    return application.exec();
}
