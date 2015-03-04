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
#include "commands/Command.h"
#include "Console.h"

bool Console::visible = false;
char Console::buffer[bufferLength + 1] = "";
unsigned long Console::lastLogLength = 0;
std::vector<std::string> Console::lastCommands;
long Console::lastCommandIndex = -1;
std::string Console::bufferedCommand;

int Console::callback(ImGuiTextEditCallbackData* data) {
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

    return 0;
}

void Console::display() {
    if (!visible)
        return;

    if (ImGui::Begin("Console", &visible, ImVec2(600, 400))) {
        static bool scrollToBottom = false;
        if (lastLogLength != Log::size()) {
            lastLogLength = Log::size();
            scrollToBottom = true;
        }

        static bool visibleLogs[LOG_COUNT] = { true, true, true, true, true };
        ImGui::Checkbox("Error##log", &visibleLogs[1]);
        ImGui::SameLine();
        ImGui::Checkbox("Warning##log", &visibleLogs[2]);
        ImGui::SameLine();
        ImGui::Checkbox("User##log", &visibleLogs[0]);
        ImGui::SameLine();
        ImGui::Checkbox("Info##log", &visibleLogs[3]);
        ImGui::SameLine();
        ImGui::Checkbox("Debug##log", &visibleLogs[4]);

        static bool logToTTY = false, logToClipboard = false, logToFile = false;
        if (ImGui::Button("Log to TTY")) { logToTTY = true; }
        ImGui::SameLine();
        if (ImGui::Button("Log to Clipboard")) { logToClipboard = true; }
        ImGui::SameLine();
        if (ImGui::Button("Log to File")) { logToFile = true; }
        ImGui::Separator();

        ImGui::BeginChild("ConsoleText", ImVec2(0, -ImGui::GetTextLineHeightWithSpacing() * 2));
        if (logToTTY)
            ImGui::LogToTTY();
        else if (logToClipboard)
            ImGui::LogToClipboard();
        else if (logToFile)
            ImGui::LogToFile();
        for (unsigned long i = 0; i < Log::size(); i++) {
            auto& entry = Log::getEntry(i);

            assertLessThan(entry.level, LOG_COUNT);
            if (!visibleLogs[entry.level]) {
                continue;
            }

            ImVec4 col(1.0f, 1.0f, 1.0f, 1.0f);
            if (entry.level == LOG_ERROR) {
                col = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
            } else if (entry.level == LOG_WARNING) {
                col = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
            } else if (entry.level == LOG_DEBUG) {
                col = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
            } else if (entry.level == LOG_USER) {
                col = ImVec4(0.5f, 0.75f, 1.0f, 1.0f);
            }

            ImGui::PushStyleColor(ImGuiCol_Text, col);
            ImGui::TextWrapped("%s", entry.text.c_str());
            ImGui::PopStyleColor();
        }
        if (logToTTY || logToClipboard || logToFile) {
            ImGui::LogFinish();
            logToTTY = logToClipboard = logToFile = false;
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
            Log::get(LOG_USER) << "> " << buffer << Log::endl;
            if (strlen(buffer) > 0) {
                int error = Command::command(buffer);
                if (error != 0) {
                    Log::get(LOG_USER) << "Error code: " << error << Log::endl;
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
        }
    }
    ImGui::End();
}

