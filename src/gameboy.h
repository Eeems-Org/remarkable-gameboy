#pragma once

#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <QPainter>
#include <QQuickPaintedItem>
#include <QSGSimpleRectNode>
#include <QScreen>
#include <QtQml/qqml.h>

#include "gameboythread.h"

class Gameboy : public QQuickPaintedItem {
  Q_OBJECT
  Q_PROPERTY(bool running READ running NOTIFY runningChanged REVISION 1)
  Q_PROPERTY(bool paused READ paused NOTIFY pausedChanged REVISION 1)
  Q_PROPERTY(
      bool slowedDown READ slowedDown NOTIFY slowedDownChanged REVISION 1)
  Q_PROPERTY(bool greyscale READ isGreyscale WRITE setGreyscale NOTIFY
                 greyscaleChanged REVISION 1)
  Q_PROPERTY(bool landscape READ isLandscape WRITE setLandscape NOTIFY
                 landscapeChanged REVISION 1)
  Q_PROPERTY(QString homeFolder READ homeFolder CONSTANT REVISION 1)
  Q_PROPERTY(QString romsFolder READ romsFolder CONSTANT REVISION 1)
  Q_PROPERTY(QString romName READ romName NOTIFY romNameChanged REVISION 1)
  QML_NAMED_ELEMENT(Gameboy)

public:
  explicit Gameboy(QQuickItem *parent = nullptr);
  ~Gameboy();
  bool running() { return thread->isRunning(); }
  bool paused() { return thread->isPaused(); }
  Q_REVISION(1) Q_INVOKABLE void loadROM(QString path) {
    thread->loadROM(path);
  }
  Q_REVISION(1) Q_INVOKABLE void stop() { thread->stop(); }
  Q_REVISION(1) Q_INVOKABLE void reset() { thread->reset(); }
  Q_REVISION(1) Q_INVOKABLE void toggle() { thread->toggle(); }
  Q_REVISION(1) Q_INVOKABLE void toggleSpeed() { thread->toggleSpeed(); }
  Q_REVISION(1) Q_INVOKABLE void keyDown(int keycode) {
    thread->keyDown(keycode);
  }
  Q_REVISION(1) Q_INVOKABLE void keyUp(int keycode) { thread->keyUp(keycode); }
  QString homeFolder() {
    return QUrl::fromLocalFile(QDir::homePath()).toString();
  }
  QString romsFolder() {
    auto home = QDir::home();
    if (!home.exists("roms")) {
      home.mkdir("roms");
    }
    return QUrl::fromLocalFile(home.absoluteFilePath("roms")).toString();
  }
  bool slowedDown() { return thread->slowedDown(); }
  QString romName() { return thread->romName(); }
  bool isGreyscale() { return greyscale; }
  void setGreyscale(bool value) {
    greyscale = value;
    emit greyscaleChanged(greyscale);
  }
  bool isLandscape() { return landscape; }
  void setLandscape(bool value) {
    landscape = value;
    emit landscapeChanged(landscape);
  }

signals:
  void runningChanged(bool);
  void pausedChanged(bool);
  void slowedDownChanged(bool);
  void romNameChanged(QString);
  void greyscaleChanged(bool);
  void landscapeChanged(bool);

protected slots:
  void updated();

protected:
  void paint(QPainter *painter);
#ifndef EPAPER
  QImage monoImage() {
    return image->convertToFormat(
        QImage::Format_Mono, Qt::MonoOnly | Qt::DiffuseDither |
                                 Qt::DiffuseAlphaDither | Qt::PreferDither);
  }
#endif

private:
  QImage *image;
  GameboyThread *thread;
  QPoint screenCentre;
  bool greyscale;
  bool landscape;
};
