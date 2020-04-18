#include "widget.hpp"
#include "utils.hpp"

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

// Found bug: after procceding to GAME state, buffer doesn't being cleared

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    std::srand(std::time(nullptr));

    m_visualBuffer = new QImage(width(), height(), QImage::Format_ARGB32);
    m_visualBuffer->fill(0x111111);

    m_statePrepareTime = STATE_PREPARE_TIME;
    m_stateGameTime = STATE_GAME_TIME;
    m_currentScore = 0;
    m_bestScore = 0;
    m_hitClicked = 0;
    m_missClicked = 0;
    m_currentState = MENU;

    this->setFocusPolicy(Qt::StrongFocus);
    this->setMouseTracking(true);

    setCircleRadius(utils::getRandomNumber(Circle::MIN_RADIUS, Circle::MAX_RADIUS));
    unsigned centerX = utils::getRandomNumber(m_circle.getRadius(), static_cast<unsigned>(width()) - m_circle.getRadius());
    unsigned centerY = utils::getRandomNumber(m_circle.getRadius(), static_cast<unsigned>(height()) - m_circle.getRadius());
    setCirclePosition(static_cast<int>(centerX), static_cast<int>(centerY));
    setCircleVelocity(utils::getRandomNumber(Widget::MIN_VELOCITY, Widget::MAX_VELOCITY),
                      utils::getRandomNumber(Widget::MIN_VELOCITY, Widget::MAX_VELOCITY));
    setCircleFlowDirection(generateRandomDirection(), generateRandomDirection());

    m_shrinkFlattenFactor = 1;

    m_canDirectionXBeChanged = true;
    m_canDirectionYBeChanged = true;


    QTimer *timer = new QTimer;
    connect(timer, SIGNAL(timeout()),
            this, SLOT(onTimerSlot()));
    timer->setInterval(static_cast<int>(timerInterval));
    timer->start();

    int sourceDimensionX = 860;
    int sourceDimensionY = 400;
    QImage *startButtonSpriteSheet = new QImage(sourceDimensionX / 4, sourceDimensionY / 4 * 3, QImage::Format_ARGB32);
    QString startButtonSpriteSheetLocation = ":/images/StartButton_SpriteSheet.png";
    startButtonSpriteSheet->load(startButtonSpriteSheetLocation);


    m_startButton.setSpriteSheet(*startButtonSpriteSheet);
    delete startButtonSpriteSheet;
    startButtonSpriteSheet = nullptr;

    m_startButton.setState(static_cast<int>(Button::State::IDLE));
    m_startButton.updateSprite();
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter mainPainter(this);
    QPainter bufferPainter(m_visualBuffer);

    switch (m_currentState)
    {

    case GAME:
    {
        int radius = static_cast<int>(m_circle.getRadius());
        unsigned bgColor = 0x111111;
        bufferPainter.drawEllipse(m_circle.center(), radius, radius);
        mainPainter.drawImage(this->rect(), *m_visualBuffer, m_visualBuffer->rect());
        bufferPainter.setPen(QColor(QRgb(bgColor)));

        bufferPainter.fillRect(m_circle.constructBoundingBox(), QBrush(QColor(QRgb(bgColor))));
    } break;

    case MENU:
    {
        m_startButton.draw(100, 100, &bufferPainter);
        mainPainter.drawImage(this->rect(), *m_visualBuffer, m_visualBuffer->rect());
    } break;

    case GAME_OVER:
    {
        mainPainter.drawText(200, 200, "Game Over");
        mainPainter.drawText(200, 220, "Score " + QString::number(m_currentScore));
        mainPainter.drawText(200, 240, "Hit circles: " + QString::number(m_hitClicked));
        mainPainter.drawText(200, 260, "Missed clickes: " + QString::number(m_missClicked));
        mainPainter.drawText(200, 280, "Hit percentage: " + QString::number(calculateHitPercentage()) + " %");

        mainPainter.drawText(200, 320, "Best Score " + QString::number(m_bestScore));
        mainPainter.drawText(200, 340, "1. Return to main menu");
        mainPainter.drawText(200, 360, "2. Exit");
        break;
    }

    }
}

void Widget::onTimerSlot()
{
    switch (m_currentState)
    {
    case PREPARE:
    {
        m_statePrepareTime -= timerInterval;
        if (m_statePrepareTime <= 0)
        {
            m_currentState = GAME;
            m_statePrepareTime = STATE_PREPARE_TIME;
        }
    } break;

    case GAME:
    {
        m_circle.radius() += m_shrinkFlattenFactor * 2;
        int radius = static_cast<int>(m_circle.getRadius());

        if (utils::inRange(static_cast<unsigned>(radius), Circle::MIN_RADIUS, Circle::MAX_RADIUS))
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

        m_stateGameTime -= timerInterval;
        if (m_stateGameTime <= 0)
        {
            m_currentState = GAME_OVER;
            m_stateGameTime = STATE_GAME_TIME;
        }
    } break;
    case GAME_OVER:
    {
        m_bestScore = (m_currentScore > m_bestScore) ? m_currentScore : m_bestScore;
    } break;
    }

    update();
}

void Widget::mousePressEvent(QMouseEvent* mouseEvent)
{
    switch(m_currentState)
    {
    case MENU:
    {
        QPoint mousePos = mouseEvent->pos();
        QRect startButtonRect = m_startButton.getRect();

        if (mouseEvent->type() == QEvent::MouseButtonPress
            && utils::inRange(mousePos.x(), startButtonRect.left(), startButtonRect.right())
            && utils::inRange(mousePos.y(), startButtonRect.top(), startButtonRect.bottom()))
        {
            m_startButton.setState(static_cast<int>(Button::State::CLICKED));
            m_startButton.updateSprite();
        }
    } break;
    case GAME:
    {
        if (isClickedInCircle(mouseEvent))
        {
            increaseScore();
            m_hitClicked++;

            update();
        }
        else
            m_missClicked++;
    } break;

    }
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint mousePos = event->pos();
    QRect startButtonRect = m_startButton.getRect();

    if (utils::inRange(mousePos.x(), startButtonRect.left(), startButtonRect.right())
        && utils::inRange(mousePos.y(), startButtonRect.top(), startButtonRect.bottom()))
    {
        m_startButton.setState(static_cast<int>(Button::State::HOVERED));
        m_startButton.updateSprite();
    }
    else
    {
        m_startButton.setState(static_cast<int>(Button::State::IDLE));
        m_startButton.updateSprite();
    }
}

void Widget::keyPressEvent(QKeyEvent* keyEvent)
{
    switch (m_currentState)
    {

    case MENU:
    {
        if (keyEvent -> key() == Qt::Key_1)
            m_currentState = PREPARE;
        if (keyEvent -> key() == Qt::Key_2)
            qApp -> quit();
    } break;
    case GAME_OVER:
    {
        if (keyEvent->key() == Qt::Key_1)
            m_currentState = MENU;
        if (keyEvent -> key() == Qt::Key_2)
            qApp -> quit();
    } break;

    }
}

Widget::~Widget() {}

int Widget::generateRandomDirection() { return utils::getRandomNumber(0, 1) == 0 ? -1 : 1; }

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
    QPoint mousePos = mouseEvent->pos();

    int clickedX = static_cast<int>(pow( (m_circle.center().x() - mousePos.x()), 2)),
        clickedY = static_cast<int>(pow( (m_circle.center().y() - mousePos.y()), 2));

    unsigned awayFromCenter = static_cast<unsigned>(sqrt(clickedY + clickedX));

    if (awayFromCenter < m_circle.getRadius())
    {
        return true;
    }

    return false;
}

void Widget::increaseScore()
{
    m_currentScore += (60 - m_circle.getRadius());
}

int Widget::calculateHitPercentage()
{
    unsigned allClicks = static_cast<unsigned>(m_missClicked + m_hitClicked);
    double attitude = static_cast<double>(m_hitClicked) / static_cast<double>(allClicks);
    double percent = attitude * 100;

    return static_cast<int>(percent);
}
