/*!
 * \file src/commands/CommandRender.cpp
 * \brief Renderer commands
 *
 * \author xythobuz
 */

#include "global.h"
#include "Game.h"
#include "Log.h"
#include "RunTime.h"
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
    getLog() << "  vertexlight" << Log::endl;
    getLog() << "  titlescreen" << Log::endl;

}

int CommandMode::execute(std::istream& args) {
    if (!getGame().isLoaded()) {
        getLog() << "Load a level to set the mode!" << Log::endl;
        return -1;
    }

    std::string s;
    args >> s;

    if (s.compare("wireframe") == 0) {
        getRender().setMode(Render::modeWireframe);
        getLog() << "Wireframe mode" << Log::endl;
    } else if (s.compare("solid") == 0) {
        getRender().setMode(Render::modeSolid);
        getLog() << "Solid mode" << Log::endl;
    } else if (s.compare("texture") == 0) {
        getRender().setMode(Render::modeTexture);
        getLog() << "Texture mode" << Log::endl;
    } else if (s.compare("vertexlight") == 0) {
        getRender().setMode(Render::modeVertexLight);
        getLog() << "Vertexlight mode" << Log::endl;
    } else if (s.compare("titlescreen") == 0) {
        getRender().setMode(Render::modeLoadScreen);
        getLog() << "Titlescreen mode" << Log::endl;
    } else {
        getLog() << "Invalid use of mode command (" << s << ")!" << Log::endl;
        return -2;
    }

    return 0;
}

// --------------------------------------

std::string CommandRenderflag::name() {
    return "renderflag";
}

std::string CommandRenderflag::brief() {
    return "STRING BOOL - Toggle Render flag";
}

void CommandRenderflag::printHelp() {
    getLog() << "renderflag-Command Usage:" << Log::endl;
    getLog() << "  renderflag STRING BOOL" << Log::endl;
    getLog() << "Where STRING is one of the following:" << Log::endl;
    getLog() << "  ralpha" << Log::endl;
    getLog() << "  entmodel" << Log::endl;
    getLog() << "  fog" << Log::endl;
    getLog() << "  light" << Log::endl;
    getLog() << "  ponytail" << Log::endl;
}

int CommandRenderflag::execute(std::istream& args) {
    if (!getRunTime().isRunning()) {
        getLog() << "Use renderflag-Command interactively!" << Log::endl;
        return -1;
    }

    std::string flag;
    bool b;
    args >> flag >> b;
    if (!args) {
        getLog() << "Pass STRING and BOOL to renderflag command!" << Log::endl;
        return -2;
    }

    int f = stringToFlag(flag);
    if (f == -1) {
        getLog() << "Unknown flag \"" << flag << "\"!" << Log::endl;
        return -3;
    }

    if (b) {
        getRender().setFlags((unsigned int)f);
    } else {
        getRender().clearFlags((unsigned int)f);
    }

    return 0;
}

int CommandRenderflag::stringToFlag(std::string flag) {
    if (flag == "ralpha") {
        return Render::fRoomAlpha;
    } else if (flag == "entmodel") {
        return Render::fEntityModels;
    } else if (flag == "fog") {
        return Render::fFog;
    } else if (flag == "light") {
        return Render::fGL_Lights;
    } else if (flag == "ponytail") {
        return Render::fRenderPonytail;
    } else {
        return -1;
    }
}

