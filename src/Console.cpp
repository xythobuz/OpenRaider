/*!
 * \file src/Console.cpp
 * \brief Console class
 *
 * \author xythobuz
 */

#include <cstring>

#include "imgui/imgui.h"

#include "global.h"
#include "Log.h"
#include "UI.h"
#include "commands/Command.h"
#include "Console.h"

bool Console::visible = false;
char Console::buffer[bufferLength + 1] = "";
unsigned long Console::lastLogLength = 0;
std::vector<std::string> Console::lastCommands;
long Console::lastCommandIndex = -1;
std::string Console::bufferedCommand;

void Console::callback(ImGuiTextEditCallbackData* data) {
    bool update = false;
    std::string completion;

    switch (data->EventKey) {
        case ImGuiKey_Tab:
            completion = Command::autoComplete(data->Buf);
            if (completion.size() > 0) {
                data->DeleteChars(0, data->BufSize);
                data->InsertChars(0, completion.c_str());
                data->CursorPos = strlen(data->Buf);
            }
            break;

        case ImGuiKey_UpArrow:
            update = true;
            if (lastCommandIndex < (long)(lastCommands.size() - 1)) {
                lastCommandIndex++;
                if (lastCommandIndex == 0)
                    bufferedCommand = data->Buf;
            }
            break;

        case ImGuiKey_DownArrow:
            update = true;
            if (lastCommandIndex > -1)
                lastCommandIndex--;
            break;
    }

    if (update) {
        if ((lastCommandIndex >= 0) && (lastCommandIndex < lastCommands.size())) {
            if (lastCommands.at(lastCommands.size() - 1 - lastCommandIndex) != buffer) {
                data->DeleteChars(0, data->BufSize);
                data->InsertChars(0, lastCommands.at(lastCommands.size() - 1 - lastCommandIndex).c_str());
            }
        } else {
            data->DeleteChars(0, data->BufSize);
            if (bufferedCommand.size() > 0) {
                data->InsertChars(0, bufferedCommand.c_str());
            }
        }

        data->CursorPos = strlen(data->Buf);
    }
}

void Console::display() {
    if (!visible)
        return;

    static bool scrollToBottom = false;
    if (ImGui::Begin("Console", &visible, ImVec2(600, 400))) {
        if (lastLogLength != getLog().size()) {
            lastLogLength = getLog().size();
            scrollToBottom = true;
        }

        ImGui::BeginChild("ConsoleText", ImVec2(0, -ImGui::GetTextLineSpacing() * 2));
        for (unsigned long i = 0; i < getLog().size(); i++) {
            ImGui::TextUnformatted(getLog().get(i).c_str());
        }
        if (scrollToBottom) {
            ImGui::SetScrollPosHere();
            scrollToBottom = false;
        }
        ImGui::EndChild();

        bool focusInput = false;
        if (ImGui::InputText("Command", buffer, bufferLength,
                             ImGuiInputTextFlags_EnterReturnsTrue
                             | ImGuiInputTextFlags_CallbackCompletion
                             | ImGuiInputTextFlags_CallbackHistory,
                             &Console::callback)) {
            getLog() << "> " << buffer << Log::endl;
            if (strlen(buffer) > 0) {
                int error = Command::command(buffer);
                if (error != 0) {
                    getLog() << "Error code: " << error << Log::endl;
                }

                if ((lastCommands.size() == 0) || (lastCommands[lastCommands.size() - 1] != buffer))
                    lastCommands.push_back(std::string(buffer));
            }

            lastCommandIndex = -1;
            buffer[0] = '\0';
            scrollToBottom = true;
            focusInput = true;
            bufferedCommand.clear();
        }

        if (ImGui::IsItemHovered() || focusInput) {
            ImGui::SetKeyboardFocusHere(-1);
            focusInput = false;
        }
    }
    ImGui::End();
}

