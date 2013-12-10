#include <cstdlib>
#include <QApplication>
#include <QMessageBox>
#include "MainWindow.hpp"

int main(int argc, char *argv[])
{
  Q_INIT_RESOURCE(CamelBabel);
  QCoreApplication::setOrganizationName("42Soft");
  QCoreApplication::setOrganizationDomain("42soft.com");
  QCoreApplication::setApplicationName("CamelBabel");
  QApplication::setQuitOnLastWindowClosed(false);
  QApplication a(argc, argv);

  if (!QSystemTrayIcon::isSystemTrayAvailable())
    {
      QMessageBox::critical(0, "Systray",
                            "I couldn't detect any system tray on this system.");
      return (EXIT_FAILURE);
    }
  MainWindow w;
  w.show();  
  return a.exec();
}
