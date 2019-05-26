#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

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
    QFont mainFont;
    QPoint center;
    int radius;
    void setTarget();
    void bounce();
    int factor;
    double vectorX,
           vectorY;
    double setRandomVector();
    int currentState;
    enum states { MENU, PREPARE, GAME, GAME_OVER };
    int statePrepareTime, stateGameTime;
    int currentScore, bestScore;
    int hitClicked;
    int missClicked;
    int timerInterval = 20;
    bool isClickedInCircle(QMouseEvent*);
    void increaseScore();
    int calculateHitPercentage();

    const int STATE_PREPARE_TIME = 3000;
    const int STATE_GAME_TIME = 10000;

    void setBackgroundImage(QString, QPainter*);
    void drawMainMenuUI(int, int, QPainter*);
    QImage* sourceBackgroundImage;
    QImage* temporaryBackground;

    void drawBufferedFrame(QPainter*);

private slots:
    void onTimerSlot();
};

#endif // WIDGET_H
