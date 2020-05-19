//
// Created by Chris Purdy on 20/04/2020.
//

#ifndef KSP_2D_CW4_GUITEXTBOX_H
#define KSP_2D_CW4_GUITEXTBOX_H

#include <utility>
#include "../KSP2D.h"

/**
 * A class implementing a textbox GUI object
 */
class GUITextbox : public DisplayableObject {
public:
    GUITextbox(KSP2D* pEngine, const char* initText, int x, int y, int _maxLength, std::function<void(std::string)> _callback) : DisplayableObject(pEngine, 48, 26),
                    callback(std::move(_callback)), inputBar(ImageManager::get()->loadImage("spacecraftBuilderUITextbox.png")),
                    maxLength(_maxLength), isKeyListener(false), currentLength(0), value(initText) {
        setPosition(x, y);
        pointerIndex = 0;
        charWidth = 17;
        inputBar.setTransparencyColour(0x000000);
    }

    ~GUITextbox() {
        getEngine()->removeDisplayableObject(this);
    }

    void virtDoUpdate(int iCurrentTime) override {
    }

    void virtMouseDown(int iButton, int x, int y) override {
        if (x >= (m_iCurrentScreenX + 11) && x < (m_iCurrentScreenX + currentLength + 11)) {
            if (y >= m_iCurrentScreenY && y < m_iCurrentScreenY + 26) {
                isKeyListener = true;
                dynamic_cast<KSP2D*>(m_pEngine)->isKeyListener = false;

                // calculate from the mouse X where to place the text input pointer
                // TODO could be more efficient?
                int pointerX = m_iCurrentScreenX + 32;
                pointerIndex = 0;
                for (char c : value) {
                    ++pointerIndex;
                    pointerX += charWidth;
                    if (pointerX > x) {
                        --pointerIndex;
                        break;
                    }
                }
                return;
            }
        }
        isKeyListener = false;
    }

//    void virtKeyUp(int iKeyCode) override {
//    }

    void virtKeyDown(int iKeyCode) override {

        if (!isKeyListener) return;

        switch (iKeyCode) {
            case SDLK_BACKSPACE:
                if (!value.empty() && pointerIndex > 0) {
                    value.erase(pointerIndex - 1, 1);
                    --pointerIndex;
                }
                break;
            case SDLK_LEFT:
                if (pointerIndex > 0) --pointerIndex;
                break;
            case SDLK_RIGHT:
                if (pointerIndex < value.size()) ++pointerIndex;
                break;
            default:
                if (value.size() < maxLength) {
                    if (iKeyCode >= 32 && iKeyCode <= 126) {
                        value.insert(pointerIndex, 1, (char)iKeyCode);
                        ++pointerIndex;
                    }
                }
        }

    }

    void draw(int time) {
        if (isVisible()) {
            // draw front end of bar
            inputBar.renderImageBlit(m_pEngine, m_pEngine->getForegroundSurface(), m_iCurrentScreenX, m_iCurrentScreenY,
                                     16, 26, 0, 0, 16, 26);

            //draw middle parts depending on length of value
            int middleParts = (value.size() * charWidth)/16 + 2;
            for (int i = 1; i <= middleParts; ++i)
                inputBar.renderImageBlit(m_pEngine, m_pEngine->getForegroundSurface(), m_iCurrentScreenX + i * 16,
                                         m_iCurrentScreenY, 16, 26, 16, 0, 16, 26);

            // draw other end of bar
            inputBar.renderImageBlit(m_pEngine, m_pEngine->getForegroundSurface(),
                                     m_iCurrentScreenX + (middleParts + 1) * 16, m_iCurrentScreenY, 16, 26, 32, 0, 16,
                                     26);

            currentLength = middleParts * 16 + 10;

            // draw text
            if (!value.empty()) {
                int stringOffset = 0;
                char* str = (char*)calloc(2, sizeof(char));
                for (char c : value) {
                    str[0] = c;
                    m_pEngine->drawForegroundString(m_iCurrentScreenX + 32 + stringOffset, m_iCurrentScreenY + 2, str,
                                                    0x000000,getEngine()->getFont("kenpixel.ttf", 16));
                    stringOffset += charWidth;
                }
                free(str);
            }

            // draw marker for text input
            if (isKeyListener) {
                m_pEngine->drawForegroundString(m_iCurrentScreenX + 32 + pointerIndex * charWidth, m_iCurrentScreenY + 2, (time%500 > 250 ? "|" : " "), 0x777777,
                                                getEngine()->getFont("kenpixel.ttf", 16));
            }
        }
    }


    void virtDraw() override {
        // do nothing since the GUIManager draws this
    }

    std::string value;

    int maxLength;
    int currentLength;

    bool isKeyListener;
    int pointerIndex; // index in value string of text input pointer (as in index of char which it is to the left of)
    int charWidth;

    std::function<void(std::string)> callback;

    SimpleImage inputBar;
};

#endif //KSP_2D_CW4_GUITEXTBOX_H
