/*!
 * \file src/commands/CommandRender.cpp
 * \brief Renderer commands
 *
 * \author xythobuz
 */

#include "global.h"
#include "Console.h"
#include "Game.h"
#include "OpenRaider.h"
#include "Render.h"
#include "commands/CommandRender.h"

std::string CommandMode::name() {
    return "mode";
}

std::string CommandMode::brief() {
    return "MODE - Render mode";
}

void CommandMode::printHelp() {
    getConsole() << "mode-Command Usage:" << Console::endl;
    getConsole() << "  mode MODE" << Console::endl;
    getConsole() << "Where MODE is one of the following:" << Console::endl;
    getConsole() << "  wireframe" << Console::endl;
    getConsole() << "  solid" << Console::endl;
    getConsole() << "  texture" << Console::endl;
    getConsole() << "  vertexlight" << Console::endl;
    getConsole() << "  titlescreen" << Console::endl;

}

int CommandMode::execute(std::istream& args) {
    if (!getGame().isLoaded()) {
        getConsole() << "Load a level to set the mode!" << Console::endl;
        return -1;
    }

    std::string s;
    args >> s;

    if (s.compare("wireframe") == 0) {
        getRender().setMode(Render::modeWireframe);
        getConsole() << "Wireframe mode" << Console::endl;
    } else if (s.compare("solid") == 0) {
        getRender().setMode(Render::modeSolid);
        getConsole() << "Solid mode" << Console::endl;
    } else if (s.compare("texture") == 0) {
        getRender().setMode(Render::modeTexture);
        getConsole() << "Texture mode" << Console::endl;
    } else if (s.compare("vertexlight") == 0) {
        getRender().setMode(Render::modeVertexLight);
        getConsole() << "Vertexlight mode" << Console::endl;
    } else if (s.compare("titlescreen") == 0) {
        getRender().setMode(Render::modeLoadScreen);
        getConsole() << "Titlescreen mode" << Console::endl;
    } else {
        getConsole() << "Invalid use of mode command (" << s << ")!" << Console::endl;
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
    getConsole() << "renderflag-Command Usage:" << Console::endl;
    getConsole() << "  renderflag STRING BOOL" << Console::endl;
    getConsole() << "Where STRING is one of the following:" << Console::endl;
    getConsole() << "  ralpha" << Console::endl;
    getConsole() << "  entmodel" << Console::endl;
    getConsole() << "  fog" << Console::endl;
    getConsole() << "  light" << Console::endl;
    getConsole() << "  ponytail" << Console::endl;
}

int CommandRenderflag::execute(std::istream& args) {
    if (!getOpenRaider().mRunning) {
        getConsole() << "Use renderflag-Command interactively!" << Console::endl;
        return -1;
    }

    std::string flag;
    bool b;
    args >> flag >> b;
    if (!args) {
        getConsole() << "Pass STRING and BOOL to renderflag command!" << Console::endl;
        return -2;
    }

    int f = stringToFlag(flag);
    if (f == -1) {
        getConsole() << "Unknown flag \"" << flag << "\"!" << Console::endl;
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

