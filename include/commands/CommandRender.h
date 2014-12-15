/*!
 * \file include/commands/CommandRender.h
 * \brief Render Command interface
 *
 * \author xythobuz
 */

#ifndef _COMMAND_RENDER_H_
#define _COMMAND_RENDER_H_

#include "commands/Command.h"

class CommandMode : public Command {
  public:
    virtual std::string name();
    virtual std::string brief();
    virtual void printHelp();
    virtual int execute(std::istream& args);
};

#endif

