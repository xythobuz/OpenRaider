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

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

// If available, use our own assert that prints the call stack
#if defined(HAVE_EXECINFO_H) && defined(HAVE_BACKTRACE) && defined(HAVE_BACKTRACE_SYMBOLS)

#ifndef NDEBUG

#include <iostream>
#include <execinfo.h>

template<typename T, typename U>
[[noreturn]] void orAssertImplementation(const char* exp, T a, U b, const char* file, int line,
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

#define orAssert(x) { \
    auto assertEvalTemp = x; \
    if (!assertEvalTemp) \
        orAssertImplementation(#x, assertEvalTemp, true, __FILE__, __LINE__); \
}

#define orAssertEqual(x, y) { \
    auto assertEvalTemp = x; \
    auto assertEvalTemp2 = y; \
    if (assertEvalTemp != assertEvalTemp2) \
        orAssertImplementation(#x " == " #y, assertEvalTemp, assertEvalTemp2, \
                                  __FILE__, __LINE__, "!="); \
}

#define orAssertNotEqual(x, y) { \
    auto assertEvalTemp = x; \
    auto assertEvalTemp2 = y; \
    if (assertEvalTemp == assertEvalTemp2) \
        orAssertImplementation(#x " != " #y, assertEvalTemp, assertEvalTemp2, \
                                  __FILE__, __LINE__, "=="); \
}

#define orAssertLessThan(x, y) { \
    auto assertEvalTemp = x; \
    auto assertEvalTemp2 = y; \
    if (assertEvalTemp >= assertEvalTemp2) \
        orAssertImplementation(#x " < " #y, assertEvalTemp, assertEvalTemp2, \
                                  __FILE__, __LINE__, ">="); \
}

#define orAssertLessThanEqual(x, y) { \
    auto assertEvalTemp = x; \
    auto assertEvalTemp2 = y; \
    if (assertEvalTemp > assertEvalTemp2) \
        orAssertImplementation(#x " <= " #y, assertEvalTemp, assertEvalTemp2, \
                                  __FILE__, __LINE__, ">"); \
}

#define orAssertGreaterThan(x, y) { \
    auto assertEvalTemp = x; \
    auto assertEvalTemp2 = y; \
    if (assertEvalTemp <= assertEvalTemp2) \
        orAssertImplementation(#x " > " #y, assertEvalTemp, assertEvalTemp2, \
                                  __FILE__, __LINE__, "<="); \
}

#define orAssertGreaterThanEqual(x, y) { \
    auto assertEvalTemp = x; \
    auto assertEvalTemp2 = y; \
    if (assertEvalTemp < assertEvalTemp2) \
        orAssertImplementation(#x " >= " #y, assertEvalTemp, assertEvalTemp2, \
                                  __FILE__, __LINE__, "<"); \
}

#else // NDEBUG

#define orAssert(x)
#define orAssertEqual(x, y)
#define orAssertNotEqual(x, y)
#define orAssertLessThan(x, y)
#define orAssertLessThanEqual(x, y)
#define orAssertGreaterThan(x, y)
#define orAssertGreaterThanEqual(x, y)

#endif // NDEBUG

#else // EXECINFO

// Fall back to the default C assert
#include <cassert>
#define orAssertEqual(x, y) assert((x) == (y))
#define orAssertNotEqual(x, y) assert((x) != (y))
#define orAssertLessThan(x, y) assert((x) < (y))
#define orAssertLessThanEqual(x, y) assert((x) <= (y))
#define orAssertGreaterThan(x, y) assert((x) > (y))
#define orAssertGreaterThanEqual(x, y) assert((x) >= (y))

#endif // EXECINFO

#endif // _GLOBAL_H_

