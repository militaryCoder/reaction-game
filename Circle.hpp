#pragma once

#include <QApplication>
#include <QPoint>
#include <QRect>

class Circle
{
public:
    Circle();
    Circle(unsigned r) : m_radius(r) {}

    unsigned getRadius() const;
    void setRadius(const unsigned val);

    auto getBoundRect() const -> QRect;
    void setBoundRect(const QRect &R);

    auto getCenter() const -> const QPoint&;
    void setCenter(const QPoint &p);

    auto center() -> QPoint&;
    auto radius() -> unsigned&;

    static const unsigned MIN_RADIUS = 10;
    static const unsigned MAX_RADIUS = 50;
private:
    QPoint m_center;
    QRect m_bounds;

    unsigned m_radius;
};
