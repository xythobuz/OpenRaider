/*!
 * \file src/Menu.cpp
 * \brief Menu 'overlay'
 *
 * \author xythobuz
 */

#include "global.h"
#include "Font.h"
#include "Log.h"
#include "Window.h"
#include "Menu.h"
#include "MenuFolder.h"

void Menu::showDialog(std::string msg, std::string btn1, std::string btn2,
                      std::function<int (bool state)> callback) {
    // Only show one dialog at a time
    assert(dialogText.length() == 0);
    assert(dialogButton1.length() == 0);
    assert(dialogButton2.length() == 0);

    assert(msg.length() > 0);
    assert(btn1.length() > 0);

    dialogText = msg;
    dialogButton1 = btn1;
    dialogButton2 = btn2;
    dialogState = false;
    dialogFunction = callback;

    getLog() << dialogText << Log::endl;
}

void Menu::ackDialog() {
    dialogText = "";
    dialogButton1 = "";
    dialogButton2 = "";

    if (dialogFunction) {
        int error = dialogFunction(dialogState);
        if (error != 0) {
            showDialog("Error processing dialog callback!", "OK", "");
        }
    }

    dialogState = false;
}

bool Menu::handleKeyboardDialog(KeyboardButton key, bool pressed) {
    if (!pressed)
        return (dialogText.length() > 0);

    if (dialogText.length() > 0) {
        if (dialogButton2.length() == 0) {
            if (key == enterKey) {
                ackDialog();
            }
        } else {
            if (key == enterKey) {
                ackDialog();
            } else if (key == leftKey) {
                dialogState = !dialogState;
            } else if (key == rightKey) {
                dialogState = !dialogState;
            } else if (key == upKey) {
                dialogState = !dialogState;
            } else if (key == downKey) {
                dialogState = !dialogState;
            }
        }
        return true;
    }

    return false;
}

bool Menu::handleMouseClickDialog(unsigned int x, unsigned int y,
                                  KeyboardButton button, bool released) {
    //! \todo Allow mouse usage of Menu dialogs!
    return (dialogText.length() > 0);
}

bool Menu::handleMouseScrollDialog(int xrel, int yrel) {
    //! \todo Allow mouse usage of Menu dialogs!
    return (dialogText.length() > 0);
}

void Menu::displayDialog() {
    if (dialogText.length() > 0) {
        unsigned int wMax = ((unsigned int)(::getWindow().getWidth() * 0.66f));

        unsigned int w0 = Font::widthText(1.0f, dialogText) + 20;
        if (w0 > wMax)
            w0 = wMax;
        unsigned int h0 =  Font::heightText(1.0f, w0, dialogText) + 10;

        assert(dialogButton1.length() > 0);
        unsigned int w1 = Font::widthText(1.0f, dialogButton1) + 20;
        if (w1 > wMax)
            w1 = wMax;
        unsigned int h1 = Font::heightText(1.0f, w1, dialogButton1) + 10;

        unsigned int wOverlay = wMax, hOverlay, w2 = 0, h2 = 0;

        if (dialogButton2.length() > 0) {
            // Show text and two buttons
            w2 = Font::widthText(1.0f, dialogButton2) + 20;
            if (w2 > wMax)
                w2 = wMax;
            h2 = Font::heightText(1.0f, w2, dialogButton2) + 10;

            if (w0 > (w1 + w2)) {
                if (w0 < wMax) {
                    wOverlay = w0;
                }
            } else if (w0 < (w1 + w2)) {
                if ((w1 + w2) < wMax) {
                    wOverlay = (w1 + w2);
                }
            }

            if ((w1 + w2) <= wMax) {
                hOverlay = h0 + ((h1 + h2) / 2);
            } else {
                hOverlay = h0 + h1 + h2;
            }
        } else {
            // Show text and one button
            if (w0 > w1) {
                if (w0 < wMax) {
                    wOverlay = w0;
                }
            } else if (w0 < w1) {
                if (w1 < wMax) {
                    wOverlay = w1;
                }
            }

            hOverlay = h0 + h1;
        }

        unsigned int xOverlay = (::getWindow().getWidth() - wOverlay) / 2;
        unsigned int yOverlay = (::getWindow().getHeight() - hOverlay) / 2;

        glColor4f(0.0f, 0.0f, 0.0f, 0.75f);
        glDisable(GL_TEXTURE_2D);
        glRecti(xOverlay, yOverlay, xOverlay + wOverlay, yOverlay + hOverlay);
        glEnable(GL_TEXTURE_2D);

        Font::drawTextWrapped(xOverlay + 10, yOverlay + 5, 1.0f, BLUE, w0, dialogText);
        if (dialogButton2.length() > 0) {
            if ((w1 + w2) <= wMax) {
                Font::drawTextWrapped(xOverlay + 10, yOverlay + 10 + h0, 1.0f,
                                      dialogState ? BLUE : RED, w1, dialogButton1);
                Font::drawTextWrapped(xOverlay + 10 + w1, yOverlay + 10 + h0,
                                      1.0f, dialogState ? RED : BLUE, w2, dialogButton2);
            } else {
                Font::drawTextWrapped((::getWindow().getWidth() - w1) / 2,
                                      yOverlay + 10 + h0, 1.0f, dialogState ? BLUE : RED, w1, dialogButton1);
                Font::drawTextWrapped((::getWindow().getWidth() - w2) / 2,
                                      yOverlay + 10 + h0 + h1, 1.0f, dialogState ? RED : BLUE, w2, dialogButton2);
            }
        } else {
            Font::drawTextWrapped((::getWindow().getWidth() - w1) / 2,
                                  yOverlay + 10 + h0, 1.0f, RED, w1, dialogButton1);
        }
    }
}

