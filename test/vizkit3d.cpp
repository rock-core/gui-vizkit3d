#include "../src/Vizkit3DWidget.hpp"
#include "../src/Vizkit3DPlugin.hpp"
#include "../src/TransformerGraph.hpp"
#include <QtCore>
#include <iostream>
#include <QApplication>
#include "../viz/GridVisualization.hpp"
#include "../viz/ModelVisualization.hpp"

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
    widget->setTransformation("frame11", "frame12", QVector3D(0,1,2), QQuaternion(1,0,0,0));

    vizkit3d::GridVisualization* plugin1 = new vizkit3d::GridVisualization();
    widget->addPlugin(plugin1);
    vizkit3d::GridVisualization* plugin2 = new vizkit3d::GridVisualization();
    widget->addPlugin(plugin2);
    vizkit3d::GridVisualization* plugin3 = new vizkit3d::GridVisualization();
    widget->addPlugin(plugin3);
    vizkit3d::GridVisualization* plugin4 = new vizkit3d::GridVisualization();
    widget->addPlugin(plugin4);
    vizkit3d::GridVisualization* plugin5 = new vizkit3d::GridVisualization();
    widget->addPlugin(plugin5);
    vizkit3d::GridVisualization* plugin6 = new vizkit3d::GridVisualization();
    widget->addPlugin(plugin6);

    widget->show();

    return app.exec();
}
