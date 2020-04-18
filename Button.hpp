#pragma once

#include <QImage>

class Button
{
    QPoint m_topLeft;
    QImage m_spriteSheet;
    QImage m_currentSprite;
    QRect m_rect;
    int m_currentState;

    int m_spriteWidth;
    int m_spriteHeight;

public:
    Button() = default;

    void setSprite();
    void updateSprite();
    void setSpriteSheet(const QImage &spriteSheet);
    void setState(int newState);

    void draw(int x, int y, QPainter *painter);

    int getState() const;

    QRect getRect() const;
    QRect &rect();

    QImage getCurrentSprite() const;

    enum class State
    {
        IDLE,
        HOVERED,
        CLICKED
    };
};
