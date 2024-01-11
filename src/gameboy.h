#pragma once

#include <QDebug>
#include <QPainter>
#include <QQuickPaintedItem>
#include <QSGSimpleRectNode>
#include <QtQml/qqml.h>

#include "gameboythread.h"

class Gameboy : public QQuickPaintedItem {
    Q_OBJECT
    Q_PROPERTY(bool running READ running NOTIFY runningChanged REVISION 1)
    Q_PROPERTY(bool paused READ paused NOTIFY pausedChanged REVISION 1)
    QML_NAMED_ELEMENT(Gameboy)

public:
    explicit Gameboy(QQuickItem* parent = nullptr)
    : QQuickPaintedItem(parent)
    {
        image = nullptr;
        connect(this, &Gameboy::runningChanged, [this](bool){
            if(running()){
                image = thread->getImage();
            }else{
                image = nullptr;
            }
        });
        thread = new GameboyThread(this);
        connect(thread, &GameboyThread::updated, [this]{ update(rect()); });
        connect(thread, &GameboyThread::started, [this]{ emit runningChanged(running()); });
        connect(thread, &GameboyThread::finished, [this]{ emit runningChanged(running()); });
        connect(thread, &GameboyThread::paused, [this]{ emit pausedChanged(true); });
        connect(thread, &GameboyThread::resumed, [this]{ emit pausedChanged(false); });
    }
    ~Gameboy(){
        delete thread;
    }
    QRect rect(){ return QRect(0, 0, width(), height()); }
    bool running(){ return thread->isRunning(); }
    bool paused(){ return thread->isPaused(); }
    Q_REVISION(1) Q_INVOKABLE void loadROM(QString path){
        thread->stop();
        thread->loadROM(path);
    }
    Q_REVISION(1) Q_INVOKABLE void stop(){ thread->stop(); }
    Q_REVISION(1) Q_INVOKABLE void toggle(){ thread->toggle(); }
    Q_REVISION(1) Q_INVOKABLE void toggleSpeed(){ thread->toggleSpeed(); }

signals:
    void runningChanged(bool);
    void pausedChanged(bool);

protected:
    void paint(QPainter* painter){
        if(image != nullptr){
            painter->drawImage(rect(), *image, image->rect());
        }
    }

private:
    QImage* image;
    GameboyThread* thread;

    GBKeypadKey qtkeytogb(int keycode) {
        switch (keycode) {
            case Qt::Key_Return:
                return GBKeypadKey_START;
            case Qt::Key_Space:
                return GBKeypadKey_SELECT;
            case Qt::Key_Left:
                return GBKeypadKey_LEFT;
            case Qt::Key_Right:
                return GBKeypadKey_RIGHT;
            case Qt::Key_Down:
                return GBKeypadKey_DOWN;
            case Qt::Key_Up:
                return GBKeypadKey_UP;
            case Qt::Key_X:
                return GBKeypadKey_B;
            case Qt::Key_Z:
                return GBKeypadKey_A;
            default:
                return GBKeypadKey_NONE;
        }
    }
};
