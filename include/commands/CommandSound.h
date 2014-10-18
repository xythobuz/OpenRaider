/*!
 * \file include/commands/CommandSound.h
 * \brief Sound Command interface
 *
 * \author xythobuz
 */

#ifndef _COMMAND_SOUND_H_
#define _COMMAND_SOUND_H_

#include "commands/Command.h"

class CommandSound : public Command {
  public:
    virtual std::string name();
    virtual std::string brief();
    virtual void printHelp();
    virtual int execute(std::istream& args);
};

#endif

