/*!
 * \file src/commands/Command.cpp
 * \brief Commands
 *
 * \author xythobuz
 */

#include <iomanip>

#include "global.h"
#include "Console.h"
#include "commands/Command.h"
#include "commands/CommandAnimate.h"
#include "commands/CommandBind.h"
#include "commands/CommandEngine.h"
#include "commands/CommandGame.h"
#include "commands/CommandMove.h"
#include "commands/CommandRender.h"
#include "commands/CommandSet.h"
#include "commands/CommandSound.h"

std::vector<std::shared_ptr<Command>> Command::commands;

Command::~Command() {
}

void Command::printHelp() {
    getConsole() << "No help available!" << Console::endl;
}

void Command::fillCommandList() {
    commands.clear();
    commands.push_back(std::shared_ptr<Command>(new CommandLoad()));
    commands.push_back(std::shared_ptr<Command>(new CommandBind()));
    commands.push_back(std::shared_ptr<Command>(new CommandSet()));
    commands.push_back(std::shared_ptr<Command>(new CommandScreenshot()));
    commands.push_back(std::shared_ptr<Command>(new CommandAnimate()));
    commands.push_back(std::shared_ptr<Command>(new CommandMove()));
    commands.push_back(std::shared_ptr<Command>(new CommandMode()));
    commands.push_back(std::shared_ptr<Command>(new CommandRenderflag()));
    commands.push_back(std::shared_ptr<Command>(new CommandSound()));
    commands.push_back(std::shared_ptr<Command>(new CommandPos()));
    commands.push_back(std::shared_ptr<Command>(new CommandViewmodel()));
    commands.push_back(std::shared_ptr<Command>(new CommandPigtail()));
    commands.push_back(std::shared_ptr<Command>(new CommandPonypos()));
    commands.push_back(std::shared_ptr<Command>(new CommandQuit()));
}

int Command::command(std::string c) {
    assert(commands.size() > 0);

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

    // Print help
    if (cmd.compare("help") == 0) {
        std::string arg;
        command >> arg;
        if (arg.length() == 0) {
            // List all available commands
            getConsole() << "Available commands:" << Console::endl;
            getConsole() << std::right << std::setw(11);
            getConsole() << "help" << " - print command help" << Console::endl;
            for (auto &x : commands) {
                if (x) {
                    getConsole() << std::right << std::setw(11);
                    getConsole() << x->name() << " - " << x->brief() << Console::endl;
                }
            }
            getConsole() << "Use help COMMAND to get additional info" << Console::endl;
            getConsole() << "Pass BOOLs as true or false" << Console::endl;
            return 0;
        } else {
            // Show help for a specific command
            for (auto &x : commands) {
                if (x) {
                    if (x->name().compare(arg) == 0) {
                        x->printHelp();
                        return 0;
                    }
                }
            }
            getConsole() << "Unknown command: \"" << arg << "\"" << Console::endl;
            return -1;
        }
    }

    // Execute command
    for (auto &x : commands) {
        if (x) {
            if (x->name().compare(cmd) == 0) {
                return x->execute(command);
            }
        }
    }

    getConsole() << "Unknown command: \"" << cmd << "\"" << Console::endl;
    return -1;
}

