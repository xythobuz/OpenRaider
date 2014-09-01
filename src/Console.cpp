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
    zPos = -1;
    mHistoryPointer = 0;
    mLineOffset = 0;

    UI::addWindow(this);
}

Console::~Console() {
    UI::removeWindow(this);
}

void Console::moveToTop() {
    if (!getWindow().getTextInput())
        getWindow().setTextInput(true);

    UI::moveToTop();
}

void Console::makeInvisible() {
    if (getWindow().getTextInput())
        getWindow().setTextInput(false);

    UI::makeInvisible();
}

void Console::display() {
    // Calculate line drawing geometry
    // Depends on window height, so recalculate every time
    unsigned int firstLine = 35;
    unsigned int lastLine = (::getWindow().getHeight() / 2) - 55;
    unsigned int inputLine = (::getWindow().getHeight() / 2) - 30;
    unsigned int lineSteps = 20;
    unsigned int lineCount = (lastLine - firstLine + lineSteps) / lineSteps;
    while (((lineCount * lineSteps) + firstLine) < inputLine) {
        lineSteps++;
        lineCount = (lastLine - firstLine + lineSteps) / lineSteps;
    }

    ::getWindow().glEnter2D();

    // Draw half-transparent *overlay*
    glColor4f(0.0f, 0.0f, 0.0f, 0.75f);
    glDisable(GL_TEXTURE_2D);
    glRecti(0, 0, ::getWindow().getWidth(), ::getWindow().getHeight() / 2);
    glEnable(GL_TEXTURE_2D);

    unsigned long scrollIndicator;
    if (mHistory.size() > lineCount) {
        scrollIndicator = (mHistory.size() - lineCount - mLineOffset) * 100 / (mHistory.size() - lineCount);
    } else {
        scrollIndicator = 100;
        mLineOffset = 0;
    }

    // Draw status line
    std::ostringstream status;
    status << VERSION << " uptime " << (systemTimerGet() / 1000) << "s scroll " << scrollIndicator << "%";
    getFont().drawText(10, 10, 0.70f, BLUE, status.str());

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
                0.75f, BLUE, mHistory.at(i + historyOffset - mLineOffset));
    }

    // Draw current input
    getFont().drawText(10, inputLine, 0.75f, BLUE, "> " + mInputBuffer + mPartialInput);

    ::getWindow().glExit2D();
}

void Console::handleKeyboard(KeyboardButton key, bool pressed) {
    if (pressed && (key == enterKey)) {
        // Execute entered command
        if (mInputBuffer.length() > 0) {
            (*this) << "> " << mInputBuffer.c_str() << endl;
            mCommandHistory.push_back(mInputBuffer.c_str());
            int error = getOpenRaider().command(mInputBuffer);
            if (error != 0) {
                (*this) << "Error Code: " << error << endl;
            }
        } else {
            (*this) << "> " << endl;
        }

        // Clear partial and input buffer
        mInputBuffer = "";
        mPartialInput = "";
        mHistoryPointer = 0;
    }

    // Delete last character
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

    // Calculate line drawing geometry
    // Depends on window height, so recalculate every time
    unsigned int firstLine = 35;
    unsigned int lastLine = (::getWindow().getHeight() / 2) - 55;
    unsigned int inputLine = (::getWindow().getHeight() / 2) - 30;
    unsigned int lineSteps = 20;
    unsigned int lineCount = (lastLine - firstLine + lineSteps) / lineSteps;
    while (((lineCount * lineSteps) + firstLine) < inputLine) {
        lineSteps++;
        lineCount = (lastLine - firstLine + lineSteps) / lineSteps;
    }

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

