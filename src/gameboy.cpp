#include "gameboy.h"

#ifdef EPAPER
#include <epframebuffer.h>
#endif

Gameboy::Gameboy(QQuickItem *parent)
    : QQuickPaintedItem(parent), greyscale(true), landscape{false} {
  image = nullptr;
  connect(this, &Gameboy::runningChanged, [this](bool) {
    if (running()) {
      image = thread->getImage();
    } else {
      image = nullptr;
    }
  });
  thread = new GameboyThread(this);
  connect(thread, &GameboyThread::updated, this, &Gameboy::updated,
          Qt::QueuedConnection);
  connect(
      thread, &GameboyThread::started, this,
      [this] { emit runningChanged(running()); }, Qt::QueuedConnection);
  connect(
      thread, &GameboyThread::finished, this,
      [this] { emit runningChanged(running()); }, Qt::QueuedConnection);
  connect(
      thread, &GameboyThread::paused, this,
      [this] { emit pausedChanged(true); }, Qt::QueuedConnection);
  connect(
      thread, &GameboyThread::resumed, this,
      [this] { emit pausedChanged(false); }, Qt::QueuedConnection);
  connect(thread, &GameboyThread::slowedDownChanged, this,
          &Gameboy::slowedDownChanged, Qt::QueuedConnection);
  connect(thread, &GameboyThread::romNameChanged, this,
          &Gameboy::romNameChanged, Qt::QueuedConnection);
#ifdef EPAPER
  screenCentre = qGuiApp->primaryScreen()->geometry().center();
#endif
}

Gameboy::~Gameboy() { delete thread; }

void Gameboy::updated() {
  QRect rect = boundingRect().toRect();
#ifdef EPAPER
  rect.moveCenter(screenCentre);
  auto *instance = EPFramebuffer::instance();
  QPainter painter(&instance->frameBuffer);
  QImage finalImage = landscape ? image->transformed(QTransform().rotate(90.0),
                                                     Qt::FastTransformation)
                                : *image;
  painter.drawImage(rect, finalImage, finalImage.rect());
  painter.end();
  instance->swapBuffers(
      rect, greyscale ? EPContentType::Color : EPContentType::Monochrome,
      EPScreenMode::Animate, EPFramebuffer::UpdateFlag::PartialUpdate);
#else
  update(rect);
#endif
}

void Gameboy::paint(QPainter *painter) {
#ifndef EPAPER
  if (image != nullptr) {
    painter->drawImage(boundingRect(), greyscale ? *image : monoImage(),
                       image->rect());
  }
#else
  Q_UNUSED(painter)
#endif
}

#ifndef EPAPER
QImage Gameboy::monoImage() {
  return image->convertToFormat(QImage::Format_Mono,
                                Qt::MonoOnly | Qt::DiffuseDither |
                                    Qt::DiffuseAlphaDither | Qt::PreferDither);
}
#endif
