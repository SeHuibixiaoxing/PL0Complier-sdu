#include "mainwindow.h"
#include "debugmanager.h"

#include <QApplication>

int main(int argc, char *argv[])
{

  DebugManager debugManager;
  //debugManager.debugLexicalAnalyzer();
  debugManager.debugParser();


    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
