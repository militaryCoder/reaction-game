#pragma once

#include <QImage>

class Button
{
    QImage m_stateSprites;
    int m_currentState;

public:
    Button(int x, int y);
    Button(const QPoint &p);
    void setSprite();
    void changeState(int newState);
    enum class State
    {
        IDLE,
        HOVERED,
        CLICKED
    };


};
