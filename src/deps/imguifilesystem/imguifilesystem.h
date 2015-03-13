// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#ifndef IMGUI_FILESYSTEM_H_
#define IMGUI_FILESYSTEM_H_

// USAGE EXAMPLE:
/*
#include "imguifilesystem.h"                            // imguifilesystem.cpp must be compiled

// Inside a ImGui window:
const bool browseButtonPressed = ImGui::Button("...");  // we need a trigger boolean variable
static ImGuiFs::Dialog dlg;                             // one per dialog (and must be static)
dlg.chooseFileDialog(browseButtonPressed);              // see other dialog types and the full list of arguments for advanced usage
if (strlen(dlg.getChosenPath())>0) {
    ImGui::Text("Chosen file: \"%s\"",dlg.getChosenPath());
}

// If you want to copy the (valid) returned path somewhere, you can use something like:
static char myPath[ImGuiFs::MAX_PATH_BYTES];
if (strlen(dlg.getChosenPath())>0) {
    strcpy(myPath,dlg.getChosenPath());
}
*/

// MISSING FEATURES:
/*
-> [File and Folder] Links are NOT supported (they don't show up).
-> Multiselections in chooseFileDialogs are NOT supported.
-> Hidden and temporary files don't show up on nix systems (by design). Nothing has been done for Windows about it yet.
*/

// COMPILING AND TESTING:
/*
-> Compiled and tested using "ImGui library v1.17 wip"

-> Successfully compiled using gcc, clang and mingw32 compilers.
x> Never compiled on any other compiler (Visual C++'s cl.exe included).

-> Tested on Ubuntu 64bit and Wine 32bit.
x> Never tested on a real Windows OS and on MacOS.
*/

//#define DIRENT_USES_UTF8_CHARS  // Optional. Affects Windows users only. Needs recompilation of imguifilesystem.cpp. Enables long UTF8 paths instead of short ASCII paths.
                                // Unfortunately it's NOT 100% functional (in my tests some folders can't be browsed). Patches are welcome. See "dirent_portable.h" for further info.
                                // When experiencing problems on Windows, trying commenting this definition out is a good start.
                                // On a second thought, I think we should leave this definition commented out (Windows users can always define it at the project level, if needed).

namespace ImGuiFs {


extern const int MAX_FILENAME_BYTES;
extern const int MAX_PATH_BYTES;


struct Dialog {
    public:

    // default arguments are usually what most users expect (better not touch them in most cases)
    Dialog(bool noKnownDirectoriesSection=false,bool noCreateDirectorySection=false,bool noFilteringSection=false,bool detectKnownDirectoriesAtEachOpening=false,bool addDisplayByOption=false,bool dontFilterSaveFilePathsEnteredByTheUser=false);
    ~Dialog();

    // "dialogTriggerButton" is usually a bool variable connected to a ImGui::Button(...).
    // returns the chosen path (internally stored). Users can check when the returned path has strlen()>0.
    // "fileFilterExtensionString" can be something like ".png;.jpg;.jpeg;.bmp;.tga;.gif;.tiff;.txt". It's case insensitive.
    // "directory" and "startingFileNameEntry" (only available in saveFileDialog(...)) are quite flexible and can be set to almost everything: the method will use the most resonable choice.
    const char* chooseFileDialog(bool dialogTriggerButton,const char* directory=NULL,const char* fileFilterExtensionString=NULL,const char* windowTitle=NULL,const ImVec2& windowSize=ImVec2(-1,-1),const ImVec2& windowPos=ImVec2(-1,-1),const float windowAlpha=0.875f);
    const char* chooseFolderDialog(bool dialogTriggerButton,const char* directory=NULL,const char* windowTitle=NULL,const ImVec2& windowSize=ImVec2(-1,-1),const ImVec2& windowPos=ImVec2(-1,-1),const float windowAlpha=0.875f);
    const char* saveFileDialog(bool dialogTriggerButton,const char* directory=NULL,const char* startingFileNameEntry=NULL,const char* fileFilterExtensionString=NULL,const char* windowTitle=NULL,const ImVec2& windowSize=ImVec2(-1,-1),const ImVec2& windowPos=ImVec2(-1,-1),const float windowAlpha=0.875f);

    // gets the chosen path (internally stored). It's valid (its strlen()>0) only when the user performs a valid selection.
    const char* getChosenPath() const;
    // returns the last directory browsed by the user using this class (internally stored). Can be passed as "directory" parameter in the methods above to reuse last used directory.
    const char* getLastDirectory() const;

    private:
    struct Internal* internal;
    friend const char* ChooseFileMainMethod(Dialog& ist,const char* directory,const bool _isFolderChooserDialog,const bool _isSaveFileDialog,const char* _saveFileName,const char* fileFilterExtensionString,const char* windowTitle,const ImVec2& windowSize,const ImVec2& windowPos,const float windowAlpha);
};


} // namespace ImGuiFs

#endif //IMGUI_FILESYSTEM_H_
