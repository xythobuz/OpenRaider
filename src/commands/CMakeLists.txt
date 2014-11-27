# Source files
set (CMD_SRCS ${CMD_SRCS} "Command.cpp")
set (CMD_SRCS ${CMD_SRCS} "CommandAnimate.cpp")
set (CMD_SRCS ${CMD_SRCS} "CommandBind.cpp")
set (CMD_SRCS ${CMD_SRCS} "CommandEngine.cpp")
set (CMD_SRCS ${CMD_SRCS} "CommandGame.cpp")
set (CMD_SRCS ${CMD_SRCS} "CommandMove.cpp")
set (CMD_SRCS ${CMD_SRCS} "CommandRender.cpp")
set (CMD_SRCS ${CMD_SRCS} "CommandSet.cpp")

# Add library
add_library (OpenRaider_commands OBJECT ${CMD_SRCS})

