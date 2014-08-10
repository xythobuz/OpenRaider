/*!
 * \file src/Console.cpp
 * \brief Console 'overlay'
 *
 * \author xythobuz
 */

#include <iostream>

#include "global.h"
#include "Font.h"
#include "OpenRaider.h"
#include "utf8-cpp/utf8.h"
#include "utils/strings.h"
#include "utils/time.h"
#include "Window.h"
#include "Console.h"

Console::Console() {
    mVisible = false;
    mHistoryPointer = 0;
    mLineOffset = 0;
}

void Console::setVisible(bool visible) {
    mVisible = visible;
    getWindow().setTextInput(mVisible);
}

bool Console::isVisible() {
    return mVisible;
}

void Console::print(const char *s, ...) {
    va_list args;
    va_start(args, s);
    char *tmp = bufferString(s, args);
    va_end(args);

    if (tmp != NULL) {
        mHistory.push_back(std::string(tmp));
#ifdef DEBUG
        std::cout << tmp << std::endl;
#endif
    }

    delete [] tmp;
}

#define LINE_GEOMETRY(window) \
    unsigned int firstLine = 35; \
    unsigned int lastLine = (window.getHeight() / 2) - 55; \
    unsigned int inputLine = (window.getHeight() / 2) - 30; \
    unsigned int lineSteps = 20; \
    unsigned int lineCount = (lastLine - firstLine + lineSteps) / lineSteps; \
    while (((lineCount * lineSteps) + firstLine) < inputLine) { \
        lineSteps++; \
        lineCount = (lastLine - firstLine + lineSteps) / lineSteps; \
    }

void Console::display() {
    if (!mVisible)
        return;

    // Calculate line drawing geometry
    // Depends on window height, so recalculate every time
    LINE_GEOMETRY(getWindow());

    // Draw half-transparent *overlay*
    glColor4f(0.0f, 0.0f, 0.0f, 0.75f);
    glDisable(GL_TEXTURE_2D);
    glRecti(0, 0, getWindow().getWidth(), getWindow().getHeight() / 2);
    glEnable(GL_TEXTURE_2D);

    unsigned long scrollIndicator;
    if (mHistory.size() > lineCount) {
        scrollIndicator = (mHistory.size() - lineCount - mLineOffset) * 100 / (mHistory.size() - lineCount);
    } else {
        scrollIndicator = 100;
        mLineOffset = 0;
    }

    getFont().drawText(10, 10, 0.70f, BLUE,
            "%s uptime %lus scroll %d%%", VERSION, systemTimerGet() / 1000, scrollIndicator);

    // Draw output log
    long end = lineCount;
    long drawOffset = 0;
    long historyOffset = 0;
    if (mHistory.size() < lineCount) {
        end = mHistory.size();
        drawOffset = lineCount - mHistory.size();
    } else if (lineCount < mHistory.size()) {
        historyOffset = mHistory.size() - lineCount;
    }
    for (int i = 0; i < end; i++) {
        getFont().drawText(10, (unsigned int)((i + drawOffset) * lineSteps) + firstLine,
                0.75f, BLUE, "%s", mHistory[i + historyOffset - mLineOffset].c_str());
    }

    // Draw current input
    getFont().drawText(10, inputLine, 0.75f, BLUE, "> %s", (mInputBuffer + mPartialInput).c_str());
}

void Console::handleKeyboard(KeyboardButton key, bool pressed) {
    if (pressed && (key == enterKey)) {
        // Execute entered command
        if (mInputBuffer.length() > 0) {
            print("> %s", mInputBuffer.c_str());
            mCommandHistory.push_back(mInputBuffer.c_str());
            int error = getOpenRaider().command(mInputBuffer);
            if (error != 0) {
                print("Error Code: %d", error);
            }
        } else {
            print("> ");
        }

        // Clear partial and input buffer
        mInputBuffer = "";
        mPartialInput = "";
        mHistoryPointer = 0;
    }

    if (pressed && (key == backspaceKey)) {
        if ((mPartialInput.length() == 0)
                && (mInputBuffer.length() > 0)) {
            utf8::iterator<std::string::iterator> it(mInputBuffer.end(), mInputBuffer.begin(), mInputBuffer.end());
            mInputBuffer.erase((--it).base(), mInputBuffer.end());
        }
    }

    if (pressed && ((key == upKey) || (key == downKey))) {
        moveInHistory(key == upKey);
    }
}

void Console::moveInHistory(bool up) {
    if (mCommandHistory.size() == 0)
        return;

    if (up) {
        if (mHistoryPointer < mCommandHistory.size()) {
            mHistoryPointer++;
            if (mHistoryPointer == 1) {
                mUnfinishedInput = mInputBuffer;
            }
        } else {
            return;
        }
    } else {
        if (mHistoryPointer > 0)
            mHistoryPointer--;
        else
            return;
    }

    if ((mHistoryPointer > 0) && (mHistoryPointer <= mCommandHistory.size())) {
        mInputBuffer = mCommandHistory[mCommandHistory.size() - mHistoryPointer];
    } else {
        if (mUnfinishedInput.length() > 0) {
            mInputBuffer = mUnfinishedInput;
            mUnfinishedInput = "";
        } else {
            mInputBuffer = "";
        }
    }
}

void Console::handleText(char *text, bool notFinished) {
    // Always scroll to bottom when text input is received
    mLineOffset = 0;

    if (!notFinished) {
        // Finished entering character
        // delete previous partial character, if present
        mPartialInput = "";

        //! \fixme Temporary hack filtering the console activation key
        if (text[0] == '`')
            return;

        // Append new input to buffer
        mInputBuffer += text;
    } else {
        // Partial character received
        mPartialInput = text;
    }
}

void Console::handleMouseScroll(int xrel, int yrel) {
    assert((xrel != 0) || (yrel != 0));
    LINE_GEOMETRY(getWindow());

    if (mHistory.size() > lineCount) {
        if (yrel > 0) {
            if (mLineOffset < (mHistory.size() - lineCount)) {
                mLineOffset++;
            }
        } else if (yrel < 0) {
            if (mLineOffset > 0) {
                mLineOffset--;
            }
        }
    }
}

