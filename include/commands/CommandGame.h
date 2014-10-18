/*!
 * \file include/commands/CommandGame.h
 * \brief Game Command interface
 *
 * \author xythobuz
 */

#ifndef _COMMAND_GAME_H_
#define _COMMAND_GAME_H_

#include "commands/Command.h"

class CommandPos : public Command {
  public:
    virtual std::string name();
    virtual std::string brief();
    virtual int execute(std::istream& args);
};

class CommandViewmodel : public Command {
  public:
    virtual std::string name();
    virtual std::string brief();
    virtual int execute(std::istream& args);
};

class CommandPigtail : public Command {
  public:
    virtual std::string name();
    virtual std::string brief();
    virtual int execute(std::istream& args);
};

class CommandPonypos : public Command {
  public:
    virtual std::string name();
    virtual std::string brief();
    virtual int execute(std::istream& args);
};

#endif

