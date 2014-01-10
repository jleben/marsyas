#include <QCoreApplication>
#include <QLibraryInfo>
#include <QDebug>
#include <QQmlEngine>
#include <QStringList>

int main(int argc, char *argv[])
{
  QCoreApplication app(argc, argv);

  qDebug() << "prefix:" << QLibraryInfo::location(QLibraryInfo::PrefixPath);
  qDebug() << "qml:" << QLibraryInfo::location(QLibraryInfo::ImportsPath);
  qDebug() << "qml2:" << QLibraryInfo::location(QLibraryInfo::Qml2ImportsPath);

  QQmlEngine qml;
  QStringList qml_paths = qml.importPathList();
  foreach(const QString &path, qml_paths)
  {
    qDebug() << "qml engine path:" << path;
  }
}
