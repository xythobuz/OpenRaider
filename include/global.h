/*!
 * \file include/global.h
 * \brief Global typedefs
 *
 * \author xythobuz
 */
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "config.h"

// Visual C++ does not understand __attribute__
#ifdef _MSC_VER
#define __attribute__(x)
#endif

// Globally include OpenGL header
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif
#include <GL/gl.h>
#endif

// If available, use our own assert that prints the call stack
#if defined(HAVE_EXECINFO_H) && defined(HAVE_BACKTRACE) && defined(HAVE_BACKTRACE_SYMBOLS)
#ifndef NDEBUG
[[noreturn]] void assertImplementation(const char *exp, const char *file, int line);
#define assert(x) (void)((x) || (assertImplementation(#x, __FILE__, __LINE__),0))
#else
#define assert(x)
#endif
#else
// Fall back to the default C assert
#include <cassert>
#endif

// Colors used for Rendering
const float BLACK[]       = {  0.0f,  0.0f,  0.0f, 1.0f };
const float DIM_WHITE[]   = {  0.5f,  0.5f,  0.5f, 1.0f };
const float WHITE[]       = {  1.0f,  1.0f,  1.0f, 1.0f };
const float RED[]         = {  1.0f,  0.0f,  0.0f, 1.0f };
const float GREEN[]       = {  0.0f,  1.0f,  0.0f, 1.0f };
const float NEXT_PURPLE[] = {  0.3f,  0.3f,  0.5f, 1.0f };
const float OR_BLUE[]     = {  0.5f,  0.7f,  1.0f, 1.0f };
const float PINK[]        = {  1.0f,  0.0f,  1.0f, 1.0f };
const float YELLOW[]      = {  1.0f,  1.0f,  0.0f, 1.0f };
const float CYAN[]        = {  0.0f,  1.0f,  1.0f, 1.0f };

// Actions that can be bound to a key and sent to the game engine
typedef enum {
    menuAction = 0,
    consoleAction, // menu and console should always be the first two items
    forwardAction,
    backwardAction,
    leftAction,
    rightAction,
    jumpAction,
    crouchAction,
    useAction,
    holsterAction,
    walkAction,

    ActionEventCount // Should always be at the end
} ActionEvents;

// Keys available for binding
typedef enum {
    zeroKey = '0', oneKey = '1', twoKey = '2',
    threeKey = '3', fourKey = '4', fiveKey = '5',
    sixKey = '6', sevenKey = '7', eightKey = '8',
    nineKey = '9', aKey = 'a', bKey = 'b',
    cKey = 'c', dKey = 'd', eKey = 'e', fKey = 'f',
    gKey = 'g', hKey = 'h', iKey = 'i', jKey = 'j',
    kKey = 'k', lKey = 'l', mKey = 'm', nKey = 'n',
    oKey = 'o', pKey = 'p', qKey = 'q', rKey = 'r',
    sKey = 's', tKey = 't', uKey = 'u', vKey = 'v',
    wKey = 'w', xKey = 'x', yKey = 'y', zKey = 'z',
    quoteKey, backslashKey, backspaceKey, capslockKey,
    commaKey, delKey, upKey, downKey, leftKey, rightKey,
    endKey, equalsKey, escapeKey, f1Key, f2Key, f3Key, f4Key, f5Key,
    f6Key, f7Key, f8Key, f9Key, f10Key, f11Key, f12Key, backquoteKey,
    homeKey, insertKey, leftaltKey, leftctrlKey, leftbracketKey,
    leftguiKey, leftshiftKey, minusKey, numlockKey, pagedownKey,
    pageupKey, pauseKey, dotKey, rightaltKey, rightctrlKey, enterKey,
    rightguiKey, rightbracketKey, rightshiftKey, scrolllockKey,
    semicolonKey, slashKey, spaceKey, tabKey,
    leftmouseKey, middlemouseKey, rightmouseKey,

    unknownKey // Should always be at the end
} KeyboardButton;

ActionEvents stringToActionEvent(const char *action);

KeyboardButton stringToKeyboardButton(const char *key);

#endif

