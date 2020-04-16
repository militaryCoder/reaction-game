#pragma once

#include <QWidget>

#include "Circle.hpp"

class Widget : public QWidget
{
    Q_OBJECT
protected:
    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent*);
    void keyPressEvent(QKeyEvent*);
public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    enum states { MENU, PREPARE, GAME, GAME_OVER };

    QFont mainFont;
    Circle m_circle;

    void setCirclePosition(int x, int y);

    int m_circleVelocityX;
    int m_circleVelocityY;
    static const int MIN_VELOCITY = 5;
    static const int MAX_VELOCITY = 15;
    void setCircleVelocity(int velocityX, int velocityY);
    void setCircleRadius(unsigned r);

    QImage *m_visualBuffer;

    int m_shrinkFlattenFactor;
    int m_directionX;
    int m_directionY;
    bool m_canDirectionXBeChanged;
    bool m_canDirectionYBeChanged;
    int generateRandomDirection();
    void setCircleFlowDirection(int directionX, int directionY);

    int currentState;
    int statePrepareTime, stateGameTime;
    int currentScore, bestScore;
    int hitClicked;
    int missClicked;
    const unsigned timerInterval = 20;
    bool isClickedInCircle(QMouseEvent*);
    void increaseScore();
    int calculateHitPercentage();

    const int STATE_PREPARE_TIME = 3000;
    const int STATE_GAME_TIME = 10000;

    void drawMainMenuUI(int, int, QPainter*);

    void drawBufferedFrame(QPainter*);

private slots:
    void onTimerSlot();
};
