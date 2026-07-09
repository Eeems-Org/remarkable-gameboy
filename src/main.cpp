#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtPlugin>
#include <QtQuick>

#include <liboxide.h>
#include <liboxide/oxideqml.h>

int main(int argc, char *argv[]) {
  auto qt_version = qVersion();
  qDebug() << "Qt Runtime: " << qt_version;
  qDebug() << "Qt Build: " << QT_VERSION_STR;
  deviceSettings.setupQtEnvironment();
  QGuiApplication app(argc, argv);
  app.setApplicationName("gameboy");
  app.setApplicationDisplayName("Gameboy Emulator");
  app.setApplicationVersion(APP_VERSION);
  QQmlApplicationEngine engine;
  Oxide::QML::registerQML(&engine);
  engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
  if (engine.rootObjects().isEmpty()) {
    qDebug() << "Nothing to display";
    return -1;
  }
  return app.exec();
}
