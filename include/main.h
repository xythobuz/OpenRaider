/*!
 * \file include/main.h
 * \brief Where main() is
 *
 * \author xythobuz
 */
#ifndef _MAIN_H_
#define _MAIN_H_

#include "Console.h"
#include "Game.h"
#include "Menu.h"
#include "OpenRaider.h"
#include "Window.h"

Console    &getConsole();
Game       &getGame();
Menu       &getMenu();
OpenRaider &getOpenRaider();
Window     &getWindow();

/*!
 * \brief Program entry point
 * \param argc number of arguments
 * \param argv array with argc strings
 * \returns 0 on success
 */
int main(int argc, char *argv[]);

#endif

