/*!
 * \file src/commands/CommandAnimate.cpp
 * \brief Animate command
 *
 * \author xythobuz
 */

#include "global.h"
#include "Log.h"
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
    getLog() << "animate-Command Usage:" << Log::endl;
    getLog() << "  animate [n|p|BOOL]" << Log::endl;
    getLog() << "Where the commands have the following meaning:" << Log::endl;
    getLog() << "  BOOL to (de)activate animating all models" << Log::endl;
    getLog() << "  n to step all models to their next animation" << Log::endl;
    getLog() << "  p to step all models to their previous animation" << Log::endl;
}

int CommandAnimate::execute(std::istream& args) {
    if (!getGame().isLoaded()) {
        getLog() << "Use animate command interactively!" << Log::endl;
        return -1;
    }

    if (args.peek() == 'n') {
        // Step all skeletal models to their next animation
        if (getRender().getFlags() & Render::fAnimateAllModels) {
            for (unsigned int i = 0; i < getWorld().sizeEntity(); i++) {
                Entity& e = getWorld().getEntity(i);
                SkeletalModel& m = e.getModel();
                if (e.getAnimation() < (m.size() - 1))
                    e.setAnimation(e.getAnimation() + 1);
                else
                    e.setAnimation(0);
            }
        } else {
            getLog() << "Animations need to be enabled!" << Log::endl;
        }
    } else if (args.peek() == 'p') {
        // Step all skeletal models to their previous animation
        if (getRender().getFlags() & Render::fAnimateAllModels) {
            for (unsigned int i = 0; i < getWorld().sizeEntity(); i++) {
                Entity& e = getWorld().getEntity(i);
                SkeletalModel& m = e.getModel();
                if (e.getAnimation() > 0)
                    e.setAnimation(e.getAnimation() - 1);
                else if (m.size() > 0)
                    e.setAnimation(m.size() - 1);
            }
        } else {
            getLog() << "Animations need to be enabled!" << Log::endl;
        }
    } else {
        // Enable or disable animating all skeletal models
        bool b = false;
        if (!(args >> b)) {
            getLog() << "Pass BOOL to animate command!" << Log::endl;
            return -2;
        }
        if (b)
            getRender().setFlags(Render::fAnimateAllModels);
        else
            getRender().clearFlags(Render::fAnimateAllModels);
        getLog() << (b ? "Animating all models" : "No longer animating all models") << Log::endl;
    }

    return 0;
}

