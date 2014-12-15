/*!
 * \file src/commands/CommandRender.cpp
 * \brief Renderer commands
 *
 * \author xythobuz
 */

#include "global.h"
#include "Game.h"
#include "Log.h"
#include "Render.h"
#include "commands/CommandRender.h"

std::string CommandMode::name() {
    return "mode";
}

std::string CommandMode::brief() {
    return "MODE - Render mode";
}

void CommandMode::printHelp() {
    getLog() << "mode-Command Usage:" << Log::endl;
    getLog() << "  mode MODE" << Log::endl;
    getLog() << "Where MODE is one of the following:" << Log::endl;
    getLog() << "  wireframe" << Log::endl;
    getLog() << "  solid" << Log::endl;
    getLog() << "  texture" << Log::endl;
    getLog() << "  titlescreen" << Log::endl;
    getLog() << "  disabled" << Log::endl;

}

int CommandMode::execute(std::istream& args) {
    if (!getGame().isLoaded()) {
        getLog() << "Load a level to set the mode!" << Log::endl;
        return -1;
    }

    std::string s;
    args >> s;

    if (s == "wireframe") {
        Render::setMode(RenderMode::Wireframe);
        getLog() << "Wireframe mode" << Log::endl;
    } else if (s == "solid") {
        Render::setMode(RenderMode::Solid);
        getLog() << "Solid mode" << Log::endl;
    } else if (s == "texture") {
        Render::setMode(RenderMode::Texture);
        getLog() << "Texture Mode" << Log::endl;
    } else if (s == "titlescreen") {
        Render::setMode(RenderMode::LoadScreen);
        getLog() << "Titlescreen mode" << Log::endl;
    } else if (s == "disabled") {
        Render::setMode(RenderMode::Disabled);
        getLog() << "Disabled mode" << Log::endl;
    } else {
        getLog() << "Invalid use of mode command (" << s << ")!" << Log::endl;
        return -2;
    }

    return 0;
}

