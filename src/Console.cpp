/*!
 * \file src/Console.cpp
 * \brief Console class
 *
 * \author xythobuz
 */

#include "global.h"
#include "Log.h"
#include "UI.h"
#include "commands/Command.h"
#include "Console.h"

char Console::buffer[bufferLength] = "";
bool Console::scrollToBottom = false;
unsigned long Console::lastLogLength = 0;

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

        if (ImGui::InputText("Command", buffer, bufferLength,
                    ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue)) {
            getLog() << "> " << buffer << Log::endl;
            int error = Command::command(buffer);
            if (error != 0) {
                getLog() << "Error code: " << error << Log::endl;
            }
            buffer[0] = '\0';
            scrollToBottom = true;
        }
    }
    ImGui::End();
}

