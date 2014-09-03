/*!
 * \file src/commands/CommandAnimate.cpp
 * \brief Animate command
 *
 * \author xythobuz
 */

#include "global.h"
#include "Console.h"
#include "Game.h"
#include "Render.h"
#include "World.h"
#include "commands/CommandAnimate.h"

std::string CommandAnimate::name() {
    return "animate";
}

std::string CommandAnimate::brief() {
    return "[BOOL|n|p] - Animate models";
}

void CommandAnimate::printHelp() {
    getConsole() << "animate-Command Usage:" << Console::endl;
    getConsole() << "  animate [n|p|BOOL]" << Console::endl;
    getConsole() << "Where the commands have the following meaning:" << Console::endl;
    getConsole() << "  BOOL to (de)activate animating all models" << Console::endl;
    getConsole() << "  n to step all models to their next animation" << Console::endl;
    getConsole() << "  p to step all models to their previous animation" << Console::endl;
}

int CommandAnimate::execute(std::istream& args) {
    if (!getGame().isLoaded()) {
        getConsole() << "Use animate command interactively!" << Console::endl;
        return -1;
    }

    if (args.peek() == 'n') {
        // Step all skeletal models to their next animation
        if (getRender().getFlags() & Render::fAnimateAllModels) {
            for (unsigned int i = 0; i < getWorld().sizeEntity(); i++) {
                Entity &e = getWorld().getEntity(i);
                SkeletalModel &m = e.getModel();
                if (e.getAnimation() < (m.size() - 1))
                    e.setAnimation(e.getAnimation() + 1);
                else
                    e.setAnimation(0);
            }
        } else {
            getConsole() << "Animations need to be enabled!" << Console::endl;
        }
    } else if (args.peek() == 'p') {
        // Step all skeletal models to their previous animation
        if (getRender().getFlags() & Render::fAnimateAllModels) {
            for (unsigned int i = 0; i < getWorld().sizeEntity(); i++) {
                Entity &e = getWorld().getEntity(i);
                SkeletalModel &m = e.getModel();
                if (e.getAnimation() > 0)
                    e.setAnimation(e.getAnimation() - 1);
                else
                    if (m.size() > 0)
                        e.setAnimation(m.size() - 1);
            }
        } else {
            getConsole() << "Animations need to be enabled!" << Console::endl;
        }
    } else {
        // Enable or disable animating all skeletal models
        bool b = false;
        if (!(args >> b)) {
            getConsole() << "Pass BOOL to animate command!" << Console::endl;
            return -2;
        }
        if (b)
            getRender().setFlags(Render::fAnimateAllModels);
        else
            getRender().clearFlags(Render::fAnimateAllModels);
        getConsole() << (b ? "Animating all models" : "No longer animating all models") << Console::endl;
    }

    return 0;
}

