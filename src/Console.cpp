/*!
 * \file src/Console.cpp
 * \brief Console class
 *
 * \author xythobuz
 */

#include <cstring>

#include "global.h"
#include "Log.h"
#include "UI.h"
#include "commands/Command.h"
#include "Console.h"

char Console::buffer[bufferLength + 1] = "";
bool Console::scrollToBottom = false;
bool Console::focusInput = false;
unsigned long Console::lastLogLength = 0;
std::vector<std::string> Console::lastCommands;
long Console::lastCommandIndex = -1;

void Console::display() {
    if (ImGui::Begin("Console", NULL, ImVec2(600, 400), -1.0f)) {
        if (lastLogLength != getLog().size()) {
            lastLogLength = getLog().size();
            scrollToBottom = true;
        }

        ImGui::BeginChild("ConsoleText", ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowSize().y - 70));
        for (unsigned long i = 0; i < getLog().size(); i++) {
            ImGui::Text("%s", getLog().get(i).c_str());
        }
        if (scrollToBottom) {
            ImGui::SetScrollPosHere();
            scrollToBottom = false;
        }
        ImGui::EndChild();

        if (focusInput) {
            ImGui::SetKeyboardFocusHere();
            focusInput = false;
        }

        if (ImGui::GetWindowIsFocused()) {
            ImGuiIO& io = ImGui::GetIO();
            static bool hold = false;
            bool update = false;

            if ((!hold) && (io.KeysDown[upKey] || io.KeysDown[downKey]))
                update = hold = true;

            if (hold && (!(io.KeysDown[upKey] || io.KeysDown[downKey])))
                hold = false;

            if (update) {
                if (io.KeysDown[upKey]) {
                    if (lastCommandIndex < static_cast<long>(lastCommands.size() - 1))
                        lastCommandIndex++;
                }

                if (io.KeysDown[downKey]) {
                    if (lastCommandIndex > -1)
                        lastCommandIndex--;
                }

                if ((lastCommandIndex >= 0) && (lastCommandIndex < lastCommands.size())) {
                    strncpy(buffer,
                            lastCommands.at(lastCommands.size() - 1 - lastCommandIndex).c_str(),
                            bufferLength);
                    buffer[bufferLength] = '\0';
                } else {
                    buffer[0] = '\0';
                }

                update = false;
            }
        }

        if (ImGui::InputText("Command", buffer, bufferLength,
                    ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue)) {
            getLog() << "> " << buffer << Log::endl;
            int error = Command::command(buffer);
            if (error != 0) {
                getLog() << "Error code: " << error << Log::endl;
            }

            if ((lastCommands.size() == 0) || (lastCommands[lastCommands.size() - 1] != buffer))
                lastCommands.push_back(std::string(buffer));
            lastCommandIndex = -1;

            buffer[0] = '\0';
            scrollToBottom = true;
            focusInput = true;
        }
    }
    ImGui::End();
}

