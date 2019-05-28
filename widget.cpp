#include "widget.h"

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

void Widget::paintEvent(QPaintEvent *)
{
    QPainter mainPainter(this);
    QPainter bufferPainter(temporaryBackground);

    drawBufferedFrame(&bufferPainter);
    mainPainter.drawImage(0, 0, *temporaryBackground);

    switch (currentState)
    {

    case MENU:
    {
        drawMainMenuUI(width()/2 - 80, height()/2, &mainPainter);
        break;
    }
//    case GAME:
//    {
//        mainPainter.drawEllipse(center, radius, radius);
//        mainPainter.drawText(width()/2 - 10, 20, QString::number(stateGameTime / 1000 + 1));
//        mainPainter.drawText(width()/2 - 10, height() - 20, QString::number(currentScore));
//        break;
//    }
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

    case MENU:
    {
        break;
    }
    case PREPARE:
    {
        statePrepareTime -= timerInterval;
        if (statePrepareTime <= 0)
        {
            currentState = GAME;
            statePrepareTime = STATE_PREPARE_TIME;
        }
        break;
    }
    case GAME:
    {
        radius += factor * 2;
        if (radius > 40 || radius < 20)
        {
            factor *= -1;
        }

        bounce();
        center.setX(static_cast<int>(center.x() + vectorX*5));
        center.setY(static_cast<int>(center.y() + vectorY*5));

        stateGameTime -= timerInterval;
        if (stateGameTime <= 0)
        {
            currentState = GAME_OVER;
            stateGameTime = STATE_GAME_TIME;
        }
        break;
    }
    case GAME_OVER:
    {
        bestScore = (currentScore > bestScore) ? currentScore : bestScore;
        break;
    }
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

            vectorX = setRandomVector();
            vectorY = setRandomVector();
            setTarget();
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
        break;
    }
    case GAME_OVER:
    {
        if (keyEvent->key() == Qt::Key_1)
            currentState = MENU;
        if (keyEvent -> key() == Qt::Key_2)
            qApp -> quit();
        break;
    }

    }
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

    setTarget();

    QTimer* timer = new QTimer;
    connect(timer, SIGNAL(timeout()),
            this, SLOT(onTimerSlot()));
    timer -> setInterval(timerInterval);
    timer -> start();
    factor = 1;
    vectorX = setRandomVector();
    vectorY = setRandomVector();

    mainFont.setFamily("monaco");
    mainFont.setPixelSize(20);
    mainFont.setBold(true);
    setFont(mainFont);

    QString pathToDirectory = "/Users/rostislavtolushkin/Qt/Projects/ReactionGame/";
    QString backgroundImageFileName = "background_image_cut.png";

    sourceBackgroundImage = new QImage(pathToDirectory + backgroundImageFileName);
    temporaryBackground = new QImage(*sourceBackgroundImage);
}

Widget::~Widget()
{

}

void Widget::setRandomTarget()
{
    radius = setRandomRadius();

    int centerX = qrand() % (width() - 2 * radius) + radius;
    int centerY = qrand() % (height() - 2 * radius) + radius;
    center.setX(centerX);
    center.setY(centerY);
    vectorX = setRandomVector();
    vectorY = setRandomVector();
}

void Widget::bounce()
{
    if (vectorX > 0 && center.x()+radius >= width())
        vectorX *= -1;
    if (vectorX < 0 && center.x()-radius <= 0)
        vectorX *= -1;
    if (vectorY > 0 && center.y()+radius >= height())
        vectorY *= -1;
    if (vectorY < 0 && center.y()-radius <= 0)
        vectorY *= -1;
}

double Widget::setRandomVector()
{
    srand(static_cast<unsigned int>(time(nullptr)));
    double vectorWeight = (qrand() % 20 - 10) * 0.1;
    return vectorWeight;
}

bool Widget::isClickedInCircle(QMouseEvent* mouseEvent)
{
    QPoint pointClicked = mouseEvent -> pos();

    int clickedX = static_cast<int>(pow( (center.x() - pointClicked.x()), 2)),
        clickedY = static_cast<int>(pow( (center.y() - pointClicked.y()), 2));

    int awayFromCenter = static_cast<int>(sqrt(clickedY + clickedX));

    if (awayFromCenter < radius)
        return true;

    return false;
}

void Widget::increaseScore()
{
    currentScore += (40 - radius);
}

int Widget::calculateHitPercentage()
{
    double allClicks = static_cast<int>(missClicked + hitClicked);
    double attitude = static_cast<double>(hitClicked) / static_cast<double>(allClicks);
    double percent = attitude * 100;

    return static_cast<int>(percent);
}

int Widget::setRandomRadius()
{
    int min = 2,
        max = 6;
    int r = (qrand() % (max - min) + min) * 5;
    return r;
}

void Widget::setBackgroundImage(QString fullPathToFile, QPainter* currentPainter)
{
    try
    {
        QImage* backgroundImage = new QImage(fullPathToFile);
        QImage scaledBackgroundImage = backgroundImage->scaled(width(), height());
        if (scaledBackgroundImage.isNull())
            throw 404;

        currentPainter->drawImage(0, 0, scaledBackgroundImage);
    } catch (int err)
    {
        if (err == 404)
            currentPainter->drawText(2, height() - 12, "Background image cannot be loaded");
    }
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

void Widget::drawBufferedFrame(QPainter* painter)
{
    switch (currentState)
    {
    case PREPARE:
    {
        int topLeftX = width()/2 - 80;
        int topLeftY = height()/2;

        QImage piece = sourceBackgroundImage->copy(QRect(topLeftX - 20, topLeftY - 20, 200, 30));

        if (statePrepareTime <= 20)
        {
            painter->drawImage(topLeftX - 20, topLeftY - 20, piece);
            break;
        }
        painter->drawImage(topLeftX - 20, topLeftY - 20, piece);
        painter->drawText(topLeftX, topLeftY, "Prepare..." + QString::number(statePrepareTime / 1000) + "." +
                          QString::number(statePrepareTime % 1000));
        break;
    }
    case GAME:
    {

        int circleBoxLX = center.x() + ((vectorX > 0) ? -radius : radius);
        int circleBoxLY = center.y() + ((vectorY > 0) ? -radius : radius);

        QRect boundingBox(circleBoxLX, circleBoxLY, (radius*2), (radius*2));
        QImage circleEraser = sourceBackgroundImage->copy(boundingBox);

        painter->drawImage(circleBoxLX, circleBoxLY, circleEraser);

        QPoint newCenter = center;
        newCenter.setX(static_cast<int>(center.x() + vectorX*5));
        newCenter.setY(static_cast<int>(center.y() + vectorY*5));


        painter->drawEllipse(newCenter, radius, radius);
    }
    }
}
