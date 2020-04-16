#include <cmath>

#include "Circle.hpp"

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

auto Circle::getBoundRect() const -> QRect
{
    return m_bounds;
}

void Circle::setBoundRect(const QRect &R)
{
    m_bounds = R;
}
