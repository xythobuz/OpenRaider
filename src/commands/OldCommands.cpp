/*!
 * \file src/Command.cpp
 * \brief OpenRaider command implementation
 *
 * \author xythobuz
 */

#include <fstream>
#include <sstream>

#include "global.h"
#include "Camera.h"
#include "Console.h"
#include "Entity.h"
#include "Font.h"
#include "Game.h"
#include "math/math.h"
#include "Menu.h"
#include "Render.h"
#include "Sound.h"
#include "TombRaider.h"
#include "Window.h"
#include "World.h"
#include "utils/strings.h"
#include "utils/time.h"
#include "OpenRaider.h"

int OpenRaider::command(std::string c) {
    // Remove comment, if any
    size_t comment = c.find_first_of('#');
    if (comment != std::string::npos)
        c.erase(comment);

    // Execute command
    std::stringstream command(c);
    std::string cmd;
    command >> cmd;
    command >> std::boolalpha >> std::ws;

    if (cmd.length() == 0)
        return 0;

    if (cmd.compare("help") == 0) {
        std::string tmp;
        if (!(command >> tmp)) {
/*
            getConsole() << "  ralpha    - BOOL - Room Alpha" << Console::endl;
            getConsole() << "  upf       - BOOL - Update Room List Per Frame" << Console::endl;
            getConsole() << "  entmodel  - BOOL" << Console::endl;
            getConsole() << "  ponytail  - BOOL" << Console::endl;
            getConsole() << "  pigtail   - BOOL" << Console::endl;
            getConsole() << "  ponypos   - FLOAT FLOAT FLOAT FLOAT - x y z angle" << Console::endl;
*/
        } else {
            return help(tmp);
        }
/*
    } else if (cmd.compare("ralpha") == 0) {
        if (args->size() > 0) {
            bool b;
            if (readBool(args->at(0), &b) < 0) {
                getConsole().print("Pass BOOL to ralpha command!");
                return -24;
            }
            if (b)
                getRender().setFlags(Render::fRoomAlpha);
            else
                getRender().clearFlags(Render::fRoomAlpha);
            getConsole().print("Room Alpha is now %s", b ? "on" : "off");
        } else {
            getConsole().print("Invalid use of ralpha-command!");
            return -25;
        }
    } else if (cmd.compare("upf") == 0) {
        if (args->size() > 0) {
            bool b;
            if (readBool(args->at(0), &b) < 0) {
                getConsole().print("Pass BOOL to upf command!");
                return -30;
            }
            if (b)
                getRender().setFlags(Render::fUpdateRoomListPerFrame);
            else
                getRender().clearFlags(Render::fUpdateRoomListPerFrame);
            getConsole().print("URLPF is now %s", b ? "on" : "off");
        } else {
            getConsole().print("Invalid use of upf-command!");
            return -31;
        }
    } else if (cmd.compare("entmodel") == 0) {
        if (args->size() > 0) {
            bool b;
            if (readBool(args->at(0), &b) < 0) {
                getConsole().print("Pass BOOL to entmodel command!");
                return -38;
            }
            if (b)
                getRender().setFlags(Render::fEntityModels);
            else
                getRender().clearFlags(Render::fEntityModels);
            getConsole().print("Entmodels are now %s", b ? "on" : "off");
        } else {
            getConsole().print("Invalid use of entmodel-command!");
            return -39;
        }
    } else if (cmd.compare("ponytail") == 0) {
        if (args->size() > 0) {
            bool b;
            if (readBool(args->at(0), &b) < 0) {
                getConsole().print("Pass BOOL to ponytail command!");
                return -44;
            }
            if (b)
                getRender().setFlags(Render::fRenderPonytail);
            else
                getRender().clearFlags(Render::fRenderPonytail);
            getConsole().print("Ponytail is now %s", b ? "on" : "off");
        } else {
            getConsole().print("Invalid use of ponytail-command!");
            return -45;
        }
    } else if (cmd.compare("pigtail") == 0) {
        if ((!mRunning) || (!getGame().isLoaded())) {
            getConsole().print("Use pigtail command interactively!");
            return -999;
        }
        if (args->size() > 0) {
            bool b;
            if (readBool(args->at(0), &b) < 0) {
                getConsole().print("Pass BOOL to pigtail command!");
                return -46;
            }
            SkeletalModel &tmp = getGame().getLara().getModel();
            tmp.setPigTail(b);
            getConsole().print("Pigtail is now %s", b ? "on" : "off");
        } else {
            getConsole().print("Invalid use of pigtail-command!");
            return -47;
        }
    } else if (cmd.compare("ponypos") == 0) {
        if ((!mRunning) || (!getGame().isLoaded())) {
            getConsole().print("Use ponypos command interactively!");
            return -999;
        }
        if (args->size() > 3) {
            SkeletalModel &tmp = getGame().getLara().getModel();
            tmp.setPonyPos((float)atof(args->at(0)), (float)atof(args->at(1)),
                    (float)atof(args->at(2)), (float)atof(args->at(3)));
        } else {
            getConsole().print("Invalid use of ponypos-command!");
            return -48;
        }
*/
    } else {
        getConsole() << "Unknown command: " << cmd.c_str() << Console::endl;
        return -50;
    }

    return 0;
}

