#include "gameboy.h"

#ifdef EPAPER
#include <epframebuffer.h>
#include <liboxide/oxideqml.h>
#endif

Gameboy::Gameboy(QQuickItem *parent)
    : QQuickPaintedItem(parent), greyscale(true) {
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
#ifdef EPAPER
  // Only send repaints every 16ms (~60fps)
  if (m_LastPaint.elapsed() < 0.016) {
    return;
  }
  auto windowImage = Oxide::QML::getImageForWindow(window());
  auto rect = mapRectToScene(boundingRect());
  {
    QPainter painter(&windowImage);
    painter.setClipRect(rect);
    painter.drawImage(rect, greyscale ? *image : monoImage(), image->rect());
  }
  Oxide::QML::repaint(window(), rect, Blight::WaveformMode::Animate,
                      greyscale ? Blight::ContentType::Color
                                : Blight::ContentType::Monochrome,
                      Blight::UpdateMode::PartialUpdate);
  m_LastPaint.reset();
#else
  update(boundingRect().toRect());
#endif
}

void Gameboy::paint(QPainter *painter) {
  if (image != nullptr) {
    painter->drawImage(boundingRect(), greyscale ? *image : monoImage(),
                       image->rect());
  }
}

QImage Gameboy::monoImage() {
  return image->convertToFormat(QImage::Format_Mono,
                                Qt::MonoOnly | Qt::DiffuseDither |
                                    Qt::DiffuseAlphaDither | Qt::PreferDither);
}
