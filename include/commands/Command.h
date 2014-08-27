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
};

#define DECLARE_SIMPLE_CMD(x) \
    class x : public Command { \
    public: \
        virtual std::string name(); \
        virtual std::string brief(); \
        virtual void printHelp(); \
        virtual int execute(std::istream& args); \
    }

#define DECLARE_SIMPLE_CMD_NO_HELP(x) \
    class x : public Command { \
    public: \
        virtual std::string name(); \
        virtual std::string brief(); \
        virtual int execute(std::istream& args); \
    }

#endif

