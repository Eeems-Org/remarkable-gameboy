#pragma once

#include <QThread>
#include <QFile>
#include <QImage>
#include <QDebug>
#include <QTimer>
#include <QAbstractEventDispatcher>
#include <QWaitCondition>
#include <QMutex>
#include <QFileInfo>

#include <chrono>
#include <libqboy.h>

using ms_t = std::chrono::milliseconds;

class GameboyThread : public QThread{
    Q_OBJECT
    Q_PROPERTY(bool slowedDown READ slowedDown NOTIFY slowedDownChanged REVISION 1)
    Q_PROPERTY(QString romName READ romName NOTIFY romNameChanged REVISION 1)

public:
    explicit GameboyThread(QObject* parent = 0)
    : QThread(parent),
      slowdown(true),
      pauseRequested(false)
    {
        qboy = new libqboy();
    }
    ~GameboyThread(){
        stop();
        delete qboy;
    }
    bool loadROM(QString path){
        stop();
        qDebug() << "Loading ROM:" << path;
        QFile file(path);
        if(!file.exists()){
            qDebug() << "Path does not exist!";
            return false;
        }
        qboy->reset();
        qboy->loadgame(path.toStdString());
        currentROM = path;
        emit romNameChanged(romName());
        start();
        return true;
    }
    void reset(){
        if(!currentROM.isEmpty()){
            loadROM(currentROM);
        }
    }
    void toggle(){
        mutex.lock();
        condition.notify_all();
        if(pauseRequested){
            pauseRequested = false;
            emit resumed();
        }else{
            pauseRequested = true;
            emit paused();
        }
        mutex.unlock();
    }
    void toggleSpeed(){
        slowdown = !slowdown;
        emit slowedDownChanged(slowdown);
    }
    void stop(){
        if(!isRunning()){
            return;
        }
        requestInterruption();
        quit();
        while(isRunning()){}
        emit updated();
    }
    QImage* getImage(){ return new QImage(qboy->getLCD(), 160, 144, QImage::Format_RGB32); }
    bool isPaused(){ return isRunning() && pauseRequested; }
    void keyDown(int keycode){
        GBKeypadKey key = qtkeytogb(keycode);
        if(key != GBKeypadKey_NONE){
            qDebug() << "Down" << key;
            qboy->keydown(key);
        }
    }
    void keyUp(int keycode){
        GBKeypadKey key = qtkeytogb(keycode);
        if(key != GBKeypadKey_NONE){
            qDebug() << "Up" << key;
            qboy->keyup(key);
        }
    }
    bool slowedDown(){ return slowdown; }
    QString romName(){
        if(currentROM.isEmpty()){
            return "";
        }
        return QFileInfo(currentROM).completeBaseName();
    }

signals:
    void updated();
    void paused();
    void resumed();
    void slowedDownChanged(bool);
    void romNameChanged(QString);

protected:
    void run(){
        qDebug() << "Starting emulation";
        pauseRequested = false;
        int thirds = 0;
        auto lastFrame = std::chrono::steady_clock::now();;
        while(!isInterruptionRequested()){
            if(pauseRequested){
                mutex.lock();
                condition.wait(&mutex);
                mutex.unlock();
            }
            qboy->cycle();
            if(!qboy->refresh_screen()){
                continue;
            }
            auto now = std::chrono::steady_clock::now();
            int diff = std::chrono::duration_cast<ms_t>(now - lastFrame).count();
            lastFrame = now;
            emit updated();
            if(!slowdown){
                continue;
            }
            if(thirds++ == 3){
                thirds = 0;
                diff--;
            }
            if(16 - diff > 0){
                msleep(16 - diff);
            }
        }
        qDebug() << "Stopping emulation";
    }

private:
    libqboy* qboy;
    QMutex mutex;
    QWaitCondition condition;
    bool slowdown;
    bool pauseRequested;
    QString currentROM;

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
