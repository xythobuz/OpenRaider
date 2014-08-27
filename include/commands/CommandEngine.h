/*!
 * \file include/commands/CommandEngine.h
 * \brief Engine Command interface
 *
 * \author xythobuz
 */

#ifndef _COMMAND_ENGINE_H_
#define _COMMAND_ENGINE_H_

#include "commands/Command.h"

DECLARE_SIMPLE_CMD(CommandLoad);

DECLARE_SIMPLE_CMD(CommandScreenshot);

DECLARE_SIMPLE_CMD_NO_HELP(CommandQuit);

#endif

