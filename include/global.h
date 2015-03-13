/*!
 * \file include/global.h
 * \brief Included everywhere
 *
 * \author xythobuz
 */
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "config.h"

void renderFrame();

// Actions that can be bound to a key and sent to the game engine
typedef enum {
    menuAction = 0,
    debugAction,
    consoleAction,

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
    // Keyboard
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

    // Mouse
    leftmouseKey, middlemouseKey, rightmouseKey,
    fourthmouseKey, fifthmouseKey,

    // Game Controller
    aButton, bButton, xButton, yButton, backButton, startButton,
    guideButton, leftStick, rightStick, leftShoulder, rightShoulder,
    padUp, padDown, padLeft, padRight,
    leftXAxis, leftYAxis, rightXAxis, rightYAxis, leftTrigger, rightTrigger,

    unknownKey // Should always be at the end
} KeyboardButton;

// Globally include OpenGL header
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else // __APPLE__
#ifdef _WIN32
#include <GL/glew.h>
#include <GL/wglew.h>
#pragma comment(lib, "glew32.lib")
#else // _WIN32
//! \fixme gl3 header?
#include <GL/gl.h>
#include <GL/glext.h>
#endif // _WIN32
#endif // __APPLE__

/*! \fixme Is there a better way to handle this?
 * We wan't to use our own assert(). Unfortunately, glm includes
 * cassert in its headers. So we need to define NDEBUG here.
 * To avoid a conflict, our flag is now called NODEBUG instead.
 */
#define NDEBUG
#include <glm/glm.hpp>

// If available, use our own assert that prints the call stack
#if defined(HAVE_EXECINFO_H) && defined(HAVE_BACKTRACE) && defined(HAVE_BACKTRACE_SYMBOLS)

#ifndef NODEBUG

#include <iostream>
#include <execinfo.h>

template<typename T, typename U>
[[noreturn]] void assertEqualImplementation(const char* exp, T a, U b, const char* file, int line,
        const char* str = nullptr) {
    const unsigned int maxSize = 128;
    void* callstack[maxSize];
    int frames = backtrace(callstack, maxSize);
    char** strs = backtrace_symbols(callstack, frames);

    std::cout << std::endl << "assertion failed:" << std::endl;
    std::cout << "\t" << exp << std::endl;
    if (str != nullptr)
        std::cout << "\t(" << a << " " << str << " " << b << ")" << std::endl;
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
        assertEqualImplementation(#x, assertEvalTemp, true, __FILE__, __LINE__); \
}

#define assertEqual(x, y) { \
    auto assertEvalTemp = x; \
    auto assertEvalTemp2 = y; \
    if (assertEvalTemp != assertEvalTemp2) \
        assertEqualImplementation(#x " == " #y, assertEvalTemp, assertEvalTemp2, \
                                  __FILE__, __LINE__, "!="); \
}

#define assertNotEqual(x, y) { \
    auto assertEvalTemp = x; \
    auto assertEvalTemp2 = y; \
    if (assertEvalTemp == assertEvalTemp2) \
        assertEqualImplementation(#x " != " #y, assertEvalTemp, assertEvalTemp2, \
                                  __FILE__, __LINE__, "=="); \
}

#define assertLessThan(x, y) { \
    auto assertEvalTemp = x; \
    auto assertEvalTemp2 = y; \
    if (assertEvalTemp >= assertEvalTemp2) \
        assertEqualImplementation(#x " < " #y, assertEvalTemp, assertEvalTemp2, \
                                  __FILE__, __LINE__, ">="); \
}

#define assertLessThanEqual(x, y) { \
    auto assertEvalTemp = x; \
    auto assertEvalTemp2 = y; \
    if (assertEvalTemp > assertEvalTemp2) \
        assertEqualImplementation(#x " <= " #y, assertEvalTemp, assertEvalTemp2, \
                                  __FILE__, __LINE__, ">"); \
}

#define assertGreaterThan(x, y) { \
    auto assertEvalTemp = x; \
    auto assertEvalTemp2 = y; \
    if (assertEvalTemp <= assertEvalTemp2) \
        assertEqualImplementation(#x " > " #y, assertEvalTemp, assertEvalTemp2, \
                                  __FILE__, __LINE__, "<="); \
}

#define assertGreaterThanEqual(x, y) { \
    auto assertEvalTemp = x; \
    auto assertEvalTemp2 = y; \
    if (assertEvalTemp < assertEvalTemp2) \
        assertEqualImplementation(#x " >= " #y, assertEvalTemp, assertEvalTemp2, \
                                  __FILE__, __LINE__, "<"); \
}

#else // NODEBUG

#define assert(x)
#define assertEqual(x, y)
#define assertNotEqual(x, y)
#define assertLessThan(x, y)
#define assertLessThanEqual(x, y)
#define assertGreaterThan(x, y)
#define assertGreaterThanEqual(x, y)

#endif // NODEBUG

#else // EXECINFO

// Fall back to the default C assert
#include <cassert>
#define assertEqual(x, y) assert((x) == (y))
#define assertNotEqual(x, y) assert((x) != (y))
#define assertLessThan(x, y) assert((x) < (y))
#define assertLessThanEqual(x, y) assert((x) <= (y))
#define assertGreaterThan(x, y) assert((x) > (y))
#define assertGreaterThanEqual(x, y) assert((x) >= (y))

#endif // EXECINFO

#endif // _GLOBAL_H_

