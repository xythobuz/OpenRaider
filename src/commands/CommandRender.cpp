/*!
 * \file src/commands/CommandRender.cpp
 * \brief Renderer commands
 *
 * \author xythobuz
 */

#include "global.h"
#include "Console.h"
#include "Game.h"
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

std::string CommandFog::name() {
    return "fog";
}

std::string CommandFog::brief() {
    return "BOOL - GL Fog";
}

int CommandFog::execute(std::istream& args) {
    bool b;
    args >> b;
    if (!args) {
        getConsole() << "Pass BOOL to fog command!" << Console::endl;
        return -1;
    }

    if (b)
        getRender().setFlags(Render::fFog);
    else
        getRender().clearFlags(Render::fFog);

    getConsole() << "Fog is now " << (b ? "on" : "off") << Console::endl;
    return 0;
}

// --------------------------------------

std::string CommandLight::name() {
    return "light";
}

std::string CommandLight::brief() {
    return "BOOL - GL Lights";
}

int CommandLight::execute(std::istream& args) {
    bool b;
    args >> b;
    if (!args) {
        getConsole() << "Pass BOOL to light command!" << Console::endl;
        return -1;
    }

    if (b)
        getRender().setFlags(Render::fGL_Lights);
    else
        getRender().clearFlags(Render::fGL_Lights);

    getConsole() << "GL-Lights are now " << (b ? "on" : "off") << Console::endl;
    return 0;
}

