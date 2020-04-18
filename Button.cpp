#include "Button.hpp"

#include <QPainter>

void Button::setSpriteSheet(const QImage &spriteSheet)
{
    m_spriteSheet = spriteSheet;
    m_rect.setWidth(m_spriteSheet.width());
    m_rect.setHeight(m_spriteSheet.height() / 3);

    m_spriteWidth = m_spriteSheet.width();
    m_spriteHeight = m_spriteSheet.height() / 3;
}

int Button::getState() const
{
    return m_currentState;
}

void Button::setState(int newState)
{
    m_currentState = newState;
}

QRect Button::getRect() const
{
    return m_rect;
}

QRect &Button::rect()
{
    return m_rect;
}

QImage Button::getCurrentSprite() const
{
    return m_currentSprite;
}

void Button::updateSprite()
{
    switch(m_currentState)
    {
    case static_cast<int>(State::IDLE):
    {
        m_currentSprite = m_spriteSheet.copy(0, 0, m_spriteSheet.width(), m_spriteSheet.height() / 3);
    } break;

    case static_cast<int>(State::HOVERED):
    {
        m_currentSprite = m_spriteSheet.copy(0, m_spriteSheet.height()/3, m_spriteSheet.width(), m_spriteSheet.height() / 3);
    } break;

    case static_cast<int>(State::CLICKED):
    {
        m_currentSprite = m_spriteSheet.copy(0, m_spriteSheet.height()/3*2, m_spriteSheet.width(), m_spriteSheet.height() / 3);
    } break;

    }
}

void Button::draw(int x, int y, QPainter *painter)
{
    m_rect.setX(x);
    m_rect.setY(y);
    m_rect.setRight(x + m_spriteWidth);
    m_rect.setBottom(y + m_spriteHeight);
    painter->drawImage(x, y, m_currentSprite);
}
