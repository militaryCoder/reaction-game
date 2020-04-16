#include "widget.hpp"

#include <QApplication>
#include <QPainter>
#include <QMouseEvent>
#include <QPoint>
#include <QRect>
#include <QTimer>
#include <QImage>
#include <QBuffer>

#include <cmath>
#include <cstdlib>
#include <ctime>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    std::srand(std::time(nullptr));

    m_visualBuffer = new QImage(width(), height(), QImage::Format_RGB32);
    m_visualBuffer->fill(0xFFB6A6);

    statePrepareTime = STATE_PREPARE_TIME;
    stateGameTime = STATE_GAME_TIME;
    currentScore = 0;
    bestScore = 0;
    hitClicked = 0;
    missClicked = 0;
    currentState = MENU;

    setFocusPolicy(Qt::StrongFocus);

    setCircleRadius(getRandomNumber(Circle::MIN_RADIUS, Circle::MAX_RADIUS));
    unsigned centerX = getRandomNumber(m_circle.getRadius(), static_cast<unsigned>(width()) - m_circle.getRadius());
    unsigned centerY = getRandomNumber(m_circle.getRadius(), static_cast<unsigned>(height()) - m_circle.getRadius());
    setCirclePosition(static_cast<int>(centerX), static_cast<int>(centerY));
    setCircleVelocity(getRandomNumber(Widget::MIN_VELOCITY, Widget::MAX_VELOCITY),
                      getRandomNumber(Widget::MIN_VELOCITY, Widget::MAX_VELOCITY));
    setCircleFlowDirection(generateRandomDirection(), generateRandomDirection());

    m_shrinkFlattenFactor = 1;

    m_canDirectionXBeChanged = true;
    m_canDirectionYBeChanged = true;


    QTimer *timer = new QTimer;
    connect(timer, SIGNAL(timeout()),
            this, SLOT(onTimerSlot()));
    timer->setInterval(static_cast<int>(timerInterval));
    timer->start();

    mainFont.setFamily("monaco");
    mainFont.setPixelSize(20);
    mainFont.setBold(true);
    setFont(mainFont);
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter mainPainter(this);
    QPainter bufferPainter(m_visualBuffer);

    switch (currentState)
    {

    case GAME:
    {
        int radius = static_cast<int>(m_circle.getRadius());
        unsigned bgColor = 0xFFB6A6;
        bufferPainter.drawEllipse(m_circle.center(), radius, radius);
        mainPainter.drawImage(this->rect(), *m_visualBuffer, m_visualBuffer->rect());
        bufferPainter.setPen(QColor(QRgb(bgColor)));

        QRect eraser(m_circle.center().x() - radius, m_circle.center().y() - radius,
                     radius*2 + 1, radius*2 + 1);
        bufferPainter.fillRect(m_circle.constructBoundingBox(), QBrush(QColor(QRgb(bgColor))));
    } break;

    case MENU:
    {
        drawMainMenuUI(width()/2 - 80, height()/2, &mainPainter);
        break;
    }
    case GAME_OVER:
    {
        mainPainter.drawText(200, 200, "Game Over");
        mainPainter.drawText(200, 220, "Score " + QString::number(currentScore));
        mainPainter.drawText(200, 240, "Hit circles: " + QString::number(hitClicked));
        mainPainter.drawText(200, 260, "Missed clickes: " + QString::number(missClicked));
        mainPainter.drawText(200, 280, "Hit percentage: " + QString::number(calculateHitPercentage()) + " %");

        mainPainter.drawText(200, 320, "Best Score " + QString::number(bestScore));
        mainPainter.drawText(200, 340, "1. Return to main menu");
        mainPainter.drawText(200, 360, "2. Exit");
        break;
    }

    }
}

void Widget::onTimerSlot()
{
    switch (currentState)
    {
    case PREPARE:
    {
        statePrepareTime -= timerInterval;
        if (statePrepareTime <= 0)
        {
            currentState = GAME;
            statePrepareTime = STATE_PREPARE_TIME;
        }
    } break;

    case GAME:
    {
        m_circle.radius() += m_shrinkFlattenFactor * 2;
        int radius = static_cast<int>(m_circle.getRadius());

        if (radius >= static_cast<int>(Circle::MAX_RADIUS) || radius < static_cast<int>(Circle::MIN_RADIUS))
        {
            m_shrinkFlattenFactor *= -1;
        }

        if (m_circle.center().x() > radius
                && m_circle.center().x() < width() - radius)
        {
            m_canDirectionXBeChanged = true;
        }

        if (m_circle.center().y() > radius
                && m_circle.center().y() < height() - radius)
        {
            m_canDirectionYBeChanged = true;
        }

        if ((m_circle.center().x() >= width() - radius
                || m_circle.center().x() <= radius)
                && m_canDirectionXBeChanged)
        {
            setCircleFlowDirection(-m_directionX, m_directionY);
            m_canDirectionXBeChanged = false;
        }
        if ((m_circle.center().y() >= height() - radius
                || m_circle.center().y() <= radius)
                && m_canDirectionYBeChanged)
        {
            setCircleFlowDirection(m_directionX, -m_directionY);
            m_canDirectionYBeChanged = false;
        }

        m_circle.center().setX(static_cast<int>(m_circle.center().x() + m_circleVelocityX*m_directionX));
        m_circle.center().setY(static_cast<int>(m_circle.center().y() + m_circleVelocityY*m_directionY));

        stateGameTime -= timerInterval;
        if (stateGameTime <= 0)
        {
            currentState = GAME_OVER;
            stateGameTime = STATE_GAME_TIME;
        }
    } break;
    case GAME_OVER:
    {
        bestScore = (currentScore > bestScore) ? currentScore : bestScore;
    } break;
    }

    update();
}

void Widget::mousePressEvent(QMouseEvent* mouseEvent)
{
    if (currentState == GAME)
    {
        if (isClickedInCircle(mouseEvent))
        {
            increaseScore();
            hitClicked++;

            update();
        }
        else
            missClicked++;
    }

}

void Widget::keyPressEvent(QKeyEvent* keyEvent)
{
    switch (currentState)
    {

    case MENU:
    {
        if (keyEvent -> key() == Qt::Key_1)
            currentState = PREPARE;
        if (keyEvent -> key() == Qt::Key_2)
            qApp -> quit();
    } break;
    case GAME_OVER:
    {
        if (keyEvent->key() == Qt::Key_1)
            currentState = MENU;
        if (keyEvent -> key() == Qt::Key_2)
            qApp -> quit();
    } break;

    }
}

Widget::~Widget() {}

int Widget::generateRandomDirection() { return getRandomNumber(0, 1) == 0 ? -1 : 1; }

void Widget::setCircleFlowDirection(int directionX, int directionY)
{
    m_directionX = directionX;
    m_directionY = directionY;
}

void Widget::setCircleVelocity(int velocityX, int velocityY)
{
    m_circleVelocityX = velocityX;
    m_circleVelocityY = velocityY;
}

void Widget::setCirclePosition(int x, int y)
{
    m_circle.center().setX(x);
    m_circle.center().setY(y);
}

void Widget::setCircleRadius(unsigned int r)
{
    m_circle.radius() = r;
}

bool Widget::isClickedInCircle(QMouseEvent* mouseEvent)
{
    QPoint pointClicked = mouseEvent->pos();

    int clickedX = static_cast<int>(pow( (m_circle.center().x() - pointClicked.x()), 2)),
        clickedY = static_cast<int>(pow( (m_circle.center().y() - pointClicked.y()), 2));

    unsigned awayFromCenter = static_cast<unsigned>(sqrt(clickedY + clickedX));

    if (awayFromCenter < m_circle.getRadius())
    {
        return true;
    }

    return false;
}

void Widget::increaseScore()
{
    currentScore += (60 - m_circle.getRadius());
}

int Widget::calculateHitPercentage()
{
    unsigned allClicks = static_cast<unsigned>(missClicked + hitClicked);
    double attitude = static_cast<double>(hitClicked) / static_cast<double>(allClicks);
    double percent = attitude * 100;

    return static_cast<int>(percent);
}

void Widget::drawMainMenuUI(int x, int y, QPainter* menuPainter)
{
    unsigned int shift = 20;

    menuPainter->drawText(width()/2 - 100, 30, "Reaction Game");

    menuPainter->drawText(x, y, "1. Start game");
    y += shift;

    menuPainter->drawText(x, y, "2. Quit game");
    y += shift * 2;

    menuPainter->drawText(x, y, "Best score " + QString::number(bestScore));
}
