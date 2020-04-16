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

// TODO: Should refactor it from here.
template <typename T>
T getRandomNumber(T min, T max) {
    static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
    return static_cast<T>(rand() * fraction * (max - min + 1) + min);
}


Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    statePrepareTime = STATE_PREPARE_TIME;
    stateGameTime = STATE_GAME_TIME;
    currentScore = 0;
    bestScore = 0;
    hitClicked = 0;
    missClicked = 0;
    currentState = MENU;

    setFocusPolicy(Qt::StrongFocus);

    // setRandomTarget();



    QTimer *timer = new QTimer;
    connect(timer, SIGNAL(timeout()),
            this, SLOT(onTimerSlot()));
    timer->setInterval(static_cast<int>(timerInterval));
    timer->start();
    m_shrinkFlattenFactor = 1;
    m_circleVelocityX = getRandomNumber(Widget::MIN_VELOCITY, Widget::MAX_VELOCITY);
    m_circleVelocityY = getRandomNumber(Widget::MIN_VELOCITY, Widget::MAX_VELOCITY);


    mainFont.setFamily("monaco");
    mainFont.setPixelSize(20);
    mainFont.setBold(true);
    setFont(mainFont);
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter mainPainter(this);
    QPainter bufferPainter;

    // TODO: Rewrite double-buffered image draw logic
    //drawBufferedFrame(&bufferPainter);

    switch (currentState)
    {

    case GAME:
    {
        mainPainter.drawEllipse(m_circle.center(), m_circle.getRadius(), m_circle.getRadius());
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
        if (m_circle.getRadius() >= Circle::MAX_RADIUS || m_circle.getRadius() < Circle::MIN_RADIUS)
        {
            m_shrinkFlattenFactor *= -1;
        }

        m_circle.center().setX(static_cast<int>(m_circle.center().x() + m_circleVelocityX));
        m_circle.center().setY(static_cast<int>(m_circle.center().y() + m_circleVelocityY));

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

//            vectorX = setRandomVector();
//            vectorY = setRandomVector();
            setRandomTarget();
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

void Widget::setRandomTargetPosition()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    unsigned centerX = getRandomNumber(m_circle.getRadius(), static_cast<unsigned>(width()) - m_circle.getRadius());
    unsigned centerY = getRandomNumber(m_circle.getRadius(), static_cast<unsigned>(height()) - m_circle.getRadius());
    m_circle.center().setX(static_cast<int>(centerX));
    m_circle.center().setY(static_cast<int>(centerY));
}

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

//void Widget::drawBufferedFrame(QPainter* painter)
//{
//    switch (currentState)
//    {
//    case PREPARE:
//    {
//        int topLeftX = width()/2 - 80;
//        int topLeftY = height()/2;

//        if (statePrepareTime <= 20)
//        {
//            painter->drawImage(topLeftX - 20, topLeftY - 20, piece);
//            break;
//        }
//        painter->drawImage(topLeftX - 20, topLeftY - 20, piece);
//        painter->drawText(topLeftX, topLeftY, "Prepare..." + QString::number(statePrepareTime / 1000) + "." +
//                          QString::number(statePrepareTime % 1000));
//        break;
//    }
//    case GAME:
//    {

//        int circleBoxLX = center.x() + ((vectorX > 0) ? -radius : radius);
//        int circleBoxLY = center.y() + ((vectorY > 0) ? -radius : radius);

//        QRect boundingBox(circleBoxLX, circleBoxLY, (radius*2), (radius*2));
//        QImage circleEraser = sourceBackgroundImage->copy(boundingBox);

//        painter->drawImage(circleBoxLX, circleBoxLY, circleEraser);

//        QPoint newCenter = center;
//        newCenter.setX(static_cast<int>(center.x() + vectorX*5));
//        newCenter.setY(static_cast<int>(center.y() + vectorY*5));


//        painter->drawEllipse(newCenter, radius, radius);
//    }
//    }
//}
