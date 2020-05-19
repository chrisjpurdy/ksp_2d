//
// Created by Chris Purdy on 21/03/2020.
//

#ifndef KSP_2D_CW4_GUISLIDER_H
#define KSP_2D_CW4_GUISLIDER_H

#include "header.h"
#include "DragableObject.h"

template <typename T>
class GUISlider : public DragableObject {
public:
    GUISlider(BaseEngine* pEngine, int x, int y, int length, T& _value, T _minVal, T _maxVal, std::function<void(T)> _callback, unsigned int _colour, bool _logarithmic = true)
                    : DragableObject(pEngine, 15, 30), xMin(x), xMax(x + length), yStatic(y), colour(_colour), minVal(_minVal), maxVal(_maxVal), callback(_callback),
                    halfWidth(getDrawWidth()/2), halfHeight(getDrawHeight()/2), logarithmic(_logarithmic), maxMinusMin(_maxVal - _minVal), value(_value), locked(false) {
        if (logarithmic) {
            logMin = ::log(minVal);
            logMax = ::log(maxVal);
            logMaxMinusMin = logMax - logMin;
        }
        m_iCurrentScreenX = (xMin - halfWidth) + (length/2.0);
        doDragRedraw();
    }

    void virtMouseDown(int iButton, int iX, int iY) override {
        if (!locked)
            DragableObject::virtMouseDown(iButton, iX, iY);
    }

    void doDragRedraw() override {
        m_iCurrentScreenY = yStatic - halfHeight;
        restrictX();
        DragableBaseClassObject::doDragRedraw();
    }

    void virtDoUpdate(int iCurrentTime) override {
        if (changeCheckVal != value) {
            //std::cerr << "Something else changed the slider value!" << std::endl;
            updateSliderPos();
            changeCheckVal = value;
        }
    }

    void draw() {
        if (isVisible()) {
            getEngine()->drawForegroundString(xMin, yStatic + (halfHeight + 5), std::to_string(value).c_str(), 0xffffff, getEngine()->getFont("kenpixel.ttf", 16));
            getEngine()->drawForegroundLine(xMin, yStatic, xMax, yStatic, 0xffffff);
            getEngine()->drawForegroundRectangle(m_iCurrentScreenX, yStatic - halfHeight,
                                                 m_iCurrentScreenX + m_iDrawWidth, yStatic + halfHeight,
                                                 colour);
            if (locked)
                getEngine()->drawForegroundRectangle(m_iCurrentScreenX - 1, yStatic - 3,
                                                     m_iCurrentScreenX + m_iDrawWidth + 1, yStatic + 3,
                                                     0x444444);
        }
    }

    void virtDraw() override {
        // do nothing since the GUIManager draws this
    }

    inline void restrictX() {
        if ((m_iCurrentScreenX + halfWidth) > xMax) m_iCurrentScreenX = xMax - halfWidth;
        else if ((m_iCurrentScreenX + halfWidth) < xMin) m_iCurrentScreenX = xMin - halfWidth;
        else updateValue();
    }

    void updateValue() {
        float sliderPercent = ((float)(m_iCurrentScreenX + halfWidth) - xMin)/(xMax - xMin);
        if (logarithmic) value = ::exp(logMin + (logMaxMinusMin * sliderPercent));
        else value = minVal + (maxMinusMin * sliderPercent);
        changeCheckVal = value;
        // now perform callback with updated value
        if (callback) callback(value);
    }

    void updateMin(T minNew) {
        if (logarithmic) {
            logMin = ::log(minNew);
            logMaxMinusMin = logMax - logMin;
            if (value < ::exp(logMin)) value = ::exp(logMin);
        } else {
            minVal = minNew;
            maxMinusMin = maxVal - minVal;
            if (value < minNew) value = minNew;
        }
        if (!m_bIsBeingDragged) updateSliderPos();
    }

    void updateMax(T maxNew) {
        if (logarithmic) {
            logMax = ::log(maxNew);
            logMaxMinusMin = logMax - logMin;
            if (value > ::exp(logMax)) value = ::exp(logMax);
        } else {
            maxVal = maxNew;
            maxMinusMin = maxVal - minVal;
            if (value > maxNew) value = maxNew;
        }
        if (!m_bIsBeingDragged) updateSliderPos();
    }

    void updateSliderPos() {

        float sliderPercent;

        if (logarithmic) sliderPercent = (::log(value) - logMin) / logMaxMinusMin;
        else sliderPercent = (value - minVal) / maxMinusMin;

        m_iCurrentScreenX = (xMin + sliderPercent*(xMax - xMin)) - m_iDrawWidth/2;
        //doDragRedraw();
        if (callback) callback(value);
    }

    inline void lock() { locked = true; }
    inline void unlock() { locked = false; }

    double xMin, xMax;
    double yStatic;
    unsigned int colour;
    double halfWidth, halfHeight;

    // slightly poor practice, but these are here to use if the slider is a logarithmic scale
    bool logarithmic;
    double logMin, logMax;
    double logMaxMinusMin;

    T minVal, maxVal;
    T maxMinusMin;
    T& value;
    T changeCheckVal;

    bool locked;

    std::function<void(T)> callback;

};


#endif //KSP_2D_CW4_GUISLIDER_H
