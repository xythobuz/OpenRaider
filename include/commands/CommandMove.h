/*!
 * \file include/commands/CommandMove.h
 * \brief Move Command interface
 *
 * \author xythobuz
 */

#ifndef _COMMAND_MOVE_H_
#define _COMMAND_MOVE_H_

#include "commands/Command.h"

class CommandMove : public Command {
public:
    virtual std::string name();
    virtual std::string brief();
    virtual void printHelp();
    virtual int execute(std::istream& args);
};

#endif

