#pragma once

#include <QThread>
#include <QFile>
#include <QImage>
#include <QDebug>
#include <QTimer>

#include <chrono>
#include <libqboy.h>

using ms_t = std::chrono::milliseconds;

class GameboyThread : public QThread{
    Q_OBJECT

public:
    explicit GameboyThread(QObject* parent = 0)
    : QThread(parent),
      slowdown(true),
      pauseRequested(false),
      thirds(0)
    {
        qboy = new libqboy();
        timer = new QTimer();
        timer->moveToThread(this);
        timer->setSingleShot(true);
        connect(timer, &QTimer::timeout, this, &GameboyThread::cycle);
    }
    ~GameboyThread(){
        stop();
        delete qboy;
    }
    bool loadROM(QString path){
        qDebug() << "Loading ROM:" << path;
        QFile file(path);
        if(!file.exists()){
            qDebug() << "Path does not exist!";
            return false;
        }
        qboy->loadgame(path.toStdString());
        start();
        return true;
    }
    void toggle(){
        if(pauseRequested){
            pauseRequested = false;
            emit resumed();
        }else{
            pauseRequested = true;
            emit paused();
        }
    }
    void toggleSpeed(){ slowdown = !slowdown; }
    void stop(){
        if(!isRunning()){
            return;
        }
        quit();
        while(isRunning()){}
    }
    QImage* getImage(){ return new QImage(qboy->getLCD(), 160, 144, QImage::Format_RGB32); }
    bool isPaused(){ return isRunning() && pauseRequested; }

signals:
    void updated();
    void paused();
    void resumed();

protected:
    void run(){
        qDebug() << "Starting emulation";
        pauseRequested = false;
        thirds = 0;
        lastFrame = std::chrono::steady_clock::now();
        schedule(0);
        exec();
        qDebug() << "Stopping emulation";
    }
    void schedule(int ms){
        timer->setInterval(ms);
        QMetaObject::invokeMethod(timer, "start", Qt::QueuedConnection, Q_ARG(int, 0));
    }

private slots:
    void cycle(){
        while(true){
            qboy->cycle();
            if(qboy->refresh_screen()){
                auto now = std::chrono::steady_clock::now();
                int diff = std::chrono::duration_cast<ms_t>(now - lastFrame).count();
                lastFrame = now;
                emit updated();
                if(thirds++ == 3){
                    thirds = 0;
                    diff--;
                }
                schedule(slowdown && 16 - diff > 0 ? 16 - diff : 0);
                return;
            }
        }
    }

private:
    libqboy* qboy;
    QTimer* timer;
    bool slowdown;
    bool pauseRequested;
    int thirds;
    std::chrono::time_point<std::chrono::steady_clock> lastFrame;
};
