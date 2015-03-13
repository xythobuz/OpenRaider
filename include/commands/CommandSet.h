/*!
 * \file include/commands/CommandSet.h
 * \brief Get/Set Commands
 *
 * \author xythobuz
 */

#ifndef _COMMAND_SET_H_
#define _COMMAND_SET_H_

#include "commands/Command.h"

class CommandSet : public Command {
  public:
    virtual std::string name();
    virtual std::string brief();
    virtual void printHelp();
    virtual int execute(std::istream& args);
};

class CommandGet : public Command {
  public:
    virtual std::string name();
    virtual std::string brief();
    virtual void printHelp();
    virtual int execute(std::istream& args);
};

#endif

