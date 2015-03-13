imguifilesystem dialogs for imgui v.1.17 wip (https://github.com/ocornut/imgui).
See also: https://github.com/ocornut/imgui/issues/88

It's composed by three files:

-    imguifilesystem.h (usage instructions are here)
-    imguifilesystem.cpp
-    dirent_portable.h

It needs testing and feedback (expecially for Windows/VisualC++ and MacOS).

It does not use any C++ string class: so it should be straightforward to merge its code
into imgui.h/imgui.cpp if we want to (and if the code is robust enough). 