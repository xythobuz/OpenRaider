/*!
 * \file src/Console.cpp
 * \brief Console 'overlay'
 *
 * \author xythobuz
 */

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "config.h"
#include "Console.h"
#include "main.h"
#include "utils/strings.h"
#include "utils/time.h"

#define INPUT_BUFFER_SIZE 255

Console::Console() {
    mVisible = false;
    mInputBuffer = new char[INPUT_BUFFER_SIZE + 1];
    mInputBuffer[INPUT_BUFFER_SIZE] = '\0';
    mInputBufferPointer = 0;
    mPartialInput = NULL;
    mHistoryPointer = 0;
    mUnfinishedInput = NULL;
    mLineOffset = 0;
}

Console::~Console() {
    if (mInputBuffer)
        delete [] mInputBuffer;

    if (mPartialInput)
        delete [] mPartialInput;

    if (mUnfinishedInput)
        delete [] mUnfinishedInput;

    while (mHistory.size() > 0) {
        char *tmp = mHistory.back();
        if (tmp != NULL)
            delete [] tmp;
        mHistory.pop_back();
    }

    while (mCommandHistory.size() > 0) {
        char *tmp = mCommandHistory.back();
        if (tmp != NULL)
            delete [] tmp;
        mCommandHistory.pop_back();
    }
}

void Console::setVisible(bool visible) {
    mVisible = visible;
    gOpenRaider->mWindow->setTextInput(mVisible);
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
        mHistory.push_back(tmp);
        printf("%s\n", tmp);
    }
}

#define LINE_GEOMETRY(window) unsigned int firstLine = 35; \
        unsigned int lastLine = (window->mHeight / 2) - 55; \
        unsigned int inputLine = (window->mHeight / 2) - 30; \
        unsigned int lineSteps = 20; \
        unsigned int lineCount = (lastLine - firstLine + lineSteps) / lineSteps; \
        while (((lineCount * lineSteps) + firstLine) < inputLine) { \
            lineSteps++; \
            lineCount = (lastLine - firstLine + lineSteps) / lineSteps; \
        }

void Console::display() {
    Window *window = gOpenRaider->mWindow;
    unsigned char color[4] = {0xFF, 0xFF, 0xFF, 0xFF};

    if (mVisible) {
        // Calculate line drawing geometry
        // Depends on window height, so recalculate every time
        LINE_GEOMETRY(window);

        // Draw half-transparent *overlay*
        glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
        glDisable(GL_TEXTURE_2D);
        glRecti(0, 0, window->mWidth, window->mHeight / 2);
        glEnable(GL_TEXTURE_2D);

        int scrollIndicator;
        if (mHistory.size() > lineCount) {
            scrollIndicator = (mHistory.size() - lineCount - mLineOffset) * 100 / (mHistory.size() - lineCount);
        } else {
            scrollIndicator = 100;
        }

        gOpenRaider->mWindow->drawText(10, 10, 0.70f, color,
                "%s uptime %lus scroll %d%%", VERSION, systemTimerGet() / 1000, scrollIndicator);

        // Draw output log
        int end = lineCount;
        int drawOffset = 0;
        int historyOffset = 0;
        if (mHistory.size() < lineCount) {
            end = mHistory.size();
            drawOffset = lineCount - mHistory.size();
        } else if (lineCount < mHistory.size()) {
            historyOffset = mHistory.size() - lineCount;
        }
        for (int i = 0; i < end; i++) {
            gOpenRaider->mWindow->drawText(10, ((i + drawOffset) * lineSteps) + firstLine,
                    0.75f, color, "%s", mHistory[i + historyOffset - mLineOffset]);
        }

        // Draw current input
        if ((mInputBufferPointer > 0) && (mInputBuffer[0] != '\0')) {
            gOpenRaider->mWindow->drawText(10, inputLine, 0.75f, color, "> %s", mInputBuffer);
        } else {
            gOpenRaider->mWindow->drawText(10, inputLine, 0.75f, color, ">");
        }

        //! \todo display the current mPartialInput. The UTF-8 segfaults SDL-TTF, somehow?
    }
}

void Console::handleKeyboard(KeyboardButton key, bool pressed) {
    if (pressed && (key == enter)) {
        // Execute entered command
        if ((mInputBufferPointer > 0) && (mInputBuffer[0] != '\0')) {
            mHistory.push_back(bufferString("> %s", mInputBuffer));
            mCommandHistory.push_back(bufferString("%s", mInputBuffer));
            int error = gOpenRaider->command(mInputBuffer);
            if (error != 0) {
                print("Error Code: %d", error);
            }
        } else {
            mHistory.push_back(bufferString("> "));
        }

        // Clear partial and input buffer
        mInputBufferPointer = 0;
        mInputBuffer[0] = '\0';
        if (mPartialInput != NULL) {
            delete [] mPartialInput;
            mPartialInput = NULL;
        }

        mHistoryPointer = 0;
    }

    //! \fixme only deleting the last byte is not valid for non-ASCII UTF-8 strings
    if (pressed && (key == backspace)) {
        if (mInputBufferPointer > 0) {
            mInputBufferPointer--;
            mInputBuffer[mInputBufferPointer] = '\0';
        }
    }

    if (pressed && ((key == up) || (key == down))) {
        moveInHistory(key == up);
    }
}

void Console::moveInHistory(bool up) {
    if (mCommandHistory.size() == 0)
        return;

    if (up) {
        if (mHistoryPointer < mCommandHistory.size()) {
            mHistoryPointer++;
            if (mHistoryPointer == 1) {
                mUnfinishedInput = bufferString("%s", mInputBuffer);
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
        strcpy(mInputBuffer, mCommandHistory[mCommandHistory.size() - mHistoryPointer]);
        mInputBufferPointer = strlen(mInputBuffer);
    } else {
        if (mUnfinishedInput != NULL) {
            strcpy(mInputBuffer, mUnfinishedInput);
            mInputBufferPointer = strlen(mInputBuffer);
            delete [] mUnfinishedInput;
            mUnfinishedInput = NULL;
        } else {
            mInputBuffer[0] = '\0';
            mInputBufferPointer = 0;
        }
    }
}

void Console::handleText(char *text, bool notFinished) {
    //printf("Text: %s (%s)\n", text, (notFinished ? "not finished" : "finished"));

    // Always scroll to bottom when text input is received
    mLineOffset = 0;

    if (!notFinished) {
        // Finished entering character
        // delete previous partial character, if present
        if (mPartialInput != NULL) {
            delete [] mPartialInput;
        }

        //! \fixme Temporary hack filtering the console activation key
        if (text[0] == '`')
            return;

        // Append new input to buffer
        size_t length = strlen(text);
        if (length > 0) {
            if (((INPUT_BUFFER_SIZE - mInputBufferPointer) < length)) {
                printf("Console input buffer overflowed! (> %d)\n", INPUT_BUFFER_SIZE);
                return;
            }
            strcpy((mInputBuffer + mInputBufferPointer), text);
            mInputBufferPointer += length;
            mInputBuffer[mInputBufferPointer] = '\0';
        }
    } else {
        // Partial character received
        mPartialInput = bufferString("%s", text);
    }
}

void Console::handleMouseScroll(int xrel, int yrel) {
    LINE_GEOMETRY(gOpenRaider->mWindow);

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

