#pragma once

#include <QApplication>
#include <QPoint>
#include <QRect>

class Circle
{
public:
    Circle();
    Circle(unsigned r) : m_radius(r) {}
    Circle(const QPoint &center, unsigned r);

    unsigned getRadius() const;
    void setRadius(const unsigned val);

    auto getCenter() const -> const QPoint&;
    void setCenter(const QPoint &p);

    auto center() -> QPoint&;
    auto radius() -> unsigned&;

    const QRect constructBoundingBox();

    static const unsigned MIN_RADIUS = 10;
    static const unsigned MAX_RADIUS = 50;
private:
    QPoint m_center;

    unsigned m_radius;
};
