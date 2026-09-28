#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts); //расшаривание контекстов OpenGL
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/img/logo.ico"));
    MainWindow w;
    w.show();
    return a.exec();
}
