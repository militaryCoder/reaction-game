#pragma once

#include "Circle.hpp"

#include <QWidget>

class Widget : public QWidget
{
    Q_OBJECT
protected:
    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
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

    int m_currentState;
    int m_statePrepareTime, m_stateGameTime;
    int m_currentScore, m_bestScore;
    int m_hitClicked;
    int m_missClicked;
    const unsigned timerInterval = 20;
    bool isClickedInCircle(QMouseEvent*);
    void increaseScore();
    int calculateHitPercentage();

    const int STATE_PREPARE_TIME = 3000;
    const int STATE_GAME_TIME = 10000;

    void drawMainMenuUI(int, int, QPainter*);

private slots:
    void onTimerSlot();
};
