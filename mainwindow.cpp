#include "mainwindow.h"
#include "./ui_mainwindow.h"

#ifdef __cplusplus
extern "C"{
#include "defs.h"
#endif
#ifdef __cplusplus
}
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

