/*!
 * \file src/commands/Command.cpp
 * \brief Commands
 *
 * \author xythobuz
 */

#include <fstream>
#include <iomanip>

#include "global.h"
#include "Log.h"
#include "utils/strings.h"
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
    getLog() << "No help available!" << Log::endl;
}

void Command::fillCommandList() {
    commands.clear();
    commands.push_back(std::shared_ptr<Command>(new CommandLoad()));
    commands.push_back(std::shared_ptr<Command>(new CommandBind()));
    commands.push_back(std::shared_ptr<Command>(new CommandSet()));
    commands.push_back(std::shared_ptr<Command>(new CommandGet()));
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
            getLog() << "Available commands:" << Log::endl;
            getLog() << std::right << std::setw(11);
            getLog() << "help" << " - print command help" << Log::endl;
            for (auto& x : commands) {
                if (x) {
                    getLog() << std::right << std::setw(11);
                    getLog() << x->name() << " - " << x->brief() << Log::endl;
                }
            }
            getLog() << "Use help COMMAND to get additional info" << Log::endl;
            getLog() << "Pass BOOLs as true or false" << Log::endl;
            return 0;
        } else {
            // Show help for a specific command
            for (auto& x : commands) {
                if (x) {
                    if (x->name().compare(arg) == 0) {
                        x->printHelp();
                        return 0;
                    }
                }
            }
            getLog() << "Unknown command: \"" << arg << "\"" << Log::endl;
            return -1;
        }
    }

    // Execute command
    for (auto& x : commands) {
        if (x) {
            if (x->name().compare(cmd) == 0) {
                return x->execute(command);
            }
        }
    }

    getLog() << "Unknown command: \"" << cmd << "\"" << Log::endl;
    return -1;
}

int Command::executeFile(std::string file) {
    std::string configFile = expandHomeDirectory(file);
    getLog() << "Loading config from \"" << configFile << "\"..." << Log::endl;

    std::ifstream f(configFile);
    if (!f) {
        getLog() << "Could not open file!" << Log::endl;
        return -1;
    }

    for (std::string line; std::getline(f, line);) {
        if (line.length() == 0)
            continue;

        int error = Command::command(line);
        if (error != 0)
            getLog() << "Error Code: " << error << Log::endl;
    }

    f.close();
    return 0;
}

