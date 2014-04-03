/*!
 * \file include/main.h
 * \brief Service locator
 *
 * \author xythobuz
 */
#ifndef _MAIN_H_
#define _MAIN_H_

#include "Console.h"
#include "Game.h"
#include "Menu.h"
#include "OpenRaider.h"
#include "Sound.h"
#include "Window.h"
#include "World.h"

Console &getConsole();

Game &getGame();

Menu &getMenu();

OpenRaider &getOpenRaider();

Sound &getSound();

Window &getWindow();

World &getWorld();

/*!
 * \brief Program entry point
 * \param argc number of arguments
 * \param argv array with argc strings
 * \returns 0 on success
 */
int main(int argc, char *argv[]);

#endif

