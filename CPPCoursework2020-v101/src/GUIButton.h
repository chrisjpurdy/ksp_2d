//
// Created by Chris Purdy on 14/05/2020.
//

#ifndef KSP_2D_CW4_GUIBUTTON_H
#define KSP_2D_CW4_GUIBUTTON_H

#include <utility>
#include "KSP2D.h"

/**
 * A class implementing a button GUI object
 */
class GUIButton : public DisplayableObject {
public:
    GUIButton(KSP2D* pEngine, const char* promptText, int x, int y, int w, int h, std::function<void(void)> _callback) : DisplayableObject(pEngine, w, h),
                    callback(std::move(_callback)), prompt(promptText), pressed(false),
                    buttonUp(ImageManager::get()->getSpriteFromImg(ImageManager::get()->getSpriteData("buttonUp_0"))),
                    buttonDown(ImageManager::get()->getSpriteFromImg(ImageManager::get()->getSpriteData("buttonDown_0"))) {
        setPosition(x, y);
        buttonUp.setTransparencyColour(0x000000);
        buttonDown.setTransparencyColour(0x000000);
    }

    ~GUIButton() {
        getEngine()->removeDisplayableObject(this);
    }

    void virtDoUpdate(int iCurrentTime) override {
    }

    void virtMouseUp(int iButton, int x, int y) override {
        pressed = false;
        if (x >= (m_iCurrentScreenX) && x < (m_iCurrentScreenX + getDrawWidth())) {
            if (y >= m_iCurrentScreenY && y < m_iCurrentScreenY + getDrawHeight()) {
                //std::cout << "Calling callback" << std::endl;
                callback();
            }
        }
    }

    void virtMouseDown(int iButton, int x, int y) override {
        if (x >= (m_iCurrentScreenX) && x < (m_iCurrentScreenX + getDrawWidth())) {
            if (y >= m_iCurrentScreenY && y < m_iCurrentScreenY + getDrawHeight()) {
                pressed = true;
            }
        }
    }

    void draw(int time) {
        if (isVisible()) {
            SimpleImage* button = pressed ? &buttonDown : &buttonUp;

            button->renderImageBlit(m_pEngine, m_pEngine->getForegroundSurface(),
                    m_iCurrentScreenX, m_iCurrentScreenY + (pressed ? 4 : 0),
                    getDrawWidth(), getDrawHeight(), 0, 0,
                    buttonUp.getWidth(), buttonUp.getHeight());

            m_pEngine->drawForegroundString(m_iCurrentScreenX + 10, m_iCurrentScreenY + 10 + (pressed ? 4 : 0), prompt.c_str(),
                                            0x000000, getEngine()->getFont("kenpixel.ttf", 16));
        }
    }


    void virtDraw() override {
        // do nothing since the GUIManager draws this
    }

    std::string prompt;
    bool pressed;

    std::function<void(void)> callback;
    SimpleImage buttonUp;
    SimpleImage buttonDown;
};

#endif //KSP_2D_CW4_GUIBUTTON_H
