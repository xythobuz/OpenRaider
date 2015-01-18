/*!
 * \file src/system/FontImGui.cpp
 * \brief Default Font implementation
 *
 * \author xythobuz
 */

#include "imgui/imgui.h"

#include "global.h"
#include "UI.h"
#include "system/FontImGui.h"

#define SCALE_CALC 30.0f
#define SCALE_DRAW SCALE_CALC

unsigned int FontImGui::widthText(float scale, std::string s) {
    ImGuiIO& io = ImGui::GetIO();
    ImFont* font = io.Fonts->Fonts.at(0);
    ImVec2 size = font->CalcTextSizeA(scale * SCALE_CALC, FLT_MAX, io.DisplaySize.y, s.c_str(),
                                                    s.c_str() + s.length());
    return size.x;
}

void FontImGui::drawText(unsigned int x, unsigned int y, float scale,
                         const unsigned char color[4], std::string s) {
    ImGuiIO& io = ImGui::GetIO();
    ImFont* font = io.Fonts->Fonts.at(0);
    ImVec2 pos = ImVec2(x, y);
    ImU32 col = color[0] | (color[1] << 8) | (color[2] << 16) | (color[3] << 24);

    ImDrawList dl;
    dl.PushTextureID(font->ContainerAtlas->TexID);
    dl.PushClipRect(ImVec4(0.0f, 0.0f, io.DisplaySize.x, io.DisplaySize.y));
    dl.AddText(font, scale * SCALE_DRAW, pos, col, s.c_str(), s.c_str() + s.length());

    ImDrawList* dlp = &dl;
    UI::renderImGui(&dlp, 1);
}

unsigned int FontImGui::heightText(float scale, unsigned int maxWidth, std::string s) {
    ImGuiIO& io = ImGui::GetIO();
    ImFont* font = io.Fonts->Fonts.at(0);
    ImVec2 size = font->CalcTextSizeA(scale * SCALE_CALC, FLT_MAX, maxWidth, s.c_str(),
                                                    s.c_str() + s.length());
    return size.y;
}

void FontImGui::drawTextWrapped(unsigned int x, unsigned int y, float scale,
                                const unsigned char color[4], unsigned int maxWidth, std::string s) {
    ImGuiIO& io = ImGui::GetIO();
    ImFont* font = io.Fonts->Fonts.at(0);
    ImVec2 pos = ImVec2(x, y);
    ImU32 col = color[0] | (color[1] << 8) | (color[2] << 16) | (color[3] << 24);

    ImDrawList dl;
    dl.PushTextureID(font->ContainerAtlas->TexID);
    dl.PushClipRect(ImVec4(0.0f, 0.0f, io.DisplaySize.x, io.DisplaySize.y));
    dl.AddText(font, scale * SCALE_DRAW, pos, col, s.c_str(), s.c_str() + s.length(), maxWidth);

    ImDrawList* dlp = &dl;
    UI::renderImGui(&dlp, 1);
}

