#include <cmath>

#include "Circle.hpp"

Circle::Circle()
{
    m_radius = Circle::MIN_RADIUS;
}

auto Circle::getCenter() const -> const QPoint&
{
    return m_center;
}

void Circle::setCenter(const QPoint &p)
{
    m_center = p;

    int halfRadius = m_radius / 2;
    m_bounds.setLeft(m_center.x() - halfRadius);
    m_bounds.setTop(m_center.y() - halfRadius);
    m_bounds.setRight(m_center.x() + halfRadius);
    m_bounds.setBottom(m_center.y() + halfRadius);
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

    m_bounds.setWidth(static_cast<int>(m_radius) * 2);
    m_bounds.setHeight(static_cast<int>(m_radius) * 2);
}

auto Circle::getBoundRect() const -> QRect
{
    return m_bounds;
}

void Circle::setBoundRect(const QRect &R)
{
    m_bounds = R;
}
