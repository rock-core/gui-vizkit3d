#include <iostream>
#include <QtWidgets>
#include <QtUiTools>
#include "../src/Vizkit3DWidget.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QApplication app(argc,argv);
    QUiLoader loader;
    vizkit3d::Vizkit3DWidget widget;
    widget.show();

    widget.loadPlugin("vizkit3d","GridVisualization");
    app.exec();
}

