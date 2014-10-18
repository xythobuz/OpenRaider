/*!
 * \file include/commands/CommandAnimate.h
 * \brief Animate Command interface
 *
 * \author xythobuz
 */

#ifndef _COMMAND_ANIMATE_H_
#define _COMMAND_ANIMATE_H_

#include "commands/Command.h"

class CommandAnimate : public Command {
  public:
    virtual std::string name();
    virtual std::string brief();
    virtual void printHelp();
    virtual int execute(std::istream& args);
};

#endif

