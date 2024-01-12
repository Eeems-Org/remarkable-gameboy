#pragma once

#include <QDebug>
#include <QDir>
#include <QPainter>
#include <QQuickPaintedItem>
#include <QSGSimpleRectNode>
#include <QGuiApplication>
#include <QScreen>
#include <QtQml/qqml.h>

#ifdef EPAPER
#include <epframebuffer.h>
#endif

#include "gameboythread.h"

class Gameboy : public QQuickPaintedItem {
    Q_OBJECT
    Q_PROPERTY(bool running READ running NOTIFY runningChanged REVISION 1)
    Q_PROPERTY(bool paused READ paused NOTIFY pausedChanged REVISION 1)
    Q_PROPERTY(bool slowedDown READ slowedDown NOTIFY slowedDownChanged REVISION 1)
    Q_PROPERTY(bool greyscale READ isGreyscale WRITE setGreyscale NOTIFY greyscaleChanged REVISION 1)
    Q_PROPERTY(QString homeFolder READ homeFolder CONSTANT REVISION 1)
    Q_PROPERTY(QString romsFolder READ romsFolder CONSTANT REVISION 1)
    Q_PROPERTY(QString romName READ romName NOTIFY romNameChanged REVISION 1)
    QML_NAMED_ELEMENT(Gameboy)

public:
    explicit Gameboy(QQuickItem* parent = nullptr)
    : QQuickPaintedItem(parent),
      greyscale(true)
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
        connect(thread, &GameboyThread::updated, this, &Gameboy::updated, Qt::QueuedConnection);
        connect(thread, &GameboyThread::started, this, [this]{ emit runningChanged(running()); }, Qt::QueuedConnection);
        connect(thread, &GameboyThread::finished, this, [this]{ emit runningChanged(running()); }, Qt::QueuedConnection);
        connect(thread, &GameboyThread::paused, this, [this]{ emit pausedChanged(true); }, Qt::QueuedConnection);
        connect(thread, &GameboyThread::resumed, this, [this]{ emit pausedChanged(false); }, Qt::QueuedConnection);
        connect(thread, &GameboyThread::slowedDownChanged, this, &Gameboy::slowedDownChanged, Qt::QueuedConnection);
        connect(thread, &GameboyThread::romNameChanged, this, &Gameboy::romNameChanged, Qt::QueuedConnection);
#ifdef EPAPER
        screenCentre = qGuiApp->primaryScreen()->geometry().center();
#endif
    }
    ~Gameboy(){
        delete thread;
    }
    bool running(){ return thread->isRunning(); }
    bool paused(){ return thread->isPaused(); }
    Q_REVISION(1) Q_INVOKABLE void loadROM(QString path){ thread->loadROM(path); }
    Q_REVISION(1) Q_INVOKABLE void stop(){ thread->stop(); }
    Q_REVISION(1) Q_INVOKABLE void reset(){ thread->reset(); }
    Q_REVISION(1) Q_INVOKABLE void toggle(){ thread->toggle(); }
    Q_REVISION(1) Q_INVOKABLE void toggleSpeed(){ thread->toggleSpeed(); }
    Q_REVISION(1) Q_INVOKABLE void keyDown(int keycode){ thread->keyDown(keycode); }
    Q_REVISION(1) Q_INVOKABLE void keyUp(int keycode){ thread->keyUp(keycode); }
    QString homeFolder(){ return QUrl::fromLocalFile(QDir::homePath()).toString(); }
    QString romsFolder(){
        auto home = QDir::home();
        if(!home.exists("roms")){
            home.mkdir("roms");
        }
        return QUrl::fromLocalFile(home.absoluteFilePath("roms")).toString();
    }
    bool slowedDown(){ return thread->slowedDown(); }
    QString romName(){ return thread->romName(); }
    bool isGreyscale(){ return greyscale; }
    void setGreyscale(bool value){
        greyscale = value;
        emit greyscaleChanged(greyscale);
    }

signals:
    void runningChanged(bool);
    void pausedChanged(bool);
    void slowedDownChanged(bool);
    void romNameChanged(QString);
    void greyscaleChanged(bool);

protected slots:
    void updated(){
        QRect rect = boundingRect().toRect();
#ifdef EPAPER
        rect.moveCenter(screenCentre);
        QPainter painter(EPFrameBuffer::instance()->framebuffer());
        painter.drawImage(rect, *image, image->rect());
        painter.end();
        EPFrameBuffer::sendUpdate(
            rect,
            greyscale ? EPFrameBuffer::Grayscale : EPFrameBuffer::Mono,
            EPFrameBuffer::PartialUpdate
        );
#else
        update(rect);
#endif
    }

protected:
    void paint(QPainter* painter){
#ifndef EPAPER
        if(image != nullptr){
            painter->drawImage(
                boundingRect(),
                greyscale ? *image : monoImage(),
                image->rect()
            );
        }
#else
        Q_UNUSED(painter)
#endif
    }
#ifndef EPAPER
    QImage monoImage(){
        return image->convertToFormat(
            QImage::Format_Mono,
            Qt::MonoOnly | Qt::DiffuseDither | Qt::DiffuseAlphaDither | Qt::PreferDither
        );
    }
#endif

private:
    QImage* image;
    GameboyThread* thread;
    QPoint screenCentre;
    bool greyscale;
};
