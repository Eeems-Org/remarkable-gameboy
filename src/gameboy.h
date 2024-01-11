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
    Q_REVISION(1) Q_INVOKABLE void loadROM(QString path){ thread->loadROM(path); }
    Q_REVISION(1) Q_INVOKABLE void stop(){ thread->stop(); }
    Q_REVISION(1) Q_INVOKABLE void toggle(){ thread->toggle(); }
    Q_REVISION(1) Q_INVOKABLE void toggleSpeed(){ thread->toggleSpeed(); }
    Q_REVISION(1) Q_INVOKABLE void keyDown(int keycode){ thread->keyDown(keycode); }
    Q_REVISION(1) Q_INVOKABLE void keyUp(int keycode){ thread->keyUp(keycode); }

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
};
