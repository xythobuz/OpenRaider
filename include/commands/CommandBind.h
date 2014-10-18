/*!
 * \file include/commands/CommandBind.h
 * \brief Bind Command interface
 *
 * \author xythobuz
 */

#ifndef _COMMAND_BIND_H_
#define _COMMAND_BIND_H_

#include "commands/Command.h"

class CommandBind : public Command {
  public:
    virtual std::string name();
    virtual std::string brief();
    virtual void printHelp();
    virtual int execute(std::istream& args);

  private:
    ActionEvents stringToActionEvent(std::string action);
    KeyboardButton stringToKeyboardButton(std::string key);
};

#endif

