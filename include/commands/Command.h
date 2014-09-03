/*!
 * \file include/commands/Command.h
 * \brief Command interface
 *
 * \author xythobuz
 */

#ifndef _COMMAND_H_
#define _COMMAND_H_

#include <istream>
#include <memory>
#include <string>
#include <vector>

class Command {
public:
    virtual ~Command();
    virtual std::string name() = 0;
    virtual std::string brief() = 0;
    virtual void printHelp();
    virtual int execute(std::istream& args) = 0;

    static void fillCommandList();
    static int command(std::string c);
    static int executeFile(std::string file);

private:
    static std::vector<std::shared_ptr<Command>> commands;
};

#endif

