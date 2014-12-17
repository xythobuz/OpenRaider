/*!
 * \file include/global.h
 * \brief Global typedefs
 *
 * \author xythobuz
 */
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "config.h"

void renderFrame();

// Colors used where ever needed
const unsigned char BLACK[]  = {   0,   0,   0, 255 };
const unsigned char GREY[]   = { 128, 128, 128, 255 };
const unsigned char WHITE[]  = { 255, 255, 255, 255 };
const unsigned char RED[]    = { 255,   0,   0, 255 };
const unsigned char GREEN[]  = {   0, 255,   0, 255 };
const unsigned char PURPLE[] = {  77,  77, 128, 255 };
const unsigned char BLUE[]   = { 128, 179, 255, 255 };
const unsigned char PINK[]   = { 255,   0, 255, 255 };
const unsigned char YELLOW[] = { 255, 255,   0, 255 };
const unsigned char CYAN[]   = {   0, 255, 255, 255 };

// Actions that can be bound to a key and sent to the game engine
typedef enum {
    menuAction = 0,
    debugAction,

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
    fourthmouseKey, fifthmouseKey,
    unknownKey // Should always be at the end
} KeyboardButton;

// Globally include OpenGL header
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else // __APPLE__
#ifdef _WIN32
#include <windows.h>
#endif // _WIN32
//! \todo gl3 header?
#include <GL/gl.h>
#include <GL/glext.h>
#endif // __APPLE__

// If available, use our own assert that prints the call stack
#if defined(HAVE_EXECINFO_H) && defined(HAVE_BACKTRACE) && defined(HAVE_BACKTRACE_SYMBOLS)

#ifndef NDEBUG

#include <iostream>
#include <execinfo.h>

template<typename T, typename U>
[[noreturn]] void assertEqualImplementation(const char* exp, T a, U b, const char* file, int line,
        bool print) {
    const unsigned int maxSize = 128;
    void* callstack[maxSize];
    int frames = backtrace(callstack, maxSize);
    char** strs = backtrace_symbols(callstack, frames);

    std::cout << std::endl << "assertion failed:" << std::endl;
    std::cout << "\t" << exp << std::endl;
    if (print)
        std::cout << "\t (" << a << " != " << b << ")" << std::endl;
    std::cout << "in " << file << ":" << line << std::endl << std::endl;

    for (int i = 0; i < frames; i++)
        std::cout << strs[i] << std::endl;

    delete [] strs;
    abort();
}

template<typename T, typename U>
[[noreturn]] void assertNotEqualImplementation(const char* exp, T a, U b, const char* file,
        int line, bool print) {
    const unsigned int maxSize = 128;
    void* callstack[maxSize];
    int frames = backtrace(callstack, maxSize);
    char** strs = backtrace_symbols(callstack, frames);

    std::cout << std::endl << "assertion failed:" << std::endl;
    std::cout << "\t" << exp << std::endl;
    if (print)
        std::cout << "\t (" << a << " == " << b << ")" << std::endl;
    std::cout << "in " << file << ":" << line << std::endl << std::endl;

    for (int i = 0; i < frames; i++)
        std::cout << strs[i] << std::endl;

    delete [] strs;
    abort();
}

// Evaluating x or y could have side-effects
// So we only do it once!

#define assert(x) { \
    auto assertEvalTemp = x; \
    if (!assertEvalTemp) \
        assertEqualImplementation(#x, assertEvalTemp, true, __FILE__, __LINE__, false); \
}

#define assertEqual(x, y) { \
    auto assertEvalTemp = x; \
    auto assertEvalTemp2 = y; \
    if (assertEvalTemp != assertEvalTemp2) \
        assertEqualImplementation(#x " == " #y, assertEvalTemp, assertEvalTemp2, __FILE__, __LINE__, true); \
}

#define assertNotEqual(x, y) { \
    auto assertEvalTemp = x; \
    auto assertEvalTemp2 = y; \
    if (assertEvalTemp == assertEvalTemp2) \
        assertNotEqualImplementation(#x " != " #y, assertEvalTemp, assertEvalTemp2, __FILE__, __LINE__, true); \
}

#else // NDEBUG

#define assert(x)
#define assertEqual(x, y)
#define assertNotEqual(x, y)

#endif // NDEBUG

#else // EXECINFO

// Fall back to the default C assert
#include <cassert>
#define assertEqual(x, y) assert((x) == (y))
#define assertNotEqual(x, y) assert((x) != (y))

#endif // EXECINFO

#endif // _GLOBAL_H_

