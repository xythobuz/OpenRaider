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
#include "commands/CommandBind.h"
#include "commands/CommandEngine.h"
#include "commands/CommandSet.h"

std::vector<std::shared_ptr<Command>> Command::commands;

Command::~Command() {
}

void Command::printHelp() {
    Log::get(LOG_USER) << "No help available!" << Log::endl;
}

void Command::fillCommandList() {
    commands.clear();
    commands.push_back(std::shared_ptr<Command>(new CommandBind()));
    commands.push_back(std::shared_ptr<Command>(new CommandLoad()));
    commands.push_back(std::shared_ptr<Command>(new CommandSet()));
    commands.push_back(std::shared_ptr<Command>(new CommandGet()));
    commands.push_back(std::shared_ptr<Command>(new CommandScreenshot()));
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
    if (cmd == "help") {
        std::string arg;
        command >> arg;
        if (arg.length() == 0) {
            // List all available commands
            Log::get(LOG_USER) << "Available commands:" << Log::endl;
            Log::get(LOG_USER) << std::right << std::setw(11);
            Log::get(LOG_USER) << "help" << " - print command help" << Log::endl;
            for (auto& x : commands) {
                if (x) {
                    Log::get(LOG_USER) << std::right << std::setw(11);
                    Log::get(LOG_USER) << x->name() << " - " << x->brief() << Log::endl;
                }
            }
            Log::get(LOG_USER) << "Use help COMMAND to get additional info" << Log::endl;
            Log::get(LOG_USER) << "Pass BOOLs as true or false" << Log::endl;
            return 0;
        } else {
            // Show help for a specific command
            for (auto& x : commands) {
                if (x) {
                    if (x->name() == arg) {
                        x->printHelp();
                        return 0;
                    }
                }
            }
            Log::get(LOG_USER) << "Unknown command: \"" << arg << "\"" << Log::endl;
            return -1;
        }
    }

    // Execute command
    for (auto& x : commands) {
        if (x) {
            if (x->name() == cmd) {
                return x->execute(command);
            }
        }
    }

    Log::get(LOG_USER) << "Unknown command: \"" << cmd << "\"" << Log::endl;
    return -1;
}

int Command::executeFile(std::string file) {
    std::string configFile = expandHomeDirectory(file);
    Log::get(LOG_INFO) << "Loading config from \"" << configFile << "\"..." << Log::endl;

    std::ifstream f(configFile);
    if (!f) {
        Log::get(LOG_ERROR) << "Could not open file!" << Log::endl;
        return -1;
    }

    for (std::string line; std::getline(f, line);) {
        if (line.length() == 0)
            continue;

        int error = Command::command(line);
        if (error != 0)
            Log::get(LOG_ERROR) << "Error Code: " << error << Log::endl;
    }

    f.close();
    return 0;
}

std::string Command::autoComplete(std::string begin) {
    std::vector<std::string> candidates;

    std::string help("help");
    if (begin.size() <= help.size()) {
        if (begin.compare(0, begin.size(), help, 0, begin.size()) == 0) {
            candidates.push_back(help);
        }
    }

    for (auto& x : commands) {
        if (x) {
            std::string name = x->name();
            if (begin.size() <= name.size()) {
                if (begin.compare(0, begin.size(), name, 0, begin.size()) == 0) {
                    candidates.push_back(name);
                }
            }
        }
    }

    if (candidates.size() == 0) {
        return "";
    } else if (candidates.size() == 1) {
        return candidates.at(0);
    } else {
        std::string common = candidates.at(0);
        for (int i = 0; i < candidates.size(); i++) {
            Log::get(LOG_USER) << candidates.at(i);
            if (i < (candidates.size() - 1))
                Log::get(LOG_USER) << "/";

            for (int c = 0; (c < common.size()) && (c < candidates.at(i).size()); c++) {
                if (common.at(c) != candidates.at(i).at(c)) {
                    common.erase(c);
                    break;
                }
            }
        }
        Log::get(LOG_USER) << Log::endl;
        return common;
    }
}

