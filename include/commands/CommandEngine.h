/*!
 * \file include/commands/CommandEngine.h
 * \brief Engine Command interface
 *
 * \author xythobuz
 */

#ifndef _COMMAND_ENGINE_H_
#define _COMMAND_ENGINE_H_

#include "commands/Command.h"

class CommandLoad : public Command {
  public:
    virtual std::string name();
    virtual std::string brief();
    virtual void printHelp();
    virtual int execute(std::istream& args);
};

class CommandScreenshot : public Command {
  public:
    virtual std::string name();
    virtual std::string brief();
    virtual void printHelp();
    virtual int execute(std::istream& args);
};

class CommandQuit : public Command {
  public:
    virtual std::string name();
    virtual std::string brief();
    virtual int execute(std::istream& args);
};

#endif

