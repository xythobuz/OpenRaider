/*!
 * \file include/global.h
 * \brief Global typedefs
 *
 * \author xythobuz
 */
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

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

    ActionEventCount // Should always be at the end
} ActionEvents;

typedef enum {
    zero = '0', one = '1',
    two = '2', three = '3',
    four = '4', five = '5',
    six = '6', seven = '7',
    eight = '8', nine = '9',
    a = 'a', b = 'b',
    c = 'c', d = 'd',
    e = 'e', f = 'f',
    g = 'g', h = 'h',
    i = 'i', j = 'j',
    k = 'k', l = 'l',
    m = 'm', n = 'n',
    o = 'o', p = 'p',
    q = 'q', r = 'r',
    s = 's', t = 't',
    u = 'u', v = 'v',
    w = 'w', x = 'x',
    y = 'y', z = 'z',
    quote, backslash, backspace, capslock,
    comma, del, up, down, left, right,
    end, equals, escape, f1, f2, f3, f4, f5,
    f6, f7, f8, f9, f10, f11, f12, backquote,
    home, insert, leftalt, leftctrl, leftbracket,
    leftgui, leftshift, minus, numlock, pagedown,
    pageup, pause, dot, rightalt, rightctrl, enter,
    rightgui, rightbracket, rightshift, scrolllock,
    semicolon, slash, space, tab,
    leftmouse, middlemouse, rightmouse,
    unknown
} KeyboardButton;

typedef struct {
    char *text;
    unsigned int x;
    unsigned int y;
    int w;
    int h;
    float scale;
    unsigned char color[4];
} WindowString;

#endif

