#include "mainwindow.h"
#include "debugmanager.h"

#include <QApplication>

int main(int argc, char *argv[])
{

  DebugManager debugManager;
  debugManager.debugLexicalAnalyzer();


    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
