#include <QCoreApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtPlugin>
#include <QtQuick>

#include "eventfilter.h"

int main(int argc, char *argv[]) {
  auto qt_version = qVersion();
  qDebug() << "Qt Runtime: " << qt_version;
  qDebug() << "Qt Build: " << QT_VERSION_STR;
#ifdef EPAPER
  QCoreApplication::addLibraryPath("/opt/usr/lib/plugins");
  qputenv("QMLSCENE_DEVICE", "epaper");
  qputenv("QT_QUICK_BACKEND", "epaper");
  qputenv("QT_QPA_PLATFORM", "epaper:enable_fonts");
  qputenv("QT_QPA_GENERIC_PLUGINS", "evdevtablet");
  QFile file("/sys/devices/soc0/machine");
  if (!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qDebug() << "Couldn't open " << file.fileName();
    return EXIT_FAILURE;
  }
  QTextStream in(&file);
  QString modelName = in.readLine();
  if (modelName.startsWith("reMarkable 2.0")) {
    qDebug() << "RM2 detected...";
    qputenv("QT_QPA_EVDEV_TOUCHSCREEN_PARAMETERS", "rotate=180:invertx");
    qputenv("QT_QPA_EVDEV_TABLET_PARAMETERS", "");
  } else if (modelName.startsWith("reMarkable 1.0") ||
             modelName.startsWith("reMarkable Prototype 1")) {
    qDebug() << "RM1 detected...";
    qputenv("QT_QPA_EVDEV_TOUCHSCREEN_PARAMETERS", "rotate=180");
    qputenv("QT_QPA_EVDEV_TABLET_PARAMETERS", "");
  } else {
    qDebug() << "Unknown device...";
    qputenv("QT_QPA_EVDEV_TOUCHSCREEN_PARAMETERS", "");
    qputenv("QT_QPA_EVDEV_TABLET_PARAMETERS", "");
  }
#else
  qDebug() << "Desktop detected...";
#endif
  QGuiApplication app(argc, argv);
  auto filter = new EventFilter(&app);
  app.installEventFilter(filter);
  app.setApplicationName("gameboy");
  app.setApplicationDisplayName("Gameboy Emulator");
  app.setApplicationVersion(APP_VERSION);
  QQmlApplicationEngine engine;
  engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
  if (engine.rootObjects().isEmpty()) {
    qDebug() << "Nothing to display";
    return -1;
  }
  return app.exec();
}
