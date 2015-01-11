/*!
 * \file src/commands/CommandRender.cpp
 * \brief Renderer commands
 *
 * \author xythobuz
 */

#include "global.h"
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
    Log::get(LOG_USER) << "mode-Command Usage:" << Log::endl;
    Log::get(LOG_USER) << "  mode MODE" << Log::endl;
    Log::get(LOG_USER) << "Where MODE is one of the following:" << Log::endl;
    Log::get(LOG_USER) << "  wireframe" << Log::endl;
    Log::get(LOG_USER) << "  solid" << Log::endl;
    Log::get(LOG_USER) << "  texture" << Log::endl;
    Log::get(LOG_USER) << "  titlescreen" << Log::endl;

}

int CommandMode::execute(std::istream& args) {
    std::string s;
    args >> s;

    if (s == "wireframe") {
        Render::setMode(RenderMode::Wireframe);
        Log::get(LOG_USER) << "Wireframe mode" << Log::endl;
    } else if (s == "solid") {
        Render::setMode(RenderMode::Solid);
        Log::get(LOG_USER) << "Solid mode" << Log::endl;
    } else if (s == "texture") {
        Render::setMode(RenderMode::Texture);
        Log::get(LOG_USER) << "Texture Mode" << Log::endl;
    } else if (s == "titlescreen") {
        Render::setMode(RenderMode::LoadScreen);
        Log::get(LOG_USER) << "Titlescreen mode" << Log::endl;
    } else {
        Log::get(LOG_USER) << "Invalid use of mode command (" << s << ")!" << Log::endl;
        return -2;
    }

    return 0;
}

