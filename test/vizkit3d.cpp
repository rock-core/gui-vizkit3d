#include "../src/Vizkit3DWidget.hpp"
#include "../src/Vizkit3DPlugin.hpp"
#include "../src/TransformerGraph.hpp"
#include <QtCore>
#include <iostream>
#include <QApplication>

typedef osg::ref_ptr<osg::Node> NodePtr;

static const osg::Quat Identity(0, 0, 0, 1);
static const osg::Vec3d Zero(0, 0, 0);

using namespace vizkit3d;

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    Vizkit3DWidget *widget = new vizkit3d::Vizkit3DWidget();


    NodePtr root(TransformerGraph::create("root"));

    widget->setTransformation("frame1", "frame2", QVector3D(2,0,0), QQuaternion(1,0,0,0));
    widget->setTransformation("frame1", "frame3", QVector3D(0,2,0), QQuaternion(1,0,0,0));
    widget->setTransformation("frame2", "frame4", QVector3D(0,0,2), QQuaternion(1,0,0,0));

    //widget->setRootFrame("frame4");

    widget->show();

    return app.exec();
}
