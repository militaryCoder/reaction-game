#include <cmath>

#include "Circle.hpp"

Circle::Circle()
{
    m_radius = Circle::MIN_RADIUS;
}

Circle::Circle(const QPoint &center, unsigned r)
{
    m_center = center;
    m_radius = r;
}

auto Circle::getCenter() const -> const QPoint&
{
    return m_center;
}

void Circle::setCenter(const QPoint &p)
{
    m_center = p;
}

auto Circle::center() -> QPoint&
{
    return m_center;
}

auto Circle::radius() -> unsigned&
{
    return m_radius;
}

unsigned Circle::getRadius() const
{
    return m_radius;
}

void Circle::setRadius(const unsigned val)
{
    m_radius = static_cast<unsigned>(std::abs(static_cast<float>(val)));
}

const QRect Circle::constructBoundingBox()
{
    QRect boundBox;
    boundBox.setX(m_center.x() - m_radius);
    boundBox.setY(m_center.y() - m_radius);
    boundBox.setWidth(m_radius * 2 + 1);
    boundBox.setHeight(m_radius * 2 + 1);

    return boundBox;
}
